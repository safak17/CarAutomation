#include <DS3232RTC.h>      // https://github.com/JChristensen/DS3232RTC
//  tmElements_t  is defined in https://github.com/JChristensen/DS3232RTC
//  time_t        is defined in http://www.cplusplus.com/reference/ctime/time_t/

String DateTime = "2018 08 24 16 20 00";
void setup()
{
  Serial.begin(9600);
  RTC.set( getDateTimeAsSeconds( DateTime ) );
}

void loop()
{
  Serial.println("RTC DateTime:\n" + displayRTCTime() );
  delay(1000);
}

String displayRTCTime()
{
  tmElements_t rtcTime;
  RTC.read( rtcTime );

  return (  "Year: "    + String(rtcTime.Year + 1970) + "\n"  //  Here rtcTime.Year will return 48. Because, time is offset from 1970.
            "Month: "   + String(rtcTime.Month) + "\n"
            "Day: "     + String(rtcTime.Day) + "\n"
            "Hour: "    + String(rtcTime.Hour) + "\n"
            "Minute: "  + String(rtcTime.Minute) + "\n"
            "Second: "  + String(rtcTime.Second) + "\n" );
}

time_t getDateTimeAsSeconds(String& DateTime)
{
  String delimiter = " ";   //  Space is the indicator between parameters.

  uint16_t parameters[6];
  size_t start = 0;
  size_t end = DateTime.indexOf(delimiter);
  for(int i=0; i<6; i++)
  {
      parameters[i] = (DateTime.substring(start, end)).toInt();

      start = end + 1;
      end = DateTime.indexOf(delimiter, start);
  }

  tmElements_t rtcTime;
  rtcTime.Year        =       parameters[0] - 1970;
  rtcTime.Month       =       parameters[1];
  rtcTime.Day         =       parameters[2];
  rtcTime.Hour        =       parameters[3];
  rtcTime.Minute      =       parameters[4];
  rtcTime.Second      =       parameters[5];

  return makeTime( rtcTime );
}
