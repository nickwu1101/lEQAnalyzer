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

class makePlots {
public:
    makePlots();
    ~makePlots();

    void initialize(int);
    void execute(int);
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

    string instrumentCh0 = "NaI";
    double inputVoltageCh0 = 0.;
    bool isCh0Amplified = true;

    void readAnalysisParameter();
    void prepareDataList();
    void assignTimeIntervals();
    void assignSingleLengthOfIntervals();
};

#endif
