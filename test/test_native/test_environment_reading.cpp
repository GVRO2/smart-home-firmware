#include <unity.h>

#include <cmath>

#include "EnvironmentReading.h"

void setUp() {}
void tearDown() {}

void shouldAcceptReadingWhenTemperatureAndHumidityAreValid() {
    EnvironmentReading reading;

    TEST_ASSERT_TRUE(EnvironmentReading::isValidTemperature(27.4f));
    TEST_ASSERT_TRUE(EnvironmentReading::isValidHumidity(62.5f));

    reading.hasTemperature = EnvironmentReading::isValidTemperature(27.4f);
    reading.hasHumidity = EnvironmentReading::isValidHumidity(62.5f);

    TEST_ASSERT_TRUE(reading.hasAnyValidValue());
}

void shouldAcceptReadingWhenOnlyTemperatureIsValid() {
    EnvironmentReading reading;

    TEST_ASSERT_TRUE(EnvironmentReading::isValidTemperature(27.4f));
    TEST_ASSERT_FALSE(EnvironmentReading::isValidHumidity(NAN));

    reading.hasTemperature = EnvironmentReading::isValidTemperature(27.4f);
    reading.hasHumidity = EnvironmentReading::isValidHumidity(NAN);

    TEST_ASSERT_TRUE(reading.hasAnyValidValue());
}

void shouldAcceptReadingWhenOnlyHumidityIsValid() {
    EnvironmentReading reading;

    TEST_ASSERT_FALSE(EnvironmentReading::isValidTemperature(NAN));
    TEST_ASSERT_TRUE(EnvironmentReading::isValidHumidity(62.5f));

    reading.hasTemperature = EnvironmentReading::isValidTemperature(NAN);
    reading.hasHumidity = EnvironmentReading::isValidHumidity(62.5f);

    TEST_ASSERT_TRUE(reading.hasAnyValidValue());
}

void shouldRejectReadingWhenAllSensorValuesAreInvalid() {
    EnvironmentReading reading;

    TEST_ASSERT_FALSE(EnvironmentReading::isValidTemperature(NAN));
    TEST_ASSERT_FALSE(EnvironmentReading::isValidHumidity(NAN));

    TEST_ASSERT_FALSE(reading.hasAnyValidValue());
}

void shouldRejectReadingWhenTemperatureIsBelowMinimum() {
    TEST_ASSERT_FALSE(EnvironmentReading::isValidTemperature(-40.1f));
}

void shouldRejectReadingWhenTemperatureIsAboveMaximum() {
    TEST_ASSERT_FALSE(EnvironmentReading::isValidTemperature(80.1f));
}

void shouldRejectReadingWhenHumidityIsBelowMinimum() {
    TEST_ASSERT_FALSE(EnvironmentReading::isValidHumidity(-0.1f));
}

void shouldRejectReadingWhenHumidityIsAboveMaximum() {
    TEST_ASSERT_FALSE(EnvironmentReading::isValidHumidity(100.1f));
}