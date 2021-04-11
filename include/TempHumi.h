#ifndef TempHumi_h
#define TempHumi_h 1

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

class Calendar;

using namespace std;

class TempHumi {
public:
    TempHumi();
    ~TempHumi();

    void execute();
    void test();

    void makeTempHumiPlotting();

    void setEndDateTime(string);
    void setStartDateTime(string);

private:
    vector<string> dataList;
    map<string, double> temp;
    map<string, double> humi;

    Calendar *startDateTime;
    Calendar *endDateTime;

    double calAvgTemp(string);
    double calAvgHumi(string);
    void   doTempHumiMap();
    void   prepareDataList();
};

#endif
