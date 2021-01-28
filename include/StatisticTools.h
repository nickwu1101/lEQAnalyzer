#ifndef StatisticTools_h
#define StatisticTools_h 1

#include <iostream>
#include <map>

using namespace std;

class TFile;
class TH1D;

class StatisticTools {
public:
    StatisticTools();
    ~StatisticTools();

    void test();
    void execute();

private:
    map<string, TH1D*> histoMap;
    string anaFilename;
    bool isHistoMapDone = false;
    TFile *f = nullptr;

    void prepareHistoMap();
    void doEntriesGraphByTime();
    void doPeakFitting();

    void openFile();
    void closeFile();
};

#endif
