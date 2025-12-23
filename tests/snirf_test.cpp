#ifdef SNIRF_USE_GTEST
// Google Test version - for automated testing
#include <gtest/gtest.h>
#include <snirf/SNIRFFactory.h>

class SNIRFTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(SNIRFTest, CreateSNIRFUnknownType) {
    auto snirf = SNIRFFactory::CreateSNIRF(SNIRFType::UNKNOWN);
    EXPECT_GE(snirf.numDataPoints, 0) << "Number of data points should be non-negative";
}

TEST_F(SNIRFTest, ExampleTest) {
    EXPECT_TRUE(true);
}

#else
// Development executable version - for manual testing during development
#include <iostream>
#include <snirf/SNIRFFactory.h>

int main() {
    std::cout << "=== SNIRF Development Test ===" << std::endl;

    // Test 1: Create SNIRF with UNKNOWN type
    std::cout << "\n[Test 1] Creating SNIRF with UNKNOWN type..." << std::endl;

    SNIRF snirf;

    std::vector<SNIRFError> errors;
    auto success = SNIRFFactory::CreateSNIRF(snirf, "example.snirf", SNIRFType::UNKNOWN, errors);
    if (success) {
        std::cout << "  SNIRF created successfully!" << std::endl;
    } else {
        std::cout << "  Failed to create SNIRF." << std::endl;
        
        for (const auto& error : errors) {
            std::cout << "    Error: " << error.message << std::endl;
        }
    }

    // Add your development tests here
    std::cout << "\n[Test 2] Your test here..." << std::endl;
    // ... your code ...

    std::cout << "\n=== All tests completed ===" << std::endl;
    return 0;
}
#endif