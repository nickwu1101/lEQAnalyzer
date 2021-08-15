#ifndef TimeAnalysis_h
#define TimeAnalysis_h 1

#include <iostream>
#include <vector>

using namespace std;

class TimeAnalysis {
public:
    TimeAnalysis();
    ~TimeAnalysis();

    void execute();
    void test();

    void analyzeByFitting();
    void analyzeByCounting();
    void normalizeFittingResult();
    void normalizeCountingResult();    
    void correctFittingAnaByTemp();
    void correctCountingAnaByTemp();
    void correctCountingAnaByTempU();

    void fitOnTotalTime();

private:
    vector<double> vd;
    string handledTU;
    string quantity;
    double correctByTemp(string, string, string, double, double);
    double calErrAfterCorrect(string, string, string, double, double, double);
    double takeTimeLength(string);
};

#endif
