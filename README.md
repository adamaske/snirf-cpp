# snirf-cpp

A C++ library for reading [SNIRF](https://github.com/fNIRS/snirf) (Shared Near Infrared Spectroscopy Format) files.

## Installation

```sh
git clone --recurse-submodules https://github.com/adamaske/snirf-cpp.git
```

In your `CMakeLists.txt`:

```cmake
add_subdirectory("path/to/snirf-cpp")
target_link_libraries(your-target PRIVATE snirf)
```

## Usage

```cpp
#include <snirfcpp/snirfcpp.h>
#include <iostream>

int main() {
    SNIRFCPP::SNIRF snirf;
    std::vector<SNIRFCPP::SNIRFError> errors;

    if (!SNIRFCPP::LoadSNIRF("recording.snirf", snirf, errors)) {
        for (auto& e : errors)
            std::cerr << e.message << "\n";
        return 1;
    }

    // Basic info
    std::cout << "File:       " << snirf.GetFilepath() << "\n";
    std::cout << "Sources:    " << snirf.GetSourceAmount() << "\n";
    std::cout << "Detectors:  " << snirf.GetDetectorAmount() << "\n";
    std::cout << "Duration:   " << snirf.GetDurationSeconds() << " s\n";
    std::cout << "Rate:       " << snirf.GetSamplingRate() << " Hz\n";

    // Channel data  [channels x timepoints]
    auto& store = snirf.channel_store;
    std::cout << "Channels:   " << store.rows << "\n";
    std::cout << "Timepoints: " << store.cols << "\n";
    double sample = store.at(0, 0); // channel 0, timepoint 0

    // Events / stimulus markers
    for (auto& event : snirf.events.events) {
        std::cout << "Event: " << event.name
                  << " (" << event.markers.size() << " markers)\n";
        for (auto& m : event.markers)
            std::cout << "  onset=" << m.onset << "s  duration=" << m.duration << "s\n";
    }

    return 0;
}
```
