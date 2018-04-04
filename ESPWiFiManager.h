
#ifndef ESPWiFiManager_h
#define ESPWiFiManager_h

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <memory>

extern "C"
{
    #include "user_interface.h"
}


class ESPWiFiManager
{
    public:
    
      ESPWiFiManager();
	  
	  void          handleReset();
  
      boolean       autoConnect(char const *apName, char const *apPassword = NULL);
  
      boolean       startConfigPortal(char const *apName, char const *apPassword = NULL);
  
      String        getConfigPortalSSID();
  
  
      void          setConfigPortalTimeout(unsigned long seconds);
      
  	  void          setTimeout(unsigned long seconds);
  
      void          setConnectTimeout(unsigned long seconds);
  
  
      void          setDebugOutput(boolean debug);
  	
      void          setAPCallback( void (*func)(ESPWiFiManager*) );
      
      void          setSaveConfigCallback( void (*func)(void) );
      
      void          setBreakAfterConfig(boolean shouldBreak);
      
  
    private:
      
      std::unique_ptr<DNSServer>        dnsServer;
      std::unique_ptr<ESP8266WebServer> server;
  
      void          setupConfigPortal();
  
      const char*   _apName                 = "no-net";
      const char*   _apPassword             = NULL;
      String        _ssid                   = "";
      String        _pass                   = "";
      unsigned long _configPortalTimeout    = 0;
      unsigned long _connectTimeout         = 0;
      unsigned long _configPortalStart      = 0;
  
      
      boolean       _shouldBreakAfterConfig = false;
      
      int           status = WL_IDLE_STATUS;
      int           connectWifi(String ssid, String pass);
      uint8_t       waitForConnectResult();
  
      
      void          handleWifiSave();
  
      const byte    DNS_PORT = 53;
  
      int           getRSSIasQuality(int RSSI);
      boolean       isIp(String str);
      String        toStringIp(IPAddress ip);
  
      boolean       connect;
      boolean       _debug = true;
  
      void (*_apcallback)(ESPWiFiManager*) = NULL;
      void (*_savecallback)(void) = NULL;
  
      template <typename Generic>
      void          DEBUG_WM(Generic text);

};

#endif
