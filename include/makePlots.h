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

class TFile;

class makePlots {
public:
    makePlots();
    ~makePlots();

    void setAnaType(int inputAnaType) { anaType = inputAnaType; };

    void initialize(int);
    void execute();
    void makeHistoCh0();
    void test();

private:
    vector<string> dataList;
    vector<string> startDateTime;
    vector<string> endDateTime;

    vector<Int_t> bin;
    vector<Double_t> min;
    vector<Double_t> max;
    double unitConverter = 1.;
    double conversingFactorCh0 = 1.;

    TFile* outfile;
    int anaType;
    bool   isInit = false;

    string instrumentCh0 = "NaI";
    double inputVoltageCh0 = 0.;
    bool   isCh0Amplified = true;
    string instrumentCh1 = "none";
    double inputVoltageCh1 = 0.;
    bool   isCh1Amplified = false;

    void readAnalysisParameter();
    void prepareDataList();
    void prepareOutputFile(string);
    void assignTimeIntervals();
    void assignSingleLengthOfIntervals();
};

#endif
