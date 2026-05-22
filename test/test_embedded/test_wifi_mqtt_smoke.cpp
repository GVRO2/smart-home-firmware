#include <unity.h>

void setUp() {}
void tearDown() {}

void shouldCompileEmbeddedSmokeTestForWifiAndMqtt() {
    TEST_ASSERT_TRUE(true);
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(shouldCompileEmbeddedSmokeTestForWifiAndMqtt);
    return UNITY_END();
}