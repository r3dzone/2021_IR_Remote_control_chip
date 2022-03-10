#define BLYNK_TEMPLATE_ID ""
#define BLYNK_DEVICE_NAME ""
#define BLYNK_AUTH_TOKEN ""
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

const uint16_t kIrLed = 5;  // 5 is Wemos D1's d1 pin
IRsend irsend(kIrLed);  //set pin for IR
String ssid;
int32_t rssi;
uint8_t encryptionType;
uint8_t* MAC;
int32_t channel;
bool hidden;
int scanResult;

String open_ssid;
int32_t open_rssi;

bool net_flag = false;
char auth[] = BLYNK_AUTH_TOKEN;
char ossid[100];
char pass[] = "";

void find_open_wifi(){
  open_rssi = -100;
  WiFi.disconnect();
  delay(100);
  scanResult = WiFi.scanNetworks(/*async=*/false, /*hidden=*/true);
  Serial.printf(PSTR("%d networks found:\n"), scanResult);
  for (int8_t i = 0; i < scanResult; i++) {
    WiFi.getNetworkInfo(i, ssid, encryptionType, rssi,MAC, channel, hidden);  
    if(encryptionType == ENC_TYPE_NONE && open_rssi < rssi){
      open_ssid = ssid;
      Serial.printf("%s\n",open_ssid.c_str());  
    }
    delay(0);
  }
  if(open_ssid != ""){
    net_flag = true;
    printf("connect to %s\n",open_ssid.c_str());
    open_ssid.toCharArray(ossid,open_ssid.length()+1);
    printf("ossid is %s\n",ossid);
    Blynk.begin(auth,ossid, pass);
    irsend.begin();
  }
}

BLYNK_WRITE(V0)//power
{
  irsend.sendNEC(0x20DF10EF, 32);
  delay(100); 
}

BLYNK_WRITE(V1)//up
{
  irsend.sendNEC(0x20DF02FD, 32);
  delay(100); 
}

BLYNK_WRITE(V2)//down
{
  irsend.sendNEC(0x20DF827D, 32);
  delay(100); 
}

BLYNK_WRITE(V3)//left
{
  irsend.sendNEC(0x20DFE01F, 32);
  delay(100); 
}

BLYNK_WRITE(V4)//right
{
  irsend.sendNEC(0x20DF609F, 32);
  delay(100); 
}

BLYNK_WRITE(V5)//ok
{
  irsend.sendNEC(0x20DF22DD, 32);
  delay(100); 
}

BLYNK_WRITE(V6)//menu
{
  irsend.sendNEC(0x20DF3EC1, 32);
  delay(100); 
}

BLYNK_WRITE(V7)//back
{
  irsend.sendNEC(0x20DF14EB, 32);
  delay(100); 
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  Serial.begin(115200, SERIAL_8N1);
  open_rssi = -100;
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  find_open_wifi();
  delay(100);
}

void loop()
{
  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on
  if(net_flag){
    Blynk.run();
  }else{
    find_open_wifi();
    delay(10000);
  }
}