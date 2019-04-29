#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include<EEPROM.h>
#include <FS.h> 

ESP8266WebServer server(80);


//password counter table starting from 0 to 29
int LoginPasswordCounter_storing_addr=0; //it contains 1 bit 0--0
int WifiSSIDCounter_storing_addr=1; //it contins 2 bit 1--2
int WifiPasswordCounter_storing_addr=3; // it contains 2 bit 3--5


//sub counter start from 21 to 29
int WifiSSIDCounter_Length_storing_addr=21; // it contains 1bit 21--21
int WifiPasswordCounter_Length_storing_addr=22; //it contains 1bit 22--22


//password storing table start from 30
int loginPassword_starting_addr=30; //stop adr 39
int wifiSSID_strting_addr=40; //stop addr 59
int wifiPassword_starting_addr=60; //stop addr 79


//start eeprom
void eepromStart(){
  EEPROM.begin(512);
  delay(300);
  Serial.println();
  Serial.println();
  Serial.println("Startup");
}

//EEPROM Write function
void eepromWrite(String x, int i){//x=which you want to store and i=starting address
  int z=i+x.length();
  int p=0;
  for(i; i<z; i++){
    EEPROM.write(i, x[p]);
    Serial.print("Writing at:");
    Serial.print(i);
    p += 1;
  }
  Serial.print("Memory Written Successfully!");
  EEPROM.commit();
  EEPROM.end();
}

String eepromRead(int i, int y){//i=start y=end address
  int z=i+y;
  String phrase;
  for(i; i<z; i++){
     phrase += char(EEPROM.read(i));
     Serial.print("Reading");
    Serial.print(i);
  }
  return phrase;
  EEPROM.end();
}

//end eeprom file
bool is_authentified() {
  Serial.println("Enter is_authentified");
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    if (cookie.indexOf("ESPSESSIONID=1") != -1) {
      Serial.println("Authentification Successful");
      return true;
    }
  }
  Serial.println("Authentification Failed");
  return false;
}

//login page, also called for disconnect
void handleLogin() {
  String msg;
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
  }
  if (server.hasArg("LOGOUT")) {
    Serial.println("Log Out!");
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Set-Cookie", "ESPSESSIONID=0");
    server.send(301);
    return;
  }
  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")) {
    eepromStart();
    String loginString=eepromRead(LoginPasswordCounter_storing_addr,LoginPasswordCounter_storing_addr+1); //
    int loginCounter = atoi(loginString.c_str());

    eepromStart();
    String loginPassword = eepromRead(loginPassword_starting_addr,loginCounter);
    
    if (server.arg("USERNAME") == "admin" &&  server.arg("PASSWORD") == loginPassword) {
      server.sendHeader("Location", "/");
      server.sendHeader("Cache-Control", "no-cache");
      server.sendHeader("Set-Cookie", "ESPSESSIONID=1");
      server.send(301);
      Serial.println("Log in Successful");
      return;
    }
    msg = "Wrong username/password! try again.";
    Serial.println("Log in Failed");
  }
  String content = "<html><body><form action='/login' method='POST'>To log in, please use : admin/admin<br>";
  content += "User:<input type='text' name='USERNAME' placeholder='user name' required><br>";
  content += "Password:<input type='password' name='PASSWORD' placeholder='password' required><br>";
  content += "<input type='submit' name='SUBMIT' value='Submit'></form>" + msg + "<br>";
  content += "You also can go <a href='/inline'>here</a></body></html>";
  server.send(200, "text/html", content);
}

//root page can be accessed only if authentification is ok

//all okk till this
void handleRoot() {
  Serial.println("Enter handleRoot");
  String header;
  if (!is_authentified()) {
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
  }
  
  if (server.hasArg("NEWLPASSWORD"))
  {
    String lpass = server.arg("NEWLPASSWORD");
    eepromStart();
    eepromWrite(lpass,loginPassword_starting_addr); // 10 is the starting address of new login password
    
    //Storing loginPasswordCounter
    String LoginPasswordCounter = String(lpass.length());
    eepromStart();
    eepromWrite(LoginPasswordCounter,LoginPasswordCounter_storing_addr); // 0 is the address for stroeing login password counter
    
    Serial.print("new Login password\n");
    Serial.print(lpass);
    Serial.print(lpass.length());
    String content = "<html><body><H1>Your Login Password hasbeen changed!</H2><br>";
    content += "<a href=\"/\"><H3>HOME</H3></a></body></html>";
    content += "<a href=\"/login?LOGOUT=YES\">Log Out</a></body></html>";
    server.send(200, "text/html", content);
  }

  
  if (server.hasArg("NEWWSSID"))
  {
    String wssid = server.arg("NEWWSSID");
    eepromStart();
    eepromWrite(wssid,wifiSSID_strting_addr);
    
    //Storing WifiSSIDCounter
    
    String WifiSSIDCounter = String(wssid.length());
    eepromStart();
    eepromWrite(WifiSSIDCounter,WifiSSIDCounter_storing_addr);

    String WifiSSIDCounter2 = String(WifiSSIDCounter.length());
    eepromStart();
    eepromWrite(WifiSSIDCounter2,WifiSSIDCounter_Length_storing_addr);
    
    Serial.print("New WIFI SSID\n");
    Serial.print(wssid);
    Serial.print(wssid.length());
    String content = "<html><body><H1>Your WIFI SSID hasbeen changed!</H2><br>";
    content += "<a href=\"/\"><H3>HOME</H3></a></body></html>";
    
    content += "<a href=\"/login?LOGOUT=YES\">Log Out</a></body></html>";
    
    server.send(200, "text/html", content);
    delay(2000);
    ESP.reset();
  }
  if (server.hasArg("NEWWPASSWORD"))
  {
    String wpass = server.arg("NEWWPASSWORD");
    eepromStart();
    eepromWrite(wpass,wifiPassword_starting_addr);

    //Stroring WifiPasswordCounter

    String WifiPasswordCounter = String(wpass.length());
    eepromStart();
    eepromWrite(WifiPasswordCounter,WifiPasswordCounter_storing_addr);
    
    String WifiPasswordCounter2 = String(WifiPasswordCounter.length());
    eepromStart();
    eepromWrite(WifiPasswordCounter2,WifiPasswordCounter_Length_storing_addr);
    
    
    Serial.print("new WIFI password\n");
    Serial.print(wpass);
    String content = "<html><body><H1>Your WIFI Password hasbeen changed</H2><br>";
    content += "<a href=\"/\"><H3>HOMEt</H3></a></body></html>";
    
    content += "<a href=\"/login?LOGOUT=YES\">Log Out</a></body></html>";
    
    server.send(200, "text/html", content);
    delay(2000);
    ESP.reset();
  }
  if (server.hasArg("SHOWLP"))
  {
    eepromStart();
    String loginString=eepromRead(LoginPasswordCounter_storing_addr,LoginPasswordCounter_storing_addr+1);
    int loginCounter = atoi(loginString.c_str());

    eepromStart();
    String readlpass = eepromRead(loginPassword_starting_addr,loginCounter);
    Serial.print("Showing Login password\n");
    Serial.print(readlpass);
    String content = "<html><body><H1>Your LOGIN Password is:"+readlpass+"</H2><br>";
    content += "<a href=\"/\"><H3>HOME</H3></a></body></html>";
    content += "<a href=\"/login?LOGOUT=YES\">Log Out</a></body></html>";
    server.send(200, "text/html", content);
  }
  if (server.hasArg("SHOWWSSID"))
  {
    eepromStart();
    String ssidString1=eepromRead(WifiPasswordCounter_Length_storing_addr,WifiPasswordCounter_Length_storing_addr+1);
    int ssidCounter1 = atoi(ssidString1.c_str());

    eepromStart();
    String ssidString2=eepromRead(WifiSSIDCounter_storing_addr,ssidCounter1);
    int ssidCounter2 = atoi(ssidString2.c_str());

    eepromStart();
    String readwssid = eepromRead(wifiSSID_strting_addr,ssidCounter2);
    Serial.print("Showing WIFI SSID\n");
    Serial.print(readwssid);
    String content = "<html><body><H1>Your SSID is: "+readwssid+"</H2><br>";
    content += "<a href=\"/\"><H3>HOME</H3></a></body></html>";
    content += "<a href=\"/login?LOGOUT=YES\">Log Out</a></body></html>";
    server.send(200, "text/html", content);
  }
  if (server.hasArg("SHOWWP"))
  {
    eepromStart();
    String wpassString1=eepromRead(WifiPasswordCounter_Length_storing_addr,WifiPasswordCounter_Length_storing_addr+1);
    int wpassCounter1 = atoi(wpassString1.c_str());

    eepromStart();
    String wpassString2=eepromRead(WifiPasswordCounter_storing_addr,wpassCounter1);
    int wpassCounter2 = atoi(wpassString2.c_str());
    
    eepromStart();
    String readwpass = eepromRead(wifiPassword_starting_addr,wpassCounter2);
    Serial.print("Showing Login password\n");
    Serial.print(readwpass);
    String content = "<html><body><H1>Your WIFI Password is:"+readwpass+"</H2><br>";
    content += "<a href=\"/\"><H3>HOME</H3></a></body></html>";
    content += "<a href=\"/login?LOGOUT=YES\">Log Out</a></body></html>";
    server.send(200, "text/html", content);
  }
   //Serial.println("Password submitted");
   
  if (server.hasArg("CHANGELP")) {
    Serial.println("Login Password Changeing Tab");
    String content = "<html><body><H1>Change LOGIN Password!</H2><form action='/' method='POST'><br>";
    content += "<p>Password must contain 5 or more but less than 9 Character</p><br>";
    content += "Enter New password:<input type='txt' name='NEWLPASSWORD' pattern='.{5,9}' title='Password must contain 5 or more characters' 'placeholder='enter new password'><br>";
    //content += "Confirm New password:<input type='txt' name='CNEWLPASSWORD' placeholder='Confirm new password'><br>";
    //content += "<input type='submit' name='SUBMIT' value='Change Login Password'><br>";
    content += "<input type='submit' name='SUBMIT' value='Submit'></form><br>";
    
    content += "<a href=\"/\"><H3>HOME</H3></a></body></html>";
    
    content += "Click here to  <a href=\"/login?LOGOUT=YES\">Log Out</a></body></html>";
    
    server.send(200, "text/html", content);
  }
  if (server.hasArg("CHANGEWSSID")) {
    Serial.println("SSID Changeing Tab");
    String content = "<html><body><H1>Change WIFI SSID!</H2><form action='/' method='POST'><br>";
    content += "<p>SSID Must be 3 or more but less than 20 character</p><br>";
    content += "Enter New SSID:<input type='txt' name='NEWWSSID' pattern='.{3,20}' title='SSID Must contain more than 3 characters' placeholder='enter new ssid'><br>";
    //content += "<input type='submit' name='SUBMIT' value='Change Login Password'><br>";
    content += "<input type='submit' name='SUBMIT' value='Submit'></form><br>";
    content += "<a href=\"/\"><H3>HOME</H3></a></body></html>";
    
    content += "Click here to  <a href=\"/login?LOGOUT=YES\">Log Out</a></body></html>";
    
    server.send(200, "text/html", content);
  }
  if (server.hasArg("CHANGEWP")) {
    Serial.println("WIFI Password Changeing Tab");
    String content = "<html><body><H1>Change WIFI Password!</H2><form action='/' method='POST'><br>";
    content += "<p>Password Must contain 8 or more but less than 20 Character</p><br>";
    content += "Enter New password:<input type='txt' name='NEWWPASSWORD' pattern='.{8,20}' title='Password Must contain 8 or more characters' placeholder='enter new password'><br>";
    //content += "<input type='submit' name='SUBMIT' value='Change Login Password'><br>";
    content += "<input type='submit' name='SUBMIT' value='Submit'></form><br>";
    content += "<a href=\"/\"><H3>HOME</H3></a></body></html>";
    content += "Click here to  <a href=\"/login?LOGOUT=YES\">Log Out</a></body></html>";
    server.send(200, "text/html", content);
  }

  String content = "<html><body><H1>Welcome!</H2><form action='/?CHANGELP=YES' method='POST'><br>";
  content += "<input type='submit' name='SUBMIT' value='Change Login Password'></form><br>";
  
  content += "<form action='/?CHANGEWSSID=YES' method='POST'><br>";
  content += "<input type='submit' name='SUBMIT' value='Change Wifi SSID'></form><br>";
  
  content += "<form action='/?CHANGEWP=YES' method='POST'><br>";
  content += "<input type='submit' name='SUBMIT' value='Change WIFI Password '></form><br>";
  
  content += "<form action='/?SHOWWSSID=YES' method='POST'><br>";
  content += "<input type='submit' name='SUBMIT' value='Show Wifi SSID '></form><br>";

  content += "<form action='/?SHOWWP=YES' method='POST'><br>";
  content += "<input type='submit' name='SUBMIT' value='Show WIFI Password '></form><br>";
  
  content += "<form action='/?SHOWLP=YES' method='POST'><br>";
  content += "<input type='submit' name='SUBMIT' value='Show Login Password '></form><br>";

  content += "Click here to  <a href=\"/login?LOGOUT=YES\"><H3>Log Out</H3></a></body></html>";
  server.send(200, "text/html", content);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {
  Serial.begin(9600);
  Serial.println("");
  delay(500);
  int a=0;
  while (a<10)
  {
   if (Serial.available()>0){
   switch(Serial.read())
  {
    case 'r':
      String lpass = "admin";
      eepromStart();
      eepromWrite(lpass,loginPassword_starting_addr);
      
      //Storing loginPasswordCounter
      String LoginPasswordCounter = String(lpass.length());
      eepromStart();
      eepromWrite(LoginPasswordCounter,LoginPasswordCounter_storing_addr);


      String wssid = "ESPDebug";
      eepromStart();
      eepromWrite(wssid,wifiSSID_strting_addr);
      
      //Storing WifiSSIDCounter
      
      String WifiSSIDCounter = String(wssid.length());
      eepromStart();
      eepromWrite(WifiSSIDCounter,WifiSSIDCounter_storing_addr);
  
      String WifiSSIDCounter2 = String(WifiSSIDCounter.length());
      eepromStart();
      eepromWrite(WifiSSIDCounter2,WifiSSIDCounter_Length_storing_addr);

      String wpass = "espdebugpassword";
      eepromStart();
      eepromWrite(wpass,wifiPassword_starting_addr);
  
      //Stroring WifiPasswordCounter
  
      String WifiPasswordCounter = String(wpass.length());
      eepromStart();
      eepromWrite(WifiPasswordCounter,WifiPasswordCounter_storing_addr);
      
      String WifiPasswordCounter2 = String(WifiPasswordCounter.length());
      eepromStart();
      eepromWrite(WifiPasswordCounter2,WifiPasswordCounter_Length_storing_addr);
      a=12;
      break;
      }
  }
  delay(600);
  a++; 
  }

 
  eepromStart();
  String ssidString1=eepromRead(WifiSSIDCounter_Length_storing_addr,WifiSSIDCounter_Length_storing_addr+1);
  int ssidCounter1 = atoi(ssidString1.c_str());

  eepromStart();
  String ssidString2=eepromRead(WifiSSIDCounter_storing_addr,ssidCounter1);
  int ssidCounter2 = atoi(ssidString2.c_str());

  eepromStart();
  String ssid = eepromRead(wifiSSID_strting_addr,ssidCounter2);

  eepromStart();
  String wpassString1=eepromRead(WifiPasswordCounter_Length_storing_addr,WifiPasswordCounter_Length_storing_addr+1);
  int wpassCounter1 = atoi(wpassString1.c_str());

  eepromStart();
  String wpassString2=eepromRead(WifiPasswordCounter_storing_addr,wpassCounter1);
  int wpassCounter2 = atoi(wpassString2.c_str());
    
  eepromStart();
  String password = eepromRead(wifiPassword_starting_addr,wpassCounter2);
  
  
  WiFi.mode(WIFI_AP);
  //Only Access point
  WiFi.softAP(ssid.c_str(), password.c_str());  //Start HOTspot removing password will disable security
 
  IPAddress myIP = WiFi.softAPIP(); //Get IP address
  Serial.print("HotSpt IP:");
  Serial.println(myIP);



  server.on("/", handleRoot);
  server.on("/login", handleLogin);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works without need of authentification");
  });

  server.onNotFound(handleNotFound);
  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent", "Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  if (Serial.available()>0){
   switch(Serial.read())
  {
    case 'r':
      String lpass = "admin";
      eepromStart();
      eepromWrite(lpass,loginPassword_starting_addr);
      
      //Storing loginPasswordCounter
      String LoginPasswordCounter = String(lpass.length());
      eepromStart();
      eepromWrite(LoginPasswordCounter,LoginPasswordCounter_storing_addr);


      String wssid = "ESPDebug";
      eepromStart();
      eepromWrite(wssid,wifiSSID_strting_addr);
      
      //Storing WifiSSIDCounter
      
      String WifiSSIDCounter = String(wssid.length());
      eepromStart();
      eepromWrite(WifiSSIDCounter,WifiSSIDCounter_storing_addr);
  
      String WifiSSIDCounter2 = String(WifiSSIDCounter.length());
      eepromStart();
      eepromWrite(WifiSSIDCounter2,WifiSSIDCounter_Length_storing_addr);

      String wpass = "espdebugpassword";
      eepromStart();
      eepromWrite(wpass,wifiPassword_starting_addr);
  
      //Stroring WifiPasswordCounter
  
      String WifiPasswordCounter = String(wpass.length());
      eepromStart();
      eepromWrite(WifiPasswordCounter,WifiPasswordCounter_storing_addr);
      
      String WifiPasswordCounter2 = String(WifiPasswordCounter.length());
      eepromStart();
      eepromWrite(WifiPasswordCounter2,WifiPasswordCounter_Length_storing_addr);
      break;
      }
  }

      
  server.handleClient();
}
