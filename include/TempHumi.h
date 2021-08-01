#ifndef TempHumi_h
#define TempHumi_h 1

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

class Calendar;
class TGraph;

class TempHumi {
public:
    TempHumi();
    ~TempHumi();

    void execute();
    void test();

    void makeTempHumiPlotting();
    void printCanvas();

    double getAvgTemp(string dt) { return calAvgTemp(dt); };
    double getStdTemp(string dt) { return calStdTemp(dt); };
    double getErrTemp(string dt) { return calErrTemp(dt); };
    double getAvgHumi(string dt) { return calAvgHumi(dt); };
    double getStdHumi(string dt) { return calStdHumi(dt); };
    double getErrHumi(string dt) { return calErrHumi(dt); };

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

    void      setGraphAtt(TGraph*, string);

    double    calAvgTemp(string);
    double    calStdTemp(string);
    double    calErrTemp(string);
    double    calAvgHumi(string);
    double    calStdHumi(string);
    double    calErrHumi(string);
    void      doTempHumiMap();
    void      assignTimeInterval();
    void      prepareDataList();
};

#endif
