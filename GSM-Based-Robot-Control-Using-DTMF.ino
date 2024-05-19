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
  // Set motor pins as outputs
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

  // Initialize SIM800L
  SIM800L.println("AT"); // Send AT command
  delay(500);

  /*
   +DDET: <mode>,<interval>,<reportMode>,<ssdet>
   +DDET: (0,1),(0-10000),(0,1),(0,1)
  */
  SIM800L.println("AT+DDET=1,0,0,0"); // Enable DTMF
  delay(500);
}

void loop() {
  // Check for incoming data from SIM800L
  while (SIM800L.available()) {
    buff = SIM800L.readString();
    Serial.println(buff);

    if (is_call) {
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

  // Forward data from Serial Monitor to SIM800L
  while (Serial.available()) {
    SIM800L.println(Serial.readString());
  }
}

void doAction() {
  // Perform action based on DTMF command
  if (dtmf_cmd == "1") {
    controlMotor(1, true);
    controlMotor(2, true);
    Serial.println("Move Forward");
  } else if (dtmf_cmd == "2") {
    controlMotor(1, false);
    controlMotor(2, false);
    Serial.println("Move Backward");
  } else if (dtmf_cmd == "0") {
    stopMotors();
    Serial.println("Motors Stop");
  }
}

void controlMotor(int motor, bool state) {
  // Control motor direction based on state
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

void stopMotors() {
  // Stop both motors
  digitalWrite(MOTOR1_PIN1, LOW);
  digitalWrite(MOTOR1_PIN2, LOW);
  digitalWrite(MOTOR2_PIN1, LOW);
  digitalWrite(MOTOR2_PIN2, LOW);
}
