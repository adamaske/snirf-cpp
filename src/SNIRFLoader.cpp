#include "snirfcpp/SNIRFLoader.h"

#include <HighFive/H5File.hpp>
#include <highfive/H5DataSet.hpp>
#include <highfive/H5DataSpace.hpp>
#include <highfive/H5Easy.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>

// ============================================================================
// HDF5 Helpers
// ============================================================================

namespace {
    using namespace HighFive;

    template <typename T>
    std::vector<T> ReadVector(const Group& group, const std::string& name) {
        try {
            DataSet dataset = group.getDataSet(name);
            std::vector<T> data;
            dataset.read(data);
            return data;
        }
        catch (const Exception&) {
            return {};
        }
    }
}

// ============================================================================
// Section Parsers  (each populates one part of the SNIRF struct)
// ============================================================================

namespace SNIRFCPP {

    // --------------- Metadata ---------------------------------------------------

    static void ParseMetadata(const HighFive::Group& metadata_group,
        Metadata& out)
    {
        for (const auto& name : metadata_group.listObjectNames()) {
            if (name == "wingsGeneration") {
                out.has_wings_generation = true;
            }

            try {
                auto dataset = metadata_group.getDataSet(name);
                auto type_class = dataset.getDataType().getClass();
                if (type_class == HighFive::DataTypeClass::String) {
                    std::string value;
                    dataset.read(value);
                    out.tags.push_back({ name, value });
                }
                else {
                    out.tags.push_back({ name, "(non-string)" });
                }
            }
            catch (const HighFive::Exception&) {
                // Not a scalar dataset; skip
            }
        }
    }

    // --------------- Probe (sources, detectors, wavelengths) --------------------

    static void ParseProbe(const HighFive::Group& probe_group,
        Probe& out)
    {
        using ChannelValue = double;

        // ---- Detectors ----
        {
            auto det2D = probe_group.getDataSet("detectorPos2D");
            auto det3D = probe_group.getDataSet("detectorPos3D");

            auto dims2D = det2D.getDimensions();
            auto dims3D = det3D.getDimensions();
            if (dims2D[0] != dims3D[0])
                throw HighFive::Exception("Detector count mismatch between 2D and 3D positions in SNIRF probe.");

            std::vector<ChannelValue> data2D(dims2D[0] * dims2D[1]);
            std::vector<ChannelValue> data3D(dims3D[0] * dims3D[1]);
            det2D.read_raw<ChannelValue>(data2D.data());
            det3D.read_raw<ChannelValue>(data3D.data());

            int n = static_cast<int>(dims2D[0]);
            for (int i = 0; i < n; i++) {
                Optode o;
                o.type = OptodeType::DETECTOR;
                o.id = i + 1;
                o.position_2D = Vec2{ static_cast<float>(data2D[i * 2 + 0]),
                                      static_cast<float>(data2D[i * 2 + 1]) };
                o.position_3D = Vec3{ static_cast<float>(data3D[i * 3 + 0]),
                                      static_cast<float>(data3D[i * 3 + 2]),   // z-y swap (matches original)
                                      static_cast<float>(data3D[i * 3 + 1]) };
                out.detectors[o.id] = o;
            }
        }

        // ---- Sources ----
        {
            auto src2D = probe_group.getDataSet("sourcePos2D");
            auto src3D = probe_group.getDataSet("sourcePos3D");

            auto dims2D = src2D.getDimensions();
            auto dims3D = src3D.getDimensions();
            if (dims2D[0] != dims3D[0])
                throw HighFive::Exception("Source count mismatch between 2D and 3D positions in SNIRF probe.");

            std::vector<ChannelValue> data2D(dims2D[0] * dims2D[1]);
            std::vector<ChannelValue> data3D(dims3D[0] * dims3D[1]);
            src2D.read_raw<ChannelValue>(data2D.data());
            src3D.read_raw<ChannelValue>(data3D.data());

            int n = static_cast<int>(dims2D[0]);
            for (int i = 0; i < n; i++) {
                Optode o;
                o.type = OptodeType::SOURCE;
                o.id = i + 1;
                o.position_2D = Vec2{ static_cast<float>(data2D[i * 2 + 0]),
                                      static_cast<float>(data2D[i * 2 + 1]) };
                o.position_3D = Vec3{ static_cast<float>(data3D[i * 3 + 0]),
                                      static_cast<float>(data3D[i * 3 + 2]),
                                      static_cast<float>(data3D[i * 3 + 1]) };
                out.sources[o.id] = o;
            }
        }

        // ---- Wavelengths ----
        {
            auto wl_ds = probe_group.getDataSet("wavelengths");
            auto dims = wl_ds.getDimensions();
            std::vector<int> wl(dims[0]);
            wl_ds.read(wl);
            std::sort(wl.begin(), wl.end());
            out.wavelengths = std::move(wl);
        }
    }

    // --------------- Time -------------------------------------------------------

    static void ParseTime(const HighFive::DataSet& time_ds,
        TimeData& out)
    {
        std::vector<double> time_vec(time_ds.getDimensions()[0]);
        time_ds.read(time_vec);

        double total_duration = time_vec.back() - time_vec.front();
        size_t num_intervals = time_vec.size() - 1;
        double avg_dt = total_duration / static_cast<double>(num_intervals);

        out.time = std::move(time_vec);
        out.duration = total_duration;
        out.sampling_frequency = 1.0 / avg_dt;
    }

    // --------------- Channel Data (data1) ---------------------------------------

    static void ParseData1(const HighFive::Group& data1,
        TimeData& time_out,
        ChannelDataStore& store_out,
        Probe& probe_out)
    {
        // ---- Time ----
        ParseTime(data1.getDataSet("time"), time_out);

        // ---- Raw matrix ----
        auto ts_ds = data1.getDataSet("dataTimeSeries");
        auto dims = ts_ds.getDimensions();
        // dims[0] = timepoints, dims[1] = channels (HDF5 layout)

        std::vector<double> raw(dims[0] * dims[1]);
        ts_ds.read_raw<double>(raw.data());

        // Transpose from [timepoints × channels] to [channels × timepoints]
        store_out.rows = dims[1];
        store_out.cols = dims[0];
        store_out.data.resize(dims[0] * dims[1]);
        for (size_t t = 0; t < dims[0]; t++)
            for (size_t c = 0; c < dims[1]; c++)
                store_out.data[c * dims[0] + t] = raw[t * dims[1] + c];

        if ((store_out.rows % 2) != 0)
            throw HighFive::Exception("SNIRF data1 must have an even number of measurement rows (HbR + HbO pairs).");

        size_t half = store_out.rows / 2;

        // ---- Measurement lists → channels ----
        for (size_t i = 0; i < half; i++) {
            size_t hbr_idx = i;
            size_t hbo_idx = i + half;

            auto ml = data1.getGroup("measurementList" + std::to_string(i + 1));

            int sourceIndex = 0, detectorIndex = 0;
            ml.getDataSet("sourceIndex").read(sourceIndex);
            ml.getDataSet("detectorIndex").read(detectorIndex);

            Channel ch;
            ch.id = static_cast<ChannelID>(i);
            ch.source_id = static_cast<OptodeID>(sourceIndex);
            ch.detector_id = static_cast<OptodeID>(detectorIndex);

            // HbR
            ch.hbr_data.resize(store_out.cols);
            for (size_t t = 0; t < store_out.cols; t++)
                ch.hbr_data[t] = store_out.at(hbr_idx, t);

            // HbO
            ch.hbo_data.resize(store_out.cols);
            for (size_t t = 0; t < store_out.cols; t++)
                ch.hbo_data[t] = store_out.at(hbo_idx, t);

            // HbT = HbO + HbR
            ch.hbt_data.resize(store_out.cols);
            for (size_t t = 0; t < store_out.cols; t++)
                ch.hbt_data[t] = ch.hbo_data[t] + ch.hbr_data[t];

            probe_out.channels[ch.id] = std::move(ch);
        }
    }

    // --------------- Stims / Events ---------------------------------------------

    static void ParseStims(const HighFive::Group& nirs,
        EventsContainer& out)
    {
        for (size_t i = 1; i < 1000; i++) {
            std::string stim_name = "stim" + std::to_string(i);
            if (!nirs.exist(stim_name))
                break;

            auto stim = nirs.getGroup(stim_name);

            std::vector<std::vector<double>> data;
            try {
                stim.getDataSet("data").read(data);
            }
            catch (const HighFive::Exception&) {
                continue;
            }

            Event event;

            std::string name;
            stim.getDataSet("name").read<std::string>(name);
            event.name = name;

            event.markers.reserve(data.size());
            for (const auto& row : data) {
                if (row.size() < 3)
                    continue;
                event.markers.push_back({
                    .onset    = row[0],
                    .duration = row[1],
                    .value    = row[2]
                });
            }

            std::sort(event.markers.begin(), event.markers.end(),
                [](const EventMarker& a, const EventMarker& b) {
                    return a.onset < b.onset;
                });

            out.events.push_back(std::move(event));
        }
    }

    // --------------- Biosignals (aux) -------------------------------------------

    static void ParseBiosignals(const HighFive::Group& nirs,
        BiosignalData& out)
    {
        static const std::vector<std::pair<std::string, AuxilaryLabel>> aux_groups = {
            {"aux1", AUX1}, {"aux2", AUX2}, {"aux3", AUX3},
            {"aux4", AUX4}, {"aux5", AUX5}, {"aux6", AUX6},
            {"aux7", AUX7}, {"aux8", AUX8}, {"aux9", AUX9},
        };

        for (auto& [group_name, label] : aux_groups) {
            if (!nirs.exist(group_name))
                continue;

            auto aux_group = nirs.getGroup(group_name);

            std::string name;
            aux_group.getDataSet("name").read(name);

            AuxlaryType type = UNKNOWN;
            if      (name == "Respiration") type = RESPIRATION;
            else if (name == "GSR")         type = GSR;
            else if (name == "Temperature") type = TEMPERATURE;
            else if (name == "ExGa_1")      type = EX_GA_1;
            else if (name == "ExGa_2")      type = EX_GA_2;
            else if (name == "ExGa_3")      type = EX_GA_3;
            else if (name == "PPG")         type = PPG;
            else if (name == "SpO2")        type = SP_O2;
            else if (name == "Heartrate")   type = HEARTRATE;

            std::string data_unit;
            aux_group.getDataSet("dataUnit").read(data_unit);

            std::vector<double> time_series;
            aux_group.getDataSet("dataTimeSeries").read(time_series);

            std::vector<double> time_vec;
            aux_group.getDataSet("time").read(time_vec);

            out.aux_data.push_back({
                .label = label,
                .type  = type,
                .name  = name,
                .unit  = data_unit,
                .data  = std::move(time_series),
                .time  = std::move(time_vec),
            });
        }
    }

    // ============================================================================
    // Public API
    // ============================================================================

    bool LoadSNIRF(const std::filesystem::path& filepath,
        SNIRF& out,
        std::vector<SNIRFError>& out_errors)
    {
        if (!std::filesystem::exists(filepath)) {
            out_errors.push_back(SNIRFError(
                SNIRFError::FILE_NOT_FOUND,
                "File does not exist: " + filepath.string()));
            return false;
        }

        try {
            HighFive::File file(filepath.string(), HighFive::File::ReadOnly);

            auto nirs = file.getGroup("/nirs");

            out.file_descriptor.filepath = filepath;

            // ---- 1. Metadata ----
            if (nirs.exist("metaDataTags")) {
                ParseMetadata(nirs.getGroup("metaDataTags"), out.metadata);
            }

            // ---- 2. Probe ----
            if (nirs.exist("probe")) {
                ParseProbe(nirs.getGroup("probe"), out.probe);
            }

            // ---- 3. Data1 ----
            if (nirs.exist("data1")) {
                ParseData1(nirs.getGroup("data1"),
                    out.time_data,
                    out.channel_store,
                    out.probe);
            }

            // ---- 4. Stims / Events ----
            ParseStims(nirs, out.events);

            // ---- 5. Biosignals ----
            if (out.metadata.has_wings_generation) {
                ParseBiosignals(nirs, out.biosignals);
            }
        }
        catch (const HighFive::Exception& e) {
            out_errors.push_back(SNIRFError(
                SNIRFError::INVALID_FORMAT,
                "HDF5 parse error: " + std::string(e.what())));
            return false;
        }

        return true;
    }

} // namespace SNIRFCPP
