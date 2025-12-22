#pragma once

#include "snirf/snirf.h"
#include "snirf/snirftype.h"

class SNIRFFactory {
public:

	static SNIRF CreateSNIRF(SNIRFType type);
};