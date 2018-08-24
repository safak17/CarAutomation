#define REPEAT_MASK 0x8000
#define DAY_OF_WEEK_MASK 0x7000
#define HOUR_MASK 0x0F80
#define MINUTE_MASK 0x0070
#define RELAY_NUMBER_MASK 0x000E
#define STATUS_MASK 0x0001

#define DIGIT_VALUE_REPEAT 32768            //  2^15
#define DIGIT_VALUE_DAY_OF_WEEK 4096        //  2^12
#define DIGIT_VALUE_HOUR 128                //  2^7
#define DIGIT_VALUE_MINUTE 16               //  2^4
#define DIGIT_VALUE_RELAY_NUMBER 2          //  2^1

class AlarmTask{
private:
    
    uint16_t    alarmId;
    uint16_t    alarmTask;
    
public:
    AlarmTask( uint16_t alarmId ):alarmTask(0), alarmId(alarmId){};
    
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
    AlarmTask (String alarmDescription):alarmTask(0)
    {
        String delimiter = " ";
        uint16_t parameters[7];
        
        size_t start = 0;
        size_t end = alarmDescription.indexOf(delimiter);
        for(int i=0; i<7; i++)
        {
            parameters[i] = (alarmDescription.substring(start, end)).toInt();
            
            start = end + 1;
            end = alarmDescription.indexOf(delimiter, start);
        }
        
        alarmId    = parameters[0];                                   //  alarmId
        alarmTask += parameters[1] * DIGIT_VALUE_REPEAT;              //  repeat
        alarmTask += parameters[2] * DIGIT_VALUE_DAY_OF_WEEK;         //  dayOfWeek
        alarmTask += parameters[3] * DIGIT_VALUE_HOUR;                //  hour
        alarmTask += parameters[4] / 10 * DIGIT_VALUE_MINUTE;         //  minute: User can only set the alarm multiplies of ten.
        alarmTask +=(parameters[5] - 1) * DIGIT_VALUE_RELAY_NUMBER;   //  relayNumber: Relay numbers can be from 1 to 8, but we can store from 0 to 7 in 3 bits.
        alarmTask += parameters[6];                                   //  relayStatus
    }
    
    void     setId( uint16_t id )    {  alarmId = id;                                                                 }
    uint16_t getId()         const   {   return ( alarmId );                                                          }
    
    uint16_t isRepeat()      const   {   return ( alarmTask & REPEAT_MASK )         / DIGIT_VALUE_REPEAT;             }
    uint16_t day()           const   {   return ( alarmTask & DAY_OF_WEEK_MASK )    / DIGIT_VALUE_DAY_OF_WEEK;        }
    uint16_t hour()          const   {   return ( alarmTask & HOUR_MASK )           / DIGIT_VALUE_HOUR;               }
    uint16_t minute()        const   {   return ( alarmTask & MINUTE_MASK )         / DIGIT_VALUE_MINUTE * 10;        }
    uint16_t relayNumber()   const   {   return ((alarmTask & RELAY_NUMBER_MASK)    / DIGIT_VALUE_RELAY_NUMBER) + 1;  }
    uint16_t relayStatus()   const   {   return ( alarmTask & STATUS_MASK );                                          }
    
    
    String getDescription()
    {
        return (  String( alarmId         ) + " " +
                String( isRepeat()      ) + " " +
                String( day()           ) + " " +
                String( hour()          ) + " " +
                String( minute()        ) + " " +
                String( relayNumber()   ) + " " +
                String( relayStatus()   ) + " ;");
    }
    
    bool operator< (const AlarmTask& rhs) const
    {
        if( day() < rhs.day() )
            return true;
        
        if( day() == rhs.day() && hour() < rhs.hour() )
            return true;
        
        if( day() == rhs.day() && hour() == rhs.hour() && minute() < rhs.minute() )
            return true;
        
        return false;
    }
    
    bool operator== (const AlarmTask& rhs) const
    {
        return ( alarmId == rhs.alarmId );
    }
    
    bool operator!= (const AlarmTask& rhs) const
    {
        return !(  (*this) == rhs  );
    }
    
    bool isSameTime(const AlarmTask& rhs) const
    {
        if ( &rhs == NULL) return false;
        
        return  (day()        ==   rhs.day()     &&
                 hour()       ==   rhs.hour()    &&
                 minute()     ==   rhs.minute()   )?     true    :   false;
    }
};
