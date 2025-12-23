#include "snirf/SNIRFFactory.h"

#include "HighFive/HighFive.hpp"
#include "HighFive/H5File.hpp"


bool SNIRFFactory::CreateSNIRF(SNIRF& out_snirf, const std::filesystem::path& filepath, SNIRFType type, std::vector<SNIRFError>& out_errors)
{
	if (!std::filesystem::exists(filepath)) {
		out_errors.emplace_back(SNIRFError::FILE_NOT_FOUND, "File not found");

		return false;
	}

	return true;
}
