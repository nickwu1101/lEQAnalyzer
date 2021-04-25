#include "Handling.h"

#include "Calendar.h"
#include "GetExterSet.h"
#include "PeakFitter.h"

#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TFitResult.h>
#include <TGraph.h>
#include <TH1.h>
#include <TLegend.h>
#include <TPaveText.h>

#include <fstream>
#include <map>

Handling::Handling() {}



Handling::~Handling() {}



void Handling::execute() {
    doProcedure1();
}



void Handling::test() {
    /*
    TCanvas* c = new TCanvas("c", "c", 1400, 800);
    TFile* f1 = new TFile("ready/0404to0415aHistPDay.root", "READ");

    TList* listItem = f1->GetListOfKeys();
    TIter iterItem(listItem);

    TObject* objItem = nullptr;
    while((objItem = iterItem())) {
	TDirectory* dirItem = f1->GetDirectory(objItem->GetName());
	TList* listDate = dirItem->GetListOfKeys();
	listDate->Sort();

	TIter iterDate(listDate);
	TObject* objDate = nullptr;
	while((objDate = iterDate())) {
	    TDirectory* dirDate = dirItem->GetDirectory(objDate->GetName());
	    TList* listTime = dirDate->GetListOfKeys();

	    TIter iterTime(listTime);
	    TObject* objTime = nullptr;
	    while((objTime = iterTime())) {
		cout << objItem->GetName() << "/" << objDate->GetName()
		     << "/" << objTime->GetName() << endl;
	    }
	}
    }

    TH1D* h1 = (TH1D*)f1->Get("HistoCh0/20210409/000000");
    h1->SetTitle("20210409000000 (for test)");
    cout << h1->GetTitle() << endl;

    h1->Draw("HIST");
    c->Print("plotting/overlap/testing.png");

    TF1* testF = new TF1("testF",  "expo(0)", 0., 5.);
    testF->SetRange(0.5, 1.5);
    testF->SetParameter(0, 1.);
    testF->SetParameter(1, -3.);
    testF->Draw();
    c->Update();
    c->Print("plotting/fittingHualien/rangeTest.png");

    f1->Close();
    */
    doProcedure2();
}



void Handling::doProcedure1() {
    TCanvas* c = new TCanvas("c", "c", 1400, 800);
    TLegend* legend = new TLegend(0.5, 0.5, 0.9, 0.9);

    TFile* fLab923 = new TFile("ready/bkg_lab923.root");
    TFile* fLab201 = new TFile("ready/bkg_lab201.root");
    TFile* fHLStat = new TFile("ready/bkg_HL0409.root");

    TH1D* hLab923 = (TH1D*)fLab923->Get("HistoCh0/20210313/003000");
    TH1D* hLab201 = (TH1D*)fLab201->Get("HistoCh0/20201222/153000");
    TH1D* hHLStat = (TH1D*)fHLStat->Get("HistoCh0/20210409/003000");

    hLab201->Draw("HIST");
    hLab923->Draw("HIST SAME");
    hHLStat->Draw("HIST SAME");

    gPad->SetLogy(0);

    hLab201->SetTitle("Background at Three Different Location in 30 Minutes");
    hLab201->SetStats(kFALSE);

    hLab201->SetLineColor(kBlack);
    hLab923->SetLineColor(kBlue);
    hHLStat->SetLineColor(kRed);

    legend->SetHeader("Legend", "C");
    char entryLine[50];
    sprintf(entryLine, "Lab201, %.0f events", hLab201->GetEntries());
    legend->AddEntry(hLab201, entryLine);
    sprintf(entryLine, "Lab923, %.0f events", hLab923->GetEntries());
    legend->AddEntry(hLab923, entryLine);
    sprintf(entryLine, "Hualien, %.0f events", hHLStat->GetEntries());
    legend->AddEntry(hHLStat, entryLine);

    legend->Draw();

    c->Print("plotting/overlap/bkg_overlap.png");
    fLab923->Close();
    fLab201->Close();
    fHLStat->Close();
}



void Handling::doProcedure2() {
    TFile* f1Hist = new TFile("ready/0404to0415oneHist.root", "READ");
    TH1D* h1Hist = (TH1D*)f1Hist->Get("HistoCh0/20210404/000000");

    vector<string> termList;
    vector<string> peakList;
    vector<string> timeUnitList;

    termList.push_back("cExp");
    termList.push_back("expo");
    termList.push_back("cGauss");
    termList.push_back("mean");
    termList.push_back("std");

    peakList.push_back("K40");
    peakList.push_back("Rn222");

    timeUnitList.push_back("P11D");
    timeUnitList.push_back("PD");

    map<string, TF1*> f;
    map<string, TGraph*> g;

    for(unsigned iTerm = 0; iTerm < termList.size(); iTerm++) {
	for(unsigned iPeak = 0; iPeak < peakList.size(); iPeak++) {
	    for(unsigned iTU = 0; iTU < timeUnitList.size(); iTU++) {
		string mapLabel = termList[iTerm] + "," + peakList[iPeak] + "," + timeUnitList[iTU];
		string setName = "f" + termList[iTerm] + peakList[iPeak] + timeUnitList[iTU];

		if(timeUnitList[iTU] == "P11D")
		    f[mapLabel] = new TF1(setName.c_str(), "[0]", 0., 11.);
		else if(timeUnitList[iTU] == "PD")
		    g[mapLabel] = new TGraph();
	    }
	}
    }

    h1Hist->SetTitle("20210404000000_per_11_days");

    PeakFitter* pf = new PeakFitter(h1Hist, "K40");
    pf->setPeakType("K40");
    pf->setHistoName("20210404000000_per_11_days_K40");
    pf->fitPeak();

    for(map<string, TF1*>::iterator it = f.begin(); it != f.end(); ++it) {
	string term = giveTermLabel(it->first);
	string peakType = givePeakLabel(it->first);
	string timeUnit = giveTimeUnitLabel(it->first);

	if(timeUnit == "P11D")
	    if(peakType == "K40")
		it->second->SetParameter(0, pf->getAssignedValue(term));
    }

    pf->setPeakType("Rn222");
    pf->setHistoName("20210404000000_per_11_days_Rn222");
    pf->fitPeak();

    for(map<string, TF1*>::iterator it = f.begin(); it != f.end(); ++it) {
	string term = giveTermLabel(it->first);
	string peakType = givePeakLabel(it->first);
	string timeUnit = giveTimeUnitLabel(it->first);

	if(timeUnit == "P11D")
	    if(peakType == "Rn222")
		it->second->SetParameter(0, pf->getAssignedValue(term));
    }



    Calendar* startDateTime = nullptr;

    TFile* f1HpD = new TFile("ready/0404to0415aHistPDay.root", "READ");
    TDirectory* dirCh0 = f1HpD->GetDirectory("HistoCh0");
    TList* listDate = dirCh0->GetListOfKeys();
    listDate->Sort();

    TIter iterDate(listDate);
    TObject* objDate = nullptr;
    while((objDate = iterDate())) {
	if(startDateTime == nullptr)
	    startDateTime = new Calendar((string)(objDate->GetName()) + "000000");

	Calendar* thisDateTime = new Calendar((string)(objDate->GetName()) + "000000");
	double countDay = (double)thisDateTime->getYDay() - (double)startDateTime->getYDay() + 0.5;

	TDirectory* dirDate = dirCh0->GetDirectory(objDate->GetName());
	TList* listTime = dirDate->GetListOfKeys();

	TIter iterTime(listTime);
	TObject* objTime = nullptr;
	while((objTime = iterTime())) {
	    TH1D* thisH = (TH1D*)dirDate->Get(objTime->GetName());
	    char histTitle[50];
	    sprintf(histTitle, "%s%s_per_01_day", objDate->GetName(), objTime->GetName());
	    thisH->SetTitle(histTitle);
	    pf->setHistogram(thisH);

	    string histoName = (string)histTitle + "_K40";
	    pf->setPeakType("K40");
	    pf->setHistoName(histoName);
	    pf->fitPeak();

	    for(map<string, TGraph*>::iterator it = g.begin();
		it != g.end(); ++it) {
		string term = giveTermLabel(it->first);
		string peakType = givePeakLabel(it->first);
		string timeUnit = giveTimeUnitLabel(it->first);

		if(timeUnit == "PD")
		    if(peakType == "K40")
			it->second->SetPoint(it->second->GetN(),
					     countDay, pf->getAssignedValue(term));
	    }

	    histoName = (string)histTitle + "_Rn222";
	    pf->setPeakType("Rn222");
	    pf->setHistoName(histoName);
	    pf->fitPeak();

	    for(map<string, TGraph*>::iterator it = g.begin();
		it != g.end(); ++it) {
		string term = giveTermLabel(it->first);
		string peakType = givePeakLabel(it->first);
		string timeUnit = giveTimeUnitLabel(it->first);

		if(timeUnit == "PD")
		    if(peakType == "Rn222")
			it->second->SetPoint(it->second->GetN(),
					     countDay, pf->getAssignedValue(term));
	    }
	}
    }


    TCanvas* cGraph = new TCanvas("cGraph", "cGraph", 1400, 800);

    for(unsigned int iTerm = 0; iTerm < termList.size(); iTerm++) {
	for(unsigned int iPeak = 0; iPeak < peakList.size(); iPeak++) {
	    string funcLabel = termList[iTerm] + "," + peakList[iPeak] + ","
		+ timeUnitList[0];
	    string graphLabel = termList[iTerm] + "," + peakList[iPeak] + ","
		+ timeUnitList[1];
	    string outputFilename = "plotting/fittingHualien/g_" + termList[iTerm] + peakList[iPeak] + ".png";

	    TGraph* thisG = g[graphLabel];
	    TF1*    thisF = f[funcLabel];

	    thisG->Draw("ALP");
	    thisF->Draw("SAME");
	    setGraphAtt(thisG, termList[iTerm]);
	    setRangeUser(thisG, termList[iTerm], peakList[iPeak]);
	    cGraph->Update();
	    cGraph->Print(outputFilename.c_str());
	}
    }

    f1Hist->Close();
    f1HpD->Close();
}



void Handling::doProcedure3() {
}



void Handling::readFilenameListTxt(string inputTxtFile) {
    ifstream filenameListTxt;
    filenameListTxt.open(inputTxtFile.c_str());

    string line;
    while(filenameListTxt.is_open()) {
	if(filenameListTxt.eof()) break;
	getline(filenameListTxt, line);
	filenameList.push_back(line);
    }
}



void Handling::setGraphAtt(TGraph* inputG, string term) {
    if(term == "cExp") {
	inputG->SetTitle("Coefficient of Exponential Term");
	inputG->GetYaxis()->SetTitle("Coefficient");
    } else if(term == "expo") {
	inputG->SetTitle("Exponential Constant");
	inputG->GetYaxis()->SetTitle("Exponential Constant");
    } else if(term == "cGauss") {
	inputG->SetTitle("Coefficient of Gaussian Term");
	inputG->GetYaxis()->SetTitle("Coefficient");
    } else if(term == "mean") {
	inputG->SetTitle("Mean of the Peak");
	inputG->GetYaxis()->SetTitle("Mean");
    } else if(term == "std") {
	inputG->SetTitle("Std of the Peak");
	inputG->GetYaxis()->SetTitle("Std");
    } else {
	cout << "Unknown term to described. Skip this plotting." << endl;
	return;
    }

    inputG->GetXaxis()->SetTitle("Days");
    inputG->SetMarkerSize(2);
    inputG->SetMarkerStyle(kFullDotLarge);
    inputG->SetMarkerColor(kRed);
    inputG->SetLineWidth(4);
    inputG->SetLineColor(kBlack);
}



string Handling::giveTermLabel(string inputStr) {
    return inputStr.substr(0, inputStr.find_first_of(","));
}



string Handling::givePeakLabel(string inputStr) {
    inputStr = inputStr.substr(inputStr.find_first_of(",") + 1);
    return inputStr.substr(0, inputStr.find_first_of(","));
}



string Handling::giveTimeUnitLabel(string inputStr) {
    inputStr = inputStr.substr(inputStr.find_first_of(",") + 1);
    inputStr = inputStr.substr(inputStr.find_first_of(",") + 1);
    return inputStr.substr(0, inputStr.find_first_of(","));
}



void Handling::setRangeUser(TGraph* inputG, string term, string peakType) {
    double upper = 1.;
    double lower = 0.;

    if(term == "cExp") {
	if(peakType == "K40") {
	    upper = 20.;
	    lower = 0.;
	} else if(peakType == "Rn222") {
	    upper = 20.;
	    lower = 0.;
	}
    } else if(term == "expo") {
	if(peakType == "K40") {
	    upper = 0.;
	    lower = -3.;
	} else if(peakType == "Rn222") {
	    upper = 0.;
	    lower = -4.;
	}
    } else if(term == "cGauss") {
	if(peakType == "K40") {
	    upper = 200000.;
	    lower = 0.;
	} else if(peakType == "Rn222") {
	    upper = 13500.;
	    lower = 0.;
	}
    } else if(term == "mean") {
	if(peakType == "K40") {
	    upper = 2.;
	    lower = 0.;
	} else if(peakType == "Rn222") {
	    upper = 0.8;
	    lower = 0.;
	}
    } else if(term == "std") {
	if(peakType == "K40") {
	    upper = 0.05;
	    lower = 0.;
	} else if(peakType == "Rn222") {
	    upper = 0.035;
	    lower = 0.;
	}
    }

    inputG->GetYaxis()->SetRangeUser(lower, upper);
}
