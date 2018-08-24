# Arduino and Real Time Clock
> The RTC maintains seconds, minutes, hours, day, date, month, and year information.
>- https://datasheets.maximintegrated.com/en/ds/DS3232.pdf
<br>

![Arduino_Uno_and_RTC](Photos/Arduino_Uno_and_RTC.png)<br><br><br>

## O. Setting Compiling Time To RTC
![SetCompileTimeToRTC](Photos/SetCompileTimeToRTC.png)<br><br><br>

## 1. Setting And Getting RTC Time
![SetAndGetRTCTime](Photos/SetAndGetRTCTime.png)<br><br><br>

## 2. Getting Interrupt From RTC
Here, we used the alarm which is triggered when the specified **day, hour and minute match.** <br>
You should also examine the **Timelib.h**.
![RTCInterrupt](Photos/RTCInterrupt.png)<br><br><br>

REFERENCES: https://github.com/JChristensen/DS3232RTC
