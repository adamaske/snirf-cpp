#pragma once


struct SNIRF {
	int version;
	int numDataPoints;
	int numTimePoints;
	float* data; // Pointer to the data array
};