#ifndef TempHumi_h
#define TempHumi_h 1

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

class Calendar;

class TempHumi {
public:
    TempHumi();
    ~TempHumi();

    void execute();
    void test();

    void makeTempHumiPlotting();

    void setEndDateTime(string);
    void setStartDateTime(string);
    void setTimeLengthOfGraph(string inputStr) { timeLengthOfGraph = inputStr; };
    void setTimeUnit(string inputStr) { timeUnit = inputStr; };

private:
    vector<string> dataList;
    map<string, double> temp;
    map<string, double> humi;

    Calendar *startDateTime;
    Calendar *endDateTime;
    string    timeLengthOfGraph;
    string    timeUnit;

    double    calAvgTemp(string);
    double    calAvgHumi(string);
    void      doTempHumiMap();
    void      assignTimeInterval();
    void      prepareDataList();
};

#endif
