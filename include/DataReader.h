#ifndef DataReader_h
#define DataReader_h 1

#include <fstream>
#include <iostream>
#include <vector>

class Calendar;
class TBranch;
class TFile;
class TH1D;
class TTree;

using namespace std;

class DataReader {
public:
    DataReader(string inputProject);
    ~DataReader();

    TFile   *fFile;
    TTree   *fTree;

    float    ch0;
    float    timestamp;

    double   ch0d;
    double   timestampD;

    TBranch *b_ch0;
    TBranch *b_timestamp;

    void     initialize();
    void     runFillingLoop(TH1D*);

    void     setEndDateTime(string);
    void     setProjectName(string inputStr) { projectName = inputStr; };
    void     setStartDateTime(string);

private:
    double    conversingFactorCh0 = 1.;

    string    instrumentCh0 = "NaI";
    double    inputVoltageCh0 = 0.;
    bool      isCh0Amplified = true;

    int       recordLength = 0;
    double    referencePosition = 0.;
    double    sampleRate = 1000000.;
    string    triggerSource = "!!python/unicode \'0\'";
    double    voltageThreshold = 0.;
    int       signalstart = 0;
    int       signalend = 1000;

    string    projectName;
    Calendar *fileDateTime;
    Calendar *startDateTime;
    Calendar *endDateTime;
    

    void   readYAML();
};

#endif
