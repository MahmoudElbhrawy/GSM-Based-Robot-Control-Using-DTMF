/***********************
Author: Mahmoud Elbhrawy
***********************/

#include <SoftwareSerial.h>

#define SIM800L_Tx 2
#define SIM800L_Rx 3

SoftwareSerial SIM800L(SIM800L_Tx, SIM800L_Rx);

#define MOTOR1_PIN1 4
#define MOTOR1_PIN2 5
#define MOTOR2_PIN1 6
#define MOTOR2_PIN2 7

// Stores incoming data from SIM800L
String buff;
String dtmf_cmd;
boolean is_call = false;

void setup() {
  pinMode(MOTOR1_PIN1, OUTPUT);
  pinMode(MOTOR1_PIN2, OUTPUT);
  pinMode(MOTOR2_PIN1, OUTPUT);
  pinMode(MOTOR2_PIN2, OUTPUT);

  // Begin serial communication with (Serial Monitor)
  Serial.begin(115200);

  // Begin serial communication with (SIM800L)
  SIM800L.begin(9600);
  Serial.println("Begin serial communication with (SIM800L)");
  delay(7000);

  SIM800L.println("AT"); // Send AT
  delay(500);

  SIM800L.println("AT+DDET=1,0,0,0"); // Enable DTMF
  delay(500);
}

void loop() {
  while (SIM800L.available()) {
    buff = SIM800L.readString();
    Serial.println(buff);

    if (is_call == true) {
      int index = buff.indexOf("+DTMF:");
      if (index > -1) {
        index = buff.indexOf(":");
        dtmf_cmd = buff.substring(index + 1, buff.length());
        dtmf_cmd.trim();
        Serial.println("dtmf_cmd: " + dtmf_cmd);
        doAction();
      }
      if (buff.indexOf("NO CARRIER") > -1) {
        SIM800L.println("ATH");
        is_call = false;
      }
    }
    if (buff.indexOf("RING") > -1) {
      delay(2000);
      SIM800L.println("ATA");
      is_call = true;
    }
  }

  while (Serial.available()) {
    SIM800L.println(Serial.readString());
  }
}

void doAction() {
  if (dtmf_cmd == "1") { // Use double quotes for string comparison
    controlMotor(1, true);
    controlMotor(2, true);
    Serial.println("Move Forward");
  } else if (dtmf_cmd == "2") { // Use double quotes for string comparison
    controlMotor(1, false);
    controlMotor(2, false);
    Serial.println("Move Backward");
  }
}

void controlMotor(int motor, bool state) {
  switch (motor) {
    case 1:
      digitalWrite(MOTOR1_PIN1, state ? HIGH : LOW);
      digitalWrite(MOTOR1_PIN2, state ? LOW : HIGH);
      break;
    case 2:
      digitalWrite(MOTOR2_PIN1, state ? HIGH : LOW);
      digitalWrite(MOTOR2_PIN2, state ? LOW : HIGH);
      break;
    default:
      break;
  }
}
