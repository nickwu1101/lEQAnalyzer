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
    void   setPeakNumb(int);
    void   setPeakType(string);
    void   setFitterStr(string inputStr)  { fitterStr = inputStr; };
    void   setNeedZoom(bool inputBool)    { needZoom = inputBool; };
    void   setNeedExtend(bool inputBool)  { needExtend = inputBool; };

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
    void   setCGauss(double start, double low, double up, int i = 0) {
	startCGauss[i] = start; upperCGauss[i] = up; lowerCGauss[i] = low; };
    void   setMean(double start, double low, double up, int i = 0) {
	startMean[i] = start; upperMean[i] = up; lowerMean[i] = low; };
    void   setSTD(double start, double low, double up, int i = 0) {
	startSTD[i] = start; upperSTD[i] = up; lowerSTD[i] = low; };

    TH1D*  getHistogram()  { return histogram; };
    int    getPeakNumb()   { return peakNumb; };
    string getPeakType()   { return peakType; };
    string getFitterStr()  { return fitterStr; };
    bool   getNeedZoom()   { return needZoom; };
    bool   getNeedExtend() { return needExtend; };

    double getChi2();
    double getCPow();
    double getCExp();
    double getExpo();
    double getCGauss(int i = 0);
    double getMean(int i = 0);
    double getSTD(int i = 0);
    double getRatioSM(int i = 0);
    double getErrorCGaus(int i = 0);
    double getErrorM(int i = 0);
    double getEstEvent();

    void   getSetRange(double& low, double& up) {
	low = lowerRange; up = upperRange; };
    void   getSetCPow(double& start, double& low, double& up) {
	start = startCPow; low = lowerCPow; up = upperCPow; };
    void   getSetCExp(double&, double&, double&);
    void   getSetExpo(double& start, double& low, double& up) {
	start = startExpo; low = lowerExpo; up = upperExpo; };
    void   getSetCGauss(double& start, double& low, double& up, int i = 0) {
	start = startCGauss[i];  low = lowerCGauss[i]; up = upperCGauss[i]; };
    void   getSetMean(double& start, double& low, double& up, int i = 0) {
	start = startMean[i]; low = lowerMean[i]; up = upperMean[i]; };
    void   getSetSTD(double& start, double& low, double& up, int i = 0) {
	start = startSTD[i]; low = lowerSTD[i]; up = upperSTD[i]; };

    void   fitPeak();
    void   fitBkg();
    double getAssignedValue(string, int i = 0);
    double getAssignedError(string, int i = 0);

private:
    vector<string> fileList;
    TH1D*  histogram;

    string fittingType = "Minuit";
    int    peakNumb;
    string peakType;
    string fitterStr;
    bool   needZoom;
    bool   needExtend;

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

    vector<double> startCGauss;
    vector<double> upperCGauss;
    vector<double> lowerCGauss;

    vector<double> startMean;
    vector<double> upperMean;
    vector<double> lowerMean;

    vector<double> startSTD;
    vector<double> upperSTD;
    vector<double> lowerSTD;

    vector<double> ratioSM;
};

#endif
