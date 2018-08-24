#include <DS3232RTC.h>      // https://github.com/JChristensen/DS3232RTC
//  tmElements_t  is defined in https://github.com/JChristensen/DS3232RTC
//  time_t        is defined in http://www.cplusplus.com/reference/ctime/time_t/

void setup()
{
  Serial.begin(9600);
  SetDefaultValuesOfRTC();
  SetInterruptPin2ForRTC();
  /*
    Timelib.h
    typedef enum {  dowInvalid, dowSunday, dowMonday, dowTuesday, dowWednesday, dowThursday, dowFriday, dowSaturday } timeDayOfWeek_t;
                    0           1          2          3           4             5           6           7
  */
  SetAlarm2Register(20, 23, 1);
}


void loop()
{
  if ( RTC.alarm( ALARM_2 ) )   //  Checks the alarm2 flag.
    Serial.println("Alarm is triggered at 23:20 on Sunday.");

}


void alarmFunction() {
  Serial.println("Interrupt functions shouldn't contain too much code.");
  Serial.println("Do the complicated stuff in the loop().");
}

void SetDefaultValuesOfRTC()
{
  RTC.setAlarm(ALM1_MATCH_DAY, 0, 0, 0, 1);
  RTC.setAlarm(ALM2_MATCH_DAY, 0, 0, 0, 1);           //  void DS3232RTC::setAlarm(ALARM_TYPES_t alarmType, byte minutes, byte hours, byte daydate)
  RTC.alarm(ALARM_1);
  RTC.alarm(ALARM_2);
  RTC.alarmInterrupt(ALARM_1, false);
  RTC.alarmInterrupt(ALARM_2, false);
  RTC.squareWave(SQWAVE_NONE);
}

void SetInterruptPin2ForRTC()
{
  pinMode(2, INPUT_PULLUP);
  attachInterrupt( digitalPinToInterrupt( 2 ), alarmFunction, CHANGE );
}

void SetAlarm2Register (uint8_t minute, uint8_t hour, uint8_t day)
{
  RTC.setAlarm(ALM2_MATCH_DAY, minute, hour , day);   //  Causes an alarm when the day of the week and hours and minutes match.
  RTC.alarm(ALARM_2);                                 //  Clear the Alarm_2 flag.
  RTC.alarmInterrupt(ALARM_2, true);                  //  Set interrupt.
}
