#ifndef Handling_h
#define Handling_h 1

#include <iostream>
#include <vector>

using namespace std;

class PeakFitter;
class TH1D;
class TGraph;

class Handling {
public:
    Handling();
    ~Handling();

    void   execute();
    void   test();

    void   doProcedure1(); // Overlap
    void   doProcedure2();
    void   doProcedure3();

private:
    vector<string> filenameList;

    void   readFilenameListTxt(string);
    void   setGraphAtt(TGraph*, string, string);
    string giveTermLabel(string);
    string givePeakLabel(string);
    string giveTimeUnitLabel(string);
    void   setRangeUser(TGraph*, string, string);
    void   adjustFittingRange(string, string, PeakFitter*);
};

#endif
