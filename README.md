# Controlling-Node-MCU-ESP8266-Over-Local-host
This is the project on Controlling the Node MCU ESP8266 over the local host.

=====================================================================

Follow the steps to control the Node MCU Over Localhost

Step1: Upload the .ino code to the Node MCU ESP8266.

Step2: Open Serial Monitor within 5 Second after uploading code and enter “r” and sent it.
 
After sending “r” to the Node MCU ESP8266 it writes default value to the EEPROM.

The defaults values:

				SSID: ESPDebug
				Password: espdebugpassword
				Login Username: admin
				Login password: admin

** When writing is complete to the EEPROM of the Node MCU Esp8266 it automatically restarted. (If it does not restart automatically then try to restart manually).


** After restarting it creates an access point for the client. 

Step4: The client can access the NODE MCU ESP8266 with the valid SSID and Password.

				SSID: ESPDebug
				Password: espdebugpassword

Step5: After connecting to the Node MCU ESP8266, the client can go to the browser and type default gateway address of the Node-MCU on the address bar, after pressing "Enter" the login page of the Node MCU ESP8266 appear on the screen.

•	In this case default IP address is:
					192.168.4.1


Step6: After entering the default Username and password the client can access the internal section of the Node MCU ESP8266. 

				Login Username: admin
				Login password: admin

Step7: You have done!!
									



 Features of Controlling ESP8266
 
1.	You can change Login Username and Login Password Using EEPROM.
2.	You can change SSID and Password Using EEPROM.


* This is the First Version of the code. In the upcoming version you can do more features will be added.

