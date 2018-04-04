#include "ESPWiFiManager.h"

ESPWiFiManager::ESPWiFiManager() 
{

}


void ESPWiFiManager::setupConfigPortal() 
{
    dnsServer.reset(new DNSServer());
    server.reset(new ESP8266WebServer(80));

    DEBUG_WM(F(""));
    _configPortalStart = millis();

    DEBUG_WM(F("Configuring access point... "));
    DEBUG_WM(_apName);
    
    if (_apPassword != NULL)
    {
        if (strlen(_apPassword) < 8 || strlen(_apPassword) > 63)
        {
            DEBUG_WM(F("Invalid AccessPoint password. Ignoring"));
            _apPassword = NULL;
        }
        DEBUG_WM(_apPassword);
    }

    if (_apPassword != NULL)
    {
        WiFi.softAP(_apName, _apPassword);
    } 
    
    else
    {
      WiFi.softAP(_apName);
    }
    
    delay(500);
    DEBUG_WM(F("AP IP address: "));
    DEBUG_WM(WiFi.softAPIP());
    
    dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer->start(DNS_PORT, "*", WiFi.softAPIP());

  
    server->on("/wifisave", std::bind(&ESPWiFiManager::handleWifiSave, this));
    server->on("/r", std::bind(&ESPWiFiManager::handleReset, this));
    server->begin(); 
    
    DEBUG_WM(F("HTTP server started"));
}


boolean ESPWiFiManager::autoConnect(char const *apName, char const *apPassword) 
{
    DEBUG_WM(F(""));
    DEBUG_WM(F("AutoConnect"));
    
    WiFi.mode(WIFI_STA);
    if (connectWifi("", "") == WL_CONNECTED)
    {
		DEBUG_WM(F("SSID:"));
		DEBUG_WM(WiFi.SSID());
        DEBUG_WM(F("IP Address:"));
        DEBUG_WM(WiFi.localIP());
        return true;
    }

    return startConfigPortal(apName, apPassword);
}


boolean  ESPWiFiManager::startConfigPortal(char const *apName, char const *apPassword)
{
    WiFi.mode(WIFI_AP);
    DEBUG_WM("SET AP");

    _apName = apName;
    _apPassword = apPassword;

    if ( _apcallback != NULL)
    {
        _apcallback(this);
    }

    connect = false;
    setupConfigPortal();
    
    while (_configPortalTimeout == 0 || millis() < _configPortalStart + _configPortalTimeout)
    {
        dnsServer->processNextRequest();
        server->handleClient();
        
        if (connect)
        {
            connect = false;
            delay(2000);
            DEBUG_WM(F("Connecting to new AP"));

            if (connectWifi(_ssid, _pass) != WL_CONNECTED)
            {
                DEBUG_WM(F("Failed to connect."));
            }
           
            else
            {
                WiFi.mode(WIFI_STA);
                
				DEBUG_WM(F("SSID:"));
				DEBUG_WM(WiFi.SSID());
				DEBUG_WM(F("IP Address:"));
				DEBUG_WM(WiFi.localIP());
				
                if ( _savecallback != NULL)
                {
                    _savecallback();
                }
                break;
            }

            if (_shouldBreakAfterConfig)
            {
                if ( _savecallback != NULL)
                {
                    _savecallback();
                }
                break;
            }
        }
        
        yield();
    }

    server.reset();
    dnsServer.reset();
    return  WiFi.status() == WL_CONNECTED;
}


int ESPWiFiManager::connectWifi(String ssid, String pass)
{
    DEBUG_WM(F("Connecting as wifi client..."));
    
    if (ssid != "")
    {
        WiFi.begin(ssid.c_str(), pass.c_str());
    }

    else
    {
        if(WiFi.SSID())
        {
            DEBUG_WM("Using last saved values, should be faster");
            ETS_UART_INTR_DISABLE();
            wifi_station_disconnect();
            ETS_UART_INTR_ENABLE();

            WiFi.begin();
        }

        else
        {
            DEBUG_WM("No saved credentials");
        }
    }

    int connRes = waitForConnectResult();
    DEBUG_WM ("Connection result: ");
    DEBUG_WM ( connRes );
    
    return connRes;
}

uint8_t ESPWiFiManager::waitForConnectResult()
{
    if (_connectTimeout == 0)
    {
        return WiFi.waitForConnectResult();
    } 
    
    else
    {
        DEBUG_WM (F("Waiting for connection result with time out"));
        unsigned long start = millis();
        boolean keepConnecting = true;
        uint8_t status;
        while (keepConnecting)
        {
            status = WiFi.status();
            
            if (millis() > start + _connectTimeout)
            {
                keepConnecting = false;
                DEBUG_WM (F("Connection timed out"));
            }
            
            if (status == WL_CONNECTED || status == WL_CONNECT_FAILED)
            {
                keepConnecting = false;
            }
            delay(100);
        }
        
        return status;
    }
}



void ESPWiFiManager::handleWifiSave()
{
    DEBUG_WM(F("WiFi save"));
    
    _ssid = server->arg("s").c_str();
    _pass = server->arg("p").c_str();
	
	server->send(200, "text/html", "Saved WiFi credentials");
    DEBUG_WM(F("Sent wifi saved page"));	
    
    connect = true;
}

void ESPWiFiManager::handleReset()
{
    DEBUG_WM(F("Reset"));
    system_restore();
    delay(5000);
    
    DEBUG_WM(F("system restored default"));
    delay(5000);
    ESP.restart();
}


String ESPWiFiManager::getConfigPortalSSID()
{
    return _apName;
}



void ESPWiFiManager::setTimeout(unsigned long seconds)
{
    setConfigPortalTimeout(seconds);
}



void ESPWiFiManager::setConfigPortalTimeout(unsigned long seconds)
{
    _configPortalTimeout = seconds * 1000;
}



void ESPWiFiManager::setConnectTimeout(unsigned long seconds)
{
    _connectTimeout = seconds * 1000;
}

void ESPWiFiManager::setDebugOutput(boolean debug)
{
    _debug = debug;
}


void ESPWiFiManager::setBreakAfterConfig(boolean shouldBreak)
{
   _shouldBreakAfterConfig = shouldBreak;
}

void ESPWiFiManager::setAPCallback( void (*func)(ESPWiFiManager* myWiFiManager) )
{
    _apcallback = func;
}


void ESPWiFiManager::setSaveConfigCallback( void (*func)(void) )
{
    _savecallback = func;
}


template <typename Generic>
void ESPWiFiManager::DEBUG_WM(Generic text)
{
    if (_debug)
    {
        Serial.print("*WM: ");
        Serial.println(text);
    }
}

int ESPWiFiManager::getRSSIasQuality(int RSSI)
{
    int quality = 0;

    if (RSSI <= -100)
    {
        quality = 0;
    } 
    
    else if (RSSI >= -50)
    {
        quality = 100;
    } 
    
    else
    {
        quality = 2 * (RSSI + 100);
    }
    
    return quality;
}


boolean ESPWiFiManager::isIp(String str)
{
    for (int i = 0; i < str.length(); i++)
    {
        int c = str.charAt(i);
        if (c != '.' && (c < '0' || c > '9'))
        {
            return false;
        }
    }
    
    return true;
}


String ESPWiFiManager::toStringIp(IPAddress ip)
{
    String res = "";
    
    for (int i = 0; i < 3; i++)
    {
        res += String((ip >> (8 * i)) & 0xFF) + ".";
    }
    
    res += String(((ip >> 8 * 3)) & 0xFF);
    
    return res;
}