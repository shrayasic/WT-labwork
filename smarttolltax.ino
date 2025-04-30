#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9
#define BUZZER 2 // Buzzer pin
#define SERVO_PIN 6 // Servo pin

MFRC522 mfrc522(SS_PIN, RST_PIN);  // RFID instance
Servo myServo;                     // Servo instance
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD instance with I2C address 0x27

void setup() {
    Serial.begin(9600);
    SPI.begin();
    mfrc522.PCD_Init();
    
    myServo.attach(SERVO_PIN);
    myServo.write(0); // Ensure servo starts at 0°
    
    pinMode(BUZZER, OUTPUT);
    noTone(BUZZER);
    
    lcd.init();
    lcd.clear();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("  Put card on   ");
    lcd.setCursor(0, 1);
    lcd.print("     reader     ");
}

void loop() {
    if (!mfrc522.PICC_IsNewCardPresent()) {
        return;
    }
    if (!mfrc522.PICC_ReadCardSerial()) {
        return;
    }
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("UID tag:");
    
    String content = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        lcd.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        lcd.print(mfrc522.uid.uidByte[i], HEX);
        content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
        content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    content.toUpperCase();
    
    // ➡️ Print the UID to the Serial Monitor
    Serial.print("Card UID: ");
    Serial.println(content);

    lcd.setCursor(0, 1);

    // ➡️ Check for two allowed UIDs
    if (content.substring(1) == "19 4C 0A 21" || content.substring(1) == "64 76 27 83") {
        lcd.print("Access Granted");
        Serial.println("Access Granted");
        tone(BUZZER, 500);
        delay(300);
        noTone(BUZZER);
        myServo.write(90);  // Move servo to 90° (open position)
        delay(3000);        // Hold open for 3 seconds
        myServo.write(0);   // Move servo back to 0° (closed position)
        delay(1000);        // Small delay before resuming loop
    } else {
        lcd.print("Access Denied");
        Serial.println("Access Denied");
        tone(BUZZER, 300);
        delay(2000);
        noTone(BUZZER);
    }
    delay(2000);
}
