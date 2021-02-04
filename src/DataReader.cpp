#include "DataReader.h"

#include <TFile.h>
#include <TTree.h>
#include <TH1.h>

#include "Calendar.h"
#include "GetExterSet.h"

DataReader::DataReader(string inputProject) {
    projectName = inputProject;
    string inputFilename = "data/" + projectName + ".root";
    fFile = new TFile(inputFilename.c_str(), "READ");
    fFile->GetObject("detector_A", fTree);
    
    fileDateTime = new Calendar(projectName);
    startDateTime = new Calendar(projectName);
    endDateTime = new Calendar(projectName);
}



DataReader::~DataReader() {
    fFile->Close();
}



void DataReader::initialize() {
    if(!isInit) {
	readYAML();

	if(instrumentCh0 != "none")
	    fTree->SetBranchAddress("ch0", &ch0, &b_ch0);
	if(instrumentCh1 != "none")
	    fTree->SetBranchAddress("ch1", &ch1, &b_ch1);
	fTree->SetBranchAddress("timestamp", &timestamp, &b_timestamp);

	isInit = true;
    }
}



void DataReader::runFillingLoop(TH1D* inputH, int channel) {
    initialize();

    Long64_t nentries = fTree->GetEntries();

    for(Long64_t entry = 0; entry < nentries; ++entry) {
	if(entry % 10000 == 0)
	    cout << "Processed ... " << entry << "/" << nentries << " events" << endl;

	fTree->GetEntry(entry);
	if(instrumentCh0 != "none")
	    ch0d = ch0;
	else
	    ch0d = 0.;

	if(instrumentCh1 != "none")
	    ch1d = ch1;
	else
	    ch1d = 0.;
	
	double fillValue = 0.;
	if(channel == 0)
	    fillValue = ch0d;
	else if(channel == 1)
	    fillValue = ch1d;
	
	timestampD = timestamp;
	
	Calendar* eventDateTime = new Calendar(projectName);
	eventDateTime->addDuration(0, 0, 0, 0, timestampD);

	if(*eventDateTime >= *startDateTime
	   && *eventDateTime <= *endDateTime) {
	    if(quantity == "Voltage")
		inputH->Fill(fillValue);
	    else if(quantity == "Energy")
		inputH->Fill(V2MeV(fillValue));
	} else if(*eventDateTime > *endDateTime)
	    break;
    }
}



void DataReader::runCoincidenceFilling(TH1D* inputH, int channel, double threshold) {
    initialize();

    Long64_t nentries = fTree->GetEntries();

    for(Long64_t entry = 0; entry < nentries; ++entry) {
	if(entry % 10000 == 0)
	    cout << "Processed ... " << entry << "/" << nentries << " events" << endl;

	fTree->GetEntry(entry);
	if(instrumentCh0 != "none")
	    ch0d = ch0;
	else
	    ch0d = 0;

	if(instrumentCh1 != "none")
	    ch1d = ch1;
	else ch1d = 0.;

	double fillValue = 0.;
	double threValue = 0.;
	if(channel == 0) {
	    fillValue = ch0d;
	    threValue = ch1d;
	} else if(channel == 1) {
	    fillValue = ch1d;
	    threValue = ch0d;
	}

	timestampD = timestamp;

	Calendar* eventDateTime = new Calendar(projectName);
	eventDateTime->addDuration(0, 0, 0, 0, timestampD);

	if(*eventDateTime >= *startDateTime
	   && *eventDateTime <= *endDateTime) {
	    if(threValue >= threshold) {
		if(quantity == "Voltage")
		    inputH->Fill(fillValue);
		else if(quantity == "Energy")
		    inputH->Fill(V2MeV(fillValue));
	    }
	} else if(*eventDateTime > *endDateTime)
	    break;
    }
}



void DataReader::setEndDateTime(string dtStr) {
    endDateTime->setDateTime(dtStr);
}



void DataReader::setStartDateTime(string dtStr) {
    startDateTime->setDateTime(dtStr);
}



void DataReader::readYAML() {
    char yamlFilename[200];
    sprintf(yamlFilename, "yaml/%s.yaml", projectName.c_str());

    GetExterSet ges { yamlFilename };

    recordLength = ges.giveIntVar("ActualRecLength");
    referencePosition = ges.giveDoubleVar("ReferencePosition");
    sampleRate = ges.giveDoubleVar("SampleRate");
    triggerSource = ges.giveStrVar("TriggerSource");
    instrumentCh0 = ges.giveStrVar("Ch0Instrument");
    inputVoltageCh0 = ges.giveDoubleVar("Ch0InputVoltage");
    isCh0Amplified = ges.giveBoolVar("Ch0Amplified");
    instrumentCh1 = ges.giveStrVar("Ch1Instrument");
    inputVoltageCh1 = ges.giveDoubleVar("Ch1InputVoltage");
    isCh1Amplified = ges.giveBoolVar("Ch1Amplified");
}



double DataReader::V2MeV(double input) {
    return (input - 0.121607)/1.021556;
}
