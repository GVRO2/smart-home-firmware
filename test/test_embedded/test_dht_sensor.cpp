#include <unity.h>

void setUp() {}
void tearDown() {}

void shouldCompileEmbeddedSmokeTestForDhtSensor() {
    TEST_ASSERT_TRUE(true);
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(shouldCompileEmbeddedSmokeTestForDhtSensor);
    return UNITY_END();
}