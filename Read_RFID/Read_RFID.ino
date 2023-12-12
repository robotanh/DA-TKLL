
#include <SPI.h>//include the SPI bus library
#include <MFRC522.h>//include the RFID reader library
#include <Arduino.h>


#include <WiFi.h>
#include<Firebase_ESP_Client.h>

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "tanh"
#define WIFI_PASSWORD "12345678"
#define API_KEY "AIzaSyADY4l9sDZnqcR48svSYRanIiVyZ_IQJCk"
#define DATABASE_URL "https://esp32-project-d31cd-default-rtdb.asia-southeast1.firebasedatabase.app/"

#define RST_PIN         0          // Configurable, see typical pin layout above
#define SS_PIN          5         // Configurable, see typical pin layout above

const int LED = 33;
const int Buzzer = 26;

FirebaseAuth auth;
FirebaseConfig config;
FirebaseData fbdo;

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

bool signupOK = false;  
MFRC522::MIFARE_Key key;//create a MIFARE_Key struct named 'key', which will hold the card information


void setup() {
        Serial.begin(9600);        // Initialize serial communications with the PC
        SPI.begin();               // Init SPI bus
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
        mfrc522.PCD_Init();        // Init MFRC522 card (in case you wonder what PCD means: proximity coupling device)
        Serial.println("Scan a MIFARE Classic card");
       
        for (byte i = 0; i < 6; i++) {
                key.keyByte[i] = 0xFF;
        }
        pinMode(LED, OUTPUT);
        pinMode(Buzzer, OUTPUT); 
        digitalWrite(Buzzer, LOW);

}

int block=2;
                          

unsigned char blockcontent[16] = {'v', 'i', '\0'};
unsigned char readbackblock[18];
String readfromdb = "";
void loop()
{
  //read from data base
  if(Firebase.RTDB.getString(&fbdo,"/users/-Ni8LyV12uM-mJa_zP50/name/")){
  if(fbdo.dataType()=="string"){   
    Serial.println("Successful READ from" + fbdo.dataPath()+ ": "+ fbdo.stringData() + " ("+fbdo.dataType()+") ");
    delay(2000);
    readfromdb = fbdo.stringData();
  }
  }else{
    Serial.println("FAILED: "+ fbdo.errorReason());
  }
//---------------------------------------------------------------------------------------

  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  Serial.println("New Card Present OK");

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    Serial.println("Read Card Serial not OK");
    mfrc522.PCD_Init();        // Init MFRC522 card (in case you wonder what PCD means: proximity coupling device)
    return;
  }
  Serial.println("Read Card Serial OK");
  Serial.println("card selected");      

  readBlock(block, readbackblock);//read the block back
  Serial.print("read block: ");
  for (int j=0 ; j<16 ; j++){
    Serial.write (readbackblock[j]);//Serial.write() transmits the ASCII numbers as human readable characters to serial monitor
  }
    String blockcontentString(reinterpret_cast<char*>(readbackblock));
    if (blockcontentString == readfromdb) //compare string 
    {
      Serial.println("Strings are equal.");
      digitalWrite(Buzzer, LOW);
      digitalWrite(LED, HIGH);
    } else {
      Serial.println("Strings are not equal.");
      digitalWrite(Buzzer, HIGH);
      digitalWrite(LED, LOW);
    }
  Serial.println("");
  delay(5000);
  digitalWrite(LED, LOW);
  digitalWrite(Buzzer, LOW);
  noTone(Buzzer);
  mfrc522.PCD_Init();        // Init MFRC522 card (in case you wonder what PCD means: proximity coupling device)
  Serial.println("Scan a MIFARE Classic card");
         
 
}


int readBlock(int blockNumber,  unsigned char arrayAddress[]) 
{
  int largestModulo4Number=blockNumber/4*4;
  int trailerBlock=largestModulo4Number+3;//determine trailer block for the sector

  /*****************************************authentication of the desired block for access***********************************************************/
  MFRC522::StatusCode status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
         Serial.print("PCD_Authenticate() failed (read): ");
         Serial.println(mfrc522.GetStatusCodeName(status));
         return 3;//return "3" as error message
  }
 
  byte buffersize = 18;//we need to define a variable with the read buffer size, since the MIFARE_Read method below needs a pointer to the variable that contains the size... 
  status = mfrc522.MIFARE_Read(blockNumber, arrayAddress, &buffersize);//&buffersize is a pointer to the buffersize variable; MIFARE_Read requires a pointer instead of just a number
  if (status != MFRC522::STATUS_OK) {
          Serial.print("MIFARE_read() failed: ");
          Serial.println((const char*)mfrc522.GetStatusCodeName(status));
          return 4;//return "4" as error message
  }
  Serial.println("block was read");

  return 0;
}