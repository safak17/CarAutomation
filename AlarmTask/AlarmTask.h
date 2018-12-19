#ifndef ALARMTASK_H
#define ALARMTASK_H

/*                MASKS                 */
#define MASK_ALARM_ID           0xFF000000      //  8 bit

#define MASK_RELAY_4            0x00C00000      //  2 bit
#define MASK_RELAY_3            0x00300000      //  2 bit
#define MASK_RELAY_2            0x000C0000      //  2 bit
#define MASK_RELAY_1            0x00030000      //  2 bit

#define MASK_REPEAT             0x0000C000      //  2 bit
#define MASK_DAY_OF_WEEK        0x00003800      //  3 bit
#define MASK_HOUR               0x000007C0      //  5 bit
#define MASK_MINUTE             0x0000003F      //  6 bit

#define MASK_DAY_TIME           0x000004FF


/*              DIGIT_VALUE             */
#define DIGIT_VALUE_ID          0x01000000

#define DIGIT_VALUE_RELAY_4     0x00400000
#define DIGIT_VALUE_RELAY_3     0x00100000
#define DIGIT_VALUE_RELAY_2     0x00040000
#define DIGIT_VALUE_RELAY_1     0x00010000

#define DIGIT_VALUE_REPEAT      0x00004000
#define DIGIT_VALUE_DAY_OF_WEEK 0x00000800
#define DIGIT_VALUE_HOUR        0x00000040
#define DIGIT_VALUE_MINUTE      0x00000001

//  typedef enum { NO_REPEAT, EVERY_DAY, EVERY_WEEK, INVALID } enumRepeat_t;
//  typedef enum { DONT_CARE, ON, OFF, TOGGLE } enumRelayStatus_t;

class AlarmTask{
private:
    uint32_t alarm;
    uint32_t onesComplement(uint32_t number){ return (0xFFFFFFFF - number) ;}
    
public:
    AlarmTask (uint32_t alarmDescription ):alarm(alarmDescription){}
    uint32_t getAlarm()      const   {  return alarm; }
    uint16_t id()            const   {  return ( alarm & MASK_ALARM_ID  ) / DIGIT_VALUE_ID ; }
    
    uint16_t relay4()        const   {  return ( alarm & MASK_RELAY_4   ) / DIGIT_VALUE_RELAY_4 ; }
    uint16_t relay3()        const   {  return ( alarm & MASK_RELAY_3   ) / DIGIT_VALUE_RELAY_3 ; }
    uint16_t relay2()        const   {  return ( alarm & MASK_RELAY_2   ) / DIGIT_VALUE_RELAY_2 ; }
    uint16_t relay1()        const   {  return ( alarm & MASK_RELAY_1   ) / DIGIT_VALUE_RELAY_1 ; }
    
    uint16_t repeat()        const   {  return ( alarm & MASK_REPEAT    ) / DIGIT_VALUE_REPEAT ; }
    uint16_t dayWeek()       const   {  return ( alarm & MASK_DAY_OF_WEEK)/ DIGIT_VALUE_DAY_OF_WEEK ; }
    uint16_t hour()          const   {  return ( alarm & MASK_HOUR      ) / DIGIT_VALUE_HOUR ; }
    uint16_t minute()        const   {  return ( alarm & MASK_MINUTE    ) / DIGIT_VALUE_MINUTE ; }
    
    
    void setAlarm(uint32_t alarm){
        this->alarm = alarm;
    }
    
    void setId( uint16_t id ) {
        alarm &= onesComplement( MASK_ALARM_ID );       //  Previous id is cleared.
        alarm += id * DIGIT_VALUE_ID;                   //  The new one is set.
    }
    
    
    
    
    void setRelay4(uint16_t relay4) {
        alarm &= onesComplement( MASK_RELAY_4 );
        alarm += relay4 * DIGIT_VALUE_RELAY_4;
    }
    void setRelay3(uint16_t relay3) {
        alarm &= onesComplement( MASK_RELAY_3 );
        alarm += relay3 * DIGIT_VALUE_RELAY_3;
    }
    void setRelay2(uint16_t relay2) {
        alarm &= onesComplement( MASK_RELAY_2 );
        alarm += relay2 * DIGIT_VALUE_RELAY_2;
    }
    void setRelay1(uint16_t relay1) {
        alarm &= onesComplement( MASK_RELAY_1 );
        alarm += relay1 * DIGIT_VALUE_RELAY_1;
    }
    
    
    
    void setRepeat(uint16_t repeat){
        alarm &= onesComplement( MASK_REPEAT );
        alarm += repeat * DIGIT_VALUE_REPEAT;
    }
    void setDayOfWeek(uint16_t dayWeek){
        alarm &= onesComplement( MASK_DAY_OF_WEEK );
        alarm += dayWeek * DIGIT_VALUE_DAY_OF_WEEK;
    }
    void increaseDayByOne(){
        uint16_t increasedDay = (dayWeek() % 7) + 1;
        setDayOfWeek( increasedDay );
    }
    void setHour(uint16_t hour){
        alarm &= onesComplement( MASK_HOUR );
        alarm += hour * DIGIT_VALUE_HOUR;
    }
    void setMinute(uint16_t minute){
        alarm &= onesComplement( MASK_MINUTE );
        alarm += minute * DIGIT_VALUE_MINUTE;
    }

    
    
    bool operator< (const AlarmTask& rhs) const {
        uint16_t rhsDayTime = rhs.getAlarm() & MASK_DAY_TIME;
        uint16_t lhsDayTime = alarm & MASK_DAY_TIME;
        return ( lhsDayTime < rhsDayTime );
    }
    
    bool operator<= (const AlarmTask& rhs) const {
        uint16_t rhsDayTime = rhs.getAlarm() & MASK_DAY_TIME;
        uint16_t lhsDayTime = alarm & MASK_DAY_TIME;
        return ( lhsDayTime <= rhsDayTime );
    }
    
    bool operator== (const AlarmTask& rhs) const {
        return id() == rhs.id();
    }
    
    bool operator!= (const AlarmTask& rhs) const{
        return !( *this == rhs );
    }
    
    bool operator>= (const AlarmTask& rhs) const{
        return !(*this < rhs );
    }
    
    bool operator> (const AlarmTask& rhs) const{
        return !(*this <= rhs);
    }
    
    bool isSameTime(const AlarmTask& rhs) const{
        uint16_t rhsDayTime = rhs.getAlarm() & MASK_DAY_TIME;
        uint16_t lhsDayTime = alarm & MASK_DAY_TIME;
        return ( lhsDayTime == rhsDayTime );
    }
    
    String descriptionAsString(){
        return ("id: " +        String( id()                ) + " " +
                "relay4: " +    String( relay4()            ) + " " +
                "relay3: " +    String( relay3()            ) + " " +
                "relay2: " +    String( relay2()            ) + " " +
                "relay1: " +    String( relay1()            ) + " " +
                "repeat: " +    String( repeat()            ) + " " +
                "dayWeek: " +   String( dayWeek()           ) + " " +
                "hour: " +      String( hour()              ) + " " +
                "minute: " +    String( minute()            ));
    }
    
    
};

#endif
