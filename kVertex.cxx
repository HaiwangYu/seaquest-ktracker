#include <iostream>
#include <cmath>
#include <algorithm>
#include <string>
#include <time.h>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TRandom.h>
#include <TMatrixD.h>
#include <TLorentzVector.h>
#include <TClonesArray.h>
#include <TMath.h>

#include "GeomSvc.h"
#include "ThresholdSvc.h"
#include "SRawEvent.h"
#include "KalmanUtil.h"
#include "KalmanTrack.h"
#include "KalmanFilter.h"
#include "KalmanFitter.h"
#include "VertexFit.h"
#include "SRecEvent.h"
#include "JobOptsSvc.h"

#include "MODE_SWITCH.h"

using namespace std;
using Threshold::live;

int main(int argc, char *argv[])
{
  if(argc != 2)
    {
      cout << "Usage: " << argv[0] << "  <options file>" << endl;
      exit(0);
    }

  //Initialize job options
  JobOptsSvc* jobOptsSvc = JobOptsSvc::instance();
  jobOptsSvc->init(argv[1]);

  //Retrieve the raw event
  SRecEvent* recEvent = new SRecEvent();
  TClonesArray* tracklets = new TClonesArray("tracklets");
  SRawEvent* rawEvent = jobOptsSvc->m_mcMode ? (new SRawMCEvent()) : (new SRawEvent());;

  TFile* dataFile = new TFile(jobOptsSvc->m_inputFile.c_str(), "READ");
  TTree* dataTree = (TTree*)dataFile->Get("save");

  dataTree->SetBranchAddress("recEvent", &recEvent);
  dataTree->SetBranchAddress("tracklets", &tracklets);

  TFile* saveFile = new TFile(jobOptsSvc->m_outputFile.c_str(), "recreate");
  TTree* saveTree = new TTree("save", "save");

  saveTree->Branch("recEvent", &recEvent, 256000, 99);
  saveTree->Branch("tracklets", &tracklets, 256000, 99);

  if(jobOptsSvc->m_attachRawEvent)
    {
      dataTree->SetBranchAddress("rawEvent", &rawEvent);
      saveTree->Branch("rawEvent", &rawEvent, 256000, 99);
      tracklets->BypassStreamer();
    }
  
  //Initialize track finder
  LogInfo("Initializing the track finder and kalman filter ... ");
  VertexFit* vtxfit = new VertexFit();
  vtxfit->enableOptimization();
  if(jobOptsSvc->m_enableEvaluation) 
    {
      string evalFileName = "eval_" + jobOptsSvc->m_outputFile;
      vtxfit->bookEvaluation(evalFileName.c_str());
    }

  const int offset = jobOptsSvc->m_firstEvent;
  int nEvtMax = jobOptsSvc->m_nEvents > 0 ? jobOptsSvc->m_nEvents + offset : dataTree->GetEntries();
  if(nEvtMax > dataTree->GetEntries()) nEvtMax = dataTree->GetEntries();
  LogInfo("Running from event " << offset << " through to event " << nEvtMax);
  for(int i = offset; i < nEvtMax; ++i)
    {
      dataTree->GetEntry(i);
      if(live())
        {
          cout << "\r Processing event " << i << " with eventID = " << recEvent->getEventID() << ", ";
          cout << (i + 1)*100/nEvtMax << "% finished .. " << flush;
        }

      vtxfit->setRecEvent(recEvent);

      if(recEvent->getNDimuons() > 0) saveTree->Fill();
      if(saveTree->GetEntries() % 1000 == 0) saveTree->AutoSave("SaveSelf");

      recEvent->clear();
      tracklets->Clear();
      if(jobOptsSvc->m_attachRawEvent) rawEvent->clear();
    }
  cout << endl;
  cout << "kVertex ends successfully." << endl;

  saveFile->cd();
  saveTree->Write();
  saveFile->Close();

  delete vtxfit;

  return EXIT_SUCCESS;
}
