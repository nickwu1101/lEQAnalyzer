#include "PeakFitter.h"

#include <TCanvas.h>
#include <TFile.h>
#include <TFitResult.h>
#include <TF1.h>
#include <TH1.h>
#include <TPaveText.h>

PeakFitter::PeakFitter(TH1D* inputH, string peakType) {
    histogram = inputH;

    setPeakType(peakType);
    fitterStr = "expo(0) + gaus(2)";

    outputGraphFilename = "";
    setFolderPath("plotting/fittingHualien");
    setHistoName("");
}



PeakFitter::~PeakFitter() {}



void PeakFitter::execute() {}



void PeakFitter::test() {}



void PeakFitter::fitPeak() {
    TCanvas* c = new TCanvas("c", "c", 1400, 800);

    TF1* peak = new TF1("peak", fitterStr.c_str(), 0., 5.);
    TFitResultPtr fitptr = nullptr;

    peak->SetRange(upperRange, lowerRange);
    peak->SetParameter(0, startCExp);
    peak->SetParameter(1, startExpo);
    peak->SetParameter(2, startCGauss);
    peak->SetParameter(3, startMean);
    peak->SetParameter(4, startSTD);

    peak->SetParLimits(0, upperCExp, lowerCExp);
    peak->SetParLimits(1, upperExpo, lowerExpo);
    peak->SetParLimits(2, upperCGauss, lowerCGauss);
    peak->SetParLimits(3, upperMean, lowerMean);
    peak->SetParLimits(4, upperSTD, lowerSTD);

    fitptr = histogram->Fit(peak, "S", "", upperRange, lowerRange);

    histogram->SetStats(kFALSE);
    chi2 = fitptr->Chi2();
    cExp = fitptr->Parameter(0);
    expo = fitptr->Parameter(1);
    cGauss = fitptr->Parameter(2);
    mean = fitptr->Parameter(3);
    std = fitptr->Parameter(4);
    ratioSM = std/mean;
    errM = fitptr->Error(3);

    c->Update();
    TPaveText* pt = new TPaveText((gPad->GetUxmax() + gPad->GetUxmin())/2.,
				  (gPad->GetUymax() + gPad->GetUymin())/2.,
				  gPad->GetUxmax(), gPad->GetUymax());

    char line[40];
    pt->AddText("Statistics");
    sprintf(line, "Chi2: %f", chi2);
    pt->AddText(line);
    sprintf(line, "cExp: %f", cExp);
    pt->AddText(line);
    sprintf(line, "Expo: %f", expo);
    pt->AddText(line);
    sprintf(line, "cGauss: %f", cGauss);
    pt->AddText(line);
    sprintf(line, "Mean: %f", mean);
    pt->AddText(line);
    sprintf(line, "Std: %f", std);
    pt->AddText(line);
    sprintf(line, "RatioSM: %f", ratioSM);
    pt->AddText(line);
    sprintf(line, "Error of Mean: %f", errM);
    pt->AddText(line);
    pt->Draw();

    c->Update();
    c->Print(outputGraphFilename.c_str());
}



void PeakFitter::setPeakType(string inputStr) {
    peakType = inputStr;

    if(peakType == "K40") {
	setRange(1.4, 1.8);
	setCExp(0., -100000000., 100000000.);
	setExpo(-3., -1000., 0.);
	setCGauss(100., 0., 100000000.);
	setMean(1.55, 1.5, 1.65);
	setSTD(0.01, 0., 0.1);
    } else if(peakType == "Rn222") {
	setRange(0.6, 0.84);
	setCExp(0., -100000000., 100000000.);
	setExpo(-3., -1000., 0.);
	setCGauss(100., 0., 100000000.);
	setMean(0.725, 0.7, 0.75);
	setSTD(0.01, 0., 0.1);
    }
}



void PeakFitter::setOutputGraphFilename() {
    if(folderPath != "" && histoName != "")
	outputGraphFilename = folderPath + "/" + histoName + ".png";
    else
	outputGraphFilename = "";
}



void PeakFitter::setFolderPath(string inputStr) {
    folderPath = inputStr;
    setOutputGraphFilename();
}



void PeakFitter::setHistoName(string inputStr) {
    histoName = inputStr;
    setOutputGraphFilename();
}



double PeakFitter::getAssignedValue(string inputStr) {
    if(inputStr == "chi2")
	return chi2;
    else if(inputStr == "cExp")
	return cExp;
    else if(inputStr == "expo")
	return expo;
    else if(inputStr == "cGauss")
	return cGauss;
    else if(inputStr == "mean")
	return mean;
    else if(inputStr == "std")
	return std;
    else if(inputStr == "ratioSM")
	return ratioSM;
    else if(inputStr == "errM")
	return errM;
    else {
	cout << "Unknown Term!!!" << endl;
	return 0;
    }
}
