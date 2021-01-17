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
    readYAML();

    if(instrumentCh0 != "none")
	fTree->SetBranchAddress("ch0", &ch0, &b_ch0);
    fTree->SetBranchAddress("timestamp", &timestamp, &b_timestamp);
}



void DataReader::runFillingLoop(TH1D* inputH) {
    initialize();

    Long64_t nentries = fTree->GetEntries();

    for(Long64_t entry = 0; entry < nentries; ++entry) {
	if(entry % 100 == 0)
	    cout << "Processed ... " << entry << "/" << nentries << " events" << endl;

	fTree->GetEntry(entry);
	ch0d = ch0;
	timestampD = timestamp;
	
	Calendar* eventDateTime = new Calendar(projectName);
	eventDateTime->addDuration(0, 0, 0, 0, timestampD);

	if(*eventDateTime >= *startDateTime
	   && *eventDateTime <= *endDateTime) {
	    inputH->Fill(ch0d);	    
	}
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
}
