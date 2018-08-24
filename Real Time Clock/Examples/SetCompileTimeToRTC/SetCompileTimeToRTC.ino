#include <DS3232RTC.h>      // https://github.com/JChristensen/DS3232RTC
//  tmElements_t  is defined in https://github.com/JChristensen/DS3232RTC
//  time_t        is defined in http://www.cplusplus.com/reference/ctime/time_t/

void setup()
{
  Serial.begin(9600);
  RTC.set( getCompileTimeAsSeconds() );
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

time_t getCompileTimeAsSeconds()
{
  tmElements_t compilerDateTime;

  String  compilerDate        = __DATE__;                                               //  Example Format:   Apr 13 2018
  compilerDateTime.Year       = compilerDate.substring(7).toInt() - 1970;               //  offset from 1970
  compilerDateTime.Month      = stringMonthToInteger( compilerDate.substring(0, 3) );
  compilerDateTime.Day        = compilerDate.substring(4, 6).toInt();

  String  compilerTime        = __TIME__;                                               //  Example Format:    23:32:35
  compilerDateTime.Hour       = compilerTime.substring(0, 2).toInt();
  compilerDateTime.Minute     = compilerTime.substring(3, 5).toInt();
  compilerDateTime.Second     = compilerTime.substring(6).toInt();

  time_t compilerDateTimeAsSeconds = makeTime( compilerDateTime );                      //  Example:  1520986146  seconds
  const time_t timeRequiredForExecutingAboveCode = 14;                                  //  14 seconds.

  return compilerDateTimeAsSeconds + timeRequiredForExecutingAboveCode;
}

//  In order to cast the compilerTimeMonth string to integer.
enum Month { Jan=1, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec };
int stringMonthToInteger( String str )
{
  if (str == "Jan") return Jan;
  if (str == "Feb") return Feb;
  if (str == "Mar") return Mar;
  if (str == "Apr") return Apr;
  if (str == "May") return May;
  if (str == "Jun") return Jun;
  if (str == "Jul") return Jul;
  if (str == "Aug") return Aug;
  if (str == "Sep") return Sep;
  if (str == "Oct") return Oct;
  if (str == "Nov") return Nov;
  if (str == "Dec") return Dec;
}
