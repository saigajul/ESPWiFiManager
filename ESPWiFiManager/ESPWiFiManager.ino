/***********************************************************************************************************/
// To save credential in AP mode        http://192.168.4.1/wifisave?s=********&p=********/
/***********************************************************************************************************/
// To reset credential in AP mode       http://192.168.4.1/r
/***********************************************************************************************************/
// To reset credential in station mode  http://xxx.xxx.xxx.xxx/your-text
/***********************************************************************************************************/

#include <ESPWiFiManager.h>

ESP8266WebServer server(80);
ESPWiFiManager MyESP;

void resthandle()
{
    server.send(200, "text/plain", "Restoring to default");
    MyESP.handleReset();
}

void setup()
{
    Serial.begin(115200);    
    MyESP.autoConnect("ESPWiFiManager");
    server.on("/reset",resthandle);
    server.begin();
}

void loop()
{
    server.handleClient();
    delay(1);
} 
