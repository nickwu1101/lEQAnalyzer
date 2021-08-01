#ifndef Handling_h
#define Handling_h 1

#include <iostream>
#include <vector>

using namespace std;

class PeakFitter;
class TAxis;
class TH1;
class TH2;
class TGraph;
class TGraphErrors;
class TMultiGraph;

class Handling {
public:
    Handling();
    ~Handling();

    void   execute();
    void   test();

    void   doProcedure1(); // Overlap
    void   doProcedure2(); // fitting abandoned
    void   doProcedure3(); // count entries
    void   doProcedure4(); // Temp-Counts Linear Relation
    void   doProcedure5(); // TH2I double energy range by fluctuation
    void   doProcedure6(); // fitting 2
    void   doProcedure7(); // TH2I double energy range by fitting

    void   checkHist();
    void   overlapForComparison();

private:
    vector<string> filenameList;

    void   readFilenameListTxt(string);
    void   setErrorBandAtt(TGraphErrors*, TGraphErrors*, TGraphErrors*, string);
    void   setHist1Att(TH1*, string, string, string);
    void   setHist2Att(TH2*, string, string, string);
    void   setGraphAtt(TGraph*, string, string);
    void   setMultiGAtt(TMultiGraph*, string, string);
    void   setMultiGAtt(TMultiGraph*, string, string, string);
    void   getHistUpLow(string, string, string, double&, double&);
    string giveTermLabel(string);
    string givePeakLabel(string);
    string giveTimeUnitLabel(string);
    void   setBins(TH1*, string, string, string);
    void   setBins(TH2*, string, string, string);
    void   setRangeUser(TAxis*, string, string, string);
    void   adjustFittingRange(string, string, PeakFitter*);

    string takePeakTypeStr(string);
    double takeTimeLength(string);
};

#endif
