#include "snirfcpp/snirfcpp.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: snirf_dev <path/to/file.snirf>\n";
        return 1;
    }

    SNIRFCPP::SNIRF snirf;
    std::vector<SNIRFCPP::SNIRFError> errors;

    bool ok = SNIRFCPP::LoadSNIRF(argv[1], snirf, errors);

    for (const auto& err : errors) {
        std::cerr << "[error " << err.code << "] " << err.message << "\n";
    }

    if (!ok) return 1;

    std::cout << "Loaded: " << snirf.GetFilepath() << "\n";
    std::cout << "  Sources:    " << snirf.GetSourceAmount() << "\n";
    std::cout << "  Detectors:  " << snirf.GetDetectorAmount() << "\n";
    std::cout << "  Channels:   " << snirf.GetChannels().size() << "\n";
    std::cout << "  Duration:   " << snirf.GetDurationSeconds() << " s\n";
    std::cout << "  Rate:       " << snirf.GetSamplingRate() << " Hz\n";

    return 0;
}
