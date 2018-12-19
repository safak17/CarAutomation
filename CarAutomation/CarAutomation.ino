#include <DS3232RTC.h>                                              //  RealTimeClock Library   https://github.com/JChristensen/DS3232RTC
#include <AlarmTask.h>                                              //  To save alarms as AlarmTask format.
#include <avr/power.h>                                              //  To reduce power consumption.
#include <avr/sleep.h>                                              //  To use sleep_mode functions.

#include <SoftwareSerial.h>                                         //  https://www.arduino.cc/en/Reference/SoftwareSerial
SoftwareSerial BluetoothSerial(0, 1);                               //  Arduino's ( RX | 0 ) &&  ( TX | 1 )

#include </Users/safakakinci/Documents/Arduino/libraries/SinglyLinkedList/SinglyLinkedList.cpp>
SinglyLinkedList<AlarmTask> alarmList;                              //  Alarms will be added to the alarmList.
SinglyLinkedList<AlarmTask>::iterator currentAlarmIterator = NULL;  //  To hold the current alarm ( the one that will be triggered ) in the alarmList.

#include <RelayModule.h>                                            //  To control relay module "easily".
RelayModule module(4, LOW);                                         //  It is a 4 channel relay module and it is working with ACTIVE LOW logic.

#define ARDUINO_ANALOG_TEMPERATURE          2                       //  Temperature value will be read from Arduino's Analog Pin 2.
#define ARDUINO_ANALOG_CURRENT              3                       //  Current value     will be read from Arduino's Analog Pin 3.
#define ARDUINO_ANALOG_VOLTAGE              4                       //  Voltage value     will be read from Arduino's Analog Pin 4.
#define SMOOTH_CALCULATION_FREQUENCY_NUMBER 8                       //  Reads eight times the voltage value from Arduino Analog Pin 4 in every 50 ms and then returns the average of total measurement.

String receivedBluetoothData = "";                                  //  To hold the data sent by Android.

void setup(){
  Serial.begin(9600);                                               //  Arduino's baud rate is set to 9600.
  BluetoothSerial.begin(9600);                                      //  HC-05's (BluetoothModule) baud rate is set to 9600.
  analogReference(DEFAULT);                                         //  Configures the reference voltage used for analog input, Arduino Uno 5V

  pinMode(LED_BUILTIN, OUTPUT);                                     //  If Arduino is awake, the built-in led will be turned on.
  digitalWrite(LED_BUILTIN, HIGH);                                  //  If Arduino is awake, the built-in led will be turned on.

  SetRelayModule();                                                 //  Sets the relay module pins and deactivates all the relays.
  SetInterruptPin2ForRTC();                                         //  If an alarm is triggered, the interrupt will come to Arduino Digital Pin 2.
  SetDefaultValuesOfRTC();
  SleepNow();
}


AlarmTask triggeredAlarm(0);
void loop(){
  if ( RTC.alarm( ALARM_2 ) ) //  If Real Time Clock produces an interrupt because of ALARM_2 register.
  {
    do
    {
      triggeredAlarm = currentAlarmIterator.current().info();
      Serial.println("OK: ALARM_TRIGGERED " + String(triggeredAlarm.id()) + " ;");

      RelayOperate( 1, triggeredAlarm.relay1());
      RelayOperate( 2, triggeredAlarm.relay2());
      RelayOperate( 3, triggeredAlarm.relay3());
      RelayOperate( 4, triggeredAlarm.relay4());

      if      ( triggeredAlarm.repeat() == 0 )      //  triggeredAlarm does not repeat.
        AlarmRemove( triggeredAlarm.id() );
      
      else if( triggeredAlarm.repeat() == 1 ){      //
        AlarmRemove( triggeredAlarm.id() );         //  Remove the old alarm.
        AlarmTask newAlarm( currentAlarmIterator.current().info().getAlarm() );
        newAlarm.increaseDayByOne();                //  Update the alarm.
        AlarmAdd( newAlarm.getAlarm() );            //  Add new alarm.
      }

      UpdateCurrentAlarmIterator();

    } while (alarmList.size() != 0 && triggeredAlarm.isSameTime( currentAlarmIterator.current().info()));

    UpdateAlarm2RegisterOfRTC();
    SleepNow();
  }// if ( RTC.alarm( ALARM_2 ) )
}// loop()

void UpdateCurrentAlarmIterator(){
    if ( alarmList.size() == 0 ) 
        currentAlarmIterator = NULL;
        
    else if ( alarmList.size() == 1)
        currentAlarmIterator = alarmList.begin();
    
    else{
        
        int now = 11008; //    RTCDayTime();
        
        SinglyLinkedList<AlarmTask>::iterator it;
        it = alarmList.begin();
        
        while( it.valid() && it.current().info().dayTime() < now )
            it++;
        
        if( it.valid() )
            currentAlarmIterator = it;
        else
            currentAlarmIterator = NULL;
    }
}


void serialEvent()                                        //  Called when data is available.                          https://www.arduino.cc/en/Reference/SerialEvent
{
  receivedBluetoothData = Serial.readString();            //  Reads characters from the serial buffer into a string.  https://www.arduino.cc/en/Serial/ReadString

  while ( receivedBluetoothData.endsWith(" ;") )          //  There can be many commands in the buffer.
  {

    if ( receivedBluetoothData.startsWith("pg"))
      PeripheralGet();

    else if ( receivedBluetoothData.startsWith("cg"))
      ClockGet();

    else if ( receivedBluetoothData.startsWith("cs"))
      ClockSet( receivedBluetoothData.substring(3, receivedBluetoothData.indexOf(" ;")) );              //  ClockSet( DateTime )

    else if ( receivedBluetoothData.startsWith("ro"))
      RelayOperate( (receivedBluetoothData.charAt(3) - '0'), (receivedBluetoothData.charAt(5) - '0') ); //  RelayOperate( relayNumber, relayStatus );

    else if ( receivedBluetoothData.startsWith("rs"))
      RelaysStatus();

    else if ( receivedBluetoothData.startsWith("aa"))
      AlarmAdd( receivedBluetoothData.substring(3, receivedBluetoothData.indexOf(" ;")).toInt() );      //  AlarmSet( AlarmDescription );

    else if ( receivedBluetoothData.startsWith("ar"))
      AlarmRemove( receivedBluetoothData.substring(3, receivedBluetoothData.indexOf(" ;")).toInt() );   //  AlarmDisarm( alarmId );

    else if ( receivedBluetoothData.startsWith("as"))
      AlarmSet( receivedBluetoothData.substring(3, receivedBluetoothData.indexOf(" ;")).toInt() );      //  AlarmSet( alarmId );

    else if ( receivedBluetoothData.startsWith("al"))
      AlarmList();

    else
      Serial.println("ERROR: BAD_FOMRAT ;");

    receivedBluetoothData = receivedBluetoothData.substring( receivedBluetoothData.indexOf(";") + 1);
  }//end while ( receivedBluetoothData.endsWith(" ;") )

  SleepNow();
}//end serialEvent()

void SetInterruptPin2ForRTC(){
  pinMode(2, INPUT_PULLUP);
  attachInterrupt( digitalPinToInterrupt( 2 ), alarmFunction, CHANGE );
}

void SleepNow(){
  delay(200);
  set_sleep_mode(SLEEP_MODE_IDLE);  //  Sleep Mode is set to SLEEP_MODE_IDLE
  sleep_enable();                   //  Enables the sleep bit in the mcucr register

  power_adc_disable();
  power_spi_disable();
  power_timer0_disable();
  power_timer1_disable();
  power_timer2_disable();
  power_twi_disable();

  digitalWrite(LED_BUILTIN, LOW);   //  To show that Arduino is sleeping.
  sleep_mode();                     //  Here, the device is actually put to sleep!
  digitalWrite(LED_BUILTIN, HIGH);  //  To show that Arduino is NOT sleeping.

  sleep_disable();                  // First thing after waking from sleep:  disable sleep...
  power_all_enable();
}






/***************                Relay                ***************/
void SetRelayModule() {
  module[1].setPinNumber(7).setPinMode(OUTPUT);       //  Relay1 (IN1) is connected to the Arduino's digital pin 7.
  module[2].setPinNumber(6).setPinMode(OUTPUT);       //  Relay2 (IN2) is connected to the Arduino's digital pin 6.
  module[3].setPinNumber(5).setPinMode(OUTPUT);       //  Relay3 (IN3) is connected to the Arduino's digital pin 5.
  module[4].setPinNumber(4).setPinMode(OUTPUT);       //  Relay4 (IN4) is connected to the Arduino's digital pin 4.
  module.deactivateAll();                             //  At the beginning, deactivates all the relays.
}

void RelayOperate( uint8_t relayNumber, uint8_t relayStatus ){
  if ( relayStatus == 0 )        RelayOff    ( relayNumber );
  else if ( relayStatus == 1 )   RelayOn     ( relayNumber );
  else if ( relayStatus == 2 )   RelayToggle ( relayNumber );
  else if ( relayStatus == 3 )   Serial.println("OK: RELAY_OPERATE " + String(relayNumber) + " DONT_CARE ;");
}//end RelayOperate()

void RelayOn( uint8_t relayNumber ) {
  module.activate( relayNumber ) ?   Serial.println("OK: RELAY_OPERATE " + String(relayNumber) + " ACTIVE ;") :
  Serial.println("ERROR: RELAY_OPERATE " + String(relayNumber) + " ACTIVE ;");
}

void RelayOff( uint8_t relayNumber ) {
  module.deactivate( relayNumber ) ? Serial.println("OK: RELAY_OPERATE " + String(relayNumber) + " DEACTIVE ;") :
  Serial.println("ERROR: RELAY_OPERATE " + String(relayNumber) + " DEACTIVE ;");
}

void RelayToggle( uint8_t relayNumber ) {
  module[relayNumber].getStatus() ? RelayOff( relayNumber ) : RelayOn( relayNumber );
}

void RelaysStatus() {
  Serial.println("OK: RELAYS_STATUS " + String(module.getStatus()) + " ;" );
}





/***************                Peripheral                ***************/
void PeripheralGet() {
  String feeder =   String(module.getStatus())  + " " +
                    TemperatureValue()          + " " +
                    CurrentValue()              + " " +
                    VoltageValue()              ;

  Serial.println("OK: PERIPHERAL_GET " + feeder + " ;" );
}//end PeripheralGet()

String TemperatureValue() {
  return String( analogRead(ARDUINO_ANALOG_TEMPERATURE) );
}
String CurrentValue()     {
  return String ( analogRead(ARDUINO_ANALOG_CURRENT) );
}
String VoltageValue()     {
  return String( SmoothVoltageCalculation() );
}

//  Reads eight times the voltage value from Arduino Analog Pin 4 in every 50 ms and then returns the average of total measurement.
float SmoothVoltageCalculation(){
  float total = 0;

  for ( int i = 0; i < SMOOTH_CALCULATION_FREQUENCY_NUMBER; ++i)      // ++i,   increases i and returns i.
  {
    total += analogRead(ARDUINO_ANALOG_VOLTAGE);
    delay(50);
  }

  return total / 8;
}//end SmoothVoltageCalculation()






/***************                RealTimeClock                ***************/
void SetDefaultValuesOfRTC(){
  //  void DS3232RTC::setAlarm(ALARM_TYPES_t alarmType, byte minutes, byte hours, byte daydate)
  RTC.setAlarm(ALM1_MATCH_DAY, 0, 0, 0, 1);
  RTC.setAlarm(ALM2_MATCH_DAY, 0, 0, 0, 1);
  RTC.alarm(ALARM_1);
  RTC.alarm(ALARM_2);
  RTC.alarmInterrupt(ALARM_1, false);
  RTC.alarmInterrupt(ALARM_2, false);
  RTC.squareWave(SQWAVE_NONE);
}

void SetAlarm2Register (uint8_t minute, uint8_t hour, uint8_t dayOfWeek){
  RTC.setAlarm(ALM2_MATCH_DAY, minute, hour , dayOfWeek); //  Causes an alarm when the day of the week and hours and minutes match.
  RTC.alarm(ALARM_2);                                     //  Clears the ALARM_2 flag. ( ALARM_2 flag is set when RTC produces an interrupt. )
  RTC.alarmInterrupt(ALARM_2, true);                      //  When the day of the week and hours and minutes match, RTC will produce an interrupt.

  Serial.println("OK: ALARM_SET " + String(dayOfWeek) + ":" + String(hour) + ":" + String(minute));
}

//  Format of DateTime  =   "2018 01 01 00 00 00"
time_t DateTimeAsSeconds(String& DateTime){
  String delimiter = " ";   //  Space is the indicator between parameters.

  uint16_t parameters[6];
  size_t start = 0;
  size_t end = DateTime.indexOf(delimiter);
  for (int i = 0; i < 6; i++)
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

//  cs 2018 9 3 12 29 0 ;
void ClockSet( String DateTime ){
  bool isSet = RTC.set( DateTimeAsSeconds( DateTime ) );

  if ( isSet == 0 )      Serial.println("OK: CLOCK_SET ;");
  else                   Serial.println("ERROR: CLOCK_SET ;");

  //  UpdateAlarm2RegisterOfRTC();
}

void ClockGet(){
  tmElements_t  rtcTime;        RTC.read( rtcTime );

  String RTCTime =  String( rtcTime.Year + 1970 ) + " " +
                    String( rtcTime.Month       ) + " " +
                    String( rtcTime.Day         ) + " " +
                    String( rtcTime.Hour        ) + " " +
                    String( rtcTime.Minute      ) + " " +
                    String( rtcTime.Second      ) ;

  Serial.println("OK: CLOCK_GET " + RTCTime + " ;" );
  //Serial.println("DAY:" + DayDate( rtcTime.Wday ));
}


String DayDate( uint16_t t ){
  uint16_t dayDate = castTimeLibFormatToDay( t );

  switch ( dayDate ){
    case 0:   return "Invalid";
    case 1:   return "Monday";
    case 2:   return "Tuesday";
    case 3:   return "Wednesday";
    case 4:   return "Thursday";
    case 5:   return "Friday";
    case 6:   return "Saturday";
    case 7:   return "Sunday";
  }
}

int RTCDayTime(){
  tmElements_t  rtcTime;        RTC.read( rtcTime );
  int day = castTimeLibFormatToDay(rtcTime.Day);
  int hour = rtcTime.Hour;
  int minute = rtcTime.Minute;

  return (day *  2048) + (hour * 64) + (minute);
}





/***************                Alarm                ***************/
void AlarmAdd( uint32_t alarmDescription ){
  AlarmTask newAlarm ( alarmDescription );
  alarmList.push_sorted( newAlarm );
}

void AlarmSet( uint8_t alarmId){
  AlarmTask tempAlarm( 0 );
  tempAlarm.setId( alarmId );
  currentAlarmIterator = alarmList.find(tempAlarm);

  AlarmTask& firstAlarm = currentAlarmIterator.current().info();
  SetAlarm2Register( firstAlarm.minute(),  firstAlarm.hour(), castDayToTimelibFormat(firstAlarm.dayWeek()) );
}

void AlarmRemove( uint8_t alarmId ){
  AlarmTask tempAlarm( 0 );
  tempAlarm.setId( alarmId );
  if ( alarmList.remove( tempAlarm ))   Serial.println("OK: ALARM_REMOVE " + String(alarmId) + " ;" );
  else                                  Serial.println("ERROR: ALARM_REMOVE " + String(alarmId) + " ;" );
}

void AlarmList(){
  Serial.println("OK: ALARM_LIST_SIZE " + String( alarmList.size()) + " ;");

  SinglyLinkedList<AlarmTask>::iterator it;
  for ( it = alarmList.begin();  it.position() < alarmList.size(); it++  )
    Serial.println("OK: ALARM_LIST_ITEM " + String(it.current().info().getAlarm()) + " ;" );
}

void alarmFunction() {}

void UpdateAlarm2RegisterOfRTC(){
  if ( currentAlarmIterator != NULL){
    AlarmTask& firstAlarm = currentAlarmIterator.current().info();
    SetAlarm2Register( firstAlarm.minute(),  firstAlarm.hour(), castDayToTimelibFormat(firstAlarm.dayWeek()) );
  }
  else
    SetDefaultValuesOfRTC();
}

uint16_t castTimeLibFormatToDay(uint16_t timeLibFormatDay)    {
  return ( timeLibFormatDay == 1 ) ? 7 : timeLibFormatDay - 1;
}
uint16_t castDayToTimelibFormat(uint16_t day )                {
  return ( day == 7 ) ? 1 : day + 1;
}



/*
  Below, you see one part of the Timelib library.
  Timelib.h
  typedef enum {
  0           1          2          3           4             5           6           7
  dowInvalid, dowSunday, dowMonday, dowTuesday, dowWednesday, dowThursday, dowFriday, dowSaturday
  } timeDayOfWeek_t;

  According to the Timelib.h, week starts from Sunday, but here In Turkey, week starts from Monday.
  We need to switch between these two formats. Because, RTC will be programmed in Timelib.h format.
  1 - Monday
  2 - Tuesday
  3 - Wednesday
  4 - Thursday
  5 - Friday
  6 - Saturday
  7 - Sunday
*/
