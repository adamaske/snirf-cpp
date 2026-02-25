#pragma once

#include "snirfcpp/detail/FileDescriptor.h"
#include "snirfcpp/detail/Metadata.h"
#include "snirfcpp/detail/Probe.h"
#include "snirfcpp/detail/Channels.h"
#include "snirfcpp/detail/Time.h"
#include "snirfcpp/detail/Events.h"
#include "snirfcpp/detail/Biosignals.h"

#include <string>
#include <vector>

namespace SNIRFCPP {

    struct SNIRF {

        // ---- identity / origin ----
        FileDescriptor file_descriptor;

        // ---- metadata tags ----
        Metadata metadata;

        // ---- probe geometry (sources, detectors, wavelengths) ----
        Probe probe;

        // ---- per-channel hemodynamic time-series ----
        ChannelDataStore channel_store;

        // ---- shared time axis ----
        TimeData time_data;

        // ---- stimulus / event markers ----
        EventsContainer events;

        // ---- auxiliary biosignals (respiration, GSR, PPG, ...) ----
        BiosignalData biosignals;

        // ---- convenience accessors ----
        bool IsFileLoaded() const { return !file_descriptor.filepath.empty(); }
        std::string GetFilepath() const { return file_descriptor.filepath.string(); }

        const Probe& GetProbe() const { return probe; }
        Probe& GetProbe() { return probe; }

        int GetSourceAmount()   const { return static_cast<int>(probe.sources.size()); }
        int GetDetectorAmount() const { return static_cast<int>(probe.detectors.size()); }

        double GetSamplingRate()    const { return time_data.sampling_frequency; }
        double GetDurationSeconds() const { return time_data.duration; }
        const std::vector<double>& GetTime() const { return time_data.time; }

        const std::vector<int>& GetWavelengths() const { return probe.wavelengths; }

        ChannelMap GetChannels() const { return probe.channels; }

        bool HasBiosignals() const { return metadata.has_wings_generation; }
    };

} // namespace SNIRFCPP
