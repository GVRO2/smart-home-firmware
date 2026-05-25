#include "TimeProvider.h"

#include <time.h>
#include <WiFi.h>

#include "TimeFormatting.h"

namespace {
constexpr unsigned long NTP_RETRY_INTERVAL_MS = 60000;
constexpr unsigned long NTP_WAIT_TIMEOUT_MS = 4000;
constexpr unsigned long NTP_WAIT_POLL_MS = 250;
constexpr time_t MIN_VALID_UTC = 1700000000;
}

TimeProvider::TimeProvider()
    : isReady_(false), lastSyncAttemptAt_(0) {}

void TimeProvider::sync() {
    lastSyncAttemptAt_ = millis();

    if (WiFi.status() != WL_CONNECTED) {
        isReady_ = false;
        Serial.println("[TIME] ntp_sync_skipped reason=wifi_disconnected");
        return;
    }

    Serial.println("[TIME] ntp_sync_attempt");
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    unsigned long startAt = millis();
    while (!hasValidTime() && millis() - startAt < NTP_WAIT_TIMEOUT_MS) {
        delay(NTP_WAIT_POLL_MS);
    }

    isReady_ = hasValidTime();
    if (isReady_) {
        Serial.println("[TIME] ntp_synced=true");
    } else {
        Serial.println("[TIME] ntp_synced=false");
    }
}

void TimeProvider::ensureSynced() {
    if (hasValidTime()) {
        isReady_ = true;
        return;
    }

    unsigned long now = millis();
    if (lastSyncAttemptAt_ != 0 && now - lastSyncAttemptAt_ < NTP_RETRY_INTERVAL_MS) {
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
