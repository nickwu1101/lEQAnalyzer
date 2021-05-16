#ifndef makePlots_h
#define makePlots_h 1

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TF1.h>
#include <TH1.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

class Calendar;
class TFile;

class makePlots {
public:
    makePlots();
    ~makePlots();

    void setAnaType(int inputAnaType) { anaType = inputAnaType; };

    void execute();
    void makeHistoCh0();
    void makeHistoCh1();
    void doCoincidence(int, int, double);
    void collectWithFilter(string, double, double);
    void test();

private:
    vector<string> dataList;
    vector<string> startDateTime;
    vector<string> endDateTime;

    vector<Int_t> bin;
    vector<Double_t> min;
    vector<Double_t> max;
    double unitConverter;
    double conversingFactorCh0;
    string quantity;

    TFile* outfile;
    string filename;
    int    anaType;
    bool   isFileSet;
    bool   isInit;

    void initialize();
    void readAnalysisParameter();
    void prepareDataList();
    void prepareOutputFile(string);
    void assignTimeIntervals();
    void assignSingleLengthOfIntervals();
    bool hasDataInInterval(string, string, string, string);
    bool hasDataInInterval(string, Calendar*, Calendar*, string);
    bool hasDataInInterval(Calendar*, Calendar*, Calendar*, Calendar*);
};

#endif
