#include "Handling.h"

#include <TCanvas.h>
#include <TFile.h>
#include <TH1.h>
#include <TLegend.h>

#include <fstream>

Handling::Handling() {}



Handling::~Handling() {}



void Handling::execute() {
    doProcedure1();
}



void Handling::test() {
    TCanvas* c = new TCanvas("c", "c", 1400, 800);
    TFile* f1 = new TFile("ready/bkg_HL0409.root");

    TH1D* h1 = (TH1D*)f1->Get("20210409/003000");

    h1->Draw("HIST");
    c->Print("plotting/overlap/testing.png");
    f1->Close();
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
    TCanvas* c = new TCanvas("c", "c", 1400, 800);
    TLegend* legend = new TLegend(0.5, 0.5, 0.9, 0.9);

    TFile* f1Hist = new TFile("ready/0404to0415oneHist.root");
    TFile* f1HpD = new TFile("ready/0404to0415aHistPDay.root");

    TH1D* h1Hist = (TH1D*)f1Hist->Get("HistoCh0/20210404/000000");
    TH1D* h1HpD = (TH1D*)f1HpD->Get("HistoCh0/20210404/000000");

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
