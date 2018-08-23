#include <AlarmTask.h>

void setup() {
  Serial.begin(9600);           //  BAUD rate is set to 9600.

  //  Space is the indicator.
  //  String alarmDescription = "id repeat dayOfWeek hour minute relayNumber relayStatus";
  //  String alarmDescription = "1 1 1 23 50 7 1"
  //  alarmId     =   1
  //  repeat      =   1       ( repeats )
  //  dayOfWeek   =   1       ( Monday )
  //  hour        =   23
  //  minute      =   50      ( The user can ONLY set the alarm the multiplies of 10. )
  //  relayNumber =   7       ( 7th relay )
  //  relayStatus =   1       ( will be activated )

  String alarmDescription1 = "1 1 1 23 50 7 1";   //  repeats,     Monday, 23 : 50,    activates 7th relay.
  String alarmDescription2 = "2 0 7 00 00 3 1";   //  not repeat,  Sunday, 00 : 00,    activates 3rd relay.
  String alarmDescription3 = "3 1 1 23 50 8 0";   //  repeats,     Monday, 23 : 50,    deactivates 8th relay.

  AlarmTask alarm1 ( alarmDescription1 );
  AlarmTask alarm2 ( alarmDescription2 );
  AlarmTask alarm3 ( alarmDescription3 );

  Serial.println("alarm1: " + alarm1.getDescription());
  Serial.println("alarm2: " + alarm2.getDescription());
  Serial.println("alarm3: " + alarm3.getDescription());

  if( alarm1.isSameTime(alarm3) ) Serial.println("alarm1 and alarm3 will be triggered at the same time.");
  else                            Serial.println("alarm1 and alarm3 will NOT be triggered at the same time.");

  if( alarm1.isSameTime(alarm2) ) Serial.println("alarm1 and alarm2 will be triggered at the same time.");
  else                            Serial.println("alarm1 and alarm2 will NOT be triggered at the same time.");

  if( alarm1 < alarm2 )           Serial.println("alarm1 will be triggered before alarm2 !");
  else                            Serial.println("alarm2 will be triggered before alarm1 !");
}

void loop() {}
