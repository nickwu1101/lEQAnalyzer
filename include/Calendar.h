#ifndef Calendar_h
#define Calendar_h 1

#include <iostream>

using namespace std;

typedef struct Duration Duration;

struct Duration {
    int sec;
    double ms;
};

class Calendar {
public:
    Calendar();
    Calendar(const Calendar&);
    Calendar(string dateTime);
    Calendar(int year, int month, int mDay, int hour, int minute, double second);
    ~Calendar();

    void      addDuration(int, int, int, int, double);
    double    calculateDurationInSec();
    Calendar* clone() { return this; };
    void      setDate(string);
    void      setDateTime(string);
    void      setTime(string);
    void      setYear(int);
    void      setMonth(int);
    void      setMDay(int);
    void      setHour(int hour)  { chour = hour; };
    void      setMinute(int min) { cmin = min; };
    void      setSecond(double sec) { csec = sec; };
    void      setYDay(int);
    string    getDate();
    string    getDateTime();
    string    getDateTime_ms();
    string    getTime();
    string    getTime_ms();
    int       getYear()   { return cyear; };
    int       getMonth()  { return cmon; };
    int       getMDay()   { return cmday; };
    int       getHour()   { return chour; };
    int       getMinute() { return cmin; };
    double    getSecond() { return csec; };
    int       getYDay()   { return cyday; };

    Duration  operator- (const Calendar&);
    bool      operator==(const Calendar&);
    bool      operator!=(const Calendar&);    
    bool      operator> (const Calendar&);
    bool      operator>=(const Calendar&);
    bool      operator< (const Calendar&);
    bool      operator<=(const Calendar&);

private:
    static int daysOfYear;
    static int daysOfMonth[12];
    
    int    cyear;
    int    cmon;
    int    cmday;
    int    chour;
    int    cmin;
    double csec;
    int    cyday;
    int    cwday;

    void   yDay2MD();
    void   MD2YDay();
    double calculateMillisecond();
    void   calculateWeekday();

    bool   checkValid();
    bool   checkYearValid();
    bool   checkMonValid();
    bool   checkMDayValid();
    bool   checkHourValid();
    bool   checkMinValid();
    bool   checkSecValid();
    bool   checkYDayValid();
};

#endif
