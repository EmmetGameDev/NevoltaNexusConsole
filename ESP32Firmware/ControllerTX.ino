#include <esp_now.h>
#include <WiFi.h>
#include "esp_wifi.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET 4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

struct button{
  int pin;
  int buttonLastState;
};

const int systemButton = 40;
int currentSystemMode = 0;
/*
System modes:
0 - controller mode
1 - system control mode
2 - controller mode, display off
*/
int systemButtonLastState = HIGH;

const unsigned long interval = 10000; // 10 seconds
unsigned long previousMillis = 0;
const int batPin = 4;

const unsigned long intervalCharge = 3000; // 3 seconds
unsigned long previousMillisCharge = 0;
const int chargePin = 5;
int lastChargeState = LOW;
bool isCharging = false;
bool wasCharging = false;

/* 
Buttons:
0. L
1. Up
2. Right
3. Down
4. Left
5. Start
6. Select
7. X
8. A
9. B
10. Y
11. R

Software-based:
15. VolD
16. VolU
*/
const int buttonCount = 12;
button buttonsList[buttonCount];

uint8_t broadcastAddress[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05}; // INSERT YOUR MAC ADRESS HERE!!!

typedef struct struct_message {
  char a[32];
  int b;
  float c;
  bool d;
} struct_message;
struct_message myData;
esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void readMacAddress(){
  uint8_t baseMac[6];
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
  if (ret == ESP_OK) {
    Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
                  baseMac[0], baseMac[1], baseMac[2],
                  baseMac[3], baseMac[4], baseMac[5]);
  } else {
    Serial.println("Failed to read MAC address");
  }
}

void setup(){
  buttonsList[0].pin = 10;
  buttonsList[1].pin = 18;
  buttonsList[2].pin = 13;
  buttonsList[3].pin = 11;
  buttonsList[4].pin = 12;
  buttonsList[5].pin = 17;
  buttonsList[6].pin = 7;
  buttonsList[7].pin = 38;
  buttonsList[8].pin = 2;
  buttonsList[9].pin = 6;
  buttonsList[10].pin = 14;
  buttonsList[11].pin = 1;
  for (int i = 0; i < buttonCount; i++) {
    pinMode(buttonsList[i].pin, INPUT_PULLUP);
    buttonsList[i].buttonLastState = HIGH;
  }
  pinMode(systemButton, INPUT_PULLUP);
  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(esp_now_send_cb_t(OnDataSent));
  
  // Add peer LAST
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;   // Let ESP choose channel
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Peer add FAIL");
    return;
  }

  Serial.println("TX READY");

  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(12, 15);
  String neuro = "Nevolta";
  for(int i = 0; i < neuro.length(); i++){
    display.print(neuro[i]);
    display.display();
    delay(150);
  }
  delay(1000);
}

void displayAnimation(){
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(12, 15);
  switch (currentSystemMode) {
    case 0:{
      String contr = "Controller";
      for(int i = 0; i < contr.length(); i++){
        display.print(contr[i]);
        display.display();
        delay(150);
      }
      break;
    }
    case 1:{
      String sett = "Settings";
      for(int i = 0; i < sett.length(); i++){
        display.print(sett[i]);
        display.display();
        delay(150);
      }
      break;
    }
    case 2:{
      String disp = "Display off";
      for(int i = 0; i < disp.length(); i++){
        display.print(disp[i]);
        display.display();
        delay(150);
      }
      delay(1000);
      display.clearDisplay();
      display.display();
      break;
    }
    default:
      break;
  }
}

void chargingAnim(){
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(12, 15);
  String neuro = "Charging";
  for(int i = 0; i < neuro.length(); i++){
    display.print(neuro[i]);
    display.display();
    delay(150);
  }
  neuro = "...";
  for (int j = 0; j < 3; j++) {
    for(int i = 0; i < neuro.length(); i++){
      display.print(neuro[i]);
      display.display();
      delay(150);
    }
  }
}

void loop() {
  for (int i = 0; i < buttonCount; i++) {
    int buttonState = digitalRead(buttonsList[i].pin);
    if(buttonState != buttonsList[i].buttonLastState){
      if(currentSystemMode != 1){
        myData.b = i;
        if (buttonState == LOW) {
          myData.d = true;
        }else{
          myData.d = false;
        }
        esp_err_t result = esp_now_send(NULL, (uint8_t*)&myData, sizeof(myData));
        Serial.print("Send result: ");
        Serial.println(result);
      }else{
        if(i == 1){ // BTN up
            myData.b = 15; // Free buttonID on RX
            Serial.println("Volume Down!");
          }
          if(i == 3){ // BTN down
            myData.b = 16; // Free buttonID on RX
            Serial.println("Volume Up!");
          }
        if (buttonState == LOW) {
          myData.d = true;
        }else{
          myData.d = false;
        }
        esp_err_t result = esp_now_send(NULL, (uint8_t*)&myData, sizeof(myData));
          Serial.print("Send result: ");
          Serial.println(result);
      }
    }
    buttonsList[i].buttonLastState = buttonState;
  }

  int systemButtonState = digitalRead(systemButton);
  if(systemButtonState != systemButtonLastState){
    if (systemButtonState == LOW) {
      currentSystemMode++;
      if(currentSystemMode > 2){
        currentSystemMode = 0;
      }
      displayAnimation();
    }
  }
  systemButtonLastState = systemButtonState;

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    Serial.println("Battery:");
    int batteryPercentage = (analogReadMilliVolts(batPin) * 0.002 - 3.7) / 0.5 * 100 + 0.5;
    if(isCharging == true){
      batteryPercentage -= 15;
    }
    Serial.print(batteryPercentage);
    Serial.print("% \n \n");
  }


  if (currentMillis - previousMillisCharge >= intervalCharge) {
    previousMillisCharge = currentMillis;
    int curChargeState = analogReadMilliVolts(chargePin);
    Serial.println(analogReadMilliVolts(chargePin));
    String phrase; 
    if(curChargeState > 2000){
      isCharging = true;
      phrase = "Yes";
    }else{
      isCharging = false;
      phrase = "No";
    }
    Serial.print("Charging: ");
    Serial.print(phrase);
    Serial.print("\n \n");
    if(isCharging == true && wasCharging == false){
      chargingAnim();
    }
    wasCharging = isCharging;
  }
  
  delay(50);
}