#include "FittingWork.h"

#include <TFile.h>

FittingWork::FittingWork() {}



FittingWork::~FittingWork() {}



void FittingWork::execute() {}



void FittingWork::test() {}



void FittingWork::fit1() {
    TFile *fLab923 = new TFile("ready/bkg_lab923.root");
    TFile *fLab201 = new TFile("ready/bkg_lab201.root");
    TFile *fHLStat = new TFile("ready/bkg_HL0409.root");
}
