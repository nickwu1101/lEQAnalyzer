#include "TimeAnalysis.h"

#include <TFile.h>
#include <TH1.h>
#include <TTree.h>

#include "Calendar.h"
#include "PeakFitter.h"
#include "TempHumi.h"

TimeAnalysis::TimeAnalysis() {
    handledTU = "P2H";
}



TimeAnalysis::~TimeAnalysis() {}



void TimeAnalysis::execute() {
}



void TimeAnalysis::test() {
    //analyzeByFitting();
    analyzeByCounting();
    //correctFittingAnaByTemp();
    //correctCountingAnaByTemp();
    //normalizeFittingResult();
    normalizeCountingResult();
}



void TimeAnalysis::analyzeByFitting () {
    TFile* foutput = nullptr;
    if(handledTU == "P2H")
	foutput = new TFile("ready/fitResult.root", "RECREATE");
    else if(handledTU == "P6H")
	foutput = new TFile("ready/fitResult6.root", "RECREATE");

    int entryNo;
    int outputyr;
    int outputmon;
    int outputday;
    int outputhr;
    int outputmin;
    double outputsec;

    double outputcg04;
    double outputcg06;
    double outputcg16;
    double outputcg0406;

    double outputcge04;
    double outputcge06;
    double outputcge16;
    double outputcge0406;

    double outputmeang04;
    double outputmeang06;
    double outputmeang16;

    double outputstdg04;
    double outputstdg06;
    double outputstdg16;

    double outputest04;
    double outputest06;
    double outputest16;
    double outputest0406;

    TTree* dataTree = new TTree("dataFitting", "dataTree");
    dataTree->Branch("entryNo", &entryNo, "entryNo/I");
    dataTree->Branch("year", &outputyr, "year/I");
    dataTree->Branch("month", &outputmon, "month/I");
    dataTree->Branch("day", &outputday, "day/I");
    dataTree->Branch("hour", &outputhr, "hour/I");
    dataTree->Branch("minute", &outputmin, "minute/I");
    dataTree->Branch("second", &outputsec, "second/D");

    dataTree->Branch("cg04", &outputcg04, "cg04/D");
    dataTree->Branch("cg06", &outputcg06, "cg06/D");
    dataTree->Branch("cg16", &outputcg16, "cg16/D");
    dataTree->Branch("cg0406", &outputcg0406, "cg0406/D");

    dataTree->Branch("cge04", &outputcge04, "cge04/D");
    dataTree->Branch("cge06", &outputcge06, "cge06/D");
    dataTree->Branch("cge16", &outputcge16, "cge16/D");
    dataTree->Branch("cge0406", &outputcge0406, "cge0406/D");

    dataTree->Branch("meang04", &outputmeang04, "meang04/D");
    dataTree->Branch("meang06", &outputmeang06, "meang06/D");
    dataTree->Branch("meang16", &outputmeang16, "meang16/D");

    dataTree->Branch("stdg04", &outputstdg04, "stdg04/D");
    dataTree->Branch("stdg06", &outputstdg06, "stdg06/D");
    dataTree->Branch("stdg16", &outputstdg16, "stdg16/D");

    dataTree->Branch("est04", &outputest04, "est04/D");
    dataTree->Branch("est06", &outputest06, "est06/D");
    dataTree->Branch("est16", &outputest16, "est16/D");
    dataTree->Branch("est0406", &outputest0406, "est0406/D");

    map<string, double> mapcg04;
    map<string, double> mapcg06;
    map<string, double> mapcg16;

    map<string, double> mapcge04;
    map<string, double> mapcge06;
    map<string, double> mapcge16;

    map<string, double> mapmeang04;
    map<string, double> mapmeang06;
    map<string, double> mapmeang16;

    map<string, double> mapstdg04;
    map<string, double> mapstdg06;
    map<string, double> mapstdg16;

    map<string, double> mapest04;
    map<string, double> mapest06;
    map<string, double> mapest16;

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

    for(unsigned int iTU = 0; iTU < timeUnitList.size(); iTU++) {
	TFile* file = nullptr;
	string thisTU = timeUnitList[iTU];
	if(handledTU != thisTU)
	    continue;

	if(thisTU == "PT")
	    continue;
	else if(thisTU == "P2H")
	    file = new TFile("ready/0418to0607aHistP02h.root", "READ");
	else if(thisTU == "P6H")
	    file = new TFile("ready/0418to0607aHistP06h.root", "READ");
	else
	    continue;

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

		string strForTitle = "";
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
			string tag = thisTerm + "," + thisPeak + "," + thisTU;

			double keyValue = 0.;
			double keyError = 0.;
			if(thisTerm == "cGauss") {
			    keyValue = pf->getAssignedValue("cGauss", 0);
			    keyError = pf->getAssignedError("cGauss", 0);
			} else if(thisTerm == "mean") {
			    keyValue = pf->getAssignedValue("mean", 0);
			} else if(thisTerm == "std") {
			    keyValue = pf->getAssignedValue("std", 0);
			} else if(thisTerm == "EstEvent") {
			    keyValue = pf->getAssignedValue("cGauss", 0);
			    keyError = pf->getAssignedError("cGauss", 0);

			    double binWidth = pf->getHistogram()->GetBinWidth(0);

			    keyValue /= binWidth;
			    keyError /= binWidth;
			}

			if(keyError > 1e3)
			    keyError = 0.;

			double keyEstEvent = pf->getEstEvent();
			string dtStr = thisDateTime->getDateTime();
			if(thisTerm == "cGauss") {
			    if(thisPeak == "peak04") {
				mapcg04[dtStr] = keyValue;
				mapcge04[dtStr] = keyError;
			    } else if(thisPeak == "peak06") {
				mapcg06[dtStr] = keyValue;
				mapcge06[dtStr] = keyError;
			    } else if(thisPeak == "peak16") {
				mapcg16[dtStr] = keyValue;
				mapcge16[dtStr] = keyError;
			    }
			} else if(thisTerm == "mean") {
			    if(thisPeak == "peak04") {
				mapmeang04[dtStr] = keyValue;
			    } else if(thisPeak == "peak06") {
				mapmeang06[dtStr] = keyValue;
			    } else if(thisPeak == "peak16") {
				mapmeang16[dtStr] = keyValue;
			    }
			} else if(thisTerm == "std") {
			    if(thisPeak == "peak04") {
				mapstdg04[dtStr] = keyValue;
			    } else if(thisPeak == "peak06") {
				mapstdg06[dtStr] = keyValue;
			    } else if(thisPeak == "peak16") {
				mapstdg16[dtStr] = keyValue;
			    }
			} else if(thisTerm == "EstEvent") {
			    if(thisPeak == "peak04") {
				mapest04[dtStr] = keyValue;
			    } else if(thisPeak == "peak06") {
				mapest06[dtStr] = keyValue;
			    } else if(thisPeak == "peak16") {
				mapest16[dtStr] = keyValue;
			    }
			}
		    }

		    delete pf;
		}

		delete thisDateTime;
	    }
	}

	if(file == nullptr) {
	    file->Close();
	    delete file;
	}
    }

    foutput->cd();
    for(map<string, double>::iterator it = mapcg16.begin(); it != mapcg16.end(); ++it) {
	string tag = it->first;
	Calendar* dt = new Calendar(tag);
	static int ientry = 0;
	entryNo = ientry;
	outputyr = dt->getYear();
	outputmon = dt->getMonth();
	outputday = dt->getMDay();
	outputhr = dt->getHour();
	outputmin = dt->getMinute();
	outputsec = dt->getSecond();

	outputcg04 = mapcg04[tag];
	outputcg06 = mapcg06[tag];
	outputcg16 = mapcg16[tag];
	outputcg0406 = mapcg04[tag] + mapcg06[tag];

	outputcge04 = mapcge04[tag];
	outputcge06 = mapcge06[tag];
	outputcge16 = mapcge16[tag];
	outputcge0406 = TMath::Sqrt(mapcge04[tag]*mapcge04[tag] + mapcge06[tag]*mapcge06[tag]);

	outputmeang04 = mapmeang04[tag];
	outputmeang06 = mapmeang06[tag];
	outputmeang16 = mapmeang16[tag];

	outputstdg04 = mapstdg04[tag];
	outputstdg06 = mapstdg06[tag];
	outputstdg16 = mapstdg16[tag];

	outputest04 = mapest04[tag];
	outputest06 = mapest06[tag];
	outputest16 = mapest16[tag];
	outputest0406 = mapest04[tag] + mapest06[tag];

	dataTree->Fill();

	ientry++;
	delete dt;
    }

    dataTree->Write();

    delete dataTree;

    if(foutput != nullptr) {
	foutput->Close();
	delete foutput;
    }
}



void TimeAnalysis::analyzeByCounting() {
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
    double outcounts0to25;
    double outcounts0406;

    double outcountse04;
    double outcountse06;
    double outcountse16;
    double outcountse0to25;
    double outcountse0406;

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
    dataTree->Branch("counts0to25", &outcounts0to25, "counts0to25/D");
    dataTree->Branch("counts0406", &outcounts0406, "counts0406/D");

    dataTree->Branch("countse04", &outcountse04, "countse04/D");
    dataTree->Branch("countse06", &outcountse06, "countse06/D");
    dataTree->Branch("countse16", &outcountse16, "countse16/D");
    dataTree->Branch("countse0to25", &outcountse0to25, "countse0to25/D");
    dataTree->Branch("countse0406", &outcountse0406, "countse0406/D");

    map<string, double> mapCounts04;
    map<string, double> mapCounts06;
    map<string, double> mapCounts16;
    map<string, double> mapCounts0to25;

    vector<string> energyRange;
    vector<string> timeUnitList;

    energyRange.push_back("peak06");
    energyRange.push_back("peak16");
    energyRange.push_back("peak04");
    //energyRange.push_back("peak24");
    energyRange.push_back("0to25");
    //energyRange.push_back("peak04n06");

    timeUnitList.push_back("PD");
    timeUnitList.push_back("P12H");
    timeUnitList.push_back("P6H");
    timeUnitList.push_back("P2H");
    timeUnitList.push_back("P1H");

    for(unsigned int iTU = 0; iTU < timeUnitList.size(); iTU++) {
	TFile* file = nullptr;
	string thisTU = timeUnitList[iTU];
	if(thisTU == "PT")
	    continue;
	else if(thisTU == "P2H")
	    file = new TFile("ready/0418to0607aHistP02hD.root");
	else
	    continue;

	for(unsigned int iER = 0; iER < energyRange.size(); iER++) {
	    string thisER = energyRange[iER];
	    string tag = thisER + "," + thisTU;

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
		TObject* objTime = nullptr;
		while((objTime = iterTime())) {
		    Calendar* thisDateTime = new Calendar((string)(objDate->GetName()) + (string)(objTime->GetName()));

		    TH1D* thisH = (TH1D*)dirDate->Get(objTime->GetName());
		    double keyValue = thisH->GetEntries();

		    if(thisTU == "P2H") {
			string dtStr = thisDateTime->getDateTime();
			if(thisER == "peak04")
			    mapCounts04[dtStr] = keyValue;
			else if(thisER == "peak06")
			    mapCounts06[dtStr] = keyValue;
			else if(thisER == "peak16")
			    mapCounts16[dtStr] = keyValue;
			else if(thisER == "0to25")
			    mapCounts0to25[dtStr] = keyValue;
		    }

		    delete thisDateTime;
		}
	    }
	}

	if(file == nullptr) {
	    file->Close();
	    delete file;
	}
    }

    foutput->cd();
    for(map<string, double>::iterator it = mapCounts16.begin(); it != mapCounts16.end(); ++it) {
	string tag = it->first;
	Calendar* dt = new Calendar(tag);
	static int ientry = 0;
	entryNo = ientry;
	outputyr = dt->getYear();
	outputmon = dt->getMonth();
	outputday = dt->getMDay();
	outputhr = dt->getHour();
	outputmin = dt->getMinute();
	outputsec = dt->getSecond();

	outcounts04 = mapCounts04[tag];
	outcounts06 = mapCounts06[tag];
	outcounts16 = mapCounts16[tag];
	outcounts0to25 = mapCounts0to25[tag];
	outcounts0406 = mapCounts04[tag] + mapCounts06[tag];

	outcountse04 = TMath::Sqrt(mapCounts04[tag]);
	outcountse06 = TMath::Sqrt(mapCounts06[tag]);
	outcountse16 = TMath::Sqrt(mapCounts16[tag]);
	outcountse0to25 = TMath::Sqrt(mapCounts0to25[tag]);
	outcountse0406 = TMath::Sqrt(mapCounts04[tag] + mapCounts06[tag]);

	dataTree->Fill();

	ientry++;
	delete dt;
    }

    dataTree->Write();
    delete dataTree;

    foutput->Close();
    delete foutput;
}



void TimeAnalysis::normalizeFittingResult() {
    TFile* finput = nullptr;
    if(handledTU == "P2H")
	finput = new TFile("ready/TempCorrectFitResult.root", "READ");
    else if(handledTU == "P6H")
	finput = new TFile("ready/TempCorrectFitResult6.root", "READ");

    TTree* inTree;
    int entryNo;
    int year;
    int month;
    int day;
    int hour;
    int min;
    double sec;

    double cg04;
    double cg06;
    double cg16;
    double cg0406;

    double cge04;
    double cge06;
    double cge16;
    double cge0406;

    finput->GetObject("dataFitting", inTree);
    inTree->SetBranchAddress("entryNo", &entryNo);
    inTree->SetBranchAddress("year", &year);
    inTree->SetBranchAddress("month", &month);
    inTree->SetBranchAddress("day", &day);
    inTree->SetBranchAddress("hour", &hour);
    inTree->SetBranchAddress("minute", &min);
    inTree->SetBranchAddress("second", &sec);

    inTree->SetBranchAddress("cg04", &cg04);
    inTree->SetBranchAddress("cg06", &cg06);
    inTree->SetBranchAddress("cg16", &cg16);
    inTree->SetBranchAddress("cg0406", &cg0406);

    inTree->SetBranchAddress("cge04", &cge04);
    inTree->SetBranchAddress("cge06", &cge06);
    inTree->SetBranchAddress("cge16", &cge16);
    inTree->SetBranchAddress("cge0406", &cge0406);

    TFile* foutput = nullptr;
    if(handledTU == "P2H")
	foutput = new TFile("ready/NormalizedFitResult.root", "RECREATE");
    else if(handledTU == "P6H")
	foutput = new TFile("ready/NormalizedFitResult6.root", "RECREATE");

    int outEntryNo;
    int outyear;
    int outmonth;
    int outday;
    int outhour;
    int outminute;
    double outsecond;

    double outcgn04;
    double outcgn06;
    double outcgn16;
    double outcgn0406;

    double outcgne04;
    double outcgne06;
    double outcgne16;
    double outcgne0406;

    bool outisoutlier04;
    bool outisoutlier06;
    bool outisoutlier16;
    bool outisoutlier0406;

    map<Long64_t, bool> mapIsOutlier04;
    map<Long64_t, bool> mapIsOutlier06;
    map<Long64_t, bool> mapIsOutlier16;
    map<Long64_t, bool> mapIsOutlier0406;

    TTree* outTree = new TTree("dataFitting", "dataFitting");
    outTree->Branch("entryNo", &outEntryNo, "entryNo/I");
    outTree->Branch("year", &outyear, "year/I");
    outTree->Branch("month", &outmonth, "month/I");
    outTree->Branch("day", &outday, "day/I");
    outTree->Branch("hour", &outhour, "hour/I");
    outTree->Branch("minute", &outminute, "minute/I");
    outTree->Branch("second", &outsecond, "second/D");

    outTree->Branch("cgn04", &outcgn04, "cgn04/D");
    outTree->Branch("cgn06", &outcgn06, "cgn06/D");
    outTree->Branch("cgn16", &outcgn16, "cgn16/D");
    outTree->Branch("cgn0406", &outcgn0406, "cgn0406/D");

    outTree->Branch("cgne04", &outcgne04, "cgne04/D");
    outTree->Branch("cgne06", &outcgne06, "cgne06/D");
    outTree->Branch("cgne16", &outcgne16, "cgne16/D");
    outTree->Branch("cgne0406", &outcgne0406, "cgne0406/D");

    outTree->Branch("isoutlier04", &outisoutlier04, "isoutlier04/O");
    outTree->Branch("isoutlier06", &outisoutlier06, "isoutlier06/O");
    outTree->Branch("isoutlier16", &outisoutlier16, "isoutlier16/O");
    outTree->Branch("isoutlier0406", &outisoutlier0406, "isoutlier0406/O");

    Long64_t nentries = inTree->GetEntries();
    double meancg04 = 0.;
    double stdcg04 = 0.;
    double meancg06 = 0.;
    double stdcg06 = 0.;
    double meancg16 = 0.;
    double stdcg16 = 0.;
    double meancg0406 = 0.;
    double stdcg0406 = 0.;
    for(Long64_t entry = 0; entry < nentries; ++entry) {
	inTree->GetEntry(entry);
	meancg04 += cg04;
	stdcg04 += cg04*cg04;
	meancg06 += cg06;
	stdcg06 += cg06*cg06;
	meancg16 += cg16;
	stdcg16 += cg16*cg16;
	meancg0406 += cg0406;
	stdcg0406 += cg0406*cg0406;
    }

    meancg04 /= nentries;
    meancg06 /= nentries;
    meancg16 /= nentries;
    meancg0406 /= nentries;

    stdcg04 -= nentries*meancg04*meancg04;
    stdcg06 -= nentries*meancg06*meancg06;
    stdcg16 -= nentries*meancg16*meancg16;
    stdcg0406 -= nentries*meancg0406*meancg0406;
    stdcg04 = TMath::Sqrt(stdcg04/(nentries - 1));
    stdcg06 = TMath::Sqrt(stdcg06/(nentries - 1));
    stdcg16 = TMath::Sqrt(stdcg16/(nentries - 1));
    stdcg0406 = TMath::Sqrt(stdcg0406/(nentries - 1));

    double width = 2.;
    double width16 = width;
    if(handledTU == "P6H")
	width16 = 1.5;

    int nentriescg04n = 0;
    double meancg04n = 0.;
    double stdcg04n = 0.;
    int nentriescg06n = 0;
    double meancg06n = 0.;
    double stdcg06n = 0.;
    int nentriescg16n = 0;
    double meancg16n = 0.;
    double stdcg16n = 0.;
    int nentriescg0406n = 0;
    double meancg0406n = 0.;
    double stdcg0406n = 0.;

    for(Long64_t entry = 0; entry < nentries; ++entry) {
	inTree->GetEntry(entry);

	if(cg04 >= meancg04 - width*stdcg04 &&
	   cg04 <= meancg04 + width*stdcg04) {
	    meancg04n += cg04;
	    stdcg04n += cg04*cg04;
	    mapIsOutlier04[entry] = false;
	    nentriescg04n++;
	} else
	    mapIsOutlier04[entry] = true;

	if(cg06 >= meancg06 - width*stdcg06 &&
	   cg06 <= meancg06 + width*stdcg06) {
	    meancg06n += cg06;
	    stdcg06n += cg06*cg06;
	    mapIsOutlier06[entry] = false;
	    nentriescg06n++;
	} else
	    mapIsOutlier06[entry] = true;

	if(cg16 >= meancg16 - width16*stdcg16 &&
	   cg16 <= meancg16 + width16*stdcg16) {
	    meancg16n += cg16;
	    stdcg16n += cg16*cg16;
	    mapIsOutlier16[entry] = false;
	    nentriescg16n++;
	} else
	    mapIsOutlier16[entry] = true;

	if(cg0406 >= meancg0406 - width*stdcg0406 &&
	   cg0406 <= meancg0406 + width*stdcg0406) {
	    meancg0406n += cg0406;
	    stdcg0406n += cg0406*cg0406;
	    mapIsOutlier0406[entry] = false;
	    nentriescg0406n++;
	} else
	    mapIsOutlier0406[entry] = true;
    }

    meancg04n /= nentriescg04n;
    meancg06n /= nentriescg06n;
    meancg16n /= nentriescg16n;
    meancg0406n /= nentriescg0406n;

    stdcg04n -= nentriescg04n*meancg04n*meancg04n;
    stdcg06n -= nentriescg06n*meancg06n*meancg06n;
    stdcg16n -= nentriescg16n*meancg16n*meancg16n;
    stdcg0406n -= nentriescg0406n*meancg0406n*meancg0406n;
    stdcg04n = TMath::Sqrt(stdcg04n/(nentriescg04n - 1));
    stdcg06n = TMath::Sqrt(stdcg06n/(nentriescg06n - 1));
    stdcg16n = TMath::Sqrt(stdcg16n/(nentriescg16n - 1));
    stdcg0406n = TMath::Sqrt(stdcg0406n/(nentriescg0406n - 1));

    double errcg04n = stdcg04n/TMath::Sqrt(meancg04n);
    double errcg06n = stdcg06n/TMath::Sqrt(meancg06n);
    double errcg16n = stdcg16n/TMath::Sqrt(meancg16n);
    double errcg0406n = stdcg0406n/TMath::Sqrt(meancg0406n);

    for(Long64_t entry = 0; entry < nentries; ++entry) {
	inTree->GetEntry(entry);
	outEntryNo = entryNo;
	outyear = year;
	outmonth = month;
	outday = day;
	outhour = hour;
	outminute = min;
	outsecond = sec;

	outcgn04 = cg04/meancg04n;
	outcgn06 = cg06/meancg06n;
	outcgn16 = cg16/meancg16n;
	outcgn0406 = cg0406/meancg0406n;

	outcgne04 = TMath::Sqrt((errcg04n*errcg04n)/(meancg04n*meancg04n)
				+ (cge04*cge04)/(cg04*cg04));
	outcgne06 = TMath::Sqrt((errcg06n*errcg06n)/(meancg06n*meancg06n)
				+ (cge06*cge06)/(cg06*cg06));
	outcgne16 = TMath::Sqrt((errcg16n*errcg16n)/(meancg16n*meancg16n)
				+ (cge16*cge16)/(cg16*cg16));
	outcgne0406 = TMath::Sqrt((errcg0406n*errcg0406n)/(meancg0406n*meancg0406n)
				  + (cge0406*cge0406)/(cg0406*cg0406));

	outisoutlier04 = mapIsOutlier04[entry];
	outisoutlier06 = mapIsOutlier06[entry];
	outisoutlier16 = mapIsOutlier16[entry];
	outisoutlier0406 = mapIsOutlier0406[entry];

	outTree->Fill();
    }

    outTree->Write();
    delete outTree;

    if(foutput != nullptr) {
	foutput->Close();
	delete foutput;
    }

    if(foutput != nullptr) {
	finput->Close();
	delete finput;
    }
}



void TimeAnalysis::normalizeCountingResult() {
    //TFile* finput = new TFile("ready/TempCorrectFluctResult.root", "READ");
    TFile* finput = new TFile("ready/fluctResult.root", "READ");
    TTree* inTree;
    int entryNo;
    int year;
    int month;
    int day;
    int hour;
    int min;
    double sec;

    double counts04;
    double counts06;
    double counts16;
    double counts0to25;
    double counts0406;

    double countse04;
    double countse06;
    double countse16;
    double countse0to25;
    double countse0406;

    finput->GetObject("dataCounts", inTree);
    inTree->SetBranchAddress("entryNo", &entryNo);
    inTree->SetBranchAddress("year", &year);
    inTree->SetBranchAddress("month", &month);
    inTree->SetBranchAddress("day", &day);
    inTree->SetBranchAddress("hour", &hour);
    inTree->SetBranchAddress("minute", &min);
    inTree->SetBranchAddress("second", &sec);

    inTree->SetBranchAddress("counts04", &counts04);
    inTree->SetBranchAddress("counts06", &counts06);
    inTree->SetBranchAddress("counts16", &counts16);
    inTree->SetBranchAddress("counts0to25", &counts0to25);
    inTree->SetBranchAddress("counts0406", &counts0406);

    inTree->SetBranchAddress("countse04", &countse04);
    inTree->SetBranchAddress("countse06", &countse06);
    inTree->SetBranchAddress("countse16", &countse16);
    inTree->SetBranchAddress("countse0to25", &countse0to25);
    inTree->SetBranchAddress("countse0406", &countse0406);

    TFile* foutput = new TFile("ready/NormalizedFluctResult.root", "RECREATE");
    int outEntryNo;
    int outyear;
    int outmonth;
    int outday;
    int outhour;
    int outminute;
    double outsecond;

    double outcountsn04;
    double outcountsn06;
    double outcountsn16;
    double outcountsn0to25;
    double outcountsn0406;

    double outcountsne04;
    double outcountsne06;
    double outcountsne16;
    double outcountsne0to25;
    double outcountsne0406;

    bool outisoutlier04;
    bool outisoutlier06;
    bool outisoutlier16;
    bool outisoutlier0to25;
    bool outisoutlier0406;

    map<Long64_t, bool> mapIsOutlier04;
    map<Long64_t, bool> mapIsOutlier06;
    map<Long64_t, bool> mapIsOutlier16;
    map<Long64_t, bool> mapIsOutlier0to25;
    map<Long64_t, bool> mapIsOutlier0406;

    TTree* outTree = new TTree("dataCounts", "dataCounts");
    outTree->Branch("entryNo", &outEntryNo, "entryNo/I");
    outTree->Branch("year", &outyear, "year/I");
    outTree->Branch("month", &outmonth, "month/I");
    outTree->Branch("day", &outday, "day/I");
    outTree->Branch("hour", &outhour, "hour/I");
    outTree->Branch("minute", &outminute, "minute/I");
    outTree->Branch("second", &outsecond, "second/D");

    outTree->Branch("countsn04", &outcountsn04, "countsn04/D");
    outTree->Branch("countsn06", &outcountsn06, "countsn06/D");
    outTree->Branch("countsn16", &outcountsn16, "countsn16/D");
    outTree->Branch("countsn0to25", &outcountsn0to25, "countsn0to25/D");
    outTree->Branch("countsn0406", &outcountsn0406, "countsn0406/D");

    outTree->Branch("countsne04", &outcountsne04, "countsne04/D");
    outTree->Branch("countsne06", &outcountsne06, "countsne06/D");
    outTree->Branch("countsne16", &outcountsne16, "countsne16/D");
    outTree->Branch("countsne0to25", &outcountsne0to25, "countsne0to25/D");
    outTree->Branch("countsne0406", &outcountsne0406, "countsne0406/D");

    outTree->Branch("isoutlier04", &outisoutlier04, "isoutlier04/O");
    outTree->Branch("isoutlier06", &outisoutlier06, "isoutlier06/O");
    outTree->Branch("isoutlier16", &outisoutlier16, "isoutlier16/O");
    outTree->Branch("isoutlier0to25", &outisoutlier0to25, "isoutlier0to25/O");
    outTree->Branch("isoutlier0406", &outisoutlier0406, "isoutlier0406/O");

    string thisTU = "P2H";
    Long64_t nentries = inTree->GetEntries();
    double meancounts04 = 0.;
    double stdcounts04 = 0.;
    double meancounts06 = 0.;
    double stdcounts06 = 0.;
    double meancounts16 = 0.;
    double stdcounts16 = 0.;
    double meancounts0to25 = 0.;
    double stdcounts0to25 = 0.;
    double meancounts0406 = 0.;
    double stdcounts0406 = 0.;
    for(Long64_t entry = 0; entry < nentries; ++entry) {
	inTree->GetEntry(entry);
	meancounts04 += counts04;
	stdcounts04 += counts04*counts04;
	meancounts06 += counts06;
	stdcounts06 += counts06*counts06;
	meancounts16 += counts16;
	stdcounts16 += counts16*counts16;
	meancounts0to25 += counts0to25;
	stdcounts0to25 += counts0to25*counts0to25;
	meancounts0406 += counts0406;
	stdcounts0406 += counts0406*counts0406;
    }

    meancounts04 /= nentries;
    meancounts06 /= nentries;
    meancounts16 /= nentries;
    meancounts0to25 /= nentries;
    meancounts0406 /= nentries;

    stdcounts04 -= nentries*meancounts04*meancounts04;
    stdcounts06 -= nentries*meancounts06*meancounts06;
    stdcounts16 -= nentries*meancounts16*meancounts16;
    stdcounts0to25 -= nentries*meancounts0to25*meancounts0to25;
    stdcounts0406 -= nentries*meancounts0406*meancounts0406;
    stdcounts04 = TMath::Sqrt(stdcounts04/(nentries - 1));
    stdcounts06 = TMath::Sqrt(stdcounts06/(nentries - 1));
    stdcounts16 = TMath::Sqrt(stdcounts16/(nentries - 1));
    stdcounts0to25 = TMath::Sqrt(stdcounts0to25/(nentries - 1));
    stdcounts0406 = TMath::Sqrt(stdcounts0406/(nentries - 1));

    double width = 2.;

    int nentriescounts04n = 0;
    double meancounts04n = 0.;
    double stdcounts04n = 0.;
    int nentriescounts06n = 0;
    double meancounts06n = 0.;
    double stdcounts06n = 0.;
    int nentriescounts16n = 0;
    double meancounts16n = 0.;
    double stdcounts16n = 0.;
    int nentriescounts0to25n = 0;
    double meancounts0to25n = 0.;
    double stdcounts0to25n = 0.;
    int nentriescounts0406n = 0;
    double meancounts0406n = 0.;
    double stdcounts0406n = 0.;

    for(Long64_t entry = 0; entry < nentries; ++entry) {
	inTree->GetEntry(entry);

	if(counts04 >= meancounts04 - width*stdcounts04 &&
	   counts04 <= meancounts04 + width*stdcounts04) {
	    meancounts04n += counts04;
	    stdcounts04n += counts04*counts04;
	    mapIsOutlier04[entry] = false;
	    nentriescounts04n++;
	} else
	    mapIsOutlier04[entry] = true;

	if(counts06 >= meancounts06 - width*stdcounts06 &&
	   counts06 <= meancounts06 + width*stdcounts06) {
	    meancounts06n += counts06;
	    stdcounts06n += counts06*counts06;
	    mapIsOutlier06[entry] = false;
	    nentriescounts06n++;
	} else
	    mapIsOutlier06[entry] = true;

	if(counts16 >= meancounts16 - width*stdcounts16 &&
	   counts16 <= meancounts16 + width*stdcounts16) {
	    meancounts16n += counts16;
	    stdcounts16n += counts16*counts16;
	    mapIsOutlier16[entry] = false;
	    nentriescounts16n++;
	} else
	    mapIsOutlier16[entry] = true;

	if(counts0to25 >= meancounts0to25 - width*stdcounts0to25 &&
	   counts0to25 <= meancounts0to25 + width*stdcounts0to25) {
	    meancounts0to25n += counts0to25;
	    stdcounts0to25n += counts0to25*counts0to25;
	    mapIsOutlier0to25[entry] = false;
	    nentriescounts0to25n++;
	} else
	    mapIsOutlier0to25[entry] = true;

	if(counts0406 >= meancounts0406 - width*stdcounts0406 &&
	   counts0406 <= meancounts0406 + width*stdcounts0406) {
	    meancounts0406n += counts0406;
	    stdcounts0406n += counts0406*counts0406;
	    mapIsOutlier0406[entry] = false;
	    nentriescounts0406n++;
	} else
	    mapIsOutlier0406[entry] = true;
    }

    meancounts04n /= nentriescounts04n;
    meancounts06n /= nentriescounts06n;
    meancounts16n /= nentriescounts16n;
    meancounts0to25n /= nentriescounts0to25n;
    meancounts0406n /= nentriescounts0406n;

    stdcounts04n -= nentriescounts04n*meancounts04n*meancounts04n;
    stdcounts06n -= nentriescounts06n*meancounts06n*meancounts06n;
    stdcounts16n -= nentriescounts16n*meancounts16n*meancounts16n;
    stdcounts0to25n -= nentriescounts0to25n*meancounts0to25n*meancounts0to25n;
    stdcounts0406n -= nentriescounts0406n*meancounts0406n*meancounts0406n;
    stdcounts04n = TMath::Sqrt(stdcounts04n/(nentriescounts04n - 1));
    stdcounts06n = TMath::Sqrt(stdcounts06n/(nentriescounts06n - 1));
    stdcounts16n = TMath::Sqrt(stdcounts16n/(nentriescounts16n - 1));
    stdcounts0to25n = TMath::Sqrt(stdcounts0to25n/(nentriescounts0to25n - 1));
    stdcounts0406n = TMath::Sqrt(stdcounts0406n/(nentriescounts0406n - 1));

    double errcounts04n = stdcounts04n/TMath::Sqrt(meancounts04n);
    double errcounts06n = stdcounts06n/TMath::Sqrt(meancounts06n);
    double errcounts16n = stdcounts16n/TMath::Sqrt(meancounts16n);
    double errcounts0to25n = stdcounts0to25n/TMath::Sqrt(meancounts0to25n);
    double errcounts0406n = stdcounts0406n/TMath::Sqrt(meancounts0406n);

    for(Long64_t entry = 0; entry < nentries; ++entry) {
	inTree->GetEntry(entry);
	outEntryNo = entryNo;
	outyear = year;
	outmonth = month;
	outday = day;
	outhour = hour;
	outminute = min;
	outsecond = sec;

	outcountsn04 = counts04/meancounts04n;
	outcountsn06 = counts06/meancounts06n;
	outcountsn16 = counts16/meancounts16n;
	outcountsn0to25 = counts0to25/meancounts0to25n;
	outcountsn0406 = counts0406/meancounts0406n;

	outcountsne04 = TMath::Sqrt((errcounts04n*errcounts04n)/(meancounts04n*meancounts04n) + (countse04*countse04)/(counts04*counts04));
	outcountsne06 = TMath::Sqrt((errcounts06n*errcounts06n)/(meancounts06n*meancounts06n) + (countse06*countse06)/(counts06*counts06));
	outcountsne16 = TMath::Sqrt((errcounts16n*errcounts16n)/(meancounts16n*meancounts16n) + (countse16*countse16)/(counts16*counts16));
	outcountsne0to25 = TMath::Sqrt((errcounts0to25n*errcounts0to25n)/(meancounts0to25n*meancounts0to25n) + (countse0to25*countse0to25)/(counts0to25*counts0to25));
	outcountsne0406 = TMath::Sqrt((errcounts0406n*errcounts0406n)/(meancounts0406n*meancounts0406n) + (countse0406*countse0406)/(counts0406*counts0406));

	outisoutlier04 = mapIsOutlier04[entry];
	outisoutlier06 = mapIsOutlier06[entry];
	outisoutlier16 = mapIsOutlier16[entry];
	outisoutlier0to25 = mapIsOutlier0to25[entry];
	outisoutlier0406 = mapIsOutlier0406[entry];

	outTree->Fill();
    }

    outTree->Write();
    delete outTree;

    foutput->Close();
    delete foutput;

    finput->Close();
    delete finput;
}



void TimeAnalysis::correctFittingAnaByTemp() {
    TempHumi* th = new TempHumi();
    TFile* finput = nullptr;
    if(handledTU == "P2H")
	finput = new TFile("ready/fitResult.root", "READ");
    else if(handledTU == "P6H")
	finput = new TFile("ready/fitResult6.root", "READ");

    TTree* inTree;
    int entryNo;
    int year;
    int month;
    int day;
    int hour;
    int min;
    double sec;

    double cg04;
    double cg06;
    double cg16;
    double cg0406;

    double cge04;
    double cge06;
    double cge16;
    double cge0406;

    finput->GetObject("dataFitting", inTree);
    inTree->SetBranchAddress("entryNo", &entryNo);
    inTree->SetBranchAddress("year", &year);
    inTree->SetBranchAddress("month", &month);
    inTree->SetBranchAddress("day", &day);
    inTree->SetBranchAddress("hour", &hour);
    inTree->SetBranchAddress("minute", &min);
    inTree->SetBranchAddress("second", &sec);

    inTree->SetBranchAddress("cg04", &cg04);
    inTree->SetBranchAddress("cg06", &cg06);
    inTree->SetBranchAddress("cg16", &cg16);
    inTree->SetBranchAddress("cg0406", &cg0406);

    inTree->SetBranchAddress("cge04", &cge04);
    inTree->SetBranchAddress("cge06", &cge06);
    inTree->SetBranchAddress("cge16", &cge16);
    inTree->SetBranchAddress("cge0406", &cge0406);

    TFile* foutput = nullptr;
    if(handledTU == "P2H")
	foutput = new TFile("ready/TempCorrectFitResult.root", "RECREATE");
    else if(handledTU == "P6H")
	foutput = new TFile("ready/TempCorrectFitResult6.root", "RECREATE");

    int outEntryNo;
    int outyear;
    int outmonth;
    int outday;
    int outhour;
    int outminute;
    double outsecond;

    double outcg04;
    double outcg06;
    double outcg16;
    double outcg0406;

    double outcge04;
    double outcge06;
    double outcge16;
    double outcge0406;

    TTree* outTree = new TTree("dataFitting", "dataFitting");
    outTree->Branch("entryNo", &outEntryNo, "entryNo/I");
    outTree->Branch("year", &outyear, "year/I");
    outTree->Branch("month", &outmonth, "month/I");
    outTree->Branch("day", &outday, "day/I");
    outTree->Branch("hour", &outhour, "hout/I");
    outTree->Branch("minute", &outminute, "minute/I");
    outTree->Branch("second", &outsecond, "second/D");

    outTree->Branch("cg04", &outcg04, "cg04/D");
    outTree->Branch("cg06", &outcg06, "cg06/D");
    outTree->Branch("cg16", &outcg16, "cg16/D");
    outTree->Branch("cg0406", &outcg0406, "cg0406/D");

    outTree->Branch("cge04", &outcge04, "cge04/D");
    outTree->Branch("cge06", &outcge06, "cge06/D");
    outTree->Branch("cge16", &outcge16, "cge16/D");
    outTree->Branch("cge0406", &outcge0406, "cge0406/D");

    int nentries = inTree->GetEntries();
    for(Long64_t entry = 0; entry < nentries; ++entry) {
	inTree->GetEntry(entry);
	Calendar* dt = new Calendar(year, month, day, hour, min, sec);

	if(handledTU == "P1H")
	    dt->addDuration(0, 0, 0, 30, 0.);
	else
	    dt->addDuration(0, 0, (int)takeTimeLength(handledTU), 0, 0.);

	double thisTemp = th->getAvgTemp(dt->getDateTime());
	double errTemp = th->getErrTemp(dt->getDateTime());

	outEntryNo = entryNo;
	outyear = year;
	outmonth = month;
	outday = day;
	outhour = hour;
	outminute = min;
	outsecond = sec;

	outcg04 = correctByTemp("fitting", "peak04", handledTU, cg04, thisTemp);
	outcg06 = correctByTemp("fitting", "peak06", handledTU, cg06, thisTemp);
	outcg16 = correctByTemp("fitting", "peak16", handledTU, cg16, thisTemp);
	outcg0406 = correctByTemp("fitting", "peak0406", handledTU, cg0406, thisTemp);

	outcge04 = calErrAfterCorrect("fitting", "peak04", handledTU, cge04, thisTemp, errTemp);
	outcge06 = calErrAfterCorrect("fitting", "peak06", handledTU, cge06, thisTemp, errTemp);
	outcge16 = calErrAfterCorrect("fitting", "peak16", handledTU, cge16, thisTemp, errTemp);
	outcge0406 = calErrAfterCorrect("fitting", "peak0406", handledTU, cge0406, thisTemp, errTemp);

	outTree->Fill();

	delete dt;
    }

    outTree->Write();
    delete outTree;

    if(foutput != nullptr) {
	foutput->Close();
	delete foutput;
    }

    if(finput != nullptr) {
	finput->Close();
	delete finput;
    }

    delete th;
}



void TimeAnalysis::correctCountingAnaByTemp() {
    TempHumi* th = new TempHumi();
    TFile* finput = new TFile("ready/fluctResult.root", "READ");
    TTree* inTree;
    int entryNo;
    int year;
    int month;
    int day;
    int hour;
    int min;
    double sec;

    double counts04;
    double counts06;
    double counts16;
    double counts0to25;
    double counts0406;

    double countse04;
    double countse06;
    double countse16;
    double countse0to25;
    double countse0406;

    finput->GetObject("dataCounts", inTree);
    inTree->SetBranchAddress("entryNo", &entryNo);
    inTree->SetBranchAddress("year", &year);
    inTree->SetBranchAddress("month", &month);
    inTree->SetBranchAddress("day", &day);
    inTree->SetBranchAddress("hour", &hour);
    inTree->SetBranchAddress("minute", &min);
    inTree->SetBranchAddress("second", &sec);

    inTree->SetBranchAddress("counts04", &counts04);
    inTree->SetBranchAddress("counts06", &counts06);
    inTree->SetBranchAddress("counts16", &counts16);
    inTree->SetBranchAddress("counts0to25", &counts0to25);
    inTree->SetBranchAddress("counts0406", &counts0406);

    inTree->SetBranchAddress("countse04", &countse04);
    inTree->SetBranchAddress("countse06", &countse06);
    inTree->SetBranchAddress("countse16", &countse16);
    inTree->SetBranchAddress("countse0to25", &countse0to25);
    inTree->SetBranchAddress("countse0406", &countse0406);

    TFile* foutput = new TFile("ready/TempCorrectFluctResult.root", "RECREATE");
    int outEntryNo;
    int outyear;
    int outmonth;
    int outday;
    int outhour;
    int outminute;
    double outsecond;

    double outcounts04;
    double outcounts06;
    double outcounts16;
    double outcounts0to25;
    double outcounts0406;

    double outcountse04;
    double outcountse06;
    double outcountse16;
    double outcountse0to25;
    double outcountse0406;

    TTree* outTree = new TTree("dataCounts", "dataCounts");
    outTree->Branch("entryNo", &outEntryNo, "entryNo/I");
    outTree->Branch("year", &outyear, "year/I");
    outTree->Branch("month", &outmonth, "month/I");
    outTree->Branch("day", &outday, "day/I");
    outTree->Branch("hour", &outhour, "hour/I");
    outTree->Branch("minute", &outminute, "minute/I");
    outTree->Branch("second", &outsecond, "second/D");

    outTree->Branch("counts04", &outcounts04, "counts04/D");
    outTree->Branch("counts06", &outcounts06, "counts06/D");
    outTree->Branch("counts16", &outcounts16, "counts16/D");
    outTree->Branch("counts0to25", &outcounts0to25, "counts0to25/D");
    outTree->Branch("counts0406", &outcounts0406, "counts0406/D");

    outTree->Branch("countse04", &outcountse04, "countse04/D");
    outTree->Branch("countse06", &outcountse06, "countse06/D");
    outTree->Branch("countse16", &outcountse16, "countse16/D");
    outTree->Branch("countse0to25", &outcountse0to25, "countse0to25/D");
    outTree->Branch("countse0406", &outcountse0406, "countse0406/D");

    string thisTU = "P2H";
    int nentries = inTree->GetEntries();
    for(Long64_t entry = 0; entry < nentries; ++entry) {
	inTree->GetEntry(entry);
	Calendar* dt = new Calendar(year, month, day, hour, min, sec);

	if(thisTU == "P1H")
	    dt->addDuration(0, 0, 0, 30, 0.);
	else
	    dt->addDuration(0, 0, (int)takeTimeLength(thisTU), 0, 0.);

	double thisTemp = th->getAvgTemp(dt->getDateTime());
	double errTemp = th->getErrTemp(dt->getDateTime());

	outEntryNo = entryNo;
	outyear = year;
	outmonth = month;
	outday = day;
	outhour = hour;
	outminute = min;
	outsecond = sec;

	outcounts04 = correctByTemp("counting", "peak04", thisTU, counts04, thisTemp);
	outcounts06 = correctByTemp("counting", "peak06", thisTU, counts06, thisTemp);
	outcounts16 = correctByTemp("counting", "peak16", thisTU, counts16, thisTemp);
	outcounts0to25 = correctByTemp("counting", "0to25", thisTU, counts0to25, thisTemp);
	outcounts0406 = correctByTemp("counting", "peak0406", thisTU, counts0406, thisTemp);

	outcountse04 = calErrAfterCorrect("counting", "peak04", thisTU, countse04, thisTemp, errTemp);
	outcountse06 = calErrAfterCorrect("counting", "peak06", thisTU, countse06, thisTemp, errTemp);
	outcountse16 = calErrAfterCorrect("counting", "peak16", thisTU, countse16, thisTemp, errTemp);
	outcountse0to25 = calErrAfterCorrect("counting", "0to25", thisTU, countse0to25, thisTemp, errTemp);
	outcountse0406 = calErrAfterCorrect("counting", "peak0406", thisTU, countse0406, thisTemp, errTemp);

	outTree->Fill();

	delete dt;
    }

    outTree->Write();
    delete outTree;

    foutput->Close();
    delete foutput;
    
    finput->Close();
    delete finput;
    delete th;
}



double TimeAnalysis::correctByTemp(string method, string ER, string timeUnit, double original, double temp) { // ER means energy range
    double p1 = 0.;
    double tempRef = 30.;
    if(method == "fitting") {
	if(ER == "peak04") {
	    if(timeUnit == "P2H")
		p1 = 0.835191;
	    else if(timeUnit == "P6H")
		p1 = 2.729798;
	} else if(ER == "peak06") {
	    if(timeUnit == "P2H")
		p1 = -1.026552;
	    else if(timeUnit == "P6H")
		p1 = -2.622519;
	} else if(ER == "peak16") {
	    if(timeUnit == "P2H")
		p1 = 0.804824;
	    else if(timeUnit == "P6H")
		p1 = 2.844474;
	} else if(ER == "peak0406") {
	    if(timeUnit == "P2H")
		p1 = -0.138124;
	    else if(timeUnit == "P6H")
		p1 = 0.064622;
	}
    } else if(method == "countingU") {
	if(timeUnit == "P2H")
	    p1 = 5223.066190;
    } else if(method == "counting") {
	if(ER == "peak04")
	    p1 = 402.567788;
	else if(ER == "peak06")
	    p1 = 229.276178;
	else if(ER == "peak16")
	    p1 = -205.508207;
	else if(ER == "0to25")
	    p1 = 5223.066190;
	else if(ER == "peak0406")
	    p1 = 640.151121;
    }

    return original - p1*(temp - tempRef);
}



double TimeAnalysis::calErrAfterCorrect(string method, string ER, string timeUnit, double originalErr, double temp, double errTemp) {
    double p1 = 0.;
    double p1Err = 0.;
    if(method == "fitting") {
	if(ER == "peak04") {
	    p1 = 0.835191;
	    p1Err = 0.;
	} else if(ER == "peak06") {
	    p1 = -1.026552;
	    p1Err = 0.;
	} else if(ER == "peak16") {
	    p1 = 0.804824;
	    p1Err = 0.;
	} else if(ER == "peak0406") {
	    p1 = -0.138124;
	    p1Err = 0.;
	}
    } else if(method == "countingU") {
	if(timeUnit == "P2H")
	    p1 = 5223.066190;
    } else if(method == "counting") {
	if(ER == "peak04") {
	    p1 = 402.567788;
	    p1Err = 0.;
	} else if(ER == "peak06") {
	    p1 = 229.276178;
	    p1Err = 0.;
	} else if(ER == "peak16") {
	    p1 = -205.508207;
	    p1Err = 0.;
	} else if(ER == "0to25") {
	    p1 = 5223.066190;
	    p1Err = 0.;
	} else if(ER == "peak0406") {
	    p1 = 640.151121;
	    p1Err = 0.;
	}
    }

    return TMath::Sqrt(originalErr*originalErr + (p1Err*p1Err)/(p1*p1)
		       + (errTemp*errTemp)/(temp*temp));
}



double TimeAnalysis::takeTimeLength(string timeUnit) {
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
