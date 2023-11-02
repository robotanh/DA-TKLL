#include<WiFi.h>
#include<Firebase_ESP_Client.h>

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "KPL_TECH"
#define WIFI_PASSWORD "999999999"
#define API_KEY "AIzaSyADY4l9sDZnqcR48svSYRanIiVyZ_IQJCk"
#define DATABASE_URL "https://esp32-project-d31cd-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define PWMChannel 0
#define LDR_PIN 34

const int freq = 5000;
const int resolution = 8;
const int LED1 = 12;         // chân điều khiển LED
const int LED2 = 14; 
int buttonState = 0;        // lưu trạng thái của nút bấm

FirebaseData fbdo;
FirebaseData fbdo1;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis=0;
bool signupOK = false;
int ldrData =0;
float voltage = 0.0;
int pwmValue = 0;
bool ledStatus = false;

void setup() {

  Serial.begin(115200);
  WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
  while (WiFi.status()!= WL_CONNECTED){
    Serial.print("."); delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth,"","")){
    Serial.println("signUp OK");
    signupOK = true;
  }else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config,&auth );
  Firebase.reconnectWiFi (true);
  pinMode(LED1, OUTPUT);     // digital
  ledcSetup(PWMChannel, freq, resolution);
  ledcAttachPin(LED2, PWMChannel);
//  pinMode(button, INPUT);   // thiết lập chân nút bấm là INPUT

}

void loop() {
  if(Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    //-----------store sensor data to a RTDB
    voltage = (float)analogReadMilliVolts(LDR_PIN)/1000;
    if(Firebase.RTDB.setInt(&fbdo, "sensor/ldr_data",ldrData)){
      Serial.println();
      Serial.print(ldrData);
      Serial.print("-successfully saved to: "+fbdo.dataPath());
      Serial.println(" ("+fbdo.dataType() + ") ");
    }else{
      Serial.println("FAILED: " + fbdo.errorReason());
    }

  if(Firebase.RTDB.setFloat(&fbdo, "sensor/voltage",voltage)){
      Serial.println();
      Serial.print(voltage);
      Serial.print("-successfully saved to: "+fbdo.dataPath());
      Serial.println(" ("+fbdo.dataType() + ") ");
    }else{
      Serial.println("FAILED: " + fbdo.errorReason());
    }
  }
  fbdo.clear();

//------------------------------------------------------
if(Firebase.RTDB.getInt(&fbdo,"/LED/analog/")){
  if(fbdo.dataType()=="int"){
    pwmValue = fbdo.intData();
    Serial.println("Successful READ from" + fbdo.dataPath()+ ": "+ pwmValue+ " ("+fbdo.dataType()+") ");
    ledcWrite(PWMChannel, pwmValue);
  }
}else{
  Serial.println("FAILED: "+ fbdo.errorReason());
}

if(Firebase.RTDB.getBool(&fbdo,"/LED/digital/")){
  if(fbdo.dataType()=="boolean"){
    ledStatus = fbdo.boolData();
    Serial.println("Successful READ from" + fbdo.dataPath()+ ": "+ ledStatus+ " ("+fbdo.dataType()+") ");
    digitalWrite(LED1, ledStatus);
  }
}else{
  Serial.println("FAILED: "+ fbdo.errorReason());
}
fbdo.clear();



}