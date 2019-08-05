#ifndef EspWifiSetup_h_
#define EspWifiSetup_h_

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <esp_system.h>
#include <DNSServer.h>
#include <ESPmDNS.h>

#include "web/EspWifiSetupPage.h"
#include "web/EspWifiSetupOkPage.h"
#include "web/EspWifiSetupErrorPage.h"

class EspWifiSetup
{
public:
    static bool setup();
    static bool setup(String hostname);
    static bool setup(String hostname, bool addMacSuffix);
    static bool setup(String hostname, bool addMacSuffix, bool abortIfNotConnecting);
    static void resetSettings();

private:
    static String hostname;
    static bool doRestart;

    static void logDebug(String message);
    static void runWiFiConfigurationServer(String apName);
    static bool readWifiSettings(char *&ssid, char *&password);
    static void WiFiEventHandler(WiFiEvent_t event, system_event_info_t info);

    static void handleRoot(AsyncWebServerRequest *request);
    static void handlePostConfiguration(AsyncWebServerRequest *request);
    static void handleOk(AsyncWebServerRequest *request);
    static void handleError(AsyncWebServerRequest *request);

    static String getShortMac();

    static AsyncWebServer *server;
    static DNSServer *dnsServer;
};

#endif