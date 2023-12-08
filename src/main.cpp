#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

// put function declarations here:
//int myFunction(int, int);

/**
 * Helper routine to dump a byte array as hex values to Serial. 
 */
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}

#define SS_PIN 5
#define RST_PIN 21

#define SS_PIN2 15
#define RST_PIN2 4

#define EN_PIN 32
#define IN1_PIN 33
#define IN2_PIN 25

#define CLOSED 0
#define OPEN 1

int doorState = CLOSED;

void openDoor(){
  digitalWrite(2, HIGH);
  digitalWrite(EN_PIN, HIGH);
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  Serial.println("Open");
  delay(12000);
  doorState = OPEN;
}

void closeDoor(){
  digitalWrite(2, HIGH);
  digitalWrite(EN_PIN, HIGH);
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  Serial.println("Close");
  delay(12000);
  doorState = CLOSED;
}

void breakDoor(){
  digitalWrite(2, LOW);
  digitalWrite(EN_PIN, LOW);
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
  Serial.println("Break");
}


MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522 rfid2(SS_PIN2, RST_PIN2); // Instance of the class

MFRC522::MIFARE_Key key; 

// Init array that will store new NUID 
byte nuidPICC[4];

void setup() { 
  Serial.begin(115200);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init();
  rfid2.PCD_Init(); // Init MFRC522 

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);

  pinMode(EN_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);

  digitalWrite(EN_PIN, LOW);
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
}
 
void loop() {
  digitalWrite(2, LOW);

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (rfid.PICC_IsNewCardPresent()) {
  }
  else if (rfid2.PICC_IsNewCardPresent()) {
  }
  else {
    return;
  } 
  
  // Verify if the NUID has been readed
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  if (rfid.PICC_ReadCardSerial()){
    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  }
  else if (rfid2.PICC_ReadCardSerial()){
    MFRC522::PICC_Type piccType = rfid2.PICC_GetType(rfid.uid.sak);
  }
  else {
    return;
  } 
 

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && 
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3] ) {

    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
    Serial.println("This is a New Card");
  }
  else Serial.println(F("Card read previously."));

  Serial.println("Test Line");

  switch (doorState) {
    case CLOSED:
      openDoor();
      breakDoor();
      break;
    case OPEN:
      closeDoor();
      breakDoor();
      break;
  }
  
  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}



