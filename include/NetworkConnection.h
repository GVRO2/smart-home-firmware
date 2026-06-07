#pragma once

class NetworkConnection {
public:
    virtual ~NetworkConnection() = default;
    virtual void connect() = 0;
    virtual void ensureConnected() = 0;
    virtual bool isConnected() const = 0;
};
