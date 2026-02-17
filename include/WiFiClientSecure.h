#pragma once
#include "WiFiClient.h"

class WiFiClientSecure : public WiFiClient {
public:
    void setInsecure() {} // Skip cert verification (mock)
    void setCACert(const char* ca) { (void)ca; }
    void setCertificate(const char* cert) { (void)cert; }
    void setPrivateKey(const char* key) { (void)key; }
};
