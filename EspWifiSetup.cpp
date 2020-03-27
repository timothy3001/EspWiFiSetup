#include "EspWifiSetup.h"

#define PREFERENCES_WIFI "WifiSettings"
#define SETTING_SSID "ssid"
#define SETTING_PASSWORD "password"

AsyncWebServer *EspWifiSetup::server = NULL;
DNSServer *EspWifiSetup::dnsServer = NULL;
bool EspWifiSetup::doRestart = false;

String EspWifiSetup::getShortMac()
{
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);

    String macStringLastPart = "";
    for (int i = 4; i < 6; i++)
    {
        macStringLastPart += String(mac[i], HEX);
    }

    return macStringLastPart;
}

bool EspWifiSetup::readWifiSettings(char *&ssid, char *&password)
{
    Preferences preferences;
    preferences.begin(PREFERENCES_WIFI, false);

    String ssidString = preferences.getString(SETTING_SSID, String(""));

    bool result = false;
    if (ssidString.length() != 0)
    {
        String passwordString = preferences.getString(SETTING_PASSWORD, String(""));

        ssid = new char[ssidString.length() + 1];
        password = new char[passwordString.length() + 1];

        strcpy(ssid, ssidString.c_str());
        strcpy(password, passwordString.c_str());

        result = true;
    }

    preferences.end();

    return result;
}

bool EspWifiSetup::setup()
{
    String hostname = String("WiFiSetup");
    return setup(hostname, true, false);
}

bool EspWifiSetup::setup(String hostname)
{
    return setup(hostname, false, false);
}

bool EspWifiSetup::setup(String hostname, bool addMacSuffix)
{
    return setup(hostname, addMacSuffix, false);
}

bool EspWifiSetup::setup(String hostname, bool addMacSuffix, bool abortIfNotConnecting)
{
    if (addMacSuffix)
    {
        hostname = hostname + String("-") + String(getShortMac());
    }

    char *ssid = 0;
    char *password = 0;

    bool result = readWifiSettings(ssid, password);

    if (result)
    {
        // Try to connect to the access point
        logDebug(String("Connecting to access point with SSID '") + String(ssid) + String("' and password '") + String(password) + String("'..."));

        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        WiFi.begin(ssid, password);
        WiFi.onEvent(WiFiEventHandler);
        WiFi.setHostname(hostname.c_str());

        unsigned long startTimeConnecting = millis();
        while (WiFi.status() != WL_CONNECTED && millis() < startTimeConnecting + 30000)
        {
            delay(200);
        }

        if (WiFi.status() != WL_CONNECTED)
        {
            logDebug("Could not connect to configured access point!");
            if (!abortIfNotConnecting)
            {
                logDebug("Creating access point...");
                runWiFiConfigurationServer(hostname);
            }
            else
            {
                result = false;
            }
        }
        else
        {
            logDebug("Successfully connected to WiFi!");
            logDebug("Setting up mDNS...");
            if (!MDNS.begin(hostname.c_str()))
            {
                logDebug("Could not setup mDNS!");
            }
            else
            {
                logDebug(String("mDNS setup successfully! Hostname: ") + String(hostname));
                MDNS.addService("http", "tcp", 80);
            }

            result = true;
        }
    }
    else
    {
        runWiFiConfigurationServer(hostname);
    }

    delete ssid;
    delete password;

    return result;
}

void EspWifiSetup::resetSettings()
{
    Preferences preferences;
    preferences.begin(PREFERENCES_WIFI, false);

    preferences.clear();

    preferences.end();
}

void EspWifiSetup::logDebug(String message)
{
#ifdef DEBUG
    Serial.print("WiFiSetup: ");
    Serial.println(message);
#endif
}

// Workaround for ESP32 tending to not being able to connect. Happens sometimes...
void EspWifiSetup::WiFiEventHandler(WiFiEvent_t event, system_event_info_t info)
{
    switch (event)
    {
    case WIFI_REASON_ASSOC_TOOMANY:
        ESP.restart();
        break;
    }
}

void EspWifiSetup::runWiFiConfigurationServer(String apName)
{
    String logMessage = String("Starting access point with name '") + apName + String("'...");
    logDebug(logMessage);

    WiFi.disconnect();
    WiFi.mode(WIFI_AP);

    IPAddress ip(192, 168, 4, 1);
    IPAddress nmask(255, 255, 255, 0);
    WiFi.softAP(apName.c_str(), "");
    WiFi.softAPConfig(ip, ip, nmask);

    logDebug(String("Access point created, gateway IP is ") + String(WiFi.softAPIP().toString()));
    logDebug("Creating web server...");

    server = new AsyncWebServer(80);    
    delay(100);
    dnsServer = new DNSServer();
    dnsServer->start(53, "*", ip);
    delay(100);

    server->on("/", HTTP_GET, [&](AsyncWebServerRequest *request) -> void { handleRoot(request); });
    server->on("/config", HTTP_POST, handlePostConfiguration);
    server->on("/ok", HTTP_GET, handleOk);
    server->on("/error", HTTP_GET, handleError);

    server->begin();
    delay(200);

    while (!doRestart)
    {
        dnsServer->processNextRequest();
        yield();
    }

    logDebug("ESP about to restart...");
    unsigned long tsWaitForRestart = millis();
    while (tsWaitForRestart + 10000 > millis())
    {
        dnsServer->processNextRequest();
        yield();
    }

    ESP.restart();
}

void EspWifiSetup::handleRoot(AsyncWebServerRequest *request)
{
    logDebug("WebServer: Root called");
    request->send(200, "text/html", pageWifiSetup);
}

void EspWifiSetup::handlePostConfiguration(AsyncWebServerRequest *request)
{
    logDebug("WebServer: New configuration posted!");

    if (!request->hasArg("ssid") || request->arg("ssid").length() == 0)
    {
        request->send(400, "text/html", "No ssid!");
    }
    else
    {
        String ssid = request->arg("ssid");
        String password = request->hasArg("password") ? request->arg("password") : String("");

        Preferences preferences;
        preferences.begin(PREFERENCES_WIFI, false);

        preferences.putString(SETTING_SSID, ssid);
        preferences.putString(SETTING_PASSWORD, password);

        preferences.end();

        request->send(200, "text/html", "Settings updated!");
        doRestart = true;
    }
}

void EspWifiSetup::handleOk(AsyncWebServerRequest *request)
{
    logDebug("WebServer: Ok called");
    request->send(200, "text/html", pageWifiSetupOk);
}

void EspWifiSetup::handleError(AsyncWebServerRequest *request)
{
    logDebug("WebServer: Error called");
    request->send(200, "text/html", pageWifiSetupError);
}
