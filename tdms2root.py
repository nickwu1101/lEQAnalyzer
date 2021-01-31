#!/usr/bin/env python
import pytdms
import ROOT
from ROOT import TTree
import yaml
import sys
from array import array

if len(sys.argv) != 2:
    print ("USAGE: %s <input file>" %(sys.argv[0]))
    sys.exit(1)

tdmsFileName = sys.argv[1]
rootFileName = "data/%s.root" %((tdmsFileName.rsplit('/',1)[1]).split('.')[0])
print ("convert", tdmsFileName, "to", rootFileName)

# Read tdmsfile
(objects,rawdata) = pytdms.read(tdmsFileName)

# Read headers
Name              = objects[b'/'][3][b'name'][1].decode('utf-8')
Events            = int(objects[b'/'][3][b'Total Events'][1].decode('utf-8'))
Operator          = objects[b'/'][3][b'Operator'][1].decode('utf-8')
StartT            = objects[b'/'][3][b'Start Time'][1].decode('utf-8')
VerticalRange     = float(objects[b'/'][3][b'vertical range'][1].decode('utf-8'))
VerticalOffset    = float(objects[b'/'][3][b'vertical offset'][1].decode('utf-8'))
MaxInpFreq        = float(objects[b'/'][3][b'maximum input frequency'][1].decode('utf-8'))
SampleRate        = float(objects[b'/'][3][b'actual sample rate'][1].decode('utf-8'))
RecordLength      = int(objects[b'/'][3][b'record length'][1].decode('utf-8'))
ActualRecLength   = int(objects[b'/'][3][b'actual record length'][1].decode('utf-8'))
TriggerSource     = objects[b'/'][3][b'trigger source'][1].decode('utf-8')
TriggerSlope      = bool(objects[b'/'][3][b'trigger slope'][1].decode('utf-8'))
TriggerLevel      = float(objects[b'/'][3][b'trigger level'][1].decode('utf-8'))
TriggerDelay      = float(objects[b'/'][3][b'trigger delay'][1].decode('utf-8'))
ReferencePosition = float(objects[b'/'][3][b'reference position'][1].decode('utf-8'))
EndT              = objects[b'/'][3][b'End Time'][1].decode('utf-8')
Ch0Instrument     = str(objects[b'/'][3][b'Ch0Instrument'][1].decode('utf-8'))
Ch0InputVoltage   = float(objects[b'/'][3][b'Ch0InputVoltage'][1].decode('utf-8'))
Ch0Amplified      = str(objects[b'/'][3][b'Ch0Amplified'][1].decode('utf-8'))
Ch1Instrument     = str(objects[b'/'][3][b'Ch1Instrument'][1].decode('utf-8'))
Ch1InputVoltage   = float(objects[b'/'][3][b'Ch1InputVoltage'][1].decode('utf-8'))
Ch1Amplified      = str(objects[b'/'][3][b'Ch1Amplified'][1].decode('utf-8'))
UsedSource        = str(objects[b'/'][3][b'UsedSource'][1].decode('utf-8'))
EventRate         = float(objects[b'/'][3][b'Event Rate'][1].decode('utf-8'))

a_dict = {}
for variable in ["Name", "Events", "Operator", "StartT", "EndT", "VerticalRange", "VerticalOffset", "MaxInpFreq", "SampleRate", "RecordLength", "ActualRecLength", "TriggerSource",
                 "TriggerSlope", "TriggerLevel", "TriggerDelay", "ReferencePosition", "Ch0Instrument", "Ch0InputVoltage", "Ch0Amplified", "Ch1Instrument", "Ch1InputVoltage", "Ch1Amplified", "UsedSource", "EventRate"]:
    a_dict[variable] = eval(variable)

# Write headers into yaml file
with open(r'yaml/%s.yaml' %((tdmsFileName.rsplit('/',1)[1]).split('.')[0]), 'w') as file:
    documents = yaml.dump(a_dict, file, default_flow_style=False, sort_keys=False)

# Initialize output tree
singleLength = 1
slicedarray0 = array( 'f', [ 0 ] * singleLength )
slicedarray1 = array( 'f', [ 0 ] * singleLength )
slicedarray2 = array( 'f', [ 0 ] * singleLength )
t = ROOT.TTree("detector_A", "detector_A")
t.Branch('ch0',slicedarray0,"slicedarray0[%d]/F" %(singleLength))
t.Branch('ch1',slicedarray1,"slicedarray1[%d]/F" %(singleLength))
t.Branch('timestamp',slicedarray2,"slicedarray2[%d]/F" %(singleLength))

# Loop
for index in range (Events):
    if ( index % 1000 == 0 ):
        print ("Processing event", index )
    start_index = index * singleLength
    end_index = (index + 1) * singleLength
    for idx, val in enumerate(range(start_index, end_index)):
        slicedarray0[idx] = rawdata[b"/'Take Data'/'ch0'"][val]
        slicedarray1[idx] = rawdata[b"/'Take Data'/'ch1'"][val]
        slicedarray2[idx] = rawdata[b"/'Take Data'/'timestamp'"][val]
    t.Fill()

# Output
outHistFile = ROOT.TFile.Open(rootFileName,"RECREATE")
outHistFile.cd()
t.Write()
outHistFile.Close()
