#ifndef PeakFitter_h
#define PeakFitter_h 1

#include <iostream>
#include <vector>

#include <TFitResult.h>
#include <TFitResultPtr.h>
#include <TMath.h>

using namespace std;

class TH1D;

class PeakFitter {
public:
    PeakFitter(TH1D*, string);
    ~PeakFitter();

    void   execute();
    void   test();

    void   setHistogram(TH1D* inputH)     { histogram = inputH; };
    void   setPeakType(string);
    void   setFitterStr(string inputStr)  { fitterStr = inputStr; };
    void   setNeedZoom(bool inputBool)    { needZoom = inputBool; };

    void   setOutputGraphFilename();
    void   setFolderPath(string);
    void   setHistoName(string);

    void   setRange(double low, double up) {
	upperRange = up; lowerRange = low; };
    void   setCPow(double start, double low, double up) {
	startCPow = start; upperCPow = up; lowerCPow = low; };
    void   setCExp(double, double, double);
    void   setExpo(double start, double low, double up) {
	startExpo = start; upperExpo = up; lowerExpo = low; };
    void   setCGauss(double start, double low, double up) {
	startCGauss = start; upperCGauss = up; lowerCGauss = low; };
    void   setMean(double start, double low, double up) {
	startMean = start; upperMean = up; lowerMean = low; };
    void   setSTD(double start, double low, double up) {
	startSTD = start; upperSTD = up; lowerSTD = low; };

    double getChi2();
    double getCPow();
    double getCExp();
    double getExpo();
    double getCGauss();
    double getMean();
    double getSTD();
    double getRatioSM();
    double getErrorCGaus();
    double getErrorM();

    void   getSetRange(double& low, double& up) {
	low = lowerRange; up = upperRange; };
    void   getSetCPow(double& start, double& low, double& up) {
	start = startCPow; low = lowerCPow; up = upperCPow; };
    void   getSetCExp(double&, double&, double&);
    void   getSetExpo(double& start, double& low, double& up) {
	start = startExpo; low = lowerExpo; up = upperExpo; };
    void   getSetCGauss(double& start, double& low, double& up) {
	start = startCGauss;  low = lowerCGauss; up = upperCGauss; };
    void   getSetMean(double& start, double& low, double& up) {
	start = startMean; low = lowerMean; up = upperMean; };
    void   getSetSTD(double& start, double& low, double& up) {
	start = startSTD; low = lowerSTD; up = upperSTD; };

    void   fitPeak();
    double getAssignedValue(string);
    double getAssignedError(string);

private:
    vector<string> fileList;
    TH1D*  histogram;

    string fittingType = "Minuit";
    string peakType;
    string fitterStr;
    bool   needZoom;

    string outputGraphFilename;
    string folderPath;
    string histoName;

    TFitResultPtr fitptr;

    double upperRange;
    double lowerRange;

    double startCPow;
    double upperCPow;
    double lowerCPow;

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

    double ratioSM;
};

#endif
