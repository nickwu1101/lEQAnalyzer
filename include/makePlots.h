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
    void doCoincidence(int goalCh, int threCh, double threshold);
    void collectWithFilter(string filtingRegion, double low, double up);
    void collectWithDynamicFilter(string filtingRegion);
    void mergeHist();
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
    string fileStat;
    string filename;
    int    anaType;
    bool   isFileSet;
    bool   isInit;

    void initialize();
    void readAnalysisParameter();
    void prepareDataList();
    void prepareOutputFile(string outileName);
    void assignTimeIntervals();
    void assignSingleLengthOfIntervals();
    void assignCutEnergyRange(string, double&, double&);
    bool hasDataInInterval(string inputDTStr, string startDTStr,
			   string endDTStr, string nextDTStr);
    bool hasDataInInterval(string inputDTStr, Calendar* startDT,
			   Calendar* endDT, string nextDTStr);
    bool hasDataInInterval(Calendar* inputDT, Calendar* startDT,
			   Calendar* endDT, Calendar* nextDT);
};

#endif
