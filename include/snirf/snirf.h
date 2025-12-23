#pragma once

#include <vector>

#include "SNIRFError.h"

namespace SNIRFCPP {
	struct SNIRF {
		using ChannelDataPointType = double;

		std::vector<ChannelDataPointType> singular_channel_data;



		// This can store any errors which occur 
		std::vector<SNIRFError> errors;
	};
}