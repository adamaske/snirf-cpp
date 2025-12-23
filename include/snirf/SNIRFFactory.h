#pragma once
#include <filesystem>

#include "snirf/snirf.h"
#include "snirf/snirftype.h"
#include "snirf/SNIRFError.h"

class SNIRFFactory {
public:

	static bool CreateSNIRF(SNIRF& out_snirf,
		const std::filesystem::path& filepath, 
		SNIRFType type,
		std::vector<SNIRFError>& out_errors);
};