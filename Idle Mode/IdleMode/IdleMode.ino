#include <avr/power.h>                                              //  To reduce power consumption.
#include <avr/sleep.h>                                              //  To use sleep_mode();

String receivedSerialBusData = "";

void setup()
{
  Serial.begin(9600);                                               //  BAUD rate is set to 9600.

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);                                  //  Arduino is awake, 13th led is turned on.

  SleepNow();
}

void loop() {}

//  https://www.arduino.cc/en/Reference/SerialEvent
void serialEvent()
{
  receivedSerialBusData = Serial.readString();
  Serial.println("Received Data: [" + receivedSerialBusData + "]");
  Serial.println("Arduino is just woken up. It will stay be awake for 5 seconds!");
  for(int i=1; i<=5; i++)
  {
    Serial.println(i);
    delay(1000);
  }

  Serial.println("Don't make noise! Arduino is going to sleep now! ");
  SleepNow();
}//end serialEvent()

void SleepNow()
{
  delay(200);
  set_sleep_mode(SLEEP_MODE_IDLE);  // Sleep Mode is set to SLEEP_MODE_IDLE
  sleep_enable();                   // Enables the sleep bit in the mcucr register

  power_adc_disable();
  power_spi_disable();
  power_timer0_disable();
  power_timer1_disable();
  power_timer2_disable();
  power_twi_disable();

  digitalWrite(LED_BUILTIN, LOW);   //  Arduino is sleeping, 13th led is turned off.
  sleep_mode();                     //  Here, the device is actually put to sleep!
  digitalWrite(LED_BUILTIN, HIGH);  //  Arduino is awake, 13th led is turned on.

  sleep_disable();                  // First thing after waking from sleep:  disable sleep...
  power_all_enable();
}
