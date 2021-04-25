#ifndef PeakFitter_h
#define PeakFitter_h 1

#include <iostream>
#include <vector>

using namespace std;

class TH1D;

class PeakFitter {
public:
    PeakFitter(TH1D*, string);
    ~PeakFitter();

    void execute();
    void test();

    void setHistogram(TH1D* inputH)     { histogram = inputH; };
    void setPeakType(string);
    void setFitterStr(string inputStr)  { fitterStr = inputStr; };

    void setOutputGraphFilename();
    void setFolderPath(string);
    void setHistoName(string);

    void setRange(double up, double low) {
	upperRange = up; lowerRange = low; };
    void setCExp(double start, double up, double low) {
	startCExp = start; upperCExp = up; lowerCExp = low; cExp = start; };
    void setExpo(double start, double up, double low) {
	startExpo = start; upperExpo = up; lowerExpo = low; expo = start; };
    void setCGauss(double start, double up, double low) {
	startCGauss = start; upperCGauss = up; lowerCGauss = low; cGauss = start; };
    void setMean(double start, double up, double low) {
	startMean = start; upperMean = up; lowerMean = low; mean = start; };
    void setSTD(double start, double up, double low) {
	startSTD = start; upperSTD = up; lowerSTD = low; std = start; };

    double getChi2()    { return chi2; };
    double getCExp()    { return cExp; };
    double getExpo()    { return expo; };
    double getCGauss()  { return cGauss; };
    double getMean()    { return mean; };
    double getSTD()     { return std; };
    double getRatioSM() { return ratioSM; };
    double getError()   { return errM; };

    void fitPeak();
    double getAssignedValue(string);

private:
    vector<string> fileList;
    TH1D*  histogram;

    string fittingType = "Minuit";
    string peakType;
    string fitterStr;

    string outputGraphFilename;
    string folderPath;
    string histoName;

    double upperRange;
    double lowerRange;

    double startCExp;
    double upperCExp;
    double lowerCExp;

    double startExpo;
    double upperExpo;
    double lowerExpo;

    double startCGauss;
    double upperCGauss;
    double lowerCGauss;

    double startMean;
    double upperMean;
    double lowerMean;

    double startSTD;
    double upperSTD;
    double lowerSTD;

    double chi2;
    double cExp;
    double expo;
    double cGauss;
    double mean;
    double std;
    double ratioSM;
    double errM;
};

#endif
