#include <SoftwareSerial.h>
SoftwareSerial BTSerial(10, 11);  // Connect Bluetooth RX PIN TO ARDUINO 11 PIN | Connect BT TX PIN TO ARDUINO 10 PIN

void setup()
{
  pinMode(9, OUTPUT);             // This pin will pull the HC-05 pin 34 (key pin) HIGH to switch module to AT mode
  digitalWrite(9, HIGH);

  Serial.begin(9600);
  Serial.println("Enter AT commands:");

  BTSerial.begin(38400);          // HC-05 default speed in AT command more
}

void loop()
{

  // Keep reading from HC-05 and send to Arduino Serial Monitor
  if (BTSerial.available())
    Serial.write(BTSerial.read());

  // Keep reading from Arduino Serial Monitor and send to HC-05
  if (Serial.available())
    BTSerial.write(Serial.read());
}

//  REFERENCE:  https://www.youtube.com/watch?v=pRcw_jMIhRo
//  REFERENCE:  http://electricdiylab.com/how-to-set-at-command-mode-for-hc-05-bluetooth-module/
