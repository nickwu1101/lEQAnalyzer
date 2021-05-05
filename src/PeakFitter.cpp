#include "PeakFitter.h"

#include <TCanvas.h>
#include <TFile.h>
#include <TF1.h>
#include <TH1.h>
#include <TPaveText.h>

PeakFitter::PeakFitter(TH1D* inputH, string peakType) {
    histogram = inputH;

    setPeakType(peakType);
    fitterStr = "expo(0) + gausn(2)";

    outputGraphFilename = "";
    setFolderPath("plotting/fittingHualien");
    setHistoName("");
    needZoom = false;
    fitptr = nullptr;
}



PeakFitter::~PeakFitter() {}



void PeakFitter::execute() {}



void PeakFitter::test() {}



void PeakFitter::fitPeak() {
    if(peakType == "expo")
	return;

    TCanvas* c = new TCanvas("c", "c", 1400, 800);

    TF1* peak = new TF1("peak", fitterStr.c_str(), 0., 5.);
    fitptr = nullptr;

    peak->SetRange(lowerRange, upperRange);
    peak->SetParameter(0, startCPow);
    peak->SetParameter(1, startExpo);
    peak->SetParameter(2, startCGauss);
    peak->SetParameter(3, startMean);
    peak->SetParameter(4, startSTD);

    peak->SetParLimits(0, lowerCPow, upperCPow);
    peak->SetParLimits(1, lowerExpo, upperExpo);
    peak->SetParLimits(2, lowerCGauss, upperCGauss);
    peak->SetParLimits(3, lowerMean, upperMean);
    peak->SetParLimits(4, lowerSTD, upperSTD);

    if(needZoom)
	histogram->GetXaxis()->SetRangeUser(0.9*lowerRange, 1.1*upperRange);

    fitptr = histogram->Fit(peak, "S", "", lowerRange, upperRange);

    histogram->SetStats(kFALSE);
    ratioSM = fitptr->Parameter(4)/fitptr->Parameter(3);

    c->Update();
    double xleft = 0.75;
    double ydown = 0.4;
    TPaveText* pt = new TPaveText(xleft*gPad->GetUxmax()
				  + (1 - xleft)*gPad->GetUxmin(),
				  ydown*gPad->GetUymax()
				  + (1 - ydown)*gPad->GetUymin(),
				  gPad->GetUxmax(), gPad->GetUymax());

    char line[100];
    pt->AddText("Statistics");
    sprintf(line, "Chi2: %f", fitptr->Chi2());
    pt->AddText(line);
    sprintf(line, "cPow: %f", fitptr->Parameter(0));
    pt->AddText(line);
    sprintf(line, "cExp: %f", TMath::Exp(fitptr->Parameter(0)));
    pt->AddText(line);
    sprintf(line, "Expo: %f", fitptr->Parameter(1));
    pt->AddText(line);
    sprintf(line, "cGauss: %f", fitptr->Parameter(2));
    pt->AddText(line);
    sprintf(line, "Mean: %f", fitptr->Parameter(3));
    pt->AddText(line);
    sprintf(line, "Std: %f", fitptr->Parameter(4));
    pt->AddText(line);
    sprintf(line, "RatioSM: %f", ratioSM);
    pt->AddText(line);
    sprintf(line, "Error of Mean: %f", fitptr->Error(3));
    pt->AddText(line);
    pt->Draw();

    c->Update();
    c->Print(outputGraphFilename.c_str());
    c->Close();

    delete pt;
    delete peak;
    delete c;
}



void PeakFitter::fitBkg() {
    if(peakType != "expo")
	return;

    TCanvas* c = new TCanvas("c", "c", 1400, 800);

    TF1* bkg = new TF1("bkg", fitterStr.c_str(), 0., 5.);

    bkg->SetRange(lowerRange, upperRange);
    bkg->SetParameter(0, startCPow);
    bkg->SetParameter(1, startExpo);

    bkg->SetParLimits(0, lowerCPow, upperCPow);
    bkg->SetParLimits(1, lowerExpo, upperExpo);

    if(needZoom)
	histogram->GetXaxis()->SetRangeUser(0.9*lowerRange, 1.1*upperRange);

    fitptr = histogram->Fit(bkg, "S", "", lowerRange, upperRange);

    histogram->SetStats(kFALSE);

    c->Update();
    double xleft = 0.75;
    double ydown = 0.4;
    TPaveText* pt = new TPaveText(xleft*gPad->GetUxmax()
				  + (1 - xleft)*gPad->GetUxmin(),
				  ydown*gPad->GetUymax()
				  + (1 - ydown)*gPad->GetUymin(),
				  gPad->GetUxmax(), gPad->GetUymax());

    char line[100];
    pt->AddText("Statistics");
    sprintf(line, "Chi2: %f", fitptr->Chi2());
    pt->AddText(line);
    sprintf(line, "cPow: %f", fitptr->Parameter(0));
    pt->AddText(line);
    sprintf(line, "cExp: %f", TMath::Exp(fitptr->Parameter(0)));
    pt->AddText(line);
    sprintf(line, "Expo: %f", fitptr->Parameter(1));
    pt->AddText(line);
    pt->Draw();

    c->Update();
    c->Print(outputGraphFilename.c_str());
    c->Close();

    delete pt;
    delete bkg;
    delete c;
}



void PeakFitter::setPeakType(string inputStr) {
    peakType = inputStr;

    if(peakType == "K40") {
	setFolderPath("plotting/fittingHualien/K40");

	setRange(1.4, 1.8);
	setCPow(10., 5., 50.);
	setExpo(-3., -5., 0.);
	setCGauss(1000., 0., 100000000.);
	setMean(1.55, 1.5, 1.65);
	setSTD(0.01, 0., 0.1);
    } else if(peakType == "Rn222") {
	setFolderPath("plotting/fittingHualien/Rn222");

	setRange(0.6, 0.84);
	setCPow(10., -100., 100.);
	setExpo(-3., -5., 0.);
	setCGauss(1000., 0., 100000000.);
	setMean(0.725, 0.7, 0.75);
	setSTD(0.01, 0., 0.1);
    } else if(peakType == "expo") {
	setFolderPath("plotting/fittingHualien");

	setRange(0.4, 0.9);
	setCPow(10., -100., 100.);
	setExpo(-3., -1000., 0.);
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



void PeakFitter::setCExp(double start, double low, double up) {
    setCPow(TMath::Log(start), TMath::Log(low), TMath::Log(up));
}



double PeakFitter::getChi2()       { return fitptr->Chi2(); }
double PeakFitter::getCPow()       { return fitptr->Parameter(0); }
double PeakFitter::getCExp()       { return TMath::Exp(fitptr->Parameter(0)); }
double PeakFitter::getExpo()       { return fitptr->Parameter(1); }
double PeakFitter::getCGauss()     { return fitptr->Parameter(2); }
double PeakFitter::getMean()       { return fitptr->Parameter(3); }
double PeakFitter::getSTD()        { return fitptr->Parameter(4); }
double PeakFitter::getRatioSM()    { return ratioSM; }
double PeakFitter::getErrorCGaus() { return fitptr->ParError(2); }
double PeakFitter::getErrorM()     { return fitptr->ParError(3); }



void PeakFitter::getSetCExp(double& start, double& low, double& up) {
    start = TMath::Exp(startCPow);
    low = TMath::Exp(lowerCPow);
    up = TMath::Exp(upperCPow);
}



double PeakFitter::getAssignedValue(string inputStr) {
    if(inputStr == "chi2")
	return fitptr->Chi2();
    else if(inputStr == "cPow")
	return fitptr->Parameter(0);
    else if(inputStr == "cExp")
	return TMath::Exp(fitptr->Parameter(0));
    else if(inputStr == "expo")
	return fitptr->Parameter(1);
    else if(inputStr == "cGauss")
	return fitptr->Parameter(2);
    else if(inputStr == "mean")
	return fitptr->Parameter(3);
    else if(inputStr == "std")
	return fitptr->Parameter(4);
    else if(inputStr == "ratioSM")
	return ratioSM;
    else {
	cout << "Unknown Term!!!" << endl;
	return 0;
    }
}



double PeakFitter::getAssignedError(string inputStr) {
    if(inputStr == "cPow")
	return fitptr->ParError(0);
    else if(inputStr == "expo")
	return fitptr->ParError(1);
    else if(inputStr == "cGauss")
	return fitptr->ParError(2);
    else if(inputStr == "mean")
	return fitptr->ParError(3);
    else if(inputStr == "std")
	return fitptr->ParError(4);
    else if(inputStr == "cExp") {
	cout << "cExp cannot do an error. It will return a 0." << endl;
	return 0;
    } else {
	cout << "Unkonwn Term!!!" << endl;
	return 0;
    }
}
