# ESPWiFiManager
Connecting ESP to any network
##	How It Works

	-	ESP set's into station mode and try to connect previously
		connected network.
		
	-	If successful it falls back, to where it was called and 
		stays in station mode connected to an known Access Point.
		
	-	If fail to connect last known Access Point, ESP config's
		into AP mode and starts DNS and WebServer with Default
		IP : 192.168.4.1 .
		
	-	By using smart phone connect newly created AP and config
		ESP with your SSID and PASSWORD as per App instructions
		
	-	ESP will try to connect AP using newly provided SSID and 
		PASSWORD.
		
	-	If successful it falls back to where it was called and 
		stays in station mode connected to an known Access Point
		
	-	If not reconnect to AP and reconfigure
		
	-	ESPWiFiManager provides restore to default option in both
		AP and Station mode.
		
	-	In AP mode, ESP automatically resolves restore option.
		
	-	In station mode do the following:
		#	In your program create object of:
		
			>	ESP8266WebServer server_object(80);
			>	ESPWiFiManager ESP_oblect;
			
		#	create the function as below:
		
			void Your_reset _function_name()
			{
				server_object.send(200, "text/plain", "Your msg to client");
				ESP_oblect.handleReset();
				/*>>>	handleReset() is function for restore default	<<<*/
			}
			
		#	Set lookup for specific response and start server as following:
			
			server_object.on("/your-text",Your_reset _function_name);
			server_object.begin();
			
				/*>>>	http://xxx.xxx.xxx.xxx/your-text				<<<*/
			
##	For more details check the example
