#include "PeakFitter.h"

#include <TCanvas.h>
#include <TFile.h>
#include <TF1.h>
#include <TH1.h>
#include <TPaveText.h>

PeakFitter::PeakFitter(TH1D* inputH, string peakType) {
    histogram = inputH;

    outputGraphFilename = "";
    setFolderPath("plotting/fittingHualien");
    setHistoName("");

    setPeakType(peakType);

    needZoom = false;
    needExtend = false;
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

    peak->SetParLimits(0, lowerCPow, upperCPow);
    peak->SetParLimits(1, lowerExpo, upperExpo);

    for(int iPeak = 0; iPeak < peakNumb; iPeak++) {
	peak->SetParameter(3*iPeak + 2, startCGauss[iPeak]);
	peak->SetParameter(3*iPeak + 3, startMean[iPeak]);
	peak->SetParameter(3*iPeak + 4, startSTD[iPeak]);

	peak->SetParLimits(3*iPeak + 2, lowerCGauss[iPeak], upperCGauss[iPeak]);
	peak->SetParLimits(3*iPeak + 3, lowerMean[iPeak], upperMean[iPeak]);
	peak->SetParLimits(3*iPeak + 4, lowerSTD[iPeak], upperSTD[iPeak]);
    }

    if(needZoom)
	histogram->GetXaxis()->SetRangeUser(0.9*lowerRange, 1.1*upperRange);

    fitptr = histogram->Fit(peak, "S", "", lowerRange, upperRange);

    histogram->SetStats(kFALSE);

    for(int iPeak = 0; iPeak < peakNumb; iPeak++) {
	ratioSM[iPeak] = fitptr->Parameter(3*iPeak + 4)/fitptr->Parameter(3*iPeak + 3);
    }

    TF1* fExtend = nullptr;
    if(needExtend) {
	fExtend = new TF1("fExtend", fitterStr.c_str(), 0., 5.);
	fExtend->SetParameter(0, fitptr->Parameter(0));
	fExtend->SetParameter(1, fitptr->Parameter(1));

	for(int iPeak = 0; iPeak < peakNumb; iPeak++) {
	    fExtend->SetParameter(3*iPeak + 2, fitptr->Parameter(3*iPeak + 2));
	    fExtend->SetParameter(3*iPeak + 3, fitptr->Parameter(3*iPeak + 3));
	    fExtend->SetParameter(3*iPeak + 4, fitptr->Parameter(3*iPeak + 4));
	}

	fExtend->Draw("SAME");
    }

    c->Update();
    double xleft = 0.75;
    double ydown = 0.4;
    TPaveText* pt = new TPaveText(xleft*gPad->GetUxmax()
				  + (1 - xleft)*gPad->GetUxmin(),
				  ydown*gPad->GetUymax()
				  + (1 - ydown)*gPad->GetUymin(),
				  gPad->GetUxmax(), gPad->GetUymax());

    TF1* fGaus = new TF1("fgaus", "gausn", 0., 5.);
    fGaus->SetParameter(0, fitptr->Parameter(2));
    fGaus->SetParameter(1, fitptr->Parameter(3));
    fGaus->SetParameter(2, fitptr->Parameter(4));
    fGaus->SetLineColor(kBlue);
    //fGaus->Draw("SAME");

    TF1* fExpo = new TF1("fexpo", "expo", 0., 5.);
    fExpo->SetParameter(0, fitptr->Parameter(0));
    fExpo->SetParameter(1, fitptr->Parameter(1));
    fExpo->SetLineColor(kGreen);
    //fExpo->Draw("SAME");

    char line[100];
    pt->AddText("Statistics");
    sprintf(line, "#chi^{2}: %f", fitptr->Chi2());
    pt->AddText(line);
    sprintf(line, "Ndf: %d", fitptr->Ndf());
    pt->AddText(line);
    sprintf(line, "#chi^{2}/Ndf: %f", fitptr->Chi2()/(double)fitptr->Ndf());
    pt->AddText(line);
    sprintf(line, "cPow: %f", fitptr->Parameter(0));
    pt->AddText(line);
    sprintf(line, "cExp: %f", TMath::Exp(fitptr->Parameter(0)));
    pt->AddText(line);
    sprintf(line, "Expo: %f", fitptr->Parameter(1));
    pt->AddText(line);
    for(int i = 0; i < peakNumb; i++) {
	sprintf(line, "cGauss%d: %f", i + 1, fitptr->Parameter(3*i + 2));
	pt->AddText(line);
	sprintf(line, "Mean%d: %f", i + 1, fitptr->Parameter(3*i + 3));
	pt->AddText(line);
	sprintf(line, "Std%d: %f", i + 1, fitptr->Parameter(3*i + 4));
	pt->AddText(line);
	sprintf(line, "RatioSM%d: %f", i + 1, ratioSM[i]);
	pt->AddText(line);
	sprintf(line, "Error of Mean%d: %f", i + 1, fitptr->Error(3*i + 3));
	pt->AddText(line);
    }
    /*
    sprintf(line, "Integral: %f", fGaus->Integral(lowerRange, upperRange)/(5./865.));
    pt->AddText(line);
    sprintf(line, "ExpoInte: %f", fExpo->Integral(lowerRange, upperRange)/(5./865.));
    pt->AddText(line);
    */

    pt->Draw();

    

    c->Update();
    c->Print(outputGraphFilename.c_str());
    c->Close();

    delete fExpo;
    delete fGaus;

    if(fExtend != nullptr)
	delete fExtend;

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



void PeakFitter::setPeakNumb(int input) {
    if(peakType == "expo") {
	peakNumb = 0;
	fitterStr = "expo";
	return;
    }

    peakNumb = input;
    if(peakNumb == 1)
	fitterStr = "expo(0) + gausn(2)";
    else if(peakNumb == 2)
	fitterStr = "expo(0) + gausn(2) + gausn(5)";

    startCGauss.resize(input);
    upperCGauss.resize(input);
    lowerCGauss.resize(input);

    startMean.resize(input);
    upperMean.resize(input);
    lowerMean.resize(input);

    startSTD.resize(input);
    upperSTD.resize(input);
    lowerSTD.resize(input);

    ratioSM.resize(input);
}



void PeakFitter::setPeakType(string inputStr) {
    peakType = inputStr;

    if(peakType == "K40" || peakType == "peak16") {
	folderPath = "plotting/fittingHualien/peak16";
	setPeakNumb(1);

	/* For Voltage (V)
	setRange(1.4, 1.8);
	setCPow(10., 5., 50.);
	setExpo(-3., -5., 0.);
	setCGauss(1000., 0., 100000000.);
	setMean(1.55, 1.5, 1.65);
	setSTD(0.01, 0., 0.1);
	*/

	// For Energy (MeV)
	setRange(1.3, 1.6);
	setCPow(10., 5., 50.);
	setExpo(-3., -5., 0.);
	setCGauss(1000., 0., 100000000.);
	setMean(1.45, 1.4, 1.5);
	setSTD(0.01, 0., 0.1);
    } else if(peakType == "Rn222" || peakType == "peak06") {
	folderPath = "plotting/fittingHualien/peak06";
	setPeakNumb(1);

	/* For Voltage (V)
	setRange(0.6, 0.84);
	setCPow(10., -100., 100.);
	setExpo(-3., -5., 0.);
	setCGauss(1000., 0., 100000000., 0);
	setMean(0.725, 0.7, 0.75, 0);
	setSTD(0.01, 0., 0.1, 0);
	*/

	// For Energy (MeV)
	setRange(0.5, 0.7);
	setCPow(10., -100., 100.);
	setExpo(-3., -5., 0.);
	setCGauss(1000., 0., 100000000., 0);
	setMean(0.6, 0.55, 0.65, 0);
	setSTD(0.01, 0., 0.1, 0);

	if(peakNumb >= 2) {
	    /* For Voltage (V)
	    setCGauss(1000., 0., 100000000., 1);
	    setMean(0.58, 0.55, 0.6, 1);
	    setSTD(0.01, 0., 0.1, 1);
	    */

	    // For Energy (MeV)
	    setCGauss(1000., 0., 100000000., 1);
	    setMean(0.48, 0.45, 0.5, 1);
	    setSTD(0.01, 0., 0.1, 1);
	}
    } else if(peakType == "peak04") {
	folderPath = "plotting/fittingHualien/peak04";
	setPeakNumb(1);

	/* For Voltage (V)
	setRange(0.35, 0.45);
	setCPow(10., -100., 100.);
	setExpo(-3., -5., 0.);
	setCGauss(1000., 0., 100000000., 0);
	setMean(0.4, 0.38, 0.41, 0);
	setSTD(0.01, 0., 0.1, 0);
	*/

	// For Energy (MeV)
	setRange(0.25, 0.35);
	setCPow(10., -100., 100.);
	setExpo(-3., -5., 0.);
	setCGauss(1000., 0., 100000000.);
	setMean(0.29, 0.275, 0.3);
	setSTD(0.01, 0., 0.1);
    } else if(peakType == "peak24") {
	folderPath = "plotting/fittingHualien/peak24";
	setPeakNumb(1);

	/* For Voltage (V)
	setRange(2.3, 2.55);
	setCPow(10., -100., 100.);
	setExpo(-3., -5., 0.);
	setCGauss(1000., 0., 100000000., 0);
	setMean(2.4, 2.35, 2.45, 0);
	setSTD(0.01, 0., 0.1, 0);
	*/

	// For Energy (MeV)
	setRange(2.15, 2.45);
	setCPow(10., -100., 100.);
	setExpo(-3., -100., 0.);
	setCGauss(1000., 0., 100000000.);
	setMean(2.3, 2.25, 2.35);
	setSTD(0.01, 0., 0.1);
    } else if(peakType == "expo") {
	folderPath = "plotting/fittingHualien";
	setPeakNumb(0);

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

double PeakFitter::getCGauss(int i) {
    return fitptr->Parameter(3*i + 2);
}

double PeakFitter::getMean(int i) {
    return fitptr->Parameter(3*i + 3);
}

double PeakFitter::getSTD(int i) {
    return fitptr->Parameter(3*i + 4);
}

double PeakFitter::getRatioSM(int i) {
    return ratioSM[i];
}

double PeakFitter::getErrorCGaus(int i) {
    return fitptr->ParError(3*i + 2);
}

double PeakFitter::getErrorM(int i) {
    return fitptr->ParError(3*i + 3);
}

double PeakFitter::getEstEvent() {
    TF1* fGaus = new TF1("fgaus", "gausn", 0., 5.);
    fGaus->SetParameter(0, fitptr->Parameter(2));
    fGaus->SetParameter(1, fitptr->Parameter(3));
    fGaus->SetParameter(2, fitptr->Parameter(4));
    return fGaus->Integral(lowerRange, upperRange)/(5./865.);

    delete fGaus;
}



void PeakFitter::getSetCExp(double& start, double& low, double& up) {
    start = TMath::Exp(startCPow);
    low = TMath::Exp(lowerCPow);
    up = TMath::Exp(upperCPow);
}



double PeakFitter::getAssignedValue(string inputStr, int i) {
    if(inputStr == "chi2")
	return fitptr->Chi2();
    else if(inputStr == "cPow")
	return fitptr->Parameter(0);
    else if(inputStr == "cExp")
	return TMath::Exp(fitptr->Parameter(0));
    else if(inputStr == "expo")
	return fitptr->Parameter(1);
    else if(inputStr == "cGauss")
	return fitptr->Parameter(3*i + 2);
    else if(inputStr == "mean")
	return fitptr->Parameter(3*i + 3);
    else if(inputStr == "std")
	return fitptr->Parameter(3*i + 4);
    else if(inputStr == "ratioSM")
	return ratioSM[i];
    else {
	cout << "Unknown Term!!!" << endl;
	return 0;
    }
}



double PeakFitter::getAssignedError(string inputStr, int i) {
    if(inputStr == "cPow")
	return fitptr->ParError(0);
    else if(inputStr == "expo")
	return fitptr->ParError(1);
    else if(inputStr == "cGauss")
	return fitptr->ParError(3*i + 2);
    else if(inputStr == "mean")
	return fitptr->ParError(3*i + 3);
    else if(inputStr == "std")
	return fitptr->ParError(3*i + 4);
    else if(inputStr == "cExp") {
	cout << "cExp cannot do an error. It will return a 0." << endl;
	return 0;
    } else {
	cout << "Unkonwn Term!!!" << endl;
	return 0;
    }
}
