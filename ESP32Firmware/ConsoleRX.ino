#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

typedef struct struct_message {
  char a[32];
  int b;
  float c;
  bool d;
} struct_message;
struct_message myData;

int btn_L_Pin = 10;
int btn_Up_Pin = 18;
int btn_Right_Pin = 13;
int btn_Down_Pin = 11;
int btn_Left_Pin = 12;
int btn_Start_Pin = 17;
int btn_Select_Pin = 7;
int btn_X_Pin = 38;
int btn_A_Pin = 2;
int btn_B_Pin = 6;
int btn_Y_Pin = 5;
int btn_R_Pin = 1;

int btn_VolD_Pin = 8;
int btn_VolU_Pin = 9;


void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  /*Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Int: ");
  Serial.println(myData.b);
  Serial.print("Bool: ");
  Serial.println(myData.d);
  Serial.println();*/
  buttonPressed(myData.b, myData.d);
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

void buttonPressed(int buttonID, bool doIPress){
  uint8_t pressAction;
  if(doIPress == true){
    Serial.print("Button pressed: ");
    pressAction = LOW;
  }else{
    Serial.print("Button unpressed: ");
    pressAction = HIGH;
  }
  switch (buttonID) {
    case 0:
      digitalWrite(btn_L_Pin, pressAction);
      break;
    case 1:
      digitalWrite(btn_Up_Pin, pressAction);
      break;
    case 2:
      digitalWrite(btn_Right_Pin, pressAction);
      break;
    case 3:
      digitalWrite(btn_Down_Pin, pressAction);
      break;
    case 4:
      digitalWrite(btn_Left_Pin, pressAction);
      break;
    case 5:
      digitalWrite(btn_Start_Pin, pressAction);
      break;
    case 6:
      digitalWrite(btn_Select_Pin, pressAction);
      break;
    case 7:
      digitalWrite(btn_X_Pin, pressAction);
      break;
    case 8:
      digitalWrite(btn_A_Pin, pressAction);
      break;
    case 9:
      digitalWrite(btn_B_Pin, pressAction);
      break;
    case 10:
      digitalWrite(btn_Y_Pin, pressAction);
      break;
    case 11:
      digitalWrite(btn_R_Pin, pressAction);
      break;
    case 15:
      digitalWrite(btn_VolD_Pin, pressAction);
      break;
    case 16:
      digitalWrite(btn_VolU_Pin, pressAction);
      break;
    default:
      break;
  }
  Serial.print("\n");
}

void setup(){
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK){
    Serial.println("RX ESPNOW FAIL");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("RX READY");
  digitalWrite(btn_L_Pin, HIGH);
  pinMode(btn_L_Pin, OUTPUT);

  digitalWrite(btn_Up_Pin, HIGH);
  pinMode(btn_Up_Pin, OUTPUT);

  digitalWrite(btn_Right_Pin, HIGH);
  pinMode(btn_Right_Pin, OUTPUT);

  digitalWrite(btn_Down_Pin, HIGH);
  pinMode(btn_Down_Pin, OUTPUT);

  digitalWrite(btn_Left_Pin, HIGH);
  pinMode(btn_Left_Pin, OUTPUT);

  digitalWrite(btn_Start_Pin, HIGH);
  pinMode(btn_Start_Pin, OUTPUT);

  digitalWrite(btn_Select_Pin, HIGH);
  pinMode(btn_Select_Pin, OUTPUT);

  digitalWrite(btn_X_Pin, HIGH);
  pinMode(btn_X_Pin, OUTPUT);

  digitalWrite(btn_A_Pin, HIGH);
  pinMode(btn_A_Pin, OUTPUT);

  digitalWrite(btn_B_Pin, HIGH);
  pinMode(btn_B_Pin, OUTPUT);

  digitalWrite(btn_Y_Pin, HIGH);
  pinMode(btn_Y_Pin, OUTPUT);

  digitalWrite(btn_R_Pin, HIGH);
  pinMode(btn_R_Pin, OUTPUT);

  digitalWrite(btn_VolD_Pin, HIGH);
  pinMode(btn_VolD_Pin, OUTPUT);
  
  digitalWrite(btn_VolU_Pin, HIGH);
  pinMode(btn_VolU_Pin, OUTPUT);

  Serial.println("Buttons READY");
}

void loop(){

}