#pragma once

#include <string>

class ClockProvider {
public:
    virtual ~ClockProvider() = default;
    virtual void sync() = 0;
    virtual void ensureSynced() = 0;
    virtual std::string nowIsoUtc() = 0;
    virtual bool isReady() const = 0;
};
