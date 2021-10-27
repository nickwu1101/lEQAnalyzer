#include "TempHumi.h"

#include "Calendar.h"

#include <TAxis.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TMath.h>

#include <dirent.h>
#include <fstream>

TempHumi::TempHumi() {
    startDateTime = new Calendar("20210101000000");
    endDateTime = new Calendar("20210102000000");
    timeLengthOfGraph = "day";
    timeUnit = "1hour";
}



TempHumi::~TempHumi() {
    delete endDateTime;
    delete startDateTime;
}



void TempHumi::execute() {
    assignTimeInterval();
    prepareDataList();
    doTempHumiMap();
    makeTempHumiPlotting();
    printCanvas();
}



void TempHumi::test() {
    ifstream DHTfile;
    DHTfile.open("csv/20210411080000.csv");

    vector<double> measuredTemp;
    vector<double> measuredHumi;

    string line;
    while(DHTfile.is_open()) {
	if(DHTfile.eof()) break;
	getline(DHTfile, line);

	if(line.length() != 0) {
	    string cutLine = line.substr(line.find_first_of(",") + 1);
	    string tempStr = cutLine.substr(0, cutLine.find_first_of(","));
	    string humiStr = cutLine.substr(cutLine.find_first_of(",") + 1);

	    measuredTemp.push_back(atof(tempStr.c_str()));
	    measuredHumi.push_back(atof(humiStr.c_str()));
	}
    }

    double avgTemp = 0;
    double avgHumi = 0;

    for(unsigned int i = 0; i < measuredTemp.size(); i++)
	avgTemp += measuredTemp[i];

    for(unsigned int i = 0; i < measuredHumi.size(); i++)
	avgHumi += measuredHumi[i];

    avgTemp /= (double)measuredTemp.size();
    avgHumi /= (double)measuredHumi.size();

    cout << avgTemp << endl << avgHumi << endl;

    cout << calAvgTemp("20210411080000") << endl << calAvgHumi("20210411080000") << endl;

    prepareDataList();
    for(unsigned int i = 0; i < dataList.size(); i++) {
	cout << dataList[i] << endl;
    }

    cout << "-----------------------------------------------------" << endl;

    setStartDateTime("20210409000000");
    setEndDateTime("20210410000000");
    doTempHumiMap();
    makeTempHumiPlotting();
}



void TempHumi::makeTempHumiPlotting() {
    TFile *f = new TFile("analyzedFile/TempHumi.root", "RECREATE");

    f->mkdir("temperature", "temperature");
    f->mkdir("humidity", "humidity");

    map<string, TGraph*> gT;
    map<string, TGraph*> gH;

    double xtime = 0;
    double ytemp = 0;
    double yhumi = 0;

    for(map<string, double>::iterator it = temp.begin();
	it != temp.end(); ++it) {
	ytemp = it->second;
	yhumi = humi[it->first];

	Calendar* dt = new Calendar(it->first);

	string adding2Graph;

	string xAxisTitle;
	if(timeLengthOfGraph == "day") {
	    xAxisTitle = "Time (h)";
	    adding2Graph = dt->getDate();

	    if(timeUnit == "15min") {
		xtime = (double)dt->getHour() + ((double)dt->getMinute()/60.);
	    } else if(timeUnit == "1hour") {
		if(dt->getMinute() != 0 || dt->getSecond() != 0) {
		    continue;
		} else {
		    xtime = (double)dt->getHour();
		}
	    }
	} else if (timeLengthOfGraph == "month") {
	    adding2Graph = dt->getDate().substr(0, 6);
	} else if (timeLengthOfGraph == "year") {
	    adding2Graph = dt->getDate().substr(0, 4);
	}

	TGraph* thisGT = nullptr;
	TGraph* thisGH = nullptr;
	if(gT.find(adding2Graph) == gT.cend()) {
	    gT[adding2Graph] = new TGraph();
	    thisGT = gT[adding2Graph];

	    char line[100];
	    thisGT->SetName(adding2Graph.c_str());
	    sprintf(line, "Temperature in %s", adding2Graph.c_str());
	    thisGT->SetTitle(line);
	    thisGT->GetXaxis()->SetTitle(xAxisTitle.c_str());
	    thisGT->GetYaxis()->SetTitle("Temperature ({}^{#circ}C)");
	    thisGT->GetYaxis()->SetRangeUser(0., 40.);
	    thisGT->SetLineColor(kRed);
	} else {
	    thisGT = gT[adding2Graph];
	}

	if(gH.find(adding2Graph) == gH.cend()) {
	    gH[adding2Graph] = new TGraph();
	    thisGH = gH[adding2Graph];

	    char line[100];
	    thisGH->SetName(adding2Graph.c_str());
	    sprintf(line, "Humidity in %s", adding2Graph.c_str());
	    thisGH->SetTitle(line);
	    thisGH->GetXaxis()->SetTitle(xAxisTitle.c_str());
	    thisGH->GetYaxis()->SetTitle("Humidity (\%)");
	    thisGH->GetYaxis()->SetRangeUser(0., 100.);
	    thisGH->SetLineColor(kBlue);
	} else {
	    thisGH = gH[adding2Graph];
	}

	thisGT->SetPoint(thisGT->GetN(), xtime, ytemp);
	thisGH->SetPoint(thisGH->GetN(), xtime, yhumi);

	delete dt;
    }

    f->cd("temperature");
    for(map<string, TGraph*>::iterator it = gT.begin();
	it != gT.end(); ++it) {
	it->second->Write();

	delete it->second;
    }

    f->cd("humidity");
    for(map<string, TGraph*>::iterator it = gH.begin();
	it != gH.end(); ++it) {
	it->second->Write();

	delete it->second;
    }

    f->Close();
    delete f;
}



void TempHumi::doTempHumiMap() {
    for(unsigned int iData = 0; iData < dataList.size(); iData++) {
	Calendar* dataDateTime = new Calendar(dataList[iData]);

	if(*dataDateTime >= *startDateTime && *dataDateTime < *endDateTime) {
	    string thisDT = dataDateTime->getDateTime();
	    temp[thisDT] = calAvgTemp(thisDT);
	    humi[thisDT] = calAvgHumi(thisDT);
	}
    }
}



void TempHumi::printCanvas() {
    TCanvas* c = new TCanvas("c", "c", 1400, 800);
    TGraph* gTlong = new TGraph();
    TGraph* gHlong = new TGraph();

    TFile* f = new TFile("analyzedFile/TempHumi.root", "READ");

    map<double, double> temp;
    map<double, double> humi;

    Calendar* startDateTime = nullptr;

    TDirectory* dirTemp = f->GetDirectory("temperature");
    TList* listDateT = dirTemp->GetListOfKeys();
    listDateT->Sort();

    TIter iterDateT(listDateT);
    TObject* objDateT = nullptr;
    while((objDateT = iterDateT())) {
	if(startDateTime == nullptr)
	    startDateTime = new Calendar((string)(objDateT->GetName()) + "000000");

	Calendar* thisDateTime = new Calendar((string)(objDateT->GetName()) + "000000");

	TGraph* gT = (TGraph*)dirTemp->Get(objDateT->GetName());

	for(int i = 0; i < gT->GetN(); i++) {
	    double hourFromDay = ((double)thisDateTime->getYDay() - (double)startDateTime->getYDay())*24.;
	    double hourInDay = 0.;
	    double thisTemp = 0.;

	    gT->GetPoint(i, hourInDay, thisTemp);

	    double countHour = hourFromDay + hourInDay;
	    double countDay = countHour/24.;

	    TDatime* tdt = new TDatime(thisDateTime->getYear(),
				   thisDateTime->getMonth(),
				   thisDateTime->getMDay(),
				   thisDateTime->getHour() + hourInDay,
				   thisDateTime->getMinute(),
				   thisDateTime->getSecond());

	    temp[tdt->Convert()] = thisTemp;

	    delete tdt;
	}

	delete thisDateTime;
    }

    TDirectory* dirHumi = f->GetDirectory("humidity");
    TList* listDateH = dirHumi->GetListOfKeys();
    listDateH->Sort();

    TIter iterDateH(listDateH);
    TObject* objDateH = nullptr;
    while((objDateH = iterDateH())) {
	if(startDateTime == nullptr)
	    startDateTime = new Calendar((string)(objDateH->GetName()) + "000000");

	Calendar* thisDateTime = new Calendar((string) (objDateH->GetName()) + "000000");

	TGraph* gH = (TGraph*)dirHumi->Get(objDateH->GetName());

	for(int i = 0; i < gH->GetN(); i++) {
	    double hourFromDay = ((double)thisDateTime->getYDay() - (double)startDateTime->getYDay())*24.;
	    double hourInDay = 0.;
	    double thisHumi = 0.;

	    gH->GetPoint(i, hourInDay, thisHumi);

	    double countHour = hourFromDay + hourInDay;
	    double countDay = countHour/24.;

	    TDatime* tdt = new TDatime(thisDateTime->getYear(),
				   thisDateTime->getMonth(),
				   thisDateTime->getMDay(),
				   thisDateTime->getHour() + hourInDay,
				   thisDateTime->getMinute(),
				   thisDateTime->getSecond());

	    humi[tdt->Convert()] = thisHumi;

	    delete tdt;
	}

	delete thisDateTime;
    }

    for(map<double, double>::iterator it = temp.begin(); it != temp.end(); ++it) {
	gTlong->SetPoint(gTlong->GetN(), it->first, it->second);
    }

    for(map<double, double>::iterator it = humi.begin(); it != humi.end(); ++it) {
	gHlong->SetPoint(gHlong->GetN(), it->first, it->second);
    }

    c->Divide(1, 2);
    c->cd(1);
    setGraphAtt(gTlong, "temp");
    gTlong->Draw("AL");

    c->cd(2);
    setGraphAtt(gHlong, "humi");
    gHlong->Draw("ALC");

    if(true) {
	TDatime* startdt = new TDatime(2021, 4, 18, 0, 0, 0);
	TDatime* enddt = new TDatime(2021, 6, 7, 0, 0, 0);
	gTlong->GetXaxis()->SetRangeUser(startdt->Convert(), enddt->Convert());
	gTlong->GetXaxis()->SetTimeDisplay(kTRUE);
	gTlong->GetXaxis()->SetNdivisions(10, 5, 0, kFALSE);
	gTlong->GetXaxis()->SetTimeFormat("%m\/%d");

	gHlong->GetXaxis()->SetRangeUser(startdt->Convert(), enddt->Convert());
	gHlong->GetXaxis()->SetTimeDisplay(kTRUE);
	gHlong->GetXaxis()->SetNdivisions(10, 5, 0, kFALSE);
	gHlong->GetXaxis()->SetTimeFormat("%m\/%d");

	delete startdt;
	delete enddt;
    }

    c->Update();
    c->Print("plotting/fittingHualien/temphumi.png");

    if(startDateTime != nullptr)
	delete startDateTime;

    delete f;
    delete gHlong;
    delete gTlong;
    delete c;
}



void TempHumi::setEndDateTime(string dtStr) {
    endDateTime->setDateTime(dtStr);
}



void TempHumi::setStartDateTime(string dtStr) {
    startDateTime->setDateTime(dtStr);
}



void TempHumi::setGraphAtt(TGraph* inputG, string term) {
    string graphTitle = "";
    string yAxisTitle = "";
    Color_t termColor = kBlack;
    if(term == "temp") {
	graphTitle = "Temperature";
	yAxisTitle = "Temperature ({}^{#circ}C)";
	termColor = kRed;
    } else if(term == "humi") {
	graphTitle = "Humidity";
	yAxisTitle = "Humidity \%";
	termColor = kBlue;
    }

    inputG->SetTitle(graphTitle.c_str());
    inputG->GetXaxis()->SetTitle("Days");
    inputG->GetYaxis()->SetTitle(yAxisTitle.c_str());
    inputG->SetLineWidth(2);
    inputG->SetLineColor(termColor);
    inputG->SetMarkerSize(1);
    inputG->SetMarkerStyle(kFullCircle);
    inputG->SetMarkerColor(termColor);
}



double TempHumi::calAvgTemp(string dateTime) {
    ifstream DHTfile;
    DHTfile.open("csv/" + dateTime + ".csv");

    vector<double> measuredTemp;

    string line;
    while(DHTfile.is_open()) {
	if(DHTfile.eof()) break;
	getline(DHTfile, line);

	if(line.length() != 0) {
	    string cutLine = line.substr(line.find_first_of(",") + 1);
	    string tempStr = cutLine.substr(0, cutLine.find_first_of(","));

	    measuredTemp.push_back(atof(tempStr.c_str()));
	}
    }

    double avgTemp = 0.;
    for(unsigned int i = 0; i < measuredTemp.size(); i++)
	avgTemp += measuredTemp[i];

    avgTemp /= (double)measuredTemp.size();
    return avgTemp;
}



double TempHumi::calStdTemp(string dateTime) {
    ifstream DHTfile;
    DHTfile.open("csv/" + dateTime + ".csv");

    vector<double> measuredTemp;

    string line;
    while(DHTfile.is_open()) {
	if(DHTfile.eof()) break;
	getline(DHTfile, line);

	if(line.length() != 0) {
	    string cutLine = line.substr(line.find_first_of(",") + 1);
	    string tempStr = cutLine.substr(0, cutLine.find_first_of(","));

	    measuredTemp.push_back(atof(tempStr.c_str()));
	}
    }

    double stdTemp = 0.;
    for(unsigned int i = 0; i < measuredTemp.size(); i++)
	stdTemp += measuredTemp[i]*measuredTemp[i];

    double avgTemp = calAvgTemp(dateTime);
    stdTemp = stdTemp - (double)measuredTemp.size()*avgTemp*avgTemp;
    stdTemp /= (double)measuredTemp.size() - 1;
    stdTemp = TMath::Sqrt(stdTemp);
    return stdTemp;
}



double TempHumi::calErrTemp(string dateTime) {
    ifstream DHTfile;
    DHTfile.open("csv/" + dateTime + ".csv");

    vector<double> measuredTemp;

    string line;
    while(DHTfile.is_open()) {
	if(DHTfile.eof()) break;
	getline(DHTfile, line);

	if(line.length() != 0) {
	    string cutLine = line.substr(line.find_first_of(",") + 1);
	    string tempStr = cutLine.substr(0, cutLine.find_first_of(","));

	    measuredTemp.push_back(atof(tempStr.c_str()));
	}
    }

    double errTemp = calStdTemp(dateTime)/TMath::Sqrt(measuredTemp.size());
    return errTemp;
}



double TempHumi::calAvgHumi(string dateTime) {
    ifstream DHTfile;
    DHTfile.open("csv/" + dateTime + ".csv");

    vector<double> measuredHumi;

    string line;
    while(DHTfile.is_open()) {
	if(DHTfile.eof()) break;
	getline(DHTfile, line);

	if(line.length() != 0) {
	    string cutLine = line.substr(line.find_first_of(",") + 1);
	    string humiStr = cutLine.substr(cutLine.find_first_of(",") + 1);

	    measuredHumi.push_back(atof(humiStr.c_str()));
	}
    }

    double avgHumi = 0.;
    for(unsigned int i = 0; i < measuredHumi.size(); i++)
	avgHumi += measuredHumi[i];

    avgHumi /= (double)measuredHumi.size();
    return avgHumi;
}



double TempHumi::calStdHumi(string dateTime) {
    ifstream DHTfile;
    DHTfile.open("csv/" + dateTime + ".csv");

    vector<double> measuredHumi;

    string line;
    while(DHTfile.is_open()) {
	if(DHTfile.eof()) break;
	getline(DHTfile, line);

	if(line.length() != 0) {
	    string cutLine = line.substr(line.find_first_of(",") + 1);
	    string humiStr = cutLine.substr(cutLine.find_first_of(",") + 1);

	    measuredHumi.push_back(atof(humiStr.c_str()));
	}
    }

    double stdHumi = 0.;
    for(unsigned int i = 0; i < measuredHumi.size(); i++)
	stdHumi += measuredHumi[i]*measuredHumi[i];

    double avgHumi = calAvgHumi(dateTime);
    stdHumi = stdHumi - (double)measuredHumi.size()*avgHumi*avgHumi;
    stdHumi /= (double)measuredHumi.size() - 1;
    stdHumi = TMath::Sqrt(stdHumi);
    return stdHumi;
}



double TempHumi::calErrHumi(string dateTime) {
    ifstream DHTfile;
    DHTfile.open("csv/" + dateTime + ".csv");

    vector<double> measuredHumi;

    string line;
    while(DHTfile.is_open()) {
	if(DHTfile.eof()) break;
	getline(DHTfile, line);

	if(line.length() != 0) {
	    string cutLine = line.substr(line.find_first_of(",") + 1);
	    string humiStr = cutLine.substr(cutLine.find_first_of(",") + 1);

	    measuredHumi.push_back(atof(humiStr.c_str()));
	}
    }

    double errHumi = calStdHumi(dateTime)/TMath::Sqrt(measuredHumi.size());
    return errHumi;
}



void TempHumi::assignTimeInterval() {
    ifstream timeIntervalFile;
    timeIntervalFile.open("recordDoc/TempHumiTimeInterval.txt");

    string startline;
    string endline;
    string line;
    while(timeIntervalFile.is_open()) {
	if(timeIntervalFile.eof()) break;

	getline(timeIntervalFile, line);
	if(line.length() != 0) {
	    startline = line.substr(0, line.find_first_of("-"));
	    endline = line.substr(line.find_first_of("-"));

	    setStartDateTime(startline.substr(0, startline.find_last_not_of(" -")));
	    setEndDateTime(endline.substr(endline.find_first_not_of(" -")));
	}
    }

    timeIntervalFile.close();
}



void TempHumi::prepareDataList() {
    DIR *dir;
    struct dirent *diread;
    string filename;

    if((dir = opendir("csv/")) != nullptr) {
	while((diread = readdir(dir)) != nullptr) {
	    filename = diread->d_name;
	    if(filename != "." && filename != "..")
		dataList.push_back(filename.substr(0, filename.find(".csv")));
	}
	closedir(dir);
    } else {
	perror("opendir");
    }

    sort(dataList.begin(), dataList.end());
}
