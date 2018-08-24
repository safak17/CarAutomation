#include </Users/safakakinci/Documents/Arduino/libraries/CircularList/CircularList.cpp>
#include <SoftwareSerial.h>                                         //  Bluetooth modülün bağlı olduğu (0,1) (RX,TX) bacaklarından gelen veriyi de 9600 ayarlabilmek için.
#include <DS3232RTC.h>                                              //  https://github.com/JChristensen/DS3232RTC
#include <AlarmTask.h>                                              //  Alarmlı Görev oluşturabilmek için.
#include <RelayModule.h>                                            //  Röle modülü kontrol etmek için.
#include <avr/power.h>                                              //  Güç tüketimini azaltmak için gerekli bazı fonksiyonlar...
#include <avr/sleep.h>                                              //  Sleep mode fonksiyonlarını kullanabilmek için.

#define ARDUINO_ANALOG_CURRENT              3                       //  Akım değeri,      Arduino'nun Analog 3 pininden gelecek.
#define ARDUINO_ANALOG_VOLTAGE              4                       //  Voltaj değeri,    Arduino'nun Analog 4 pininden gelecek.
#define ARDUINO_ANALOG_TEMPERATURE          5                       //  Sıcaklık değeri,  Arduino'nun Analog 5 pininden gelecek.

#define SMOOTH_CALCULATION_FREQUENCY_NUMBER 8                       //  SmoothCalculation fonksiyonunda 8 tane ölçüm yapılıyor.

String                          receivedBluetoothData = "";         //  Android tarafından gönderilen veriyi tutmak için...

SoftwareSerial                  BluetoothSerial(0, 1);              //  Arduino'nun 0 bacağı RX; Arduino'nun 1 bacağı TX
RelayModule                     module(4, LOW);                     //  Relay kartında dört tane rölenin olduğunu ve bu kartın ACTIVE LOW ile çalıştığını belirtiyoruz.
CircularList<AlarmTask> alarmList;                                  //  Alarmları sıralı bir şekilde içinde tutacak liste.
CircularList<AlarmTask>::iterator currentAlarmIterator;


void setup()
{
  Serial.begin(9600);                                               //  9600bps hız ile RX ve TX bacaklarından veri gelecek.
  BluetoothSerial.begin(9600);                                      //  9600bps hız ile RX ve TX bacaklarından veri gelecek.

  pinMode(LED_BUILTIN, OUTPUT);                                     //  Arduino uyanıksa 13. led yanacak.
  digitalWrite(LED_BUILTIN, HIGH);                                  //  Arduino uyanıksa 13. led yanacak.

  setRelayModule();                                                 //  Pinleri ayarlar ve başlangıçta röleleri kapatır.
  SetInterruptPin2ForRTC();                                         //  RTC'dan interrupt gelebilmesi için Digital2 pini ayarlanır.
  SetDefaultValuesOfRTC();                                          //  Alarm Registerlarına default değerler yüklendi.
  SleepNow();
}

void loop()
{

  if ( RTC.alarm( ALARM_2 ) )
  {
    AlarmTask& triggeredAlarm = currentAlarmIterator.current().info();

    do
    {
      AlarmTask& triggeredAlarm  =   currentAlarmIterator.current().info();         //  Tetiklenen alarmın bilgilerini al.  (Referans olarak, kopyalamaya gerek yok.)
      RelayOperate( triggeredAlarm.relayNumber(), triggeredAlarm.relayStatus() );   //  Gerekli röle işlemlerini yap.

      if ( ! triggeredAlarm.isRepeat() )  alarmList.remove( currentAlarmIterator.current().info() );    //  Tekrar etmeyen alarm ise, listeden çıkar.

      currentAlarmIterator++;

    } while ( triggeredAlarm.isSameTime( currentAlarmIterator.current().info() ));


    UpdateAlarm2RegisterOfRTC();                                                        //  ... ve alarm registerini güncelle.

    SleepNow();
  }// if ( RTC.alarm( ALARM_2 ) )
}// loop()

void serialEvent()                                                                      //  Receive serial buffer'da veri varsa, otomatik olarak bu fonksiyon çağrılır.
{
  receivedBluetoothData = Serial.readString();                                          //  Tampondaki verileri oku ve receivedBluetoothData içine kaydet.

  if( !receivedBluetoothData.endsWith(" ;") )
    Serial.println("ERROR: RECEIVED_DATA_FORMAT ;");

  while ( receivedBluetoothData.endsWith(" ;") )                                         //  Birden fazla komut tek satırda gelebilir.
  {
    if( receivedBluetoothData.startsWith("r") )                                     //  Eğer receivedBluetoothData'deki veri "r" ile başlıyorsa...    (r)elay
    {
      if( receivedBluetoothData.charAt(1) == 'o' )  //  ro 3 1;
      {
        uint8_t relayNumber =   (receivedBluetoothData.charAt(3) - '0');  //  In order to cast character to integer, we substracted '0' from character.
        uint8_t relayStatus =   (receivedBluetoothData.charAt(5) - '0');  //  In order to cast character to integer, we substracted '0' from character.
        RelayOperate( relayNumber, relayStatus );
      }
      else if (receivedBluetoothData.charAt(1) == 's' )
        RelaysStatus();
      else
        Serial.println("ERROR: RELAY_FORMAT ;");
    }// receivedBluetoothData.startsWith("r")

    else if   ( receivedBluetoothData.startsWith("p") )                                     //  Eğer receivedBluetoothData'deki veri "c" ile başlıyorsa...    (c)lock
      PeripheralGet();

    else if ( receivedBluetoothData.startsWith("c") )                                     //  Eğer receivedBluetoothData'deki veri "c" ile başlıyorsa...    (c)lock
    {
      if (receivedBluetoothData.charAt(1) == 's' )
      {
        String DateTime = receivedBluetoothData.substring(3, receivedBluetoothData.indexOf(" ;"));
        ClockSet( DateTime );
      }
      else if (receivedBluetoothData.charAt(1) == 'g' )
        ClockGet();
      else
        Serial.println("ERROR: CLOCK_FORMAT ;");
    }// receivedBluetoothData.startsWith("c")

    else if ( receivedBluetoothData.startsWith("a") )                                     //  Eğer receivedBluetoothData'deki veri "a" ile başlıyorsa...    (a)larm
    {
      if (receivedBluetoothData.charAt(1) == 's' )
      {
        String AlarmDescription = receivedBluetoothData.substring(3, receivedBluetoothData.indexOf(";"));
        AlarmSet( AlarmDescription );             //  AlarmSet( AlarmDescription );
      }
        

      else if (receivedBluetoothData.charAt(1) == 'd' )
        AlarmDisarm( receivedBluetoothData.substring(2, receivedBluetoothData.indexOf(";")).toInt() );  //  AlarmDisarm( alarmId );

      else if (receivedBluetoothData.charAt(1) == 'l' )
        AlarmList();

      else
        Serial.println("ERROR: ALARM_FORMAT ;");
    }// receivedBluetoothData.startsWith("a")

    receivedBluetoothData = receivedBluetoothData.substring( receivedBluetoothData.indexOf(";") + 1);
  }//end while ( receivedBluetoothData.endsWith(" ;") )

  SleepNow();
}//end serialEvent()


void SetInterruptPin2ForRTC()
{
  pinMode(2, INPUT_PULLUP);
  attachInterrupt( digitalPinToInterrupt( 2 ), alarmFunction, CHANGE );
}

void SleepNow()
{
  delay(200);                       //  CPU'nun uyuyabilmesi için gerekli olan KRİTİK bekleme.
  set_sleep_mode(SLEEP_MODE_IDLE);  // Sleep Mode is set to SLEEP_MODE_IDLE
  sleep_enable();                   // Enables the sleep bit in the mcucr register

  power_adc_disable();
  power_spi_disable();
  power_timer0_disable();
  power_timer1_disable();
  power_timer2_disable();
  power_twi_disable();

  digitalWrite(LED_BUILTIN, LOW);   //  Cihazın uykuya daldığını göstermek için.
  sleep_mode();                     // Here, the device is actually put to sleep!
  digitalWrite(LED_BUILTIN, HIGH);  //  Cihazın uyumadığını göstermek için.

  sleep_disable();                  // First thing after waking from sleep:  disable sleep...
  power_all_enable();
}






/***************                Relay                ***************/
void setRelayModule()
{
  module[1].setPinNumber(7).setPinMode(OUTPUT);       //  Relay1 (IN1), Arduino'nun Digital 7 pinine bağlı.
  module[2].setPinNumber(6).setPinMode(OUTPUT);       //  Relay2 (IN2), Arduino'nun Digital 6 pinine bağlı.
  module[3].setPinNumber(5).setPinMode(OUTPUT);       //  Relay3 (IN3), Arduino'nun Digital 5 pinine bağlı.
  module[4].setPinNumber(4).setPinMode(OUTPUT);       //  Relay4 (IN4), Arduino'nun Digital 4 pinine bağlı.
  module.deactiveAll();                               //  Başlangıçta bütün rölelerin kapatılması gerekir, kararsız durumda bırakmamak için.
}

void RelayOperate( uint8_t relayNumber, uint8_t relayStatus )
{
  if ( relayStatus )
  {
    if ( module.active( relayNumber ) )   {   Serial.println("OK: RELAY_OPERATE " + String(relayNumber) + " ;");      }
    else                                  {   Serial.println("ERROR: RELAY_OPERATE " + String(relayNumber) + " ;");   }
  }
  else
  {
    if ( module.deactive( relayNumber ) ) {   Serial.println("OK: RELAY_OPERATE " + String(relayNumber) + " ;");      }
    else                                  {   Serial.println("ERROR: RELAY_OPERATE " + String(relayNumber) + " ;");   }
  }

}//end RelayOperate()

void RelaysStatus()
{
  Serial.println("OK: RELAYS_STATUS " + String(module.getStatus()) + " ;" );
}





/***************                Peripheral                ***************/
void PeripheralGet()
{
  String feeder;
  feeder  +=  String(module.getStatus())  + " ";            //Relay's Status
  feeder  +=  getCurrentValue()           + " ";            //Analog Current Value
  feeder  +=  getVoltageValue()           + " ";            //Analog Voltage Value
  feeder  +=  getTemperatureValue();      + " ";            //Analog Temperature Value

  Serial.println("OK: PERIPHERAL_GET "+ feeder +" ;" );
}//end PeripheralGet()

String getCurrentValue()
{
  analogReference(DEFAULT);                 //Configures the reference voltage used for analog input, Arduino Uno 5V
  return String ( analogRead(ARDUINO_ANALOG_CURRENT) );
}//end getCurrentValue()

String getVoltageValue()
{
  analogReference(DEFAULT);                 //Configures the reference voltage used for analog input, Arduino Uno 5V
  return String( smoothCalculation() );
}//end getVoltageValue()

String getTemperatureValue()
{
  analogReference( INTERNAL );
  return String( analogRead(ARDUINO_ANALOG_TEMPERATURE) );
}//end getTemperatureValue()

// 50ms'de bir voltaj ölçümü yapar ve 8 tane ölçüm yaptıktan sonra bunların ortalamasını alır ve geri döndürür.
float smoothCalculation()
{
  float total = 0;

  for ( int i = 0; i < SMOOTH_CALCULATION_FREQUENCY_NUMBER; ++i)      // ++i,   increases i and returns i.
  {
    total += analogRead(ARDUINO_ANALOG_VOLTAGE);
    delay(50);
  }

  return total / 8;
}//end smoothCalculation()






/***************                RealTimeClock                ***************/
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

void SetAlarm2Register (uint8_t minute, uint8_t hour, uint8_t daydate)
{
  RTC.setAlarm(ALM2_MATCH_DAY, minute, hour , daydate);  //  causes an alarm when the day of the week and hours and minutes match.
  RTC.alarm(ALARM_2);                                 //  Alarm2 flagi temizlendi.
  RTC.alarmInterrupt(ALARM_2, true);                  //  Interrupt üretecek.
}

//  DateTime  =   "2018 01 01 00 00 00"
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

void ClockSet( String& DateTime )
{
  bool isSet = RTC.set( getDateTimeAsSeconds( DateTime ) );
  if( isSet == 0 )
    Serial.println("OK: CLOCK_SET ;");
  else
    Serial.println("ERROR: CLOCK_SET ;");
}

void ClockGet()
{
  tmElements_t  rtcTime;        RTC.read( rtcTime );

  String RTCTime =  String( rtcTime.Year + 1970 ) + " " +
                    String( rtcTime.Month       ) + " " +
                    String( rtcTime.Day         ) + " " +
                    String( rtcTime.Hour        ) + " " +
                    String( rtcTime.Minute      ) + " " +
                    String( rtcTime.Second      ) ;

  Serial.println("OK: CLOCK_GET "+ RTCTime +" ;" );
  //Serial.println("DAY:" + DayDate( rtcTime.Wday ));
}

String DayDate( int timelibFormatDay )
{
  int dayDate;

  if ( 1 == timelibFormatDay ) dayDate = 7;
  else                         dayDate = timelibFormatDay - 1;

  switch ( dayDate )
  {
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





/***************                Alarm                ***************/
void AlarmSet( String& alarmDescription )
{
  AlarmTask newAlarm ( alarmDescription );        //  Yeni alarm oluşturulacak.

  if( alarmList.push_sorted( newAlarm ) )   Serial.println("OK: ALARM_SET ;" );
  else                                      Serial.println("ERROR: ALARM_SET ;" );

  if ( alarmList.size() == 1 )
  {
    currentAlarmIterator = alarmList.begin();     //  Şu anki alarm olarak listenin başı tutuluyor.
    UpdateAlarm2RegisterOfRTC();
  }
}

void AlarmDisarm( uint8_t alarmId )
{
  AlarmTask tempAlarm( alarmId );
  if( alarmList.remove( tempAlarm ) )   Serial.println("OK: ALARM_DISARM " + String(alarmId) + " ;" );
  else                                  Serial.println("ERROR: ALARM_DISARM " + String(alarmId) + " ;" );

  UpdateAlarm2RegisterOfRTC();
}

void AlarmList()
{
  Serial.println("OK: ALARM_LIST " + String( alarmList.size()) + " ;");

  CircularList<AlarmTask>::iterator it;
  for ( it = alarmList.begin();  it.position() < alarmList.size(); it++  )
    Serial.println( it.current().info().getDescription() );

}

void alarmFunction() {}

void UpdateAlarm2RegisterOfRTC()
{
  if ( alarmList.size() != 0 )
  {
    AlarmTask& firstAlarm = currentAlarmIterator.current().info();
    SetAlarm2Register( firstAlarm.minute(),  firstAlarm.hour(), castDayToTimelibFormat(firstAlarm.day()) );
  }
  else
    SetDefaultValuesOfRTC();
}

/*
  In Turkey, week starts from Monday. We need to cast Turkey standart to the Timelib format.
  1 - Monday
  2 - Tuesday
  3 - Wednesday
  4 - Thursday
  5 - Friday
  6 - Saturday
  7 - Sunday

  Below, you see one part of the Timelib library. Here, week starts from Sunday.
  Timelib.h
  typedef enum {
  0           1          2          3           4             5           6           7
  dowInvalid, dowSunday, dowMonday, dowTuesday, dowWednesday, dowThursday, dowFriday, dowSaturday
  } timeDayOfWeek_t;

  According to the Timelib.h, week starts from Sunday, but here In Turkey, week starts from Monday.
  We need to switch between these two formats. Because, RTC will be programmed in Timelib.h format.
 */
uint16_t castDayToTimelibFormat( uint16_t day )
{
    return ( day == 7 ) ? 1 : day + 1;
}
