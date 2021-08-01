#include "Calendar.h"

#include <math.h>

int Calendar::daysOfYear = 365;
int Calendar::daysOfMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };



Calendar::Calendar() {}



Calendar::Calendar(string dateTime) {
    setDateTime(dateTime);
}



Calendar::Calendar(int year, int month, int mDay, int hour, int minute, double second) {    
    cyear = year;
    cmon = month;
    cmday = mDay;
    chour = hour;
    cmin = minute;
    csec = second;

    MD2YDay();

    //cout << cyday << endl << checkValid() << endl;
}



Calendar::~Calendar() {}



void Calendar::addDuration(int addedYear, int addedDay, int addedHour, int addedMin, double addedSec) {
    csec += addedSec;
    while(!checkSecValid()) {
	if(csec > 0) {
	    csec -= 60;
	    cmin++;
	} else if(csec < 0) {
	    cmin--;
	    csec += 60;
	}
    }

    cmin += addedMin;
    while(!checkMinValid()) {
	if(cmin > 0) {
	    cmin -= 60;
	    chour++;
	} else if(cmin < 0) {
	    chour--;
	    cmin += 60;
	}
    }

    chour += addedHour;
    while(!checkHourValid()) {
	if(chour > 0) {
	    chour -= 24;
	    cyday++;
	} else if(chour < 0) {
	    cyday--;
	    chour += 24;
	}
    }

    cyday += addedDay;
    while(!checkYDayValid()) {
	if(cyday > 0) {
	    if(cyear % 4 == 0)
		cyday -= daysOfYear + 1;
	    else
		cyday -= daysOfYear;

	    cyear++;
	} else if(cyday <= 0) {
	    cyear--;
	    if(cyear % 4 == 0)
		cyday += daysOfYear + 1;
	    else
		cyday += daysOfYear;
	}
    }

    cyear += addedYear;

    yDay2MD();
}



void Calendar::setDate(string date) {
    cyear = atoi(date.substr(0, 4).c_str());
    cmon  = atoi(date.substr(4, 2).c_str());
    cmday = atoi(date.substr(6, 2).c_str());
    MD2YDay();
}



void Calendar::setDateTime(string dateTime) {
    string date = dateTime.substr(0, 8);
    string time = dateTime.substr(8, 6);
    setDate(date);
    setTime(time);
}



void Calendar::setTime(string time) {
    chour = atoi(time.substr(0, 2).c_str());
    cmin  = atoi(time.substr(2, 2).c_str());
    csec  = atof(time.substr(4, 2).c_str());
}



void Calendar::setYear(int year) {
    cyear = year;
}



void Calendar::setMonth(int month) {
    cmon = month;
    MD2YDay();
}



void Calendar::setMDay(int mDay) {
    cmday = mDay;
    MD2YDay();
}



void Calendar::setYDay(int yDay) {
    cyday = yDay;
    yDay2MD();
}



string Calendar::getDate() {
    char str[9];
    sprintf(str, "%04d%02d%02d", cyear, cmon, cmday);
    return str;
}



string Calendar::getDateTime() {
    char str[15];
    sprintf(str, "%04d%02d%02d%02d%02d%02.0f", cyear, cmon, cmday, chour, cmin, floor(csec));
    return str;
}



string Calendar::getDateTime_ms() {
    char str[18];
    sprintf(str, "%04d%02d%02d%02d%02d%02.0f%03.0f", cyear, cmon, cmday, chour, cmin, floor(csec), floor(calculateMillisecond()));
    return str;
}



string Calendar::getTime() {
    char str[7];
    sprintf(str, "%02d%02d%02.0f", chour, cmin, floor(csec));
    return str;
}



string Calendar::getTime_ms() {
    char str[11];
    sprintf(str, "%02d%02d%02.0f%03.0f", chour, cmin, floor(csec), floor(calculateMillisecond()));
    return str;
}



void Calendar::yDay2MD() {
    int calDay = cyday;
    if(checkYDayValid()) {
	for(int i = 0; i < 12; i++) {
	    if(i == 1 && cyear % 4 == 0) {
		if(calDay > daysOfMonth[i] + 1) {
		    calDay -= daysOfMonth[i] + 1;
		} else {
		    cmon = i + 1;
		    cmday = calDay;
		    break;
		}
	    } else {
		if(calDay > daysOfMonth[i]) {
		    calDay -= daysOfMonth[i];
		} else {
		    cmon = i + 1;
		    cmday = calDay;
		    break;
		}
	    }
	}
    }
}



void Calendar::MD2YDay() {
    cyday = 0;
    cyday += cmday;
    for(int i = 1; i < cmon; i++)
	if(i == 2 && cyear % 4 == 0)
	    cyday += daysOfMonth[i - 1] + 1;
	else
	    cyday += daysOfMonth[i - 1];
}



double Calendar::calculateMillisecond() {
    return 1000*(csec - floor(csec));
}



void Calendar::calculateWeekday() {
}



bool Calendar::checkValid() {
    if(checkYearValid() &&
       checkMonValid() &&
       checkMDayValid() &&
       checkHourValid() &&
       checkMinValid() &&
       checkSecValid() &&
       checkYDayValid())
	return true;
    else
	return false;
};



bool Calendar::checkYearValid() {
    return true;
}



bool Calendar::checkMonValid() {
    if(cmon > 0 && cmon <= 12)
	return true;
    else
	return false;
}



bool Calendar::checkMDayValid() {
    if(cmon == 2 && cyear % 4 == 0) {
	if(cmday > 0 && cmday <= daysOfMonth[cmon - 1] + 1)
	    return true;
	else
	    return false;
    } else {
	if(cmday > 0 && cmday <= daysOfMonth[cmon - 1])
	    return true;
	else
	    return false;
    }
}



bool Calendar::checkHourValid() {
    if(chour >= 0 && chour < 24)
	return true;
    else
	return false;
}



bool Calendar::checkMinValid() {
    if(cmin >= 0 && cmin < 60)
	return true;
    else
	return false;
}



bool Calendar::checkSecValid() {
    if(csec >= 0 && csec < 60)
	return true;
    else
	return false;
}



bool Calendar::checkYDayValid() {
    int totalDays = 0;
    if(cyear % 4 == 0) totalDays = daysOfYear + 1;
    else totalDays = daysOfYear;

    if(cyday > 0 && cyday <= totalDays)
	return true;
    else
	return false;
}



Duration Calendar::operator-(const Calendar &c2) {
    Duration duration = { 0, 0. };
    int dyear = cyear;
    int dyday = cyday;
    int dhour = chour;
    int dmin = cmin;
    double dsec = csec;

    if(cyear > c2.cyear) {
	while(dyear > c2.cyear) {
	    dyear--;
	    if(dyear % 4 == 0)
		dyday += daysOfYear + 1;
	    else
		cyday += daysOfYear;
	}
    } else if(cyear < c2.cyear) {
	while(dyear < c2.cyear) {
	    if(dyear % 4 == 0)
		dyday -= daysOfYear + 1;
	    else
		dyday -= daysOfYear;

	    dyear += daysOfYear;
	}
    } else {}

    dyday -= c2.cyday;
    dhour -= c2.chour;
    dmin -= c2.cmin;
    dsec -= c2.csec;
    int dsecI = floor(dsec);
    double dms = 1000*(dsec - floor(dsec));

    duration.sec = dsecI + 60*dmin + 60*60*dhour + 60*60*24*dyday;
    duration.ms = dms;

    return duration;
}



bool Calendar::operator==(const Calendar &c2) {
    return cyear == c2.cyear
	&& cmon == c2.cmon
	&& cmday == c2.cmday
	&& chour == c2.chour
	&& cmin == c2.cmin
	&& csec == c2.csec;
}



bool Calendar::operator!=(const Calendar &c2) {
    return cyear != c2.cyear
	|| cmon != c2.cmon
	|| cmday != c2.cmday
	|| chour != c2.chour
	|| cmin != c2.cmin
	|| csec != c2.csec;
}



bool Calendar::operator>(const Calendar &c2) {
    if(cyear > c2.cyear)
	return true;
    else if(cyear < c2.cyear)
	return false;
    else if(cyear == c2.cyear) {
	if(cmon > c2.cmon)
	    return true;
	else if(cmon < c2.cmon)
	    return false;
	else if(cmon == c2.cmon) {
	    if(cmday > c2.cmday)
		return true;
	    else if(cmday < c2.cmday)
		return false;
	    else if(cmday == c2.cmday) {
		if(chour > c2.chour)
		    return true;
		else if(chour < c2.chour)
		    return false;
		else if(chour == c2.chour) {
		    if(cmin > c2.cmin)
			return true;
		    else if(cmin < c2.cmin)
			return false;
		    else if(cmin == c2.cmin) {
			if(csec > c2.csec)
			    return true;
			else if(csec <= c2.csec)
			    return false;
			else return false;
		    } else return false;
		} else return false;
	    } else return false;
	} else return false;
    } else return false;
}



bool Calendar::operator>=(const Calendar &c2) {
    return *this > c2 || *this == c2;
}



bool Calendar::operator<(const Calendar &c2) {
    if(cyear < c2.cyear)
	return true;
    else if(cyear > c2.cyear)
	return false;
    else if(cyear == c2.cyear) {
	if(cmon < c2.cmon)
	    return true;
	else if(cmon > c2.cmon)
	    return false;
	else if(cmon == c2.cmon) {
	    if(cmday < c2.cmday)
		return true;
	    else if(cmday > c2.cmday)
		return false;
	    else if(cmday == c2.cmday) {
		if(chour < c2.chour)
		    return true;
		else if(chour > c2.chour)
		    return false;
		else if(chour == c2.chour) {
		    if(cmin < c2.cmin)
			return true;
		    else if(cmin > c2.cmin)
			return false;
		    else if(cmin == c2.cmin) {
			if(csec < c2.csec)
			    return true;
			else if(csec >= c2.csec)
			    return false;
			else return false;
		    } else return false;
		} else return false;
	    } else return false;
	} else return false;
    } else return false;
}



bool Calendar::operator<=(const Calendar &c2) {
    return *this < c2 || *this == c2;
}
