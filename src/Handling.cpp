#include "Handling.h"

#include "Calendar.h"
#include "GetExterSet.h"
#include "PeakFitter.h"

#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TFitResult.h>
#include <TGraph.h>
#include <TGraphErrors.h>
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

    peakList.push_back("K40");
    peakList.push_back("Rn222");

    timeUnitList.push_back("P11D");
    timeUnitList.push_back("PD");
    //timeUnitList.push_back("P12H");
    //timeUnitList.push_back("P6H");
    //timeUnitList.push_back("P1H");

    map<string, TF1*> f;
    map<string, TGraph*> g;
    map<string, TGraphErrors*> ge;

    for(unsigned iTerm = 0; iTerm < termList.size(); iTerm++) {
	for(unsigned iPeak = 0; iPeak < peakList.size(); iPeak++) {
	    for(unsigned iTU = 0; iTU < timeUnitList.size(); iTU++) {
		string mapLabel = termList[iTerm] + "," + peakList[iPeak] + "," + timeUnitList[iTU];
		string setName = "f" + termList[iTerm] + peakList[iPeak] + timeUnitList[iTU];

		if(timeUnitList[iTU] == "P11D")
		    f[mapLabel] = new TF1(setName.c_str(), "[0]", 0., 11.);
		else if(timeUnitList[iTU] == "PD"
			|| timeUnitList[iTU] == "P12H"
			|| timeUnitList[iTU] == "P6H"
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
	if(thisTU == "P11D")
	    file = new TFile("ready/0404to0415oneHist.root", "READ");
	else if(thisTU == "PD")
	    file = new TFile("ready/0404to0415aHistPDay.root", "READ");
	else if(thisTU == "P12H")
	    file = new TFile("ready/0404to0415aHistP12h.root", "READ");
	else if(thisTU == "P6H")
	    file = new TFile("ready/0404to0415aHistP06h.root", "READ");
	else if(thisTU == "P1H")
	    file = new TFile("ready/0404to0415aHistP01h.root", "READ");

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
		if(thisTU == "P11D") {
		    alignCenter = 0.;
		    strForTitle = "per_11_days";
		} else if(thisTU == "PD") {
		    alignCenter = 12.;
		    strForTitle = "per_01_day";
		} else if(thisTU == "P12H") {
		    alignCenter = 6.;
		    strForTitle = "per_12_hours";
		} else if(thisTU == "P6H") {
		    alignCenter = 3.;
		    strForTitle = "per_06_hours";
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





		string peakForNorm = "K40";
		PeakFitter* pf = new PeakFitter(thisH, peakForNorm);
		pf->setPeakType(peakForNorm);
		string histoName = (string)histTitle + "_" + peakForNorm;
		pf->setHistoName(histoName);
		pf->fitPeak();

		double cGaussForNorm = pf->getCGauss();
		double errCGausForNorm = pf->getErrorCGaus();
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
			keyInValue = pf->getAssignedValue(thisTerm);
			keyInError = pf->getAssignedError(thisTerm);
		    } else {
			string actualTerm = thisTerm.substr(0, thisTerm.find_first_of("_"));
			keyInValue = pf->getAssignedValue(actualTerm)/cGaussForNorm;
			if(thisTerm.find("cGauss") != string::npos) {
			    keyInError = 0.;
			} else {
			    double ubar = pf->getAssignedValue(actualTerm);
			    double usig = pf->getAssignedError(actualTerm);
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
		    pf->fitPeak();

		    for(unsigned int iTerm = 0; iTerm < termList.size(); iTerm++) {
			string thisTerm = termList[iTerm];
			string label = thisTerm + "," + thisPeak + "," + thisTU;

			double keyInValue = 0.;
			double keyInError = 0.;
			if(thisTerm.find("Norm") == string::npos) {
			    keyInValue = pf->getAssignedValue(thisTerm);
			} else {
			    string actualTerm = thisTerm.substr(0, thisTerm.find_first_of("_"));
			    keyInValue = pf->getAssignedValue(actualTerm)/cGaussForNorm;

			    double ubar = pf->getAssignedValue(actualTerm);
			    double usig = pf->getAssignedError(actualTerm);
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

		cout << endl << endl << endl;
	    }
	}

	file->Close();
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

		string graphLabel = thisTerm + "," + thisPeak + "," + thisTU;;

		if(g.find(graphLabel) != g.cend()) {
		    string folderPath = "plotting/fittingHualien/g";
		    string outputFilename = folderPath + "/g_"
			+ thisTerm + "_"
			+ thisPeak + "_"
			+ thisTU + ".png";

		    TGraph* thisG = g[graphLabel];
		    thisG->Draw("AP");
		    thisF->Draw("SAME");
		    setGraphAtt(thisG, termList[iTerm], peakList[iPeak]);
		    setRangeUser(thisG, termList[iTerm], peakList[iPeak]);
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
		    setRangeUser(thisGE, thisTerm, thisPeak);
		    cGraph->Update();
		    cGraph->Print(outputFilename.c_str());
		}
	    }
	}
    }
}



void Handling::doProcedure3() {
    TFile *f = new TFile("ready/0404to0415aHistP01h.root", "READ");
    TDirectory* dirCh0 = f->GetDirectory("HistoCh0");
    TDirectory* dirDate = dirCh0->GetDirectory("20210407");
    TH1D* h = (TH1D*)dirDate->Get("190000");

    char histTitle[50];
    sprintf(histTitle, "%s%s_forCheck", dirDate->GetName(), h->GetName());
    h->SetTitle(histTitle);

    string fitPeak = "K40";
    PeakFitter* pf = new PeakFitter(h, fitPeak);
    pf->setHistoName(histTitle);
    pf->setFolderPath("plotting/fittingHualien");
    pf->setNeedZoom(true);
    pf->fitPeak();

    f->Close();
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



void Handling::setGraphAtt(TGraph* inputG, string term, string peakType = "") {
    string histTitle = "";
    string yAxisTitle = "";
    if(term == "cPow") {
	histTitle = "Power of Coefficient of Exponential Term";
	yAxisTitle = "Power";
    } else if(term == "cExp") {
	histTitle = "Coefficient of Exponential Term";
	yAxisTitle = "Coefficient";
    } else if(term == "expo") {
	histTitle = "Exponential Power";
	yAxisTitle = "Exponential Constant";
    } else if(term == "cGauss") {
	histTitle = "Coefficient of Gaussian Term";
	yAxisTitle = "Coefficient";
    } else if(term == "mean") {
	histTitle = "Mean of the Peak";
	yAxisTitle = "Mean";
    } else if(term == "std") {
	histTitle = "Std of the Peak";
	yAxisTitle = "Std";
    } else if(term == "cExp_Norm") {
	histTitle = "Normalized Coefficient of Exponential Term";
	yAxisTitle = "Normalized Coefficient";
    } else if(term == "cGauss_Norm") {
	histTitle = "Normalized Coefficient of Gaussian Term";
	yAxisTitle = "Normalized Coefficient";
    } else {
	cout << "Unknown term to described. Skip this plotting." << endl;
	return;
    }

    if(peakType != "")
	histTitle = histTitle + " (" + peakType + ")";
	

    inputG->SetTitle(histTitle.c_str());
    inputG->GetXaxis()->SetTitle("Days");
    inputG->GetYaxis()->SetTitle(yAxisTitle.c_str());
    inputG->SetLineWidth(2);
    inputG->SetLineColor(kBlack);
    inputG->SetMarkerSize(2);
    inputG->SetMarkerStyle(kFullCircle);
    inputG->SetMarkerColor(kRed);
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

    if(term == "cPow") {
	if(peakType == "K40") {
	    upper = 20.;
	    lower = 0.;
	} else if(peakType == "Rn222") {
	    upper = 20.;
	    lower = 0.;
	}
    } else if(term == "cExp") {
	if(peakType == "K40") {
	    upper = 1500000.;
	    lower = 0.;
	} else if(peakType == "Rn222") {
	    upper = 3500000.;
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
	    upper = 1200.;
	    lower = 0.;
	} else if(peakType == "Rn222") {
	    upper = 900.;
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
    } else if(term == "cExp_Norm") {
	if(peakType == "K40") {
	    upper = 400.;
	    lower = 0.;
	} else if(peakType == "Rn222") {
	    upper = 400.;
	    lower = 0.;
	}
    } else if(term == "cGauss_Norm") {
	if(peakType == "K40") {
	    upper = 1.1;
	    lower = 0.8;	    
	} else if(peakType == "Rn222") {
	    upper = 0.8;
	    lower = 0.7;
	}
    }

    inputG->GetYaxis()->SetRangeUser(lower, upper);
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
