#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "html.h"

#define STASSID "CMCC-dii4"
#define STAPSK  "ujeqgrfn"
#define AP_SSID "POCSAG" //这里改成你的AP名字
#define AP_PSW  "" //这里改成你的AP密码 8位以上

//#define DEBUG 1

String ssid     = STASSID;
String password = STAPSK;
String comdata = "";

const char* host = "esp8266";

IPAddress local_IP(192,168,4,1);  //AP模式下IP地址
IPAddress gateway(192,168,4,1);   //AP模式默认网关
IPAddress subnet(255,255,255,0);  //AP模式子网掩码

ESP8266WebServer server(80);

void setup(void) {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  WifiAP_ON();
  //WifiSTA_ON();
  MDNS.begin(host);
}

void loop(void) {
  server.handleClient();
  receiveString();
  MDNS.update();
}

void wifi_page_server() {
  if (server.method() != HTTP_POST) {
    server.send(200, "text/html", wifi_page);
  }
  else {
#ifdef DEBUG
    String message = "";
    for(int i = 0; i < server.args(); i++)
    {
       message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    Serial.println(message);
#endif
    String r_ssid = server.arg(0);
    String r_psk = server.arg(1);
    WifiSTA_OFF();
    ssid=r_ssid;
    Serial.print("#WIFI+SAVESSID=" + r_ssid);
    delay(50);
    Serial.print("#WIFI+SAVESSID=" + r_ssid);
    delay(500);
    password = r_psk;
    Serial.print("#WIFI+SAVEPSK=" + r_psk);
    delay(500);
    WifiSTA_ON();
  }
  yield();
}

void index_page_server() {
  if (server.method() != HTTP_POST) {
    server.send(200, "text/html", index_page);
  }
  else {
#ifdef DEBUG
    String message = "";
    for(int i = 0; i < server.args(); i++)
    {
       message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    Serial.println(message);
#endif
    String r_phase = server.arg(4);
    String r_addr = server.arg(2);
    String r_rate = server.arg(5);
    String r_type = server.arg(6);
    String r_beep = server.arg(7);
    String r_message = server.arg(0);
    String r_freq = server.arg(3);
    
    SerialSetFreq(r_freq);
    SerialSetFreq(r_freq);
    SerialSendTxCmd(r_phase,r_addr,r_beep,r_rate,r_type,r_message);
  }
  yield();
}

void group_page_server() {
  if (server.method() != HTTP_POST) {
    server.send(200, "text/html", groupcall_page);
  }
  else {
#ifdef DEBUG
//    String message = "";
//    for(i = 0; i < server.args(); i++)
//    {
//       message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
//      }
//    Serial.println(message);
#endif
    int i;
    char selnum[21] = "00000000000000000000";
    String group_num = "";
    String num = "";
    String group = "";
    String r_herd_pre = "N";
    String r_group_pre = "N";
    int group_flag = 0;
    String r_message;
    for(i = 0; i < server.args(); i++){
      if(server.argName(i) == "r_group"){
        group_flag = 1;
        group_num = server.arg(i);
      }
      if(server.argName(i) == "r_group_pre"){
        r_group_pre = "Y";
      }
      if(server.argName(i) == "r_herd_pre"){
        r_herd_pre = "Y";
      }
      if(server.argName(i) == "r_message"){
        r_message = server.arg(i);
      }
      if(server.argName(i) == "r_num")
      {
        num = server.arg(i);
        selnum[num.toInt() - 1] = '1';
      }
    }
    if(group_flag == 1)
      Serial.println("#GROUP:" + r_group_pre + group_num + "=" + r_message);
    else{
      Serial.println("#HERD:" + r_herd_pre + String(selnum) + "=" + r_message);
    }
  }
  yield();
}

void UpdateProcess(){
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    Serial.setDebugOutput(true);
    WiFiUDP::stopAll();
    Serial.printf("Update: %s\n", upload.filename.c_str());
    uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
    if (!Update.begin(maxSketchSpace)) { //start with max available size
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (Update.end(true)) { //true to set the size to the current progress
      Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
    } else {
      Update.printError(Serial);
    }
    Serial.setDebugOutput(false);
  }
  yield();
}

void WifiSTA_ON(){
  int cnt = 0;
  digitalWrite(LED_BUILTIN, LOW);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED && cnt <= 100) {
    delay(100);
    cnt++;
  }
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("#WIFI+FAILE");
    return;
  }
  else{
    Serial.println("#WIFI+OK");
#ifdef DEBUG
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
#endif
    Wifi_SHOWIP();
    Wifi_SHOWRSSI();
    if (MDNS.begin("esp8266")) {
#ifdef DEBUG
      Serial.println("MDNS responder started");
#endif
    }
//    server.on("/", index_page_server);
//    server.on("/group_call", group_page_server);
//    server.on("/wifi", wifi_page_server);
//    server.on("/webupdate", HTTP_GET, []() {
//      server.sendHeader("Connection", "close");
//      server.send(200, "text/html", update_page);
//    });
//    server.on("/update", HTTP_POST, []() {
//      server.sendHeader("Connection", "close");
//      server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
//      ESP.restart();
//    }, UpdateProcess);
//    server.begin();
//    Serial.println("ESP8266 HTTP OK");
    yield();
  }
}

void WifiSTA_OFF(){
  digitalWrite(LED_BUILTIN, HIGH);
  WiFi.disconnect();
//  Serial.println("Wifi disconnected");
  Serial.println("#WIFI+FAILE");
}

void WifiAP_ON(){
  WiFi.mode(WIFI_AP);
#ifdef DEBUG
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
  //启动AP模式，并设置账号和密码
  Serial.println("Setting soft-AP ... ");
#endif
  boolean result = WiFi.softAP(AP_SSID, AP_PSW);
#ifdef DEBUG
  if(result){
    Serial.println("Ready");
    //输出 soft-ap ip地址
    Serial.println(WiFi.softAPIP());
    //输出 soft-ap mac地址
    Serial.println(String("MAC address = ") + WiFi.softAPmacAddress().c_str());
  }else{
    Serial.println("Failed!");
  }
#endif
  if (MDNS.begin("esp8266")) {
#ifdef DEBUG
    Serial.println("MDNS responder started");
#endif
  }
  server.on("/", index_page_server);
  server.on("/group_call", group_page_server);
  server.on("/wifi", wifi_page_server);
  server.on("/webupdate", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", update_page);
  });
  server.on("/update", HTTP_POST, []() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
      ESP.restart();
    }, UpdateProcess);
  server.begin();
//  Serial.println("ESP8266 AP OK");
  MDNS.addService("http", "tcp", 80);
  yield();
}

void WifiAP_OFF(){
  server.stop();
  WiFi.disconnect();
//  Serial.println("Wifi disconnected");
  Serial.println("#WIFI+FAILE");
}

int Wifi_SHOWRSSI(){
  int rssi= WiFi.RSSI();
  if(rssi <= 0 && rssi > -50){
    Serial.println("#WIFI+SHOWRSSI=4");
    return 4;
  }
  else if(rssi <= -50 && rssi > -70){
    Serial.println("#WIFI+SHOWRSSI=3");
    return 3;
  }
  else if(rssi <= -70 && rssi > -80){
    Serial.println("#WIFI+SHOWRSSI=2");
    return 2;
  }
  else if(rssi <= -80 && rssi > -100){
    Serial.println("#WIFI+SHOWRSSI=1");
    return 1;
  }
  else{
    Serial.println("#WIFI+SHOWRSSI=0");
    return 0;
  }
}

void Wifi_SHOWIP(){
  IPAddress ip = WiFi.localIP();
  String ipstr = String("#WIFI+SHOWIP=" + String(ip[0]) + '.' +  String(ip[1]) + '.' +   String(ip[2]) + '.' +   String(ip[3]));
  Serial.println(ipstr);
}

void SerialSetFreq(String r_freq){
  Serial.print("#SET+FREQ=" + r_freq);
  delay(200);
}

void SerialSendTxCmd(String r_phase, String r_addr, String r_beep, String r_rate, String r_type, String r_message){
  String cmd = "#" + r_phase + r_addr + r_beep + r_rate + r_type + r_message;
  //delay(2);
  Serial.print(cmd);
  delay(200);
}

void receiveString(){
  while (Serial.available() > 0){
        comdata += char(Serial.read());  //每次读一个char字符，并相加
        delay(2);
    }
    if (comdata.length() > 0){
        //Serial.println(comdata); //打印接收到的字符
        if(comdata.startsWith("#WIFI+STA=ON")){
          WifiSTA_ON();
        }
        else if(comdata.startsWith("#WIFI+STA=OFF")){
          WifiSTA_OFF();
        }
        else if(comdata.startsWith("#WIFI+SET+SSID=")){
          ssid=comdata.substring(15);
          //Serial.println(ssid);
        }
        else if(comdata.startsWith("#WIFI+SET+PSK=")){
          password=comdata.substring(14);
          //Serial.println(password);
        }
        else if(comdata.startsWith("#WIFI+GETIP")){
          Wifi_SHOWIP();
        }
        else if(comdata.startsWith("#WIFI+GETRSSI")){
          Wifi_SHOWRSSI();
        }
        comdata="";
        
    }
    yield();
}

