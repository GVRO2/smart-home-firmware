#include "TimeProvider.h"

#include <time.h>

#include "TimeFormatting.h"

namespace {
constexpr unsigned long NTP_RETRY_INTERVAL_MS = 60000;
constexpr unsigned long NTP_WAIT_TIMEOUT_MS = 15000;
constexpr time_t MIN_VALID_UTC = 1700000000;
}

TimeProvider::TimeProvider()
    : isReady_(false), lastSyncAttemptAt_(0) {}

void TimeProvider::sync() {
    lastSyncAttemptAt_ = millis();

    Serial.println("Sincronizando horario NTP...");
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    unsigned long startAt = millis();
    while (!hasValidTime() && millis() - startAt < NTP_WAIT_TIMEOUT_MS) {
        delay(500);
    }

    isReady_ = hasValidTime();
    if (isReady_) {
        Serial.println("Horario NTP sincronizado.");
    } else {
        Serial.println("Falha ao sincronizar NTP.");
    }
}

void TimeProvider::ensureSynced() {
    if (hasValidTime()) {
        isReady_ = true;
        return;
    }

    unsigned long now = millis();
    if (now - lastSyncAttemptAt_ < NTP_RETRY_INTERVAL_MS) {
        return;
    }

    sync();
}

String TimeProvider::nowIsoUtc() {
    if (!hasValidTime()) {
        isReady_ = false;
        return String();
    }

    time_t now = time(nullptr);
    return formatUtcIso(now);
}

bool TimeProvider::isReady() const {
    return isReady_ && hasValidTime();
}

bool TimeProvider::hasValidTime() const {
    time_t now = time(nullptr);
    return now >= MIN_VALID_UTC;
}

String TimeProvider::formatUtcIso(time_t utcSeconds) const {
    return String(formatUtcIsoUtc(utcSeconds).c_str());
}
