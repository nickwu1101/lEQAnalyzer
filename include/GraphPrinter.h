#ifndef GraphPrinter_h
#define GraphPrinter_h 1

#include <iostream>
#include <vector>

using namespace std;

class TAxis;
class TGraph;
class TGraphErrors;
class TMultiGraph;

class GraphPrinter {
public:
    GraphPrinter();
    ~GraphPrinter();

    void   execute();
    void   test();

    void   printFittingGraph();
    void   printEstEventGraph();
    void   printCountingGraph();
    void   printFittingTempCorrelation();
    void   printCountingTempCorrelation();
    void   printTempCorrectFittingGraph();
    void   printTempCorrectCountingGraph();
    void   printNormFittingGraph();
    void   printNormCountingGraph();
    void   printCompareCorrectionFittingGraph();
    void   printCompareCorrectionCountingGraph();

    void   printTemplateFittingGraph();

    void   printFittingMeanGraph();
    void   print50DaysFitting();
    void   printNormFitHist();
    void   printExpoGraph();
    void   printExpoTempCorrelation();

    void   printOverlapExpAndSimulation();
    void   printOverlapExpAndBkg();

    void   setHandledTimeUnit(string inputStr) { handledTU = inputStr; };

private:
    vector<double> vd;
    string handledTU;
    string timeRange;
    string quantity;
    string errBandStr;

    void   printCorrelationWithTemp(TGraph*, string, string, string);
    void   printCompareMultiG(TGraph*, TGraph*, string, string, string);
    void   printWithErrorBand(TGraph*, double, double, double, string, string, string);
    void   setErrorBandAtt(TGraphErrors*, TGraphErrors*, TGraphErrors*, string);
    void   setGraphAtt(TGraph*, string, string, string);
    void   setMultiGAtt(TMultiGraph*, string, string, string);
    void   setRangeUser(TAxis*, string, string, string);
    string takePeakTypeStr(string, string);
};

#endif
