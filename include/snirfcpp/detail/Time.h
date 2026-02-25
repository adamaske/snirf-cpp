#pragma once
#include <vector>

namespace SNIRFCPP {

    struct TimeData {
        std::vector<double> time;

        double duration           = 0.0;
        double sampling_frequency = 0.0;
    };

} // namespace SNIRFCPP
