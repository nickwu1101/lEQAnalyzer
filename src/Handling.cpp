#include "Handling.h"

#include "Calendar.h"
#include "GetExterSet.h"
#include "PeakFitter.h"
#include "TempHumi.h"

#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <TPaveText.h>
#include <TTree.h>

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

    doProcedure5();
    doProcedure7();
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

    c->Close();
    fLab923->Close();
    fLab201->Close();
    fHLStat->Close();

    delete c;
    delete legend;
    delete fLab923;
    delete fLab201;
    delete fHLStat;
}



void Handling::doProcedure2() {
    TCanvas* cGraph = new TCanvas("cGraph", "cGraph", 1400, 800);

    vector<string> termList;
    vector<string> peakList;
    vector<string> timeUnitList;

    termList.push_back("cPow");
    termList.push_back("cExp");
    termList.push_back("expo");
    termList.push_back("cGauss");
    termList.push_back("mean");
    termList.push_back("std");
    termList.push_back("cExp_Norm");
    termList.push_back("cGauss_Norm");
    termList.push_back("EstEvents");

    peakList.push_back("peak16");
    peakList.push_back("peak06");
    peakList.push_back("peak04");
    peakList.push_back("peak24");

    timeUnitList.push_back("PT");
    timeUnitList.push_back("PD");
    timeUnitList.push_back("P12H");
    timeUnitList.push_back("P6H");
    timeUnitList.push_back("P2H");
    timeUnitList.push_back("P1H");

    map<string, TF1*> f;
    map<string, TGraph*> g;
    map<string, TGraphErrors*> ge;

    for(unsigned int iTerm = 0; iTerm < termList.size(); iTerm++) {
	for(unsigned int iPeak = 0; iPeak < peakList.size(); iPeak++) {
	    for(unsigned int iTU = 0; iTU < timeUnitList.size(); iTU++) {
		string mapLabel = termList[iTerm] + "," + peakList[iPeak] + "," + timeUnitList[iTU];
		string setName = "f" + termList[iTerm] + peakList[iPeak] + timeUnitList[iTU];

		if(timeUnitList[iTU] == "PT")
		    f[mapLabel] = new TF1(setName.c_str(), "[0]", 0., 24.);
		else if(timeUnitList[iTU] == "PD"
			|| timeUnitList[iTU] == "P12H"
			|| timeUnitList[iTU] == "P6H"
			|| timeUnitList[iTU] == "P2H"
			|| timeUnitList[iTU] == "P1H") {
		    g[mapLabel] = new TGraph();

		    if(termList[iTerm].find("cExp") == string::npos)
			ge[mapLabel] = new TGraphErrors();
		}
	    }
	}
    }

    for(unsigned int iTU = 0; iTU < timeUnitList.size(); iTU++) {
	TFile* file = nullptr;
	string thisTU = timeUnitList[iTU];
	if(thisTU == "PT")
	    file = new TFile("ready/0418to0512oneHist.root", "READ");
	else if(thisTU == "PD")
	    file = new TFile("ready/0418to0512aHistPD.root", "READ");
	else if(thisTU == "P12H")
	    file = new TFile("ready/0418to0512aHistP12h.root", "READ");
	else if(thisTU == "P6H")
	    file = new TFile("ready/0418to0512aHistP06h.root", "READ");
	else if(thisTU == "P2H")
	    file = new TFile("ready/0418to0512aHistP02h.root", "READ");
	else if(thisTU == "P1H")
	    file = new TFile("ready/0418to0512aHistP01h.root", "READ");

	TDirectory* dirCh0 = file->GetDirectory("HistoCh0");
	TList* listDate = dirCh0->GetListOfKeys();
	listDate->Sort();

	Calendar* startDateTime = nullptr;

	TIter iterDate(listDate);
	TObject* objDate = nullptr;
	while((objDate = iterDate())) {
	    TDirectory* dirDate = dirCh0->GetDirectory(objDate->GetName());
	    TList* listTime = dirDate->GetListOfKeys();

	    TIter iterTime(listTime);
	    TObject* objTime = nullptr;
	    while((objTime = iterTime())) {
		if(startDateTime == nullptr)
		    startDateTime = new Calendar((string)(objDate->GetName()) + (string)(objTime->GetName()));

		Calendar* thisDateTime = new Calendar((string)(objDate->GetName()) + (string)(objTime->GetName()));

		double countHour = ((double)thisDateTime->getYDay()*24. + (double)thisDateTime->getHour()) - ((double)startDateTime->getYDay()*24 + (double)startDateTime->getHour());

		double alignCenter = 0.;
		string strForTitle;
		if(thisTU == "PT") {
		    alignCenter = 0.;
		    strForTitle = "all_50_days";
		} else if(thisTU == "PD") {
		    alignCenter = 12.;
		    strForTitle = "per_01_day";
		} else if(thisTU == "P12H") {
		    alignCenter = 6.;
		    strForTitle = "per_12_hours";
		} else if(thisTU == "P6H") {
		    alignCenter = 3.;
		    strForTitle = "per_06_hours";
		} else if(thisTU == "P2H") {
		    alignCenter = 1.;
		    strForTitle = "per_02_hours";
		} else if(thisTU == "P1H") {
		    alignCenter = 0.5;
		    strForTitle = "per_01_hour";
		} else {
		    cout << "Time unit term is set inappropriately!!!" << endl;
		    return;
		}
		countHour += alignCenter;
		double countDay = countHour/24.;

		TH1D* thisH = (TH1D*)dirDate->Get(objTime->GetName());
		char histTitle[80];
		sprintf(histTitle, "%s%s_%s", objDate->GetName(), objTime->GetName(), strForTitle.c_str());
		thisH->SetTitle(histTitle);
		cout << histTitle << endl;
		TH1D* hScaled = (TH1D*)thisH->Clone();





		string peakForNorm = "peak16";
		PeakFitter* pf = new PeakFitter(thisH, peakForNorm);
		pf->setPeakType(peakForNorm);
		string histoName = (string)histTitle + "_" + peakForNorm;
		pf->setHistoName(histoName);
		pf->setNeedZoom(false);
		pf->fitPeak();

		double cGaussForNorm = pf->getCGauss(0);
		double errCGausForNorm = pf->getErrorCGaus(0);
		cGraph->cd();
		hScaled->Scale(1./cGaussForNorm);
		hScaled->Draw("HISTO");
		cGraph->Update();
		char outputHistFilename[250];
		sprintf(outputHistFilename,
			"plotting/fittingHualien/scaled/%s_scaled.png",
			histTitle);
 		cGraph->Print(outputHistFilename);
		hScaled->Delete();

		for(unsigned int iTerm = 0; iTerm < termList.size(); iTerm++) {
		    string thisTerm = termList[iTerm];
		    string label = thisTerm + "," + peakForNorm + "," + thisTU;

		    double keyInValue = 0.;
		    double keyInError = 0.;
		    if(thisTerm.find("Norm") == string::npos) {
			if(thisTerm == "EstEvents") {
			    keyInValue = pf->getAssignedValue("cGauss", 0);
			    keyInError = pf->getAssignedError("cGauss", 0);

			    double binWidth = pf->getHistogram()->GetBinWidth(0);
			    keyInValue /= binWidth;
			    keyInError /= binWidth;
			} else {
			    keyInValue = pf->getAssignedValue(thisTerm, 0);
			    keyInError = pf->getAssignedError(thisTerm, 0);
			}
		    } else {
			string actualTerm = thisTerm.substr(0, thisTerm.find_first_of("_"));
			keyInValue = pf->getAssignedValue(actualTerm, 0)/cGaussForNorm;
			if(thisTerm.find("cGauss") != string::npos) {
			    keyInError = 0.;
			} else {
			    double ubar = pf->getAssignedValue(actualTerm, 0);
			    double usig = pf->getAssignedError(actualTerm, 0);
			    double vbar = cGaussForNorm;
			    double vsig = errCGausForNorm;

			    keyInError = TMath::Sqrt((usig*usig)/(ubar*ubar)
						     + (vsig*vsig)/(vbar*vbar));
			}
		    }

		    if(f.find(label) != f.cend()) {
			f[label]->SetParameter(0, keyInValue);
		    }

		    if(g.find(label) != g.cend()) {
			TGraph* thisG = g[label];
			thisG->SetPoint(thisG->GetN(), countDay, keyInValue);
		    }

		    if(ge.find(label) != ge.cend()) {
			TGraphErrors* thisGE = ge[label];
			thisGE->SetPoint(thisGE->GetN(), countDay, keyInValue);
			thisGE->SetPointError(thisGE->GetN() - 1, 0., keyInError);
		    }
		}





		for(unsigned int iPeak = 0; iPeak < peakList.size(); iPeak++) {
		    string thisPeak = peakList[iPeak];
		    if(thisPeak == peakForNorm)
			continue;

		    pf->setPeakType(thisPeak);
		    histoName = (string)histTitle + "_" + thisPeak;
		    pf->setHistoName(histoName);
		    pf->setNeedZoom(false);

		    pf->fitPeak();

		    for(unsigned int iTerm = 0; iTerm < termList.size(); iTerm++) {
			string thisTerm = termList[iTerm];
			string label = thisTerm + "," + thisPeak + "," + thisTU;

			double keyInValue = 0.;
			double keyInError = 0.;
			if(thisTerm.find("Norm") == string::npos) {
			    if(thisTerm == "EstEvents") {
				keyInValue = pf->getAssignedValue("cGauss", 0);
				keyInError = pf->getAssignedError("cGauss", 0);

				double binWidth = pf->getHistogram()->GetBinWidth(0);
				keyInValue /= binWidth;
				keyInError /= binWidth;
			    } else {
				keyInValue = pf->getAssignedValue(thisTerm, 0);
				keyInError = pf->getAssignedError(thisTerm, 0);
			    }
			} else {
			    string actualTerm = thisTerm.substr(0, thisTerm.find_first_of("_"));
			    keyInValue = pf->getAssignedValue(actualTerm, 0)/cGaussForNorm;

			    double ubar = pf->getAssignedValue(actualTerm, 0);
			    double usig = pf->getAssignedError(actualTerm, 0);
			    double vbar = cGaussForNorm;
			    double vsig = errCGausForNorm;

			    keyInError = TMath::Sqrt((usig*usig)/(ubar*ubar)
						     + (vsig*vsig)/(vbar*vbar));
			}

			if(f.find(label) != f.cend()) {
			    f[label]->SetParameter(0, keyInValue);
			}

			if(g.find(label) != g.cend()) {
			    TGraph* thisG = g[label];
			    thisG->SetPoint(thisG->GetN(), countDay, keyInValue);
			}

			if(ge.find(label) != ge.cend()) {
			    TGraphErrors* thisGE = ge[label];
			    thisGE->SetPoint(thisGE->GetN(), countDay, keyInValue);
			    thisGE->SetPointError(thisGE->GetN() - 1, 0., keyInError);
			}
		    }
		}

		delete pf;
		delete thisDateTime;

		cout << endl << endl << endl;
	    }
	}

	delete startDateTime;

	file->Close();
	delete file;
    }





    cGraph->cd();

    for(unsigned int iTerm = 0; iTerm < termList.size(); iTerm++) {
	for(unsigned int iPeak = 0; iPeak < peakList.size(); iPeak++) {
	    for(unsigned int iTU = 0; iTU < timeUnitList.size(); iTU++) {
		string thisTerm = termList[iTerm];
		string thisPeak = peakList[iPeak];
		string thisTU = timeUnitList[iTU];
		string funcLabel = thisTerm + "," + thisPeak + "," + timeUnitList[0];
		TF1* thisF = f[funcLabel];

		string graphLabel = thisTerm + "," + thisPeak + "," + thisTU;

		if(g.find(graphLabel) != g.cend()) {
		    string folderPath = "plotting/fittingHualien/g";
		    string outputFilename = folderPath + "/g_"
			+ thisTerm + "_"
			+ thisPeak + "_"
			+ thisTU + ".png";

		    TGraph* thisG = g[graphLabel];
		    thisG->Draw("AP");
		    thisF->Draw("SAME");
		    setGraphAtt(thisG, thisTerm, thisPeak);
		    setRangeUser(thisG->GetYaxis(), thisTerm, thisPeak, thisTU);
		    cGraph->Update();
		    cGraph->Print(outputFilename.c_str());
		}

		if(ge.find(graphLabel) != ge.cend()) {
		    string folderPath = "plotting/fittingHualien/ge";
		    string outputFilename = folderPath + "/ge_"
			+ thisTerm + "_"
			+ thisPeak + "_"
			+ thisTU + ".png";

		    TGraphErrors* thisGE = ge[graphLabel];
		    thisGE->Draw("AP");
		    thisF->Draw("SAME");
		    setGraphAtt(thisGE, thisTerm, thisPeak);
		    setRangeUser(thisGE->GetYaxis(), thisTerm, thisPeak, thisTU);
		    cGraph->Update();
		    cGraph->Print(outputFilename.c_str());
		}
	    }
	}
    }

    for(map<string, TF1*>::iterator it = f.begin(); it != f.end(); ++it)
	delete it->second;

    for(map<string, TGraph*>::iterator it = g.begin(); it != g.end(); ++it)
	delete it->second;

    for(map<string, TGraphErrors*>::iterator it = ge.begin(); it != ge.end(); ++it)
	delete it->second;

    cGraph->Close();
    delete cGraph;
}



void Handling::doProcedure3() {
    double totalHours = takeTimeLength("PT");
    TCanvas* cGraph = new TCanvas("cGraph", "cGraph", 1400, 800);
    TempHumi* th = new TempHumi();

    TFile* foutput = new TFile("ready/fluctResult.root", "RECREATE");
    int entryNo;
    int outputyr;
    int outputmon;
    int outputday;
    int outputhr;
    int outputmin;
    double outputsec;
    double outcounts04;
    double outcounts06;
    double outcounts16;
    double outNormCounts04;
    double outNormCounts06;
    double outNormCounts16;
    double outputtemp;

    TTree* dataTree = new TTree("dataCounts", "dataTree");
    dataTree->Branch("entryNo", &entryNo, "entryNo/I");
    dataTree->Branch("year", &outputyr, "year/I");
    dataTree->Branch("month", &outputmon, "month/I");
    dataTree->Branch("day", &outputday, "day/I");
    dataTree->Branch("hour", &outputhr, "hour/I");
    dataTree->Branch("minute", &outputmin, "minute/I");
    dataTree->Branch("second", &outputsec, "second/D");
    dataTree->Branch("counts04", &outcounts04, "counts04/D");
    dataTree->Branch("counts06", &outcounts06, "coutns06/D");
    dataTree->Branch("counts16", &outcounts16, "counts16/D");
    dataTree->Branch("normCounts04", &outNormCounts04, "counts04/D");
    dataTree->Branch("normCounts06", &outNormCounts06, "counts06/D");
    dataTree->Branch("normCounts16", &outNormCounts16, "counts16/D");
    dataTree->Branch("temperature", &outputtemp, "temperature/D");

    map<string, double> mapCounts04;
    map<string, double> mapCounts06;
    map<string, double> mapCounts16;
    map<string, double> mapNormCounts04;
    map<string, double> mapNormCounts06;
    map<string, double> mapNormCounts16;
    map<string, double> mapTemp;

    vector<string> energyRange;
    vector<string> timeUnitList;

    energyRange.push_back("peak06");
    energyRange.push_back("peak16");
    energyRange.push_back("peak04");
    energyRange.push_back("peak24");
    energyRange.push_back("0to25");
    energyRange.push_back("peak04n06");

    timeUnitList.push_back("PD");
    timeUnitList.push_back("P12H");
    timeUnitList.push_back("P6H");
    timeUnitList.push_back("P2H");
    timeUnitList.push_back("P1H");

    map<string, double> norm;
    map<string, TGraphErrors*> ge;
    map<string, TGraphErrors*> gne;
    map<string, TH1I*> h;
    map<string, TH1I*> hTC;
    map<string, TH2I*> h2h;
    map<string, TH2I*> h2d;
    map<string, TGraph*> gTCorrect;
    map<string, TGraphErrors*> geTCN;

    map<string, string> startDT;
    map<string, double> mean;
    map<string, double> std;
    map<string, double> ntimebin;

    map<string, double> p1;
    double tf = 30.;

    for(unsigned int iER = 0; iER < energyRange.size(); iER++) {
	for(unsigned int iTU = 0; iTU < timeUnitList.size(); iTU++) {
	    string mapLabel = energyRange[iER] + "," + timeUnitList[iTU];
	    ge[mapLabel] = new TGraphErrors();
	    gne[mapLabel] = new TGraphErrors();

	    char hTitle[100];
	    sprintf(hTitle, "h_%s", mapLabel.c_str());
	    h[mapLabel] = new TH1I(hTitle, hTitle, 100, 0., 100.);
	    sprintf(hTitle, "hTC_%s", mapLabel.c_str());
	    hTC[mapLabel] = new TH1I(hTitle, hTitle, 100, 0., 100.);
	    sprintf(hTitle, "h2h_%s", mapLabel.c_str());
	    h2h[mapLabel] = new TH2I(hTitle, hTitle, 100, 0., 100., 100, 0., 100.);
	    sprintf(hTitle, "h2d_%s", mapLabel.c_str());
	    h2d[mapLabel] = new TH2I(hTitle, hTitle, 100, 0., 100., 100, 0., 100.);

	    setBins(h[mapLabel], "fluct", energyRange[iER], timeUnitList[iTU]);
	    setBins(hTC[mapLabel], "fluct", energyRange[iER], timeUnitList[iTU]);
	    setBins(h2h[mapLabel], "hour", energyRange[iER], timeUnitList[iTU]);
	    setBins(h2d[mapLabel], "day", energyRange[iER], timeUnitList[iTU]);

	    if(timeUnitList[iTU] == "P1H") {
		gTCorrect[mapLabel] = new TGraph();
		geTCN[mapLabel] = new TGraphErrors();

		if(energyRange[iER] == "peak04")
		    p1[mapLabel] = 204.765266;
		else if(energyRange[iER] == "peak06")
		    p1[mapLabel] = 133.954019;
		else if(energyRange[iER] == "peak16")
		    p1[mapLabel] = -95.016695;
		else if(energyRange[iER] == "peak24")
		    p1[mapLabel] = -76.430053;
		else if(energyRange[iER] == "0to25")
		    p1[mapLabel] = 2699.321043;
		else if(energyRange[iER] == "peak04n06")
		    p1[mapLabel] = 346.309406;
	    } else if(timeUnitList[iTU] == "P2H") {
		gTCorrect[mapLabel] = new TGraph();
		geTCN[mapLabel] = new TGraphErrors();

		if(energyRange[iER] == "peak04")
		    p1[mapLabel] = 406.487772;
		else if(energyRange[iER] == "peak06")
		    p1[mapLabel] = 263.251622;
		else if(energyRange[iER] == "peak16")
		    p1[mapLabel] = -196.246095;
		else if(energyRange[iER] == "peak24")
		    p1[mapLabel] = -151.348071;
		else if(energyRange[iER] == "0to25")
		    p1[mapLabel] = 5377.795725;
		else if(energyRange[iER] == "peak04n06")
		    p1[mapLabel] = 674.998087;
	    }

	    mean[mapLabel] = 0.;
	    std[mapLabel] = 0.;
	    ntimebin[mapLabel] = 0.;
	}
    }





    TFile* fileT = new TFile("ready/0418to0607oneHist.root", "READ");
    for(unsigned int iER = 0; iER < energyRange.size(); iER++) {
	string thisER = energyRange[iER];
	if(thisER == "peak04n06")
	    continue;

	string histoPath = "withFilt_" + thisER + "/20210418/000000";
	TH1D* hT = (TH1D*)fileT->Get(histoPath.c_str());
	norm[thisER] = hT->GetEntries();
    }
    fileT->Close();
    delete fileT;

    for(unsigned int iTU = 0; iTU < timeUnitList.size(); iTU++) {
	TFile* file = nullptr;
	string thisTU = timeUnitList[iTU];
	if(thisTU == "PD")
	    file = new TFile("ready/0418to0607aHistPD.root", "READ");
	else if(thisTU == "P12H")
	    file = new TFile("ready/0418to0607aHistP12h.root", "READ");
	else if(thisTU == "P6H")
	    file = new TFile("ready/0418to0607aHistP06h.root", "READ");
	else if(thisTU == "P2H")
	    file = new TFile("ready/0418to0607aHistP02h.root", "READ");
	else if(thisTU == "P1H")
	    file = new TFile("ready/0418to0607aHistP01h.root", "READ");

	for(unsigned int iER = 0; iER < energyRange.size(); iER++) {
	    string thisER = energyRange[iER];
	    string mapLabel = thisER + "," + thisTU;

	    string dirERname = "withFilt_" + thisER;
	    TDirectory* dirER = file->GetDirectory(dirERname.c_str());
	    TList* listDate = dirER->GetListOfKeys();
	    listDate->Sort();

	    Calendar* startDateTime = nullptr;

	    TIter iterDate(listDate);
	    TObject* objDate = nullptr;
	    while((objDate = iterDate())) {
		TDirectory* dirDate = dirER->GetDirectory(objDate->GetName());
		TList* listTime = dirDate->GetListOfKeys();

		TIter iterTime(listTime);
		TObject* objTime = nullptr;
		while((objTime = iterTime())) {
		    if(startDateTime == nullptr)
			startDateTime = new Calendar((string)(objDate->GetName()) + (string)(objTime->GetName()));

		    Calendar* thisDateTime = new Calendar((string)(objDate->GetName()) + (string)(objTime->GetName()));
		    Calendar* dtTemp = new Calendar((string)(objDate->GetName()) + (string)(objTime->GetName()));
		    TDatime* tdt = new TDatime(thisDateTime->getYear(),
					       thisDateTime->getMonth(),
					       thisDateTime->getMDay(),
					       thisDateTime->getHour(),
					       thisDateTime->getMinute(),
					       thisDateTime->getSecond());

		    double countHour = ((double)thisDateTime->getYDay()*24. + (double)thisDateTime->getHour()) - ((double)startDateTime->getYDay()*24. + (double) startDateTime->getHour());

		    double alignCenter = takeTimeLength(thisTU)/2.;
		    if(thisTU == "P1H")
			dtTemp->addDuration(0, 0, 0, 30, 0.);
		    else
			dtTemp->addDuration(0, 0, alignCenter, 0, 0.);
		    double thisTemp = th->getAvgTemp(dtTemp->getDateTime());
		    countHour += alignCenter;
		    double countDay = countHour/24.;

		    TH1D* thisH = (TH1D*)dirDate->Get(objTime->GetName());
		    bool doPutOn = true;

		    if(false) {
			double upper = 0.;
			double lower = 0.;

			getHistUpLow("fluct", thisER, thisTU, upper, lower);

			if(thisH->GetEntries() <= upper &&
			   thisH->GetEntries() >= lower)
			    doPutOn = true;
			else
			    doPutOn = false;
		    }

		    if(doPutOn) {
			ge[mapLabel]->SetPoint(ge[mapLabel]->GetN(), tdt->Convert(), thisH->GetEntries());
			ge[mapLabel]->SetPointError(ge[mapLabel]->GetN() - 1, 0, TMath::Sqrt(thisH->GetEntries()));

			if(gTCorrect.find(mapLabel) != gTCorrect.cend())
			    gTCorrect[mapLabel]->SetPoint(gTCorrect[mapLabel]->GetN(), tdt->Convert(), thisH->GetEntries() - p1[mapLabel]*(thisTemp - tf));
		    }

		    h[mapLabel]->Fill(thisH->GetEntries());
		    hTC[mapLabel]->Fill(thisH->GetEntries() - p1[mapLabel]*(thisTemp - tf));
		    h2h[mapLabel]->Fill((double)thisDateTime->getHour(), thisH->GetEntries());
		    h2d[mapLabel]->Fill((double)thisDateTime->getYDay() - (double)startDateTime->getYDay(), thisH->GetEntries());

		    mean[mapLabel] += thisH->GetEntries();
		    std[mapLabel] += thisH->GetEntries()*thisH->GetEntries();
		    ntimebin[mapLabel]++;

		    if(thisTU == "P2H") {
			mapTemp[thisDateTime->getDateTime()] = thisTemp;

			if(thisER == "peak04") {
			    mapCounts04[thisDateTime->getDateTime()] = thisH->GetEntries();
			} else if(thisER == "peak06") {
			    mapCounts06[thisDateTime->getDateTime()] = thisH->GetEntries();
			} else if(thisER == "peak16") {
			    mapCounts16[thisDateTime->getDateTime()] = thisH->GetEntries();
			}
		    }

		    delete tdt;
		    delete dtTemp;
		    delete thisDateTime;
		}
	    }

	    for(int i = 0; i < ge[mapLabel]->GetN(); i++) {
		double x = 0.;
		double y = 0.;
		ge[mapLabel]->GetPoint(i, x, y);

		double dataMean = y;
		double dataMErr = TMath::Sqrt(y);
		double averMean = h[mapLabel]->GetMean();
		double averMErr = h[mapLabel]->GetMeanError();
		gne[mapLabel]->SetPoint(i, x, dataMean/averMean);
		gne[mapLabel]->SetPointError(i, 0., TMath::Sqrt((dataMErr*dataMErr)/(dataMean*dataMean) + (averMErr*averMErr)/(averMean*averMean)));

		if(gTCorrect.find(mapLabel) != gTCorrect.cend()) {
		    gTCorrect[mapLabel]->GetPoint(i, x, y);
		    dataMean = y;
		    dataMErr = TMath::Sqrt(y);
		    averMean = hTC[mapLabel]->GetMean();
		    averMErr = hTC[mapLabel]->GetMeanError();
		    double normMean = dataMean/averMean;
		    double normErr = TMath::Sqrt((dataMErr*dataMErr)/(dataMean*dataMean) + (averMErr*averMErr)/(averMean*averMean));
		    geTCN[mapLabel]->SetPoint(i, x, normMean);
		    geTCN[mapLabel]->SetPointError(i, 0, normErr);

		    if(thisTU == "P2H") {
			TDatime* dt = new TDatime(x);
			char dtStr[15];
			sprintf(dtStr, "%04d%02d%02d%02d%02d%02.f",
				dt->GetYear(), dt->GetMonth(), dt->GetDay(),
				dt->GetHour(), dt->GetMinute(), (double)dt->GetSecond());

			if(thisER == "peak04")
			    mapNormCounts04[dtStr] = normMean;
			else if(thisER == "peak06")
			    mapNormCounts06[dtStr] = normMean;
			else if(thisER == "peak16")
			    mapNormCounts16[dtStr] = normMean;

			delete dt;
		    }
		}
	    }

	    startDT[mapLabel] = startDateTime->getDateTime();

	    if(startDateTime != nullptr)
		delete startDateTime;
	}

	file->Close();
	delete file;
    }

    cGraph->cd();
    for(unsigned int iER = 0; iER < energyRange.size(); iER++) {
	string thisER = energyRange[iER];
	for(unsigned int iTU = 0; iTU < timeUnitList.size(); iTU++) {
	    string thisTU = timeUnitList[iTU];
	    string mapLabel = thisER + "," + thisTU;
	    cout << "Now processing: " << mapLabel << endl;

	    mean[mapLabel] /= ntimebin[mapLabel];
	    std[mapLabel] -= ntimebin[mapLabel]*mean[mapLabel]*mean[mapLabel];
	    std[mapLabel] /= ntimebin[mapLabel] - 1;
	    std[mapLabel] = TMath::Sqrt(std[mapLabel]);

	    TGraphErrors* geeTMean = new TGraphErrors();
	    TGraphErrors* geeT1Sigma = new TGraphErrors();
	    TGraphErrors* geeT2Sigma = new TGraphErrors();
	    TGraphErrors* geeHMean = new TGraphErrors();
	    TGraphErrors* geeH1Sigma = new TGraphErrors();
	    TGraphErrors* geeH2Sigma = new TGraphErrors();

	    TGraphErrors* gneeHMean = new TGraphErrors();
	    TGraphErrors* gneeH1Sigma = new TGraphErrors();
	    TGraphErrors* gneeH2Sigma = new TGraphErrors();

	    TGraphErrors* gTCeHMean = new TGraphErrors();
	    TGraphErrors* gTCeH1Sigma = new TGraphErrors();
	    TGraphErrors* gTCeH2Sigma = new TGraphErrors();
	    TGraphErrors* geTCNeHMean = new TGraphErrors();
	    TGraphErrors* geTCNeH1Sigma = new TGraphErrors();
	    TGraphErrors* geTCNeH2Sigma = new TGraphErrors();

	    double legendValue = 0.;
	    double legendValueTC = 0.;

	    /* For counted days
	    for(int i  = 0; i < 25; i++) {
		double normFactor = takeTimeLength(thisTU)*norm[thisER]/totalHours;

		geeTMean->SetPoint(i, i, mean[mapLabel]);
		geeT1Sigma->SetPoint(i, i, mean[mapLabel]);
		geeT2Sigma->SetPoint(i, i, mean[mapLabel]);
		geeHMean->SetPoint(i, i, h[mapLabel]->GetMean());
		geeH1Sigma->SetPoint(i, i, h[mapLabel]->GetMean());
		geeH2Sigma->SetPoint(i, i, h[mapLabel]->GetMean());

		gneeHMean->SetPoint(i, i, 1);
		gneeH1Sigma->SetPoint(i, i, 1);
		gneeH2Sigma->SetPoint(i, i, 1);

		double errorBand = std[mapLabel]/TMath::Sqrt(ntimebin[mapLabel]);
		geeT1Sigma->SetPointError(i, 0, errorBand);
		geeT2Sigma->SetPointError(i, 0, 2*errorBand);

		errorBand = h[mapLabel]->GetMeanError();
		geeH1Sigma->SetPointError(i, 0, errorBand);
		geeH2Sigma->SetPointError(i, 0, 2*errorBand);

		gneeH1Sigma->SetPointError(i, 0, errorBand/h[mapLabel]->GetMean());
		gneeH2Sigma->SetPointError(i, 0, 2*errorBand/h[mapLabel]->GetMean());

		if(i == 0)
		    legendValue = errorBand;
	    }
	    */

	    for(int i = 0; i < 2; i++) {
		TDatime* tdt = nullptr;
		if(i == 0)
		    tdt = new TDatime(2021, 4, 18, 0, 0, 0);
		else if(i == 1)
		    tdt = new TDatime(2021, 6, 7, 0, 0, 0);

		geeTMean->SetPoint(i, tdt->Convert(), mean[mapLabel]);
		geeT1Sigma->SetPoint(i, tdt->Convert(), mean[mapLabel]);
		geeT2Sigma->SetPoint(i, tdt->Convert(), mean[mapLabel]);
		geeHMean->SetPoint(i, tdt->Convert(), h[mapLabel]->GetMean());
		geeH1Sigma->SetPoint(i, tdt->Convert(), h[mapLabel]->GetMean());
		geeH2Sigma->SetPoint(i, tdt->Convert(), h[mapLabel]->GetMean());

		gneeHMean->SetPoint(i, tdt->Convert(), 1.);
		gneeH1Sigma->SetPoint(i, tdt->Convert(), 1.);
		gneeH2Sigma->SetPoint(i, tdt->Convert(), 1.);

		gTCeHMean->SetPoint(i, tdt->Convert(), hTC[mapLabel]->GetMean());
		gTCeH1Sigma->SetPoint(i, tdt->Convert(), hTC[mapLabel]->GetMean());
		gTCeH2Sigma->SetPoint(i, tdt->Convert(), hTC[mapLabel]->GetMean());
		geTCNeHMean->SetPoint(i, tdt->Convert(), 1.);
		geTCNeH1Sigma->SetPoint(i, tdt->Convert(), 1.);
		geTCNeH2Sigma->SetPoint(i, tdt->Convert(), 1.);

		double errorBand = std[mapLabel]/TMath::Sqrt(ntimebin[mapLabel]);
		geeT1Sigma->SetPointError(i, 0., errorBand);
		geeT2Sigma->SetPointError(i, 0., 2*errorBand);

		errorBand = h[mapLabel]->GetMeanError();
		geeH1Sigma->SetPointError(i, 0., errorBand);
		geeH2Sigma->SetPointError(i, 0., 2*errorBand);

		gneeH1Sigma->SetPointError(i, 0., errorBand/h[mapLabel]->GetMean());
		gneeH2Sigma->SetPointError(i, 0., 2*errorBand/h[mapLabel]->GetMean());

		errorBand = hTC[mapLabel]->GetMeanError();
		gTCeH1Sigma->SetPointError(i, 0., errorBand);
		gTCeH2Sigma->SetPointError(i, 0., 2*errorBand);

		geTCNeH1Sigma->SetPointError(i, 0., errorBand/hTC[mapLabel]->GetMean());
		geTCNeH2Sigma->SetPointError(i, 0., 2*errorBand/hTC[mapLabel]->GetMean());

		if(i == 0) {
		    legendValue = h[mapLabel]->GetMeanError();
		    legendValueTC = hTC[mapLabel]->GetMeanError();
		}

		if(tdt != nullptr)
		    delete tdt;
	    }

	    setGraphAtt(ge[mapLabel], "fluct", thisER);
	    setErrorBandAtt(geeTMean, geeT1Sigma, geeT2Sigma, "compare");
	    setErrorBandAtt(geeHMean, geeH1Sigma, geeH2Sigma, "formal");
	    setErrorBandAtt(gneeHMean, gneeH1Sigma, gneeH2Sigma, "formal");

	    TMultiGraph* mg = new TMultiGraph();
	    mg->Add(geeH2Sigma, "A3");
	    mg->Add(geeH1Sigma, "A3");
	    mg->Add(geeHMean, "LC");
	    mg->Add(ge[mapLabel], "AP");
	    setMultiGAtt(mg, "fluct", thisER, thisTU);
	    setRangeUser(mg->GetYaxis(), "fluct", thisER, thisTU);
	    mg->Draw("A");

	    TLegend* lg = new TLegend(0.6, 0.15, 0.9, 0.45);
	    Calendar* stdt = new Calendar(startDT[mapLabel]);
	    string lPeakType = takePeakTypeStr(thisER);

	    char lline[200];
	    lg->SetHeader(lPeakType.c_str(), "C");
	    lg->AddEntry(ge[mapLabel], "data");
	    sprintf(lline, "average: %.1f", h[mapLabel]->GetMean());
	    lg->AddEntry(geeHMean, lline);
	    sprintf(lline, "1 SE: %.2f", legendValue);
	    lg->AddEntry(geeH1Sigma, lline);
	    sprintf(lline, "2 SE: %.2f", 2*legendValue);
	    lg->AddEntry(geeH2Sigma, lline);
	    lg->SetTextSize(0.035);
	    lg->Draw();

	    cGraph->Update();
	    string outputGraphFolder = "plotting/fittingHualien/counting";
	    string outputGraphName = "counts_" + thisER + "_" + thisTU + ".png";
	    string outputGraphPath = outputGraphFolder + "/" + outputGraphName;
	    cGraph->Print(outputGraphPath.c_str());

	    setGraphAtt(gne[mapLabel], "nfluct", thisER);

	    TMultiGraph* mgn = new TMultiGraph();
	    mgn->Add(gneeH2Sigma, "A3");
	    mgn->Add(gneeH1Sigma, "A3");
	    mgn->Add(gneeHMean, "LC");
	    mgn->Add(gne[mapLabel], "AP");

	    setMultiGAtt(mgn, "nfluct", thisER, thisTU);
	    setRangeUser(mgn->GetYaxis(), "nfluct", thisER, thisTU);
	    mgn->Draw("A");

	    TLegend* lgn = new TLegend(0.6, 0.15, 0.9, 0.45);
	    lgn->SetHeader(lPeakType.c_str(), "C");
	    lgn->AddEntry(gne[mapLabel], "data");
	    lgn->AddEntry(gneeHMean, "average");
	    sprintf(lline, "1 SE: %.4f", legendValue/h[mapLabel]->GetMean());
	    lgn->AddEntry(gneeH1Sigma, lline);
	    sprintf(lline, "2 SE: %.4f", 2*legendValue/h[mapLabel]->GetMean());
	    lgn->AddEntry(gneeH2Sigma, lline);
	    lgn->SetTextSize(0.035);
	    lgn->Draw();

	    cGraph->Update();
	    outputGraphName = "NormCounts_" + thisER + "_" + thisTU + ".png";
	    outputGraphPath = outputGraphFolder + "/" + outputGraphName;
	    cGraph->Print(outputGraphPath.c_str());

	    h[mapLabel]->Draw("HISTO");
	    setHist1Att(h[mapLabel], "fluct", thisER, thisTU);
	    cGraph->Update();
	    outputGraphName = "h_" + thisER + "_" + thisTU + ".png";
	    outputGraphPath = outputGraphFolder + "/" + outputGraphName;
	    cGraph->Print(outputGraphPath.c_str());

	    h2h[mapLabel]->Draw("BOX");
	    setHist2Att(h2h[mapLabel], "hour", thisER, thisTU);
	    cGraph->Update();
	    outputGraphName = "h2h_" + thisER + "_" + thisTU + ".png";
	    outputGraphPath = outputGraphFolder + "/" + outputGraphName;
	    cGraph->Print(outputGraphPath.c_str());

	    h2d[mapLabel]->Draw("BOX");
	    setHist2Att(h2d[mapLabel], "day", thisER, thisTU);
	    cGraph->Update();
	    outputGraphName = "h2d_" + thisER + "_" + thisTU + ".png";
	    outputGraphPath = outputGraphFolder + "/" + outputGraphName;
	    cGraph->Print(outputGraphPath.c_str());

	    if(gTCorrect.find(mapLabel) != gTCorrect.cend()) {
		setGraphAtt(gTCorrect[mapLabel], "fluct", thisER);
		setErrorBandAtt(gTCeHMean, gTCeH1Sigma, gTCeH2Sigma, "formal");

		TMultiGraph* mgTC = new TMultiGraph();
		mgTC->Add(gTCeH2Sigma, "A3");
		mgTC->Add(gTCeH1Sigma, "A3");
		mgTC->Add(gTCeHMean, "LC");
		mgTC->Add(gTCorrect[mapLabel], "AP");
		setMultiGAtt(mgTC, "fluct", thisER, thisTU);
		setRangeUser(mgTC->GetYaxis(), "fluct", thisER, thisTU);
		mgTC->Draw("A");

		TLegend* lgTC = new TLegend(0.5, 0.15, 0.9, 0.45);
		lgTC->SetHeader(lPeakType.c_str(), "C");
		lgTC->AddEntry(gTCorrect[mapLabel], "data");
		sprintf(lline, "average: %.1f", hTC[mapLabel]->GetMean());
		lgTC->AddEntry(gTCeHMean, lline);
		sprintf(lline, "1 SE: %.2f", legendValueTC);
		lgTC->AddEntry(gTCeH1Sigma, lline);
		sprintf(lline, "2 SE: %.2f", 2*legendValueTC);
		lgTC->AddEntry(gTCeH2Sigma, lline);
		lgTC->SetTextSize(0.035);
		lgTC->Draw();

		cGraph->Update();
		outputGraphName = "gTC_" + thisER + "_" + thisTU + ".png";
		outputGraphPath = outputGraphFolder + "/" + outputGraphName;
		cGraph->Print(outputGraphPath.c_str());

		setGraphAtt(geTCN[mapLabel], "nfluct", thisER);
		setErrorBandAtt(geTCNeHMean, geTCNeH1Sigma, geTCNeH2Sigma, "formal");

		TMultiGraph* mgTCN = new TMultiGraph();
		mgTCN->Add(geTCNeH2Sigma, "A3");
		mgTCN->Add(geTCNeH1Sigma, "A3");
		mgTCN->Add(geTCNeHMean, "LC");
		mgTCN->Add(geTCN[mapLabel], "AP");
		setMultiGAtt(mgTCN, "nfluct", thisER, thisTU);
		setRangeUser(mgTCN->GetYaxis(), "nfluct", thisER, thisTU);
		mgTCN->Draw("A");

		TLegend* lgTCN = new TLegend(0.5, 0.15, 0.9, 0.45);
		lgTCN->SetHeader(lPeakType.c_str(), "C");
		lgTCN->AddEntry(geTCN[mapLabel], "data");
		lgTCN->AddEntry(geTCNeHMean, "average");
		sprintf(lline, "1 SE: %.4f", legendValueTC/hTC[mapLabel]->GetMean());
		lgTCN->AddEntry(geTCNeH1Sigma, lline);
		sprintf(lline, "2 SE: %.4f", 2*legendValueTC/hTC[mapLabel]->GetMean());
		lgTCN->AddEntry(geTCNeH2Sigma, lline);
		lgTCN->SetTextSize(0.035);
		lgTCN->Draw();

		cGraph->Update();
		outputGraphName = "geTCN_" + thisER + "_" + thisTU + ".png";
		outputGraphPath = outputGraphFolder + "/" + outputGraphName;
		cGraph->Print(outputGraphPath.c_str());

		TMultiGraph* mgCompare = new TMultiGraph();
		gTCorrect[mapLabel]->SetMarkerColor(kRed);
		mgCompare->Add(ge[mapLabel], "APX");
		mgCompare->Add(gTCorrect[mapLabel], "AP");
		setMultiGAtt(mgCompare, "fluct", thisER, thisTU);
		setRangeUser(mgCompare->GetYaxis(), "fluct", thisER, thisTU);
		mgCompare->Draw("A");

		TLegend* lgCompare = new TLegend(0.5, 0.15, 0.9, 0.45);
		lgCompare->SetHeader(lPeakType.c_str(), "C");
		sprintf(lline, "Before Correction by Temperature");
		lgCompare->AddEntry(ge[mapLabel], lline);
		sprintf(lline, "After Correction by Temperature");
		lgCompare->AddEntry(gTCorrect[mapLabel], lline);
		lgCompare->SetTextSize(0.035);
		lgCompare->Draw();

		cGraph->Update();
		outputGraphName = "Compare_" + thisER + "_" + thisTU + ".png";
		outputGraphPath = outputGraphFolder + "/" + outputGraphName;
		cGraph->Print(outputGraphPath.c_str());

		delete lgCompare;

		mgCompare->GetListOfGraphs()->Clear();
		delete mgCompare;

		delete lgTCN;

		mgTCN->GetListOfGraphs()->Clear();
		delete mgTCN;

		delete lgTC;

		mgTC->GetListOfGraphs()->Clear();
		delete mgTC;
	    }

	    delete lgn;
	    mgn->GetListOfGraphs()->Clear();
	    delete mgn;

	    delete stdt;
	    delete lg;
	    mg->GetListOfGraphs()->Clear();
	    delete mg;

	    delete geTCNeH2Sigma;
	    delete geTCNeH1Sigma;
	    delete geTCNeHMean;
	    delete gTCeH2Sigma;
	    delete gTCeH1Sigma;
	    delete gTCeHMean;

	    delete gneeH2Sigma;
	    delete gneeH1Sigma;
	    delete gneeHMean;

	    delete geeH2Sigma;
	    delete geeH1Sigma;
	    delete geeHMean;
	    delete geeT2Sigma;
	    delete geeT1Sigma;
	    delete geeTMean;
	}
    }





    for(map<string, TGraphErrors*>::iterator it = geTCN.begin();
	it != geTCN.end(); ++it) {
	delete it->second;
    }

    for(map<string, TGraph*>::iterator it = gTCorrect.begin();
	it != gTCorrect.end(); ++it) {
	delete it->second;
    }

    for(map<string, TH2I*>::iterator it = h2d.begin(); it != h2d.end(); ++it)

	delete it->second;

    for(map<string, TH2I*>::iterator it = h2h.begin(); it != h2h.end(); ++it)
	delete it->second;

    for(map<string, TH1I*>::iterator it = h.begin(); it != h.end(); ++it)
	delete it->second;

    for(map<string, TGraphErrors*>::iterator it = gne.begin(); it != gne.end(); ++it)
	delete it->second;

    for(map<string, TGraphErrors*>::iterator it = ge.begin(); it != ge.end(); ++it)
	delete it->second;

    foutput->cd();
    for(map<string, double>::iterator it = mapNormCounts16.begin(); it != mapNormCounts16.end(); ++it) {
	string label = it->first;
	Calendar* dt = new Calendar(label);
	static int ientry = 0;
	entryNo = ientry;
	outputyr = dt->getYear();
	outputmon = dt->getMonth();
	outputday = dt->getMDay();
	outputhr = dt->getHour();
	outputmin = dt->getMinute();
	outputsec = dt->getSecond();
	outcounts04 = mapCounts04[label];
	outcounts06 = mapCounts06[label];
	outcounts16 = mapCounts16[label];
	outNormCounts04 = mapNormCounts04[label];
	outNormCounts06 = mapNormCounts06[label];
	outNormCounts16 = mapNormCounts16[label];
	outputtemp = mapTemp[label];

	dataTree->Fill();

	ientry++;
	delete dt;
    }

    dataTree->Write();
    foutput->Close();
    delete foutput;

    delete th;
    delete cGraph;
}



void Handling::doProcedure4() {
    TCanvas* cGraph = new TCanvas("cGraph", "cGraph", 1400, 800);
    TempHumi* th = new TempHumi();

    vector<string> energyRange;
    vector<string> timeUnitList;

    energyRange.push_back("peak06");
    energyRange.push_back("peak16");
    energyRange.push_back("peak04");
    energyRange.push_back("peak24");
    energyRange.push_back("0to25");
    energyRange.push_back("peak04n06");

    timeUnitList.push_back("PD");
    timeUnitList.push_back("P12H");
    timeUnitList.push_back("P6H");
    timeUnitList.push_back("P2H");
    timeUnitList.push_back("P1H");

    map<string, TGraphErrors*> ge;
    map<string, TGraphErrors*> gen;

    for(unsigned int iER = 0; iER < energyRange.size(); iER++) {
	for(unsigned int iTU = 0; iTU < timeUnitList.size(); iTU++) {
	    string mapLabel = energyRange[iER] + "," + timeUnitList[iTU];
	    ge[mapLabel] = new TGraphErrors();
	    gen[mapLabel] = new TGraphErrors();
	}
    }

    for(unsigned int iTU = 0; iTU < timeUnitList.size(); iTU++) {
	TFile* file = nullptr;
	string thisTU = timeUnitList[iTU];
	if(thisTU == "PD")
	    file = new TFile("ready/0418to0607aHistPD.root", "READ");
	else if(thisTU == "P12H")
	    file = new TFile("ready/0418to0607aHistP12h.root", "READ");
	else if(thisTU == "P6H")
	    file = new TFile("ready/0418to0607aHistP06h.root", "READ");
	else if(thisTU == "P2H")
	    file = new TFile("ready/0418to0607aHistP02h.root", "READ");
	else if(thisTU == "P1H")
	    file = new TFile("ready/0418to0607aHistP01h.root", "READ");

	cout << "Now Processing" << endl;
	for(unsigned int iER = 0; iER < energyRange.size(); iER++) {
	    string thisER = energyRange[iER];
	    string mapLabel = thisER + "," + thisTU;

	    string dirERname = "withFilt_" + thisER;
	    TDirectory* dirER = file->GetDirectory(dirERname.c_str());
	    TList* listDate = dirER->GetListOfKeys();
	    listDate->Sort();

	    TIter iterDate(listDate);
	    TObject* objDate = nullptr;
	    while((objDate = iterDate())) {
		TDirectory* dirDate = dirER->GetDirectory(objDate->GetName());
		TList* listTime = dirDate->GetListOfKeys();

		TIter iterTime(listTime);
		TObject*objTime = nullptr;
		while((objTime = iterTime())) {
		    Calendar* thisDateTime = new Calendar((string)(objDate->GetName()) + (string)(objTime->GetName()));

		    if(thisTU == "P1H")
			thisDateTime->addDuration(0, 0, 0, 30, 0.);
		    else 
			thisDateTime->addDuration(0, 0, takeTimeLength(thisTU)/2., 0, 0);

		    double temp = th->getAvgTemp(thisDateTime->getDateTime());
		    double errTemp = th->getErrTemp(thisDateTime->getDateTime());

		    if(thisTU == "P1H")
			cout << thisDateTime->getDateTime() << ": "
			     << temp << endl;

		    TH1D* thisH = (TH1D*)dirDate->Get(objTime->GetName());
		    ge[mapLabel]->SetPoint(ge[mapLabel]->GetN(), temp, thisH->GetEntries());
		    ge[mapLabel]->SetPointError(ge[mapLabel]->GetN() - 1, errTemp, TMath::Sqrt(thisH->GetEntries()));

		    delete thisDateTime;
		}
	    }
	}

	if(file != nullptr)
	    delete file;
    }

    cGraph->cd();
    gPad->SetGrid(1, 1);
    for(unsigned int iER = 0; iER <energyRange.size(); iER++) {
	string thisER = energyRange[iER];
	for(unsigned int iTU = 0; iTU < timeUnitList.size(); iTU++) {
	    string thisTU = timeUnitList[iTU];
	    string mapLabel = thisER + "," + thisTU;
	    cout << "Now is run: " << mapLabel << endl
		 << "N: " << ge[mapLabel]->GetN() << endl;

	    ge[mapLabel]->Draw("AP");
	    setGraphAtt(ge[mapLabel], "ct", thisER);

	    TPaveText* pt = nullptr;
	    if(thisTU == "P1H" || thisTU == "P2H") {
		TFitResultPtr fitptr = nullptr;
		if(thisTU == "P2H")
		    fitptr = ge[mapLabel]->Fit("pol1", "ROB=0.95 WS");
		else if(thisTU == "P1H")
		    fitptr = ge[mapLabel]->Fit("pol1", "ROB=0.97 WS");

		cGraph->Update();
		double xleft = 0.6;
		double yupbd = 0.4;
		double ydnbd = 0.1;
		pt = new TPaveText(xleft*gPad->GetUxmax()
				   + (1 - xleft)*gPad->GetUxmin(),
				   ydnbd*gPad->GetUymax()
				   + (1 - ydnbd)*gPad->GetUymin(),
				   gPad->GetUxmax(),
				   yupbd*gPad->GetUymax()
				   + (1 - yupbd)*gPad->GetUymin());

		char tline[200];
		pt->AddText("y = p0 + p1*x");
		sprintf(tline, "#chi^{2}: %.4e", fitptr->Chi2());
		pt->AddText(tline);
		sprintf(tline, "Ndf: %d", fitptr->Ndf());
		pt->AddText(tline);
		sprintf(tline, "#chi^{2}/Ndf: %.4e", fitptr->Chi2()/(double)fitptr->Ndf());
		pt->AddText(tline);
		sprintf(tline, "p0 = %.4e +/- %.4e", fitptr->Parameter(0),
			fitptr->Error(0));
		pt->AddText(tline);
		sprintf(tline, "p1 = %.6f +/- %.2f", fitptr->Parameter(1),
			fitptr->Error(1));
		pt->AddText(tline);
		pt->Draw();
	    }

	    cGraph->Update();
	    string outputGraphFolder = "plotting/fittingHualien/countTemp";
	    string outputGraphName = "ct_" + thisER + "_" + thisTU + ".png";
	    string outputGraphPath = outputGraphFolder + "/" + outputGraphName;
	    cGraph->Print(outputGraphPath.c_str());

	    if(pt != nullptr)
		delete pt;
	}
    }

    for(map<string, TGraphErrors*>::iterator it = ge.begin(); it != ge.end(); ++it) {
	delete it->second;
    }

    delete th;
    delete cGraph;
}



void Handling::doProcedure5() {
    TCanvas* cGraph = new TCanvas("cGraph", "cGraph", 1400, 800);
    TempHumi* th = new TempHumi();

    vector<string> timeUnitList;

    timeUnitList.push_back("PD");
    timeUnitList.push_back("P12H");
    timeUnitList.push_back("P6H");
    timeUnitList.push_back("P2H");
    timeUnitList.push_back("P1H");

    map<string, TH1I*> h04;
    map<string, TH1I*> h06;
    map<string, TH1I*> h16; 
    map<string, TH1I*> h06f2; // for correlation with peak 16
    map<string, TH2I*> h2DoubleER; // for peak 04 & 06
    map<string, TH2I*> h2DERN;
    map<string, TH2I*> h2DER2; // for peak 06 & 16  
    map<string, TH2I*> h2DERN2; // for peak 06 & 16

    map<string, vector<double>> data04; ///// need to think a better way to store
    map<string, vector<double>> data06; ///// data and calculate their means
    map<string, vector<double>> data16;
    map<string, vector<double>> data06f2;

    map<string, double> p1f04;
    map<string, double> p1f06;
    map<string, double> p1f16;
    double tf = 30.;

    cout << "Now processing ..." << endl;
    for(unsigned int iTU = 0; iTU < timeUnitList.size(); iTU++) {
	string thisTU = timeUnitList[iTU];

	char hTitle[100];
	sprintf(hTitle, "h2DER_%s", thisTU.c_str());
	h2DoubleER[thisTU] = new TH2I(hTitle, hTitle, 100, 0., 100., 100, 0., 100.);
	sprintf(hTitle, "h2DERN_%s", thisTU.c_str());
	h2DERN[thisTU] = new TH2I(hTitle, hTitle, 100, 0., 100., 100, 0., 100.);
	sprintf(hTitle, "h2DER2_%s", thisTU.c_str());
	h2DER2[thisTU] = new TH2I(hTitle, hTitle, 100, 0., 100., 100, 0., 100.);
	sprintf(hTitle, "h2DERN2_%s", thisTU.c_str());
	h2DERN2[thisTU] = new TH2I(hTitle, hTitle, 100, 0., 100., 100, 0., 100.);

	sprintf(hTitle, "h04_%s", thisTU.c_str());
	h04[thisTU] = new TH1I(hTitle, hTitle, 100, 0., 100.);
	sprintf(hTitle, "h06_%s", thisTU.c_str());
	h06[thisTU] = new TH1I(hTitle, hTitle, 100, 0., 100.);
	sprintf(hTitle, "h16_%s", thisTU.c_str());
	h16[thisTU] = new TH1I(hTitle, hTitle, 100, 0., 100.);
	sprintf(hTitle, "h06f2_%s", thisTU.c_str());
	h06f2[thisTU] = new TH1I(hTitle, hTitle, 100, 0., 100.);

	setBins(h04[thisTU], "fluct", "peak04", thisTU);
	setBins(h06[thisTU], "fluct", "peak06", thisTU);
	setBins(h16[thisTU], "fluct", "peak16", thisTU);
	setBins(h06f2[thisTU], "fluct", "peak06", thisTU);
	setBins(h2DoubleER[thisTU], "doubleER", "", thisTU);
	setBins(h2DERN[thisTU], "DERN", "", thisTU);
	setBins(h2DER2[thisTU], "DER2", "", thisTU);
	setBins(h2DERN2[thisTU], "DERN", "", thisTU);

	if(thisTU == "P1H") {
	    p1f04[thisTU] = 198.238192;
	    p1f06[thisTU] = 117.584289;
	    p1f16[thisTU] = -104.340947;
	} else if(thisTU == "P2H") {
	    p1f04[thisTU] = 402.285465;
	    p1f06[thisTU] = 229.699101;
	    p1f16[thisTU] = -206.027847;
	}
    }





    for(unsigned int iTU = 0; iTU < timeUnitList.size(); iTU++) {
	TFile* file = nullptr;
	string thisTU = timeUnitList[iTU];
	if(thisTU == "PD")
	    file = new TFile("ready/0418to0607aHistPD.root", "READ");
	else if(thisTU == "P12H")
	    file = new TFile("ready/0418to0607aHistP12h.root", "READ");
	else if(thisTU == "P6H")
	    file = new TFile("ready/0418to0607aHistP06h.root", "READ");
	else if(thisTU == "P2H")
	    file = new TFile("ready/0418to0607aHistP02h.root", "READ");
	else if(thisTU == "P1H")
	    file = new TFile("ready/0418to0607aHistP01h.root", "READ");

	string dir04name = "withFilt_peak04";
	string dir06name = "withFilt_peak06";
	string dir16name = "withFilt_peak16";
	TDirectory* dir04 = file->GetDirectory(dir04name.c_str());
	TDirectory* dir06 = file->GetDirectory(dir06name.c_str());
	TDirectory* dir16 = file->GetDirectory(dir16name.c_str());
	TList* listDate = dir04->GetListOfKeys();
	listDate->Sort();

	TIter iterDate(listDate);
	TObject* objDate = nullptr;
	while((objDate = iterDate())) {
	    TDirectory* dir04Date = dir04->GetDirectory(objDate->GetName());
	    TDirectory* dir06Date = dir06->GetDirectory(objDate->GetName());
	    TDirectory* dir16Date = dir16->GetDirectory(objDate->GetName());
	    TList* listTime = dir04Date->GetListOfKeys();

	    TIter iterTime(listTime);
	    TObject* objTime = nullptr;
	    while((objTime = iterTime())) {
		TH1D* this04H = (TH1D*)dir04Date->Get(objTime->GetName());
		TH1D* this06H = (TH1D*)dir06Date->Get(objTime->GetName());
		TH1D* this16H = (TH1D*)dir16Date->Get(objTime->GetName());

		Calendar* dtTemp = new Calendar((string)(objDate->GetName()) + (string)(objTime->GetName()));

		if(thisTU == "P1H")
		    dtTemp->addDuration(0, 0, 0, 30, 0.);
		else
		    dtTemp->addDuration(0, 0, takeTimeLength(thisTU)/2., 0, 0.);

		double thisTemp = th->getAvgTemp(dtTemp->getDateTime());

		bool doPutOn = true;
		if(true) {
		    double upper04 = 0.;
		    double lower04 = 0.;
		    double upper06 = 0.;
		    double lower06 = 0.;

		    getHistUpLow("fluct", "peak04", thisTU, upper04, lower04);
		    getHistUpLow("fluct", "peak06", thisTU, upper06, lower06);

		    if((this04H->GetEntries() <= upper04 &&
			this04H->GetEntries() >= lower04) &&
		       (this06H->GetEntries() <= upper06 &&
			this06H->GetEntries() >= lower06))
			doPutOn = true;
		    else
			doPutOn = false;
		}

		if(doPutOn) {
		    double input04 = this04H->GetEntries();
		    double input06 = this06H->GetEntries();
		    if(thisTU == "P1H" || thisTU == "P2H") {
			input04 -= p1f04[thisTU]*(thisTemp - tf);
			input06 -= p1f06[thisTU]*(thisTemp - tf);
		    }

		    h04[thisTU]->Fill(input04);
		    h06[thisTU]->Fill(input06);
		    h2DoubleER[thisTU]->Fill(input06, input04);

		    data04[thisTU].push_back(input04);
		    data06[thisTU].push_back(input06);
		}

		if(true) {
		    double upper16 = 0.;
		    double lower16 = 0.;
		    double upper06 = 0.;
		    double lower06 = 0.;

		    getHistUpLow("fluct", "peak16", thisTU, upper16, lower16);
		    getHistUpLow("fluct", "peak06", thisTU, upper06, lower06);

		    if((this16H->GetEntries() <= upper16 &&
			this16H->GetEntries() >= lower16) &&
		       (this06H->GetEntries() <= upper06 &&
			this06H->GetEntries() >= lower06))
			doPutOn = true;
		    else
			doPutOn = false;
		}

		if(doPutOn) {
		    double input16 = this16H->GetEntries();
		    double input06 = this06H->GetEntries();
		    if(thisTU == "P1H" || thisTU == "P2H") {
			input16 -= p1f16[thisTU]*(thisTemp - tf);
			input06 -= p1f06[thisTU]*(thisTemp - tf);
		    }

		    h16[thisTU]->Fill(input16);
		    h06f2[thisTU]->Fill(input06);
		    h2DER2[thisTU]->Fill(input16, input06);

		    data16[thisTU].push_back(input16);
		    data06f2[thisTU].push_back(input06);
		}
	    }
	}

	for(unsigned int i = 0; i < data04[thisTU].size(); i++) {
	    /*
	    cout << data04[thisTU][i]/h04[thisTU]->GetMean() << "|"
		 << data06[thisTU][i]/h06[thisTU]->GetMean() << endl;
	    */
	    h2DERN[thisTU]->Fill(data06[thisTU][i]/h06[thisTU]->GetMean(),
				 data04[thisTU][i]/h04[thisTU]->GetMean());
	}

	for(unsigned int i = 0; i < data16[thisTU].size(); i++) {
	    if(thisTU == "P2H") {
		cout << h16[thisTU]->GetMean() << endl;
		cout << data16[thisTU][i]/h16[thisTU]->GetMean() << "|"
		     << data06f2[thisTU][i]/h06f2[thisTU]->GetMean() << endl;
	    }

	    h2DERN2[thisTU]->Fill(data16[thisTU][i]/h16[thisTU]->GetMean(),
				  data06f2[thisTU][i]/h06f2[thisTU]->GetMean());
	}

	file->Close();
	delete file;
    }

    cGraph->cd();
    for(unsigned int iTU = 0; iTU < timeUnitList.size(); iTU++) {
	string thisTU = timeUnitList[iTU];

	h2DoubleER[thisTU]->Draw("BOX");

	h2DoubleER[thisTU]->SetStats(kFALSE);
	h2DoubleER[thisTU]->SetTitle("");
	h2DoubleER[thisTU]->SetXTitle("Counts of Bi-214");
	h2DoubleER[thisTU]->SetYTitle("Counts of Pb-214");

	cGraph->Update();
	string outputGraphFolder = "plotting/fittingHualien/counting";
	string outputGraphName = "h2DER_" + thisTU + ".png";
	string outputGraphPath = outputGraphFolder + "/" + outputGraphName;
	cGraph->Print(outputGraphPath.c_str());



	h2DERN[thisTU]->Draw("BOX");

	h2DERN[thisTU]->SetStats(kFALSE);
	h2DERN[thisTU]->SetTitle("");
	h2DERN[thisTU]->SetXTitle("Normalized Counts of Bi-214");
	h2DERN[thisTU]->SetYTitle("Normalized Counts of Pb-214");

	cGraph->Update();
	outputGraphName = "h2DERN_" + thisTU + ".png";
	outputGraphPath = outputGraphFolder + "/" + outputGraphName;
	cGraph->Print(outputGraphPath.c_str());



	h2DER2[thisTU]->Draw("BOX");

	h2DER2[thisTU]->SetStats(kFALSE);
	h2DER2[thisTU]->SetTitle("");
	h2DER2[thisTU]->SetXTitle("Counts of K-40");
	h2DER2[thisTU]->SetYTitle("Counts of Bi-214");

	cGraph->Update();
	outputGraphName = "h2DER2_" + thisTU + ".png";
	outputGraphPath = outputGraphFolder + "/" + outputGraphName;
	cGraph->Print(outputGraphPath.c_str());



	h2DERN2[thisTU]->Draw("BOX");

	h2DERN2[thisTU]->SetStats(kFALSE);
	h2DERN2[thisTU]->SetTitle("");
	h2DERN2[thisTU]->SetXTitle("Normalized Counts of K-40");
	h2DERN2[thisTU]->SetYTitle("Normalized Counts of Bi-214");

	cGraph->Update();
	outputGraphName = "h2DERN2_" + thisTU + ".png";
	outputGraphPath = outputGraphFolder + "/" + outputGraphName;
	cGraph->Print(outputGraphPath.c_str());
    }

    for(map<string, TH1I*>::iterator it = h06f2.begin(); it != h06f2.end(); ++it)
	delete it->second;

    for(map<string, TH1I*>::iterator it = h16.begin(); it != h16.end(); ++it)
	delete it->second;

    for(map<string, TH1I*>::iterator it = h06.begin(); it != h06.end(); ++it)
	delete it->second;

    for(map<string, TH1I*>::iterator it = h04.begin(); it != h04.end(); ++it)
	delete it->second;

    for(map<string, TH2I*>::iterator it = h2DERN2.begin(); it != h2DERN2.end(); ++it)
	delete it->second;

    for(map<string, TH2I*>::iterator it = h2DER2.begin(); it != h2DER2.end(); ++it)
	delete it->second;

    for(map<string, TH2I*>::iterator it = h2DERN.begin(); it != h2DERN.end(); ++it)
	delete it->second;

    for(map<string, TH2I*>::iterator it = h2DoubleER.begin(); it != h2DoubleER.end(); ++it)
	delete it->second;

    delete th;
    delete cGraph;
}



void Handling::doProcedure6() {
    TCanvas* cGraph = new TCanvas("cGraph", "cGraph", 1400, 800);
    TempHumi* th = new TempHumi();

    TFile* foutput = new TFile("ready/fittingResult.root", "RECREATE");
    int entryNo;
    int outputyr;
    int outputmon;
    int outputday;
    int outputhr;
    int outputmin;
    double outputsec;
    double outputcgn04;
    double outputcgn06;
    double outputcgn16;
    double outputest04;
    double outputest06;
    double outputest16;
    double outputtemp;

    TTree* dataTree = new TTree("dataFitting", "dataTree");
    dataTree->Branch("entryNo", &entryNo, "entryNo/I");
    dataTree->Branch("year", &outputyr, "year/I");
    dataTree->Branch("month", &outputmon, "month/I");
    dataTree->Branch("day", &outputday, "day/I");
    dataTree->Branch("hour", &outputhr, "hour/I");
    dataTree->Branch("minute", &outputmin, "minute/I");
    dataTree->Branch("second", &outputsec, "second/D");
    dataTree->Branch("cgn04", &outputcgn04, "cgn04/D");
    dataTree->Branch("cgn06", &outputcgn06, "cgn06/D");
    dataTree->Branch("cgn16", &outputcgn16, "cgn16/D");
    dataTree->Branch("est04", &outputest04, "est04/D");
    dataTree->Branch("est06", &outputest06, "est06/D");
    dataTree->Branch("est16", &outputest16, "est16/D");
    dataTree->Branch("temperature", &outputtemp, "temperature/D");

    map<string, double> mapcgn04;
    map<string, double> mapcgn06;
    map<string, double> mapcgn16;
    map<string, double> mapest04;
    map<string, double> mapest06;
    map<string, double> mapest16;
    map<string, double> maptemp;

    vector<string> termList;
    vector<string> peakList;
    vector<string> timeUnitList;

    termList.push_back("cPow");
    termList.push_back("cExp");
    termList.push_back("expo");
    termList.push_back("cGauss");
    termList.push_back("mean");
    termList.push_back("std");
    termList.push_back("cExp_Norm");
    termList.push_back("cGauss_Norm");
    termList.push_back("EstEvents");

    peakList.push_back("peak16");
    peakList.push_back("peak06");
    peakList.push_back("peak04");
    peakList.push_back("peak24");

    timeUnitList.push_back("PT");
    timeUnitList.push_back("PD");
    timeUnitList.push_back("P12H");
    timeUnitList.push_back("P6H");
    timeUnitList.push_back("P2H");
    timeUnitList.push_back("P1H");

    map<string, TF1*> f;
    map<string, TGraph*> g;
    map<string, TGraphErrors*> ge;
    map<string, TGraphErrors*> geTcG;

    map<string, double> cGausT;
    map<string, double> errCGausT;
    map<string, double> meanT;
    map<string, double> errMeanT;

    for(unsigned int iTerm = 0; iTerm < termList.size(); iTerm++) {
	for(unsigned int iPeak = 0; iPeak < peakList.size(); iPeak++) {
	    for(unsigned int iTU = 0; iTU < timeUnitList.size(); iTU++) {
		string thisTerm = termList[iTerm];
		string thisPeak = peakList[iPeak];
		string thisTU = timeUnitList[iTU];
		string mapLabel = thisTerm + "," + thisPeak + "," + thisTU;
		string setName = "f" + thisTerm + thisPeak + thisTU;

		if(thisTU == "PT")
		    f[mapLabel] = new TF1(setName.c_str(), "[0]", 0., 24.);
		else if(thisTU == "PD" ||
			thisTU == "P12H" ||
			thisTU == "P6H" ||
			thisTU == "P2H" ||
			thisTU == "P1H") {
		    g[mapLabel] = new TGraph();

		    if(thisTerm.find("cExp") == string::npos)
			ge[mapLabel] = new TGraphErrors();

		    if(thisTerm == "cGauss") {
			geTcG[mapLabel] = new TGraphErrors();
		    }
		}
	    }
	}
    }

    string normTU = "PT";
    TFile* fileN = new TFile("ready/0418to0607oneHist.root", "READ");
    TH1D* hN = (TH1D*)fileN->Get("HistoCh0/20210418/000000");
    for(unsigned int iPeak = 0; iPeak < peakList.size(); iPeak++) {
	string thisPeak = peakList[iPeak];
	PeakFitter* pf = new PeakFitter(hN, thisPeak);
	string histoName = "20210418_all_50_days_" + thisPeak;
	hN->SetTitle(histoName.c_str());
	pf->setHistoName(histoName);
	pf->setNeedZoom(true);
	pf->fitPeak();

	cGausT[thisPeak] = pf->getCGauss(0);
	errCGausT[thisPeak] = pf->getErrorCGaus(0);
	meanT[thisPeak] = pf->getMean(0);
	errMeanT[thisPeak] = pf->getErrorM(0);

	delete pf;
    }
    delete fileN;

    for(unsigned int iTU = 0; iTU < timeUnitList.size(); iTU++) {
	TFile* file = nullptr;
	string thisTU = timeUnitList[iTU];
	if(thisTU == "PT")
	    continue;
	else if(thisTU == "PD")
	    file = new TFile("ready/0418to0607aHistPD.root", "READ");
	else if(thisTU == "P12H")
	    file = new TFile("ready/0418to0607aHistP12h.root", "READ");
	else if(thisTU == "P6H")
	    file = new TFile("ready/0418to0607aHistP06h.root", "READ");
	else if(thisTU == "P2H")
	    file = new TFile("ready/0418to0607aHistP02h.root", "READ");
	else if(thisTU == "P1H")
	    file = new TFile("ready/0418to0607aHistP01h.root", "READ");

	TDirectory* dirCh0 = file->GetDirectory("HistoCh0");
	TList* listDate = dirCh0->GetListOfKeys();
	listDate->Sort();

	TIter iterDate(listDate);
	TObject* objDate = nullptr;
	while((objDate = iterDate())) {
	    TDirectory* dirDate = dirCh0->GetDirectory(objDate->GetName());
	    TList* listTime = dirDate->GetListOfKeys();

	    TIter iterTime(listTime);
	    TObject* objTime = nullptr;
	    while((objTime = iterTime())) {
		Calendar* thisDateTime = new Calendar((string)(objDate->GetName()) + (string)(objTime->GetName()));
		Calendar* dtTemp = new Calendar(thisDateTime->getDateTime());

		double totalHours = takeTimeLength("PT");
		double timeLength = takeTimeLength(thisTU); // Unit is hour.
		string strForTitle;
		if(thisTU == "PD")
		    strForTitle = "per_01_day";
		else if(thisTU == "P12H")
		    strForTitle = "per_12_hours";
		else if(thisTU == "P6H")
		    strForTitle = "per_6_hours";
		else if(thisTU == "P2H")
		    strForTitle = "per_2_hours";
		else if(thisTU == "P1H")
		    strForTitle = "per_1_hour";

		double totalTimeBins = totalHours/timeLength;
		if(timeLength == 1.) {
		    dtTemp->addDuration(0, 0, 0, 30, 0.);
		} else {
		    dtTemp->addDuration(0, 0, (int)timeLength/2., 0, 0.);
		}

		double temp = th->getAvgTemp(dtTemp->getDateTime());
		double errTemp = th->getErrTemp(dtTemp->getDateTime());

		TDatime* tdt = new TDatime(thisDateTime->getYear(),
					   thisDateTime->getMonth(),
					   thisDateTime->getMDay(),
					   thisDateTime->getHour(),
					   thisDateTime->getMinute(),
					   thisDateTime->getSecond());

		TH1D* thisH = (TH1D*)dirDate->Get(objTime->GetName());
		char histTitle[80];
		sprintf(histTitle, "%s%s_%s", objDate->GetName(), objTime->GetName(), strForTitle.c_str());
		thisH->SetTitle(histTitle);
		cout << histTitle << endl;

		for(unsigned int iPeak = 0; iPeak < peakList.size(); iPeak++) {
		    string thisPeak = peakList[iPeak];

		    PeakFitter* pf = new PeakFitter(thisH, thisPeak);
		    string histoName = (string)histTitle + "_" + thisPeak;
		    pf->setHistoName(histoName);
		    pf->setNeedZoom(true);

		    pf->fitPeak();

		    for(unsigned int iTerm = 0; iTerm < termList.size(); iTerm++) {
			string thisTerm = termList[iTerm];
			string label = thisTerm + "," + thisPeak + "," + thisTU;

			double keyInValue = 0.;
			double keyInError = 0.;
			if(thisTerm.find("Norm") == string::npos) {
			    if(thisTerm == "EstEvent") {
				keyInValue = pf->getAssignedValue("cGauss", 0);
				keyInError = pf->getAssignedError("cGauss", 0);

				double binWidth = pf->getHistogram()->GetBinWidth(0);
				keyInValue /= binWidth;
				keyInError /= binWidth;
			    } else {
				keyInValue = pf->getAssignedValue(thisTerm, 0);
				keyInError = pf->getAssignedError(thisTerm, 0);
			    }
			} else {
			    string actualTerm = thisTerm.substr(0, thisTerm.find_first_of("_"));
			    keyInValue = pf->getAssignedValue(actualTerm, 0)/(cGausT[thisPeak]/totalTimeBins);

			    double ubar = pf->getAssignedValue(actualTerm, 0);
			    double usig = pf->getAssignedError(actualTerm, 0);
			    double vbar = cGausT[thisPeak]/totalTimeBins;
			    double vsig = errCGausT[thisPeak]/totalTimeBins;
			    keyInError = TMath::Sqrt((usig*usig)/(ubar*ubar)
						     + (vsig*vsig)/(vbar*vbar));
			}

			if(keyInError > 1e3)
			    keyInError = 0.;

			if(thisPeak == "peak04" && thisTU == "P1H")
			    if(keyInError > 10.)
				keyInError = 0.;

			if(g.find(label) != g.cend()) {
			    TGraph* thisG = g[label];
			    thisG->SetPoint(thisG->GetN(), tdt->Convert() , keyInValue);
			}

			if(ge.find(label) != ge.cend()) {
			    TGraphErrors* thisGE = ge[label];
			    thisGE->SetPoint(thisGE->GetN(), tdt->Convert(), keyInValue);
			    thisGE->SetPointError(thisGE->GetN() - 1, 0., keyInError);
			}

			if(geTcG.find(label) != geTcG.cend()) {
			    TGraphErrors* thisGE = geTcG[label];
			    thisGE->SetPoint(thisGE->GetN(), temp, keyInValue);
			    thisGE->SetPointError(thisGE->GetN() - 1, errTemp, keyInError);
			}

			if(thisTerm == "cGauss_Norm" && thisTU == "P2H") {
			    if(thisPeak == "peak04") {
				mapcgn04[thisDateTime->getDateTime()] = keyInValue;
				mapest04[thisDateTime->getDateTime()] = pf->getEstEvent();
			    } else if(thisPeak == "peak06") {
				mapcgn06[thisDateTime->getDateTime()] = keyInValue;
				mapest06[thisDateTime->getDateTime()] = pf->getEstEvent();
			    } else if(thisPeak == "peak16") {
				mapcgn16[thisDateTime->getDateTime()] = keyInValue;
				mapest16[thisDateTime->getDateTime()] = pf->getEstEvent();
			    }
			}

			if(maptemp.find(thisDateTime->getDateTime()) == maptemp.cend())
			    maptemp[thisDateTime->getDateTime()] = temp;
		    }

		    delete pf;
		}

		delete tdt;
		delete thisDateTime;

		cout << endl << endl << endl;
	    }
	}

	file->Close();
	delete file;
    }





    cGraph->cd();
    gPad->SetGrid(1, 1);
    for(unsigned int iTerm = 0; iTerm < termList.size(); iTerm++) {
	string thisTerm = termList[iTerm];

	for(unsigned int iPeak = 0; iPeak < peakList.size(); iPeak++) {
	    string thisPeak = peakList[iPeak];

	    for(unsigned int iTU = 0; iTU < timeUnitList.size(); iTU++) {
		string thisTU = timeUnitList[iTU];

		string gLabel = thisTerm + "," + thisPeak + "," + thisTU;
		cout << "Now process: " << gLabel << endl;

		TGraphErrors* geeAvg = new TGraphErrors();
		TGraphErrors* gee1Sigma = new TGraphErrors();
		TGraphErrors* gee2Sigma = new TGraphErrors();

		double legendValue = 0.;
		double legendError = 0.;

		for(int i = 0; i < 2; i++) {
		    TDatime* tdt = nullptr;
		    if(i == 0)
			tdt = new TDatime(2021, 4, 18, 0, 0, 0);
		    else if(i == 1)
			tdt = new TDatime(2021, 6, 7, 0, 0, 0);

		    double keyInValue = -99.;
		    double keyInError = 0.;

		    if(thisTerm == "cGauss") {
			keyInValue = cGausT[thisPeak];
			keyInError = errCGausT[thisPeak];

			double timeLength = 0.; // time unit is hour
			if(thisTU == "PD")
			    timeLength = 24.;
			else if(thisTU == "P12H")
			    timeLength = 12.;
			else if(thisTU == "P6H")
			    timeLength = 6.;
			else if(thisTU == "P2H")
			    timeLength = 2.;
			else if(thisTU == "P1H")
			    timeLength = 1.;

			double timeBins = 1200./timeLength;
			keyInValue /= timeBins;
			keyInError /= timeBins;
		    } else if(thisTerm == "mean") {
			keyInValue = meanT[thisPeak];
			keyInError = errMeanT[thisPeak];
		    } else if(thisTerm == "cGauss_Norm") {
			keyInValue = 1.;
			keyInError = errCGausT[thisPeak]/cGausT[thisPeak];
		    }

		    geeAvg->SetPoint(i, tdt->Convert(), keyInValue);
		    gee1Sigma->SetPoint(i, tdt->Convert(), keyInValue);
		    gee2Sigma->SetPoint(i, tdt->Convert(), keyInValue);
		    
		    double errorBand = keyInError;
		    gee1Sigma->SetPointError(i, 0, errorBand);
		    gee2Sigma->SetPointError(i, 0, 2*errorBand);

		    if(i == 0) {
			legendValue = keyInValue;
			legendError = errorBand;
		    }

		    if(tdt != nullptr)
			delete tdt;
		}

		setErrorBandAtt(geeAvg, gee1Sigma, gee2Sigma, "formal");

		if(g.find(gLabel) != g.cend()) {
		    TGraph* thisG = g[gLabel];
		    setGraphAtt(thisG, thisTerm, thisPeak);
		    TMultiGraph* mg = new TMultiGraph();
		    mg->Add(gee2Sigma, "A3");
		    mg->Add(gee1Sigma, "A3");
		    mg->Add(geeAvg, "A3");
		    mg->Add(thisG, "AP");
		    setMultiGAtt(mg, thisTerm, thisPeak, thisTU);
		    setRangeUser(mg->GetYaxis(), thisTerm, thisPeak, thisTU);
		    mg->Draw("A");

		    TLegend* lg = nullptr;
		    if(legendValue != -99.) {
			lg = new TLegend(0.6, 0.15, 0.9, 0.45);
			string lPeakType = takePeakTypeStr(thisPeak);

			char lline[200];
			lg->SetHeader(lPeakType.c_str(), "C");
			lg->AddEntry(thisG, "data");
			sprintf(lline, "average: %.4f", legendValue);
			lg->AddEntry(geeAvg, lline);
			sprintf(lline, "1 SE: %.4f", legendError);
			lg->AddEntry(gee1Sigma, lline);
			sprintf(lline, "2 SE: %.4f", 2*legendError);
			lg->AddEntry(gee2Sigma, lline);
			lg->SetTextSize(0.035);
			lg->Draw();
		    }

		    string folderPath = "plotting/fittingHualien/g";
		    string outputFilename = folderPath + "/g_"
			+ thisTerm + "_"
			+ thisPeak + "_"
			+ thisTU + ".png";

		    cGraph->Update();
		    cGraph->Print(outputFilename.c_str());

		    if(lg != nullptr)
			delete lg;

		    mg->GetListOfGraphs()->Clear();
		    delete mg;
		}

		if(ge.find(gLabel) != ge.cend()) {
		    TGraphErrors* thisGE = ge[gLabel];
		    setGraphAtt(thisGE, thisTerm, thisPeak);
		    TMultiGraph* mg = new TMultiGraph();
		    mg->Add(gee2Sigma, "A3");
		    mg->Add(gee1Sigma, "A3");
		    mg->Add(geeAvg, "A3");
		    mg->Add(thisGE, "AP");
		    setMultiGAtt(mg, thisTerm, thisPeak, thisTU);
		    setRangeUser(mg->GetYaxis(), thisTerm, thisPeak, thisTU);
		    mg->Draw("A");

		    TLegend* lg = nullptr;
		    if(legendValue != -99.) {
			lg = new TLegend(0.6, 0.15, 0.9, 0.45);
			string lPeakType = takePeakTypeStr(thisPeak);

			char lline[200];
			lg->SetHeader(lPeakType.c_str(), "C");
			lg->AddEntry(thisGE, "data");
			sprintf(lline, "average: %.4f", legendValue);
			lg->AddEntry(geeAvg, lline);
			sprintf(lline, "1 SE: %.4f", legendError);
			lg->AddEntry(gee1Sigma, lline);
			sprintf(lline, "2 SE: %.4f", 2*legendError);
			lg->AddEntry(gee2Sigma, lline);
			lg->SetTextSize(0.035);
			lg->Draw();
		    }

		    string folderPath = "plotting/fittingHualien/ge";
		    string outputFilename = folderPath + "/ge_"
			+ thisTerm + "_"
			+ thisPeak + "_"
			+ thisTU + ".png";

		    cGraph->Update();
		    cGraph->Print(outputFilename.c_str());

		    if(lg != nullptr)
			delete lg;

		    mg->GetListOfGraphs()->Clear();
		    delete mg;
		}

		if(geTcG.find(gLabel) != geTcG.cend()) {
		    string folderPath = "plotting/fittingHualien/geTcG";
		    string outputFilename = folderPath + "/geTcG_"
			+ thisTerm + "_"
			+ thisPeak + "_"
			+ thisTU + ".png";

		    TGraphErrors* thisGE = geTcG[gLabel];
		    thisGE->Draw("AP");
		    setGraphAtt(thisGE, "cgt", thisPeak);

		    TFitResultPtr fitptr = thisGE->Fit("pol1", "ROB=0.95 WS");
		    if(thisPeak == "peak04" && thisTU == "P1H")
			thisGE->GetYaxis()->SetRangeUser(0., 30.);

		    cGraph->Update();
		    double xleft = 0.6;
		    double yupbd = 0.4;
		    double ydnbd = 0.1;
		    TPaveText* pt = new TPaveText(xleft*gPad->GetUxmax()
						  + (1 - xleft)*gPad->GetUxmin(),
						  ydnbd*gPad->GetUymax()
						  + (1 - ydnbd)*gPad->GetUymin(),
						  gPad->GetUxmax(),
						  yupbd*gPad->GetUymax()
						  + (1 - yupbd)*gPad->GetUymin());
		    char tline[200];
		    pt->AddText("y = p0 + p1*x");
		    sprintf(tline, "#chi^{2}: %.4e", fitptr->Chi2());
		    pt->AddText(tline);
		    sprintf(tline, "Ndf: %d", fitptr->Ndf());
		    pt->AddText(tline);
		    sprintf(tline, "#chi^{2}/Ndf: %.4e", fitptr->Chi2()/(double)fitptr->Ndf());
		    pt->AddText(tline);
		    sprintf(tline, "p0 = %.4e +/- %.4e", fitptr->Parameter(0),
			    fitptr->Error(0));
		    pt->AddText(tline);
		    sprintf(tline, "p1 = %.6f +/- %.2f", fitptr->Parameter(1),
			    fitptr->Error(1));
		    pt->AddText(tline);
		    pt->Draw();

		    cGraph->Update();
		    cGraph->Print(outputFilename.c_str());

		    delete pt;
		}
	    }
	}
    }

    for(map<string, TGraphErrors*>::iterator it = geTcG.begin(); it != geTcG.end(); ++it)
	delete it->second;

    for(map<string, TGraphErrors*>::iterator it = ge.begin(); it != ge.end(); ++it)
	delete it->second;

    for(map<string, TGraph*>::iterator it = g.begin(); it != g.end(); ++it)
	delete it->second;

    for(map<string, TF1*>::iterator it = f.begin(); it != f.end(); ++it)
	delete it->second;

    // fill output datafile
    foutput->cd();
    for(map<string, double>::iterator it = mapcgn16.begin(); it != mapcgn16.end(); ++it) {
	string label = it->first;
	Calendar* dt = new Calendar(label);
	static int ientry = 0;
	entryNo = ientry;
	outputyr = dt->getYear();
	outputmon = dt->getMonth();
	outputday = dt->getMDay();
	outputhr = dt->getHour();
	outputmin = dt->getMinute();
	outputsec = dt->getSecond();
	outputcgn04 = mapcgn04[label];
	outputcgn06 = mapcgn06[label];
	outputcgn16 = mapcgn16[label];
	outputest04 = mapest04[label];
	outputest06 = mapest06[label];
	outputest16 = mapest16[label];
	outputtemp = maptemp[label];

	dataTree->Fill();

	ientry++;
	delete dt;
    }

    dataTree->Write();
    foutput->Close();
    delete foutput;

    delete th;
    delete cGraph;
}



void Handling::doProcedure7() {
    TCanvas* cGraph = new TCanvas("c", "c", 1400, 800);
    TGraph* g16 = new TGraph();
    TGraph* g06 = new TGraph();
    TH2D* h21606 = new TH2D("h21606", "h21606", 100, 0., 100., 100, 0., 100.);
    TH2D* h20604 = new TH2D("h20604", "h20604", 100, 0., 100., 100, 0., 100.);
    TH2D* h206cf04f = new TH2D("h206cf04f", "h206cf04f", 100, 0., 100., 100, 0., 100.);

    setBins(h21606, "testRange", "", "P2H");
    setBins(h20604, "testRange", "", "P2H");
    setBins(h206cf04f, "testRange2", "", "P2H");

    TFile* fFit = new TFile("ready/fittingResult.root", "READ");
    TFile* fFluct = new TFile("ready/fluctResult.root", "READ");

    int entryNoFit;
    int yearFit;
    int monthFit;
    int dayFit;
    int hourFit;
    int minuteFit;
    double secondFit;
    double cgn04Fit;
    double cgn06Fit;
    double cgn16Fit;
    double est04Fit;
    double est06Fit;
    double est16Fit;
    double tempFit;

    TTree* dataFit;
    fFit->GetObject("dataFitting", dataFit);
    dataFit->SetBranchAddress("entryNo", &entryNoFit);
    dataFit->SetBranchAddress("year", &yearFit);
    dataFit->SetBranchAddress("month", &monthFit);
    dataFit->SetBranchAddress("day", &dayFit);
    dataFit->SetBranchAddress("hour", &hourFit);
    dataFit->SetBranchAddress("minute", &minuteFit);
    dataFit->SetBranchAddress("second", &secondFit);
    dataFit->SetBranchAddress("cgn04", &cgn04Fit);
    dataFit->SetBranchAddress("cgn06", &cgn06Fit);
    dataFit->SetBranchAddress("cgn16", &cgn16Fit);
    dataFit->SetBranchAddress("est04", &est04Fit);
    dataFit->SetBranchAddress("est06", &est06Fit);
    dataFit->SetBranchAddress("est16", &est16Fit);
    dataFit->SetBranchAddress("temperature", &tempFit);

    int entryNoFluct;
    int yearFluct;
    int monthFluct;
    int dayFluct;
    int hourFluct;
    int minuteFluct;
    double secondFluct;
    double counts04Fluct;
    double counts06Fluct;
    double counts16Fluct;
    double normCounts04Fluct;
    double normCounts06Fluct;
    double normCounts16Fluct;
    double tempFluct;

    TTree* dataFluct;
    fFluct->GetObject("dataCounts", dataFluct);
    dataFluct->SetBranchAddress("entryNo", &entryNoFluct);
    dataFluct->SetBranchAddress("year", &yearFluct);
    dataFluct->SetBranchAddress("month", &monthFluct);
    dataFluct->SetBranchAddress("day", &dayFluct);
    dataFluct->SetBranchAddress("hour", &hourFluct);
    dataFluct->SetBranchAddress("minute", &minuteFluct);
    dataFluct->SetBranchAddress("second", &secondFluct);
    dataFluct->SetBranchAddress("counts04", &counts04Fluct);
    dataFluct->SetBranchAddress("counts06", &counts06Fluct);
    dataFluct->SetBranchAddress("counts16", &counts16Fluct);
    dataFluct->SetBranchAddress("normCounts04", &normCounts04Fluct);
    dataFluct->SetBranchAddress("normCounts06", &normCounts06Fluct);
    dataFluct->SetBranchAddress("normCounts16", &normCounts16Fluct);
    dataFluct->SetBranchAddress("temperature", &tempFluct);

    Long64_t nentriesFit = dataFit->GetEntries();
    Long64_t nentriesFluct = dataFluct->GetEntries();
    for(Long64_t evtFit = 0; evtFit < nentriesFit; evtFit++) {
	dataFit->GetEntry(evtFit);
	h21606->Fill(cgn16Fit, cgn06Fit);
	h20604->Fill(cgn06Fit, cgn04Fit);

	
	for(Long64_t evtFluct = 0; evtFluct < nentriesFluct; evtFluct++) {
	    dataFluct->GetEntry(evtFluct);

	    if(yearFluct == yearFit &&
	       monthFluct == monthFit &&
	       dayFluct == dayFit &&
	       hourFluct == hourFit &&
	       minuteFluct == minuteFit &&
	       secondFluct == secondFit) {
		double bkgfluct16 = counts16Fluct - est16Fit;
		double bkgfluct06 = counts06Fluct - est06Fit;
		TDatime* dt = new TDatime(yearFit, monthFit, dayFit,
					hourFit, minuteFit, (int)secondFit);

		g16->SetPoint(g16->GetN(), dt->Convert(), bkgfluct16);
		g06->SetPoint(g06->GetN(), dt->Convert(), bkgfluct06);

		h206cf04f->Fill(bkgfluct16, est06Fit);

		delete dt;
		continue;
	    }
	}
    }

    cGraph->cd();
    h21606->Draw("BOX");
    h21606->SetStats(kFALSE);
    h21606->SetTitle("");
    h21606->SetXTitle("C_{2hr}/#bar{C}_{2hr} of K-40");
    h21606->SetYTitle("C_{2hr}/#bar{C}_{2hr} of Bi-214");
    cGraph->Print("plotting/fittingHualien/DER/fitDER1606.png");

    h20604->Draw("BOX");
    h20604->SetStats(kFALSE);
    h20604->SetTitle("");
    h20604->SetXTitle("C_{2hr}/#bar{C}_{2hr} of Bi-214");
    h20604->SetYTitle("C_{2hr}/#bar{C}_{2hr} of Pb-214");
    cGraph->Print("plotting/fittingHualien/DER/fitDER0604.png");

    h206cf04f->Draw("BOX");
    h206cf04f->SetStats(kFALSE);
    h206cf04f->SetXTitle("Total counts cut with the estimated counts by fitting, K-40");
    h206cf04f->SetYTitle("estimated counts by fitting, Bi-214");
    cGraph->Print("plotting/fittingHualien/DER/fitDER16cf06f.png");

    g16->SetMarkerSize(1);
    g16->SetMarkerStyle(kCircle);
    g16->SetMarkerColor(kBlack);
    g16->SetTitle("TotalCounting - Fitting-Integral of K-40");
    g16->GetXaxis()->SetTimeDisplay(kTRUE);
    g16->GetXaxis()->SetTimeFormat("%m\/%d");
    g16->GetXaxis()->SetTitle("Date");
    g16->GetYaxis()->SetTitle("Counts");
    g16->Draw("AP");
    cGraph->Print("plotting/fittingHualien/DER/bkgfluct16.png");

    g06->SetMarkerSize(1);
    g06->SetMarkerStyle(kCircle);
    g06->SetMarkerColor(kBlack);
    g06->SetTitle("TotalCounting - Fitting-Integral of Bi-214");
    g06->GetXaxis()->SetTimeDisplay(kTRUE);
    g06->GetXaxis()->SetTimeFormat("%m\/%d");
    g06->GetXaxis()->SetTitle("Date");
    g06->GetYaxis()->SetTitle("Counts");
    g06->GetYaxis()->SetRangeUser(75000., 84000.);
    g06->Draw("AP");
    cGraph->Print("plotting/fittingHualien/DER/bkgfluct06.png");

    fFluct->Close();
    delete fFluct;

    fFit->Close();
    delete fFit;

    delete h206cf04f;
    delete h20604;
    delete h21606;
    delete g06;
    delete g16;

    delete cGraph;
}



void Handling::checkHist() {
    TFile* f = new TFile("ready/0418to0607aHistP02h.root", "READ");
    TDirectory* dirCh0 = f->GetDirectory("HistoCh0");
    TDirectory* dirDate = dirCh0->GetDirectory("20210504");
    TH1D* h = (TH1D*)dirDate->Get("200000");

    char histTitle[50];
    sprintf(histTitle, "%s%s_forCheck", dirDate->GetName(), h->GetName());
    h->SetTitle(histTitle);

    string fitPeak = "peak04";
    PeakFitter* pf = new PeakFitter(h, fitPeak);
    //pf->setFitterStr("expo");
    pf->setHistoName(histTitle);
    pf->setFolderPath("plotting/fittingHualien");
    pf->setNeedZoom(true);
    //pf->setNeedExtend(true);
    pf->fitPeak();

    f->Close();
    delete f;
}



void Handling::overlapForComparison() {
    TCanvas* c = new TCanvas("c", "c", 1400, 800);
    TFile* f = new TFile("ready/0404to0415aHistP01h.root", "READ");
    TDirectory* dirCh0 = f->GetDirectory("HistoCh0");
    TDirectory* dirDate = dirCh0->GetDirectory("20210407");
    TH1D* h = (TH1D*)dirDate->Get("190000");
    TF1* fBkg = new TF1("fBkg", "expo", 0., 5.);

    fBkg->SetParameter(0, 8.6);
    fBkg->SetParameter(1, -2.75);

    h->Draw("HISTO");
    fBkg->Draw("SAME");

    c->Update();
    c->Print("plotting/fittingHualien/overlapForComparison.png");

    f->Close();
    c->Close();

    delete fBkg;
    delete f;
    delete c;
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



void Handling::setErrorBandAtt(TGraphErrors* lineMean, TGraphErrors* band1Sig, TGraphErrors* band2Sig, string style) {
    Color_t colorLine = kBlack;
    Color_t color1Sig = kGreen;
    Color_t color2Sig = kYellow;

    if(style == "formal") {
	colorLine = kBlack;
	color1Sig = kGreen;
	color2Sig = kYellow;
    } else if(style == "compare") {
	colorLine = kRed;
	color1Sig = kBlue;
	color2Sig = kViolet;
    }

    lineMean->SetLineColor(colorLine);
    band1Sig->SetLineWidth(0);
    band1Sig->SetMarkerSize(0);
    band1Sig->SetFillColor(color1Sig);
    band2Sig->SetLineWidth(0);
    band2Sig->SetMarkerSize(0);
    band2Sig->SetFillColor(color2Sig);
}



void Handling::setHist1Att(TH1* inputH, string term, string peakType = "", string timeUnit = "") {
    string histTitle = "";
    string xAxisTitle = "";
    string yAxisTitle = "";
    if(term == "fluct") {
	if(timeUnit == "PD")
	    xAxisTitle = "Counts of Entries per Day";
	else if(timeUnit == "P12H")
	    xAxisTitle = "Counts of Entries per 12h";
	else if(timeUnit == "P6H")
	    xAxisTitle = "Counts of Entries per 6h";
	else if(timeUnit == "P2H")
	    xAxisTitle = "Counts of Entries per 2h";
	else if(timeUnit == "P1H")
	    xAxisTitle = "Counts of Entries per 1h";

	yAxisTitle = "Counts of Time";
    }

    inputH->SetXTitle(xAxisTitle.c_str());
    inputH->SetYTitle(yAxisTitle.c_str());
}



void Handling::setHist2Att(TH2* inputH, string term, string peakType = "", string timeUnit = "") {
    string histTitle = "";
    string xAxisTitle = "";
    string yAxisTitle = "";

    if(term == "hour")
	xAxisTitle = "time of day";
    else if(term == "day")
	xAxisTitle = "day";

    if(timeUnit == "PD")
	yAxisTitle = "Counts of Entries per Day";
    else if(timeUnit == "P12H")
	yAxisTitle = "Counts of Entries per 12h";
    else if(timeUnit == "P6H")
	yAxisTitle = "Counts of Entries per 6h";
    else if(timeUnit == "P2H")
	yAxisTitle = "Counts of Entries per 2h";
    else if(timeUnit == "P1H")
	yAxisTitle = "Counts of Entries per 1h";

    inputH->SetStats(kFALSE);
    inputH->SetXTitle(xAxisTitle.c_str());
    inputH->SetYTitle(yAxisTitle.c_str());
}



void Handling::setGraphAtt(TGraph* inputG, string term, string peakType = "") {
    string histTitle = "";
    string xAxisTitle = "";
    string yAxisTitle = "";
    if(term == "cPow") {
	histTitle = "Power of Coefficient of Exponential Term";
	xAxisTitle = "Date";
	yAxisTitle = "Power";
    } else if(term == "cExp") {
	histTitle = "Coefficient of Exponential Term";
	xAxisTitle = "Date";
	yAxisTitle = "Coefficient";
    } else if(term == "expo") {
	histTitle = "Exponential Power";
	xAxisTitle = "Date";
	yAxisTitle = "Exponential Constant";
    } else if(term == "cGauss") {
	histTitle = "Coefficient of Gaussian Term";
	xAxisTitle = "Date";
	yAxisTitle = "Coefficient";
    } else if(term == "mean") {
	histTitle = "Mean of the Peak";
	xAxisTitle = "Date";
	yAxisTitle = "Mean";
    } else if(term == "std") {
	histTitle = "Std of the Peak";
	xAxisTitle = "Date";
	yAxisTitle = "Std";
    } else if(term == "cExp_Norm") {
	histTitle = "Normalized Coefficient of Exponential Term";
	xAxisTitle = "Date";
	yAxisTitle = "Normalized Coefficient";
    } else if(term == "cGauss_Norm") {
	histTitle = "Normalized Coefficient of Gaussian Term";
	xAxisTitle = "Date";
	yAxisTitle = "Normalized Coefficient";
    } else if(term == "fluct") {
	histTitle = "Total Counts in an Energy Range";
	xAxisTitle = "Date";
	yAxisTitle = "Counts";
    } else if(term == "nfluct") {
	histTitle = "Normalized Counts in an Energy Range";
	xAxisTitle = "Date";
	yAxisTitle = "Counts/Average";
    } else if(term == "ct") {
	histTitle = "Counts-Temperature";
	xAxisTitle = "Temperature";
	yAxisTitle = "Counts";
    } else if(term == "EstEvents") {
	histTitle = "Estimated Events";
	xAxisTitle = "Date";
	yAxisTitle = "Counts";
    } else if(term == "cgt") {
	histTitle = "cGauss-Temperature";
	xAxisTitle = "Temperature";
	yAxisTitle = "Coefficient of Gaussian Term";
    } else {
	cout << "Unknown term to described. Skip this plotting." << endl;
	return;
    }

    if(peakType != "") {
	if(peakType == "wild" || peakType == "0to25")
	    histTitle = histTitle + " (0 to 2.5 MeV)";
	else if(peakType == "peak04")
	    histTitle = histTitle + " (Pb214, 0.354 MeV)";
	else if(peakType == "peak24")
	    histTitle = histTitle + " (2.28 MeV)";
	else if(peakType == "Rn222" || peakType == "peak06")
	    histTitle = histTitle + " (Bi214, 0.609 MeV)";
	else if(peakType == "K40" || peakType == "peak16")
	    histTitle = histTitle + " (K40, 1.46 MeV)";
	else
	    histTitle = histTitle + " (" + peakType + ")";
    }

    if(term == "ct" || term == "cgt") {
    } else {
	TDatime updt(2021, 6, 7, 0, 0, 0);
	TDatime lwdt(2021, 4, 18, 0, 0, 0);
	inputG->GetXaxis()->SetRangeUser(lwdt.Convert(), updt.Convert());
	inputG->GetXaxis()->SetTimeDisplay(kTRUE);
	inputG->GetXaxis()->SetNdivisions(10, 5, 0, kFALSE);
	inputG->GetXaxis()->SetTimeFormat("%m\/%d");
    }

    inputG->SetTitle(histTitle.c_str());
    inputG->GetXaxis()->SetTitle(xAxisTitle.c_str());
    inputG->GetYaxis()->SetTitle(yAxisTitle.c_str());
    inputG->SetLineWidth(1);
    inputG->SetLineColor(kBlack);
    inputG->SetMarkerSize(1);
    inputG->SetMarkerStyle(kCircle);
    inputG->SetMarkerColor(kBlack);
}



void Handling::setMultiGAtt(TMultiGraph* inputMG, string term, string peakType = "", string timeUnit = "") {
    string histTitle = "";
    string xAxisTitle = "";
    string yAxisTitle = "";
    if(term == "cPow") {
	histTitle = "Power of Coefficient of Exponential Term";
	xAxisTitle = "Date";
	yAxisTitle = "Power";
    } else if(term == "cExp") {
	histTitle = "Coefficient of Exponential Term";
	xAxisTitle = "Date";
	yAxisTitle = "Coefficient of Exponential Term";
    } else if(term == "expo") {
	histTitle = "Exponential Power";
	xAxisTitle = "Date";
	yAxisTitle = "Exponential Constant";
    } else if(term == "cGauss") {
	histTitle = "Coefficient of Gaussian Term";
	xAxisTitle = "Date";
	yAxisTitle = "Coefficient";
    } else if(term == "mean") {
	histTitle = "Mean of the Peak";
	xAxisTitle = "Date";
	yAxisTitle = "Mean";
    } else if(term == "std") {
	histTitle = "Std of the Peak";
	xAxisTitle = "Date";
	yAxisTitle = "Std";
    } else if(term == "cExp_Norm") {
	histTitle = "Normalized Coefficient of Exponential Term";
	xAxisTitle = "Date";
	yAxisTitle = "Normalized Coefficient";
    } else if(term == "cGauss_Norm") {
	histTitle = "Normalized Coefficient of Gaussian Term";
	xAxisTitle = "Date";

	if(timeUnit == "PD")
	    yAxisTitle = "C_{Gaus24hr}/#bar{C}_{Gaus24hr}";
	else if(timeUnit == "P12H")
	    yAxisTitle = "C_{Gaus12hr}/#bar{C}_{Gaus12hr}";
	else if(timeUnit == "P6H")
	    yAxisTitle = "C_{Gaus6hr}/#bar{C}_{Gaus6hr}";
	else if(timeUnit == "P2H")
	    yAxisTitle = "C_{Gaus2hr}/#bar{C}_{Gaus2hr}";
	else if(timeUnit == "P1H")
	    yAxisTitle = "C_{Gaus1hr}/#bar{C}_{Gaus1hr}";
    } else if(term == "ct") {
	histTitle = "Counts-Temperature";
	xAxisTitle = "Temperature";
	yAxisTitle = "Counts";
    } else if(term == "EstEvents") {
	histTitle = "Estimated Events";
	xAxisTitle = "Date";
	yAxisTitle = "Counts";
    } else if(term == "cgt") {
	histTitle = "cGauss-Temperature";
	xAxisTitle = "Temperature";
	yAxisTitle = "Coefficient of Gaussian Term";
    } else if(term == "fluct") {
	histTitle = "Total Counts in an Energy Range";

	if(timeUnit == "PD")
	    yAxisTitle = "N_{24hr}";
	else if(timeUnit == "P12H")
	    yAxisTitle = "N_{12hr}";
	else if(timeUnit == "P6H")
	    yAxisTitle = "N_{6hr}";
	else if(timeUnit == "P2H")
	    yAxisTitle = "N_{2hr}";
	else if(timeUnit == "P1H")
	    yAxisTitle = "N_{1hr}";
    } else if(term == "nfluct") {
	histTitle = "Normalized Counts in an Energy Range";

	if(timeUnit == "PD")
	    yAxisTitle = "N_{24hr}/#bar{N}_{24hr}";
	else if(timeUnit == "P12H")
	    yAxisTitle = "N_{12hr}/#bar{N}_{12hr}";
	else if(timeUnit == "P6H")
	    yAxisTitle = "N_{6hr}/#bar{N}_{6h}}";
	else if(timeUnit == "P2H")
	    yAxisTitle = "N_{2hr}/#bar{N}_{2hr}";
	else if(timeUnit == "P1H")
	    yAxisTitle = "N_{1hr}/#bar{N}_{1hr}";
    } else {
	cout << "Unknown term to described. Skip this plotting." << endl;
	return;
    }

    if(peakType != "") {
	if(peakType == "wild" || peakType == "0to25")
	    histTitle = histTitle + " (0 to 2.5 MeV)";
	else if(peakType == "peak04")
	    histTitle = histTitle + " (Pb214, 0.354 MeV)";
	else if(peakType == "peak24")
	    histTitle = histTitle + " (2.28 MeV)";
	else if(peakType == "Rn222" || peakType == "peak06")
	    histTitle = histTitle + " (Bi214, 0.609 MeV)";
	else if(peakType == "K40" || peakType == "peak16")
	    histTitle = histTitle + " (K40, 1.46 MeV)";
	else
	    histTitle = histTitle + " (" + peakType + ")";
    }

    TDatime updt(2021, 6, 7, 0, 0, 0);
    TDatime lwdt(2021, 4, 18, 0, 0, 0);
    inputMG->GetXaxis()->SetRangeUser(lwdt.Convert(), updt.Convert());
    inputMG->GetXaxis()->SetTimeDisplay(kTRUE);
    inputMG->GetXaxis()->SetNdivisions(5, 2, 5, kFALSE);
    inputMG->GetXaxis()->SetTimeFormat("%m\/%d");

    //inputMG->SetTitle(histTitle.c_str());
    inputMG->GetXaxis()->SetTitle(xAxisTitle.c_str());
    inputMG->GetYaxis()->SetTitle(yAxisTitle.c_str());
}



void Handling::getHistUpLow(string term, string peakType, string timeUnit, double& upper, double& lower) {
    if(term == "fluct") {
	if(peakType == "K40" || peakType == "peak16") {
	    if(timeUnit == "PD") {
		upper = 280000.;
		lower = 265000.;
	    } else if(timeUnit == "P12H") {
		upper = 142000.;
		lower = 125000.;
	    } else if(timeUnit == "P6H") {
		upper = 72000.;
		lower = 60000.;
	    } else if(timeUnit == "P2H") {
		upper = 24000.;
		lower = 20000.;
	    } else if(timeUnit == "P1H") {
		upper = 13000.;
		lower = 8000.;
	    }
	} else if(peakType == "Rn222" || peakType == "peak06") {
	    if(timeUnit == "PD") {
		upper = 1150000.;
		lower = 900000.;
	    } else if(timeUnit == "P12H") {
		upper = 580000.;
		lower = 500000.;
	    } else if(timeUnit == "P6H") {
		upper = 350000.;
		lower = 200000.;
	    } else if(timeUnit == "P2H") {
		upper = 100000.;
		lower = 80000.;
	    } else if(timeUnit == "P1H") {
		upper = 50000.;
		lower = 40000.;
	    }
	} else if(peakType == "peak04") {
	    if(timeUnit == "PD") {
		upper = 460000.;
		lower = 400000.;
	    } else if(timeUnit == "P12H") {
		upper = 250000.;
		lower = 150000.;
	    } else if(timeUnit == "P6H") {
		upper = 120000.;
		lower = 80000.;
	    } else if(timeUnit == "P2H") {
		upper = 40000.;
		lower = 30000.;
	    } else if(timeUnit == "P1H") {
		upper = 20000.;
		lower = 15000.;
	    }
	} else if(peakType == "peak24") {
	    if(timeUnit == "PD") {
		upper = 50000.;
		lower = 0.;
	    } else if(timeUnit == "P12H") {
		upper = 25000.;
		lower = 0.;
	    } else if(timeUnit == "P6H") {
		upper = 15000.;
		lower = 0.;
	    } else if(timeUnit == "P2H") {
		upper = 5000.;
		lower = 0.;
	    } else if(timeUnit == "P1H") {
		upper = 2500.;
		lower = 0.;
	    }
	} else if(peakType == "wild" || peakType == "0to25") {
	    if(timeUnit == "PD") {
		upper = 6000000.;
		lower = 4500000.;
	    } else if(timeUnit == "P12H") {
		upper = 3200000.;
		lower = 2400000.;
	    } else if(timeUnit == "P6H") {
		upper = 1500000.;
		lower = 1200000.;
	    } else if(timeUnit == "P2H") {
		upper = 480000.;
		lower = 400000.;
	    } else if(timeUnit == "P1H") {
		upper = 235000.;
		lower = 215000.;
	    }
	} else if(peakType == "peak04n06") {
	    if(timeUnit == "PD") {
		upper = 460000. + 1150000.;
		lower = 400000. + 900000.;
	    } else if(timeUnit == "P12H") {
		upper = 250000. + 580000.;
		lower = 150000. + 500000.;
	    } else if(timeUnit == "P6H") {
		upper = 120000. + 350000.;
		lower = 80000. + 200000.;
	    } else if(timeUnit == "P2H") {
		upper = 40000. + 100000.;
		lower = 30000. + 80000.;
	    } else if(timeUnit == "P1H") {
		upper = 20000. + 50000.;
		lower = 15000. + 40000.;
	    }
	}
    } else if(term == "nfluct") {
	upper = 1.1;
	lower = 0.9;
    } else if(term == "day") {
	upper = 24.;
	lower = 0.;
    }
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



void Handling::setBins(TH1* inputH, string term, string peakType, string timeUnit) {
    double nbin = 100;
    double upper = 100.;
    double lower = 0.;

    getHistUpLow(term, peakType, timeUnit, upper, lower);

    if(nbin == 1)
	nbin = upper - lower;

    inputH->SetBins(nbin, lower, upper);
}



void Handling::setBins(TH2* inputH, string term, string peakType, string timeUnit) {
    double nxbin = 100;
    double xupper = 100.;
    double xlower = 0.;
    double nybin = 100;
    double yupper = 100.;
    double ylower = 0.;

    getHistUpLow("day", peakType, timeUnit, xupper, xlower);
    if(term == "day" || term == "hour") {
	getHistUpLow("day", peakType, timeUnit, xupper, xlower);
	getHistUpLow("fluct", peakType, timeUnit, yupper, ylower);
    } else if(term == "doubleER") {
	getHistUpLow("fluct", "peak04", timeUnit, xupper, xlower);
	getHistUpLow("fluct", "peak06", timeUnit, yupper, ylower);
    } else if(term == "DERN") {
	xupper = 1.03;
	xlower = 0.97;
	yupper = 1.03;
	ylower = 0.97;
    } else if(term == "DER2") {
	getHistUpLow("fluct", "peak16", timeUnit, xupper, xlower);
	getHistUpLow("fluct", "peak06", timeUnit, yupper, ylower);
    } else if(term == "DERN2") {
	xupper = 1.03;
	xlower = 0.97;
	yupper = 1.03;
	ylower = 0.97;
    } else if(term == "testRange") {
	xupper = 1.2;
	xlower = 0.8;
	yupper = 1.2;
	ylower = 0.8;
    } else if(term == "testRange2") {
	xupper = 7000.;
	xlower = 4000.;
	yupper = 14000.;
	ylower = 11000.;
    } else {
	getHistUpLow("day", peakType, timeUnit, xupper, xlower);
	getHistUpLow(term, peakType, timeUnit, yupper, ylower);
    }

    if(nxbin == 1)
	nxbin = xupper - xlower;
    else if(term == "hour") {
	if(timeUnit == "P1H")
	    nxbin = 24;
	else if(timeUnit == "P2H")
	    nxbin = 12;
	else if(timeUnit == "P6H")
	    nxbin = 4;
	else if(timeUnit == "P12H")
	    nxbin = 2;
	else if(timeUnit == "PD")
	    nxbin = 1;
    } else if(term == "day") {
	nxbin = 24;
    } else if(term == "DoubleER") {
	nxbin = ceil((xupper - xlower)/500.);
    } else if(term == "DERN") {
	nxbin = 40;
    } else if(term == "testRange") {
	nxbin = 40;
    } else if(term == "testRange2") {
	nxbin = 50;
    } else
	nxbin = 30;

    if(nybin == 1)
	nybin = yupper - ylower;
    else if(term == "DoubleER")
	nybin = ceil((yupper - ylower)/500.);
    else if(term == "DERN")
	nybin = 40;
    else if(term == "testRange")
	nybin = 40;
    else if(term == "testRange2")
	nybin = 50;
    else
	nybin = 30;
	

    inputH->SetBins(nxbin, xlower, xupper, nybin, ylower, yupper);    
}



void Handling::setRangeUser(TAxis* inputAxis, string term, string peakType, string timeUnit) {
    double upper = 1.;
    double lower = 0.;

    if(term == "cPow") {
	if(peakType == "K40" || peakType == "peak16") {
	    upper = 15.;
	    lower = 0.;
	} else if(peakType == "Rn222" || peakType == "peak06") {
	    upper = 15.;
	    lower = 0.;
	} else if(peakType == "peak04") {
	    upper = 15.;
	    lower = 0.;
	} else if(peakType == "peak24") {
	    upper = 40.;
	    lower = 10.;
	}
    } else if(term == "cExp") {
	if(peakType == "K40" || peakType == "peak16") {
	    if(timeUnit == "PD") {
		upper = 180000.;
		lower = 24000.;
	    } else if(timeUnit == "P12H") {
		upper = 90000.;
		lower = 12000.;
	    } else if(timeUnit == "P6H") {
		upper = 45000.;
		lower = 6000.;
	    } else if(timeUnit == "P2H") {
		upper = 15000.;
		lower = 2000.;
	    } else if(timeUnit == "P1H") {
		upper = 10000.;
		lower = 0.;
	    }
	} else if(peakType == "Rn222" || peakType == "peak06") {
	    if(timeUnit == "PD") {
		upper = 360000.;
		lower = 48000.;
	    } else if(timeUnit == "P12H") {
		upper = 180000.;
		lower = 24000.;
	    } else if(timeUnit == "P6H") {
		upper = 90000.;
		lower = 12000.;
	    } else if(timeUnit == "P2H") {
		upper = 30000.;
		lower = 4000.;
	    } else if(timeUnit == "P1H") {
		upper = 15000.;
		lower = 2000.;
	    }
	} else if(peakType == "peak04") {
	    if(timeUnit == "PD") {
		upper = 240000.;
		lower = 120000.;
	    } else if(timeUnit == "P12H") {
		upper = 120000.;
		lower = 60000.;
	    } else if(timeUnit == "P6H") {
		upper = 60000.;
		lower = 30000.;
	    } else if(timeUnit == "P2H") {
		upper = 20000.;
		lower = 10000.;
	    } else if(timeUnit == "P1H") {
		upper = 10000.;
		lower = 5000.;
	    }
	} else if(peakType == "peak24") {
	    if(timeUnit == "PD") {
		upper = 5e12;
		lower = 0.;
	    } else if(timeUnit == "P12H") {
		upper = 5e12;
		lower = 0.;
	    } else if(timeUnit == "P6H") {
		upper = 5e12;
		lower = 0.;
	    } else if(timeUnit == "P2H") {
		upper = 5e12;
		lower = 0.;
	    } else if(timeUnit == "P1H") {
		upper = 5e12;
		lower = 0.;
	    }
	}
    } else if(term == "expo") {
	if(peakType == "K40" || peakType == "peak16") {
	    upper = -1.;
	    lower = -10.;
	} else if(peakType == "Rn222" || peakType == "peak06") {
	    upper = -1.;
	    lower = -10.;
	} else if(peakType == "peak04") {
	    upper = -1.;
	    lower = -10.;
	} else if(peakType == "peak24") {
	    upper = -7.;
	    lower = -15.;
	}
    } else if(term == "cGauss") {
	if(peakType == "K40" || peakType == "peak16") {
	    if(timeUnit == "PD") {
		upper = 1300.;
		lower = 1000.;
	    } else if(timeUnit == "P12H") {
		upper = 620.;
		lower = 500.;
	    } else if(timeUnit == "P6H") {
		upper = 320.;
		lower = 250.;
	    } else if(timeUnit == "P2H") {
		upper = 110.;
		lower = 60.;
	    } else if(timeUnit == "P1H") {
		upper = 60.;
		lower = 20.;
	    }
	} else if(peakType == "Rn222" || peakType == "peak06") {
	    if(timeUnit == "PD") {
		upper = 1000.;
		lower = 600.;
	    } else if(timeUnit == "P12H") {
		upper = 500.;
		lower = 300.;
	    } else if(timeUnit == "P6H") {
		upper = 250.;
		lower = 150.;
	    } else if(timeUnit == "P2H") {
		upper = 100.;
		lower = 0.;
	    } else if(timeUnit == "P1H") {
		upper = 50.;
		lower = 0.;
	    }
	} else if(peakType == "peak04") {
	    if(timeUnit == "PD") {
		upper = 360.;
		lower = 0.;
	    } else if(timeUnit == "P12H") {
		upper = 180.;
		lower = 0.;
	    } else if(timeUnit == "P6H") {
		upper = 90.;
		lower = 0.;
	    } else if(timeUnit == "P2H") {
		upper = 50.;
		lower = -10.;
	    } else if(timeUnit == "P1H") {
		upper = 20.;
		lower = -10.;
	    }
	} else if(peakType == "peak24") {
	    if(timeUnit == "PD") {
		upper = 360.;
		lower = 120.;
	    } else if(timeUnit == "P12H") {
		upper = 180.;
		lower = 60.;
	    } else if(timeUnit == "P6H") {
		upper = 90.;
		lower = 30.;
	    } else if(timeUnit == "P2H") {
		upper = 30.;
		lower = 10.;
	    } else if(timeUnit == "P1H") {
		upper = 20.;
		lower = 0.;
	    }
	}
    } else if(term == "mean") {
	if(peakType == "K40" || peakType == "peak16") {
	    upper = 1.47;
	    lower = 1.37;
	} else if(peakType == "Rn222" || peakType == "peak06") {
	    upper = 0.62;
	    lower = 0.55;
	} else if(peakType == "peak04") {
	    upper = 0.3;
	    lower = 0.25;
	} else if(peakType == "peak24") {
	    upper = 2.3;
	    lower = 2.2;
	}
    } else if(term == "std") {
	if(peakType == "K40" || peakType == "peak16") {
	    upper = 0.04;
	    lower = 0.02;
	} else if(peakType == "Rn222" || peakType == "peak06") {
	    upper = 0.04;
	    lower = 0.02;
	} else if(peakType == "peak04") {
	    upper = 0.01;
	    lower = 0.0;
	} else if(peakType == "peak24") {
	    upper = 0.04;
	    lower = 0.02;
	}
    } else if(term == "cExp_Norm") {
	double dup = 1.4;
	double ddw = 0.6;
	if(peakType == "K40" || peakType == "peak16") {
	    upper = dup;
	    lower = ddw;
	} else if(peakType == "Rn222" || peakType == "peak06") {
	    upper = dup;
	    lower = ddw;
	} else if(peakType == "peak04") {
	    upper = dup;
	    lower = ddw;
	} else if(peakType == "peak24") {
	    upper = dup;
	    lower = ddw;
	}
    } else if(term == "cGauss_Norm") {
	double dup = 2.;
	double ddw = 0.;
	if(peakType == "K40" || peakType == "peak16") {
	    upper = 1.2;
	    lower = 0.5;	    
	} else if(peakType == "Rn222" || peakType == "peak06") {
	    upper = 1.2;
	    lower = 0.6;
	} else if(peakType == "peak04") {
	    upper = 1.4;
	    lower = 0.2;
	} else if(peakType == "peak24") {
	    upper = 1.2;
	    lower = 0.6;
	}
    } else if(term == "fluct") {
	if(peakType == "K40" || peakType == "peak16") {
	    if(timeUnit == "PD") {
		upper = 290000.;
		lower = 220000.;
	    } else if(timeUnit == "P12H") {
		upper = 145000.;
		lower = 100000.;
	    } else if(timeUnit == "P6H") {
		upper = 72000.;
		lower = 50000.;
	    } else if(timeUnit == "P2H") {
		upper = 24000.;
		lower = 15000.;
	    } else if(timeUnit == "P1H") {
		upper = 13000.;
		lower = 8000.;
	    }
	} else if(peakType == "Rn222" || peakType == "peak06") {
	    if(timeUnit == "PD") {
		upper = 1150000.;
		lower = 900000.;
	    } else if(timeUnit == "P12H") {
		upper = 580000.;
		lower = 500000.;
	    } else if(timeUnit == "P6H") {
		upper = 300000.;
		lower = 200000.;
	    } else if(timeUnit == "P2H") {
		upper = 100000.;
		lower = 75000.;
	    } else if(timeUnit == "P1H") {
		upper = 49000.;
		lower = 40000.;
	    }
	} else if(peakType == "peak04") {
	    if(timeUnit == "PD") {
		upper = 460000.;
		lower = 350000.;
	    } else if(timeUnit == "P12H") {
		upper = 250000.;
		lower = 150000.;
	    } else if(timeUnit == "P6H") {
		upper = 120000.;
		lower = 80000.;
	    } else if(timeUnit == "P2H") {
		upper = 40000.;
		lower = 30000.;
	    } else if(timeUnit == "P1H") {
		upper = 20000.;
		lower = 15000.;
	    }
	} else if(peakType == "peak24") {
	    if(timeUnit == "PD") {
		upper = 50000.;
		lower = 0.;
	    } else if(timeUnit == "P12H") {
		upper = 25000.;
		lower = 0.;
	    } else if(timeUnit == "P6H") {
		upper = 15000.;
		lower = 0.;
	    } else if(timeUnit == "P2H") {
		upper = 5000.;
		lower = 0.;
	    } else if(timeUnit == "P1H") {
		upper = 2200.;
		lower = 0.;
	    }
	} else if(peakType == "wild" || peakType == "0to25") {
	    if(timeUnit == "PD") {
		upper = 6000000.;
		lower = 4500000.;
	    } else if(timeUnit == "P12H") {
		upper = 3000000.;
		lower = 2000000.;
	    } else if(timeUnit == "P6H") {
		upper = 1500000.;
		lower = 1000000.;
	    } else if(timeUnit == "P2H") {
		upper = 480000.;
		lower = 400000.;
	    } else if(timeUnit == "P1H") {
		upper = 250000.;
		lower = 150000.;
	    }
	} else if(peakType == "peak04n06") {
	    if(timeUnit == "PD") {
		upper = 460000. + 1150000.;
		lower = 400000. + 900000.;
	    } else if(timeUnit == "P12H") {
		upper = 250000. + 580000.;
		lower = 150000. + 500000.;
	    } else if(timeUnit == "P6H") {
		upper = 120000. + 350000.;
		lower = 80000. + 200000.;
	    } else if(timeUnit == "P2H") {
		upper = 40000. + 100000.;
		lower = 30000. + 80000.;
	    } else if(timeUnit == "P1H") {
		upper = 20000. + 50000.;
		lower = 15000. + 40000.;
	    }
	}
    } else if(term == "nfluct") {
	double dup = 1.05;
	double ddw = 0.75;
	if(peakType == "K40" || peakType == "peak16") {
	    if(timeUnit == "PD") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P12H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P6H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P2H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P1H") {
		upper = dup;
		lower = ddw;
	    }
	} else if(peakType == "Rn222" || peakType == "peak06") {
	    if(timeUnit == "PD") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P12H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P6H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P2H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P1H") {
		upper = dup;
		lower = ddw;
	    }
	} else if(peakType == "peak04") {
	    if(timeUnit == "PD") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P12H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P6H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P2H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P1H") {
		upper = dup;
		lower = ddw;
	    }
	} else if(peakType == "peak24") {
	    if(timeUnit == "PD") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P12H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P6H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P2H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P1H") {
		upper = dup;
		lower = ddw;
	    }
	} else if(peakType == "wild" || peakType == "0to25") {
	    if(timeUnit == "PD") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P12H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P6H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P2H") {
		upper = dup;
		lower = ddw;
	    } else if(timeUnit == "P1H") {
		upper = dup;
		lower = ddw;
	    }
	} else if(peakType == "peak04n06") {
	    upper = dup;
	    lower = ddw;
	}
    } else if(term == "EstEvents") {
	if(peakType == "K40" || peakType == "peak16") {
	    if(timeUnit == "PD") {
		upper = 310000.;
		lower = 10000.;
	    } else if(timeUnit == "P12H") {
		upper = 160000.;
		lower = 50000.;
	    } else if(timeUnit == "P6H") {
		upper = 80000.;
		lower = 26000.;
	    } else if(timeUnit == "P2H") {
		upper = 27000.;
		lower = 8500.;
	    } else if(timeUnit == "P1H") {
		upper = 18000.;
		lower = 0.;
	    }
	} else if(peakType == "Rn222" || peakType == "peak06") {
	    if(timeUnit == "PD") {
		upper = 180000.;
		lower = 100000.;
	    } else if(timeUnit == "P12H") {
		upper = 88000.;
		lower = 52000.;
	    } else if(timeUnit == "P6H") {
		upper = 45000.;
		lower = 25000.;
	    } else if(timeUnit == "P2H") {
		upper = 18000.;
		lower = 0.;
	    } else if(timeUnit == "P1H") {
		upper = 18000.;
		lower = 0.;
	    }
	} else if(peakType == "peak04") {
	    if(timeUnit == "PD") {
		upper = 65000.;
		lower = 0.;
	    } else if(timeUnit == "P12H") {
		upper = 32000.;
		lower = 0.;
	    } else if(timeUnit == "P6H") {
		upper = 16000.;
		lower = 0.;
	    } else if(timeUnit == "P2H") {
		upper = 5500.;
		lower = 0.;
	    } else if(timeUnit == "P1H") {
		upper = 2600.;
		lower = 0.;
	    }
	} else if(peakType == "peak24") {
	    if(timeUnit == "PD") {
		upper = 63000.;
		lower = 20000.;
	    } else if(timeUnit == "P12H") {
		upper = 32000.;
		lower = 10000.;
	    } else if(timeUnit == "P6H") {
		upper = 16000.;
		lower = 5200.;
	    } else if(timeUnit == "P2H") {
		upper = 5500.;
		lower = 1500.;
	    } else if(timeUnit == "P1H") {
		upper = 3500.;
		lower = 0.;
	    }
	}
    }

    inputAxis->SetRangeUser(lower, upper);
}



void Handling::adjustFittingRange(string dtStr, string peak, PeakFitter* pf) {
    double upperRange, lowerRange;
    double startCPow, upperCPow, lowerCPow;
    double startExpo, upperExpo, lowerExpo;
    double startCGauss, upperCGauss, lowerCGauss;
    double startMean, upperMean, lowerMean;
    double startSTD, upperSTD, lowerSTD;

    pf->getSetRange(lowerRange, upperRange);
    pf->getSetCPow(startCPow, lowerCPow, upperCPow);
    pf->getSetExpo(startExpo, lowerExpo, upperExpo);
    pf->getSetCGauss(startCGauss, lowerCGauss, upperCGauss);
    pf->getSetMean(startMean, lowerMean, upperMean);
    pf->getSetSTD(startSTD, lowerSTD, upperSTD);

    if(dtStr == "20210407190000" && peak == "K40") {
    }

    pf->setRange(lowerRange, upperRange);
    pf->setCPow(startCPow, lowerCPow, upperCPow);
    pf->setExpo(startExpo, lowerExpo, upperExpo);
    pf->setCGauss(startCGauss, lowerCGauss, upperCGauss);
    pf->setMean(startMean, lowerMean, upperMean);
    pf->setSTD(startSTD, lowerSTD, upperSTD);
}



string Handling::takePeakTypeStr(string peakType) {
    if(peakType == "peak04")
	return "0.354 MeV (Pb214)";
    else if(peakType == "peak06")
	return "0.609 MeV (Bi214)";
    else if(peakType == "peak16")
	return "1.46 MeV (K40)";
    else if(peakType == "peak24")
	return "2.28 MeV";
    else if(peakType == "0to25")
	return "From 0 to 2.5 MeV";
    else if(peakType == "peak04n06")
	return "0.354 MeV (Pb214) & 0.609 MeV (Bi214)";
    else {
	cout << "Unknown peak type!!!" << endl;
	return "";
    }
}



double Handling::takeTimeLength(string timeUnit) {
    // the unit of return time is hour
    if(timeUnit == "PT")
	return 24.*50.; // 24 hours multiply 50 days
    else if(timeUnit == "PD")
	return 24.;
    else if(timeUnit == "P12H")
	return 12.;
    else if(timeUnit == "P6H")
	return 6.;
    else if(timeUnit == "P2H")
	return 2.;
    else if(timeUnit == "P1H")
	return 1.;
    else {
	cout << "Time unit term is set inappropriately!!!" << endl;
	exit(0);
	return 0.;
    }
}
