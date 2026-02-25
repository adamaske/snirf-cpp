#pragma once
#include <vector>
#include <cstddef>

namespace SNIRFCPP {

    /// Raw time-series matrix parsed from data1/dataTimeSeries.
    /// Layout: row-major [channels × timepoints].
    struct ChannelDataStore {
        std::vector<double> data; // flat [channels × timepoints], row-major
        size_t rows = 0;          // number of channels
        size_t cols = 0;          // number of timepoints

        double at(size_t row, size_t col) const { return data[row * cols + col]; }
    };

} // namespace SNIRFCPP
