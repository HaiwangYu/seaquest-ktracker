/*
SRecEvent.h

Definition of the class SRecEvent and SRecTrack. SRecTrack is the  final track structure of recontructed tracks.
Contains nothing but ROOT classes, light-weighted and can be used as input for physics analysis. SRecEvent serves
as a container of SRecTrack

Author: Kun Liu, liuk@fnal.gov
Created: 01-21-2013
*/

#ifndef _SRECTRACK_H
#define _SRECTRACK_H

#include "MODE_SWITCH.h"

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <map>
#include <cmath>

#include <TObject.h>
#include <TROOT.h>
#include <TMatrixD.h>
//#include <TLorentzVector.h>

#include "SRawEvent.h"

class SRecTrack: public TObject
{
public:
  SRecTrack();

  ///Gets
  Int_t getCharge() { return (fState[0])[0][0] > 0 ? 1 : -1; }
  Int_t getNHits() { return fHitIndex.size(); }
  Double_t getChisq() { return fChisq; }
  Double_t getQuality() { return (Double_t)getNHits() - 0.4*getChisq(); }
  bool isHodoMasked();
  Int_t getNHodoHits(Int_t i) { return fNHodoHits[i]; }

  Int_t getHitIndex(Int_t i) { return fHitIndex[i]; }
  TMatrixD getStateVector(Int_t i) { return fState[i]; }
  TMatrixD getCovariance(Int_t i) { return fCovar[i]; }
  Double_t getZ(Int_t i) { return fZ[i]; }
  Double_t getChisqAtNode(Int_t i) { return fChisqAtNode[i]; } 

  Int_t getNearestNode(Double_t z);
  void getExpPositionFast(Double_t z, Double_t& x, Double_t& y, Int_t iNode = -1);
  void getExpPosErrorFast(Double_t z, Double_t& dx, Double_t& dy, Int_t iNode = -1);
  Double_t getExpMomentumFast(Double_t z, Double_t& px, Double_t& py, Double_t& pz, Int_t iNode = -1);
  Double_t getExpMomentumFast(Double_t z, Int_t iNode = -1);

  Double_t getMomentumSt1(Double_t& px, Double_t& py, Double_t& pz) { return getMomentum(fState.front(), px, py, pz); }
  Double_t getMomentumSt1() { Double_t px, py, pz; return getMomentumSt1(px, py, pz); }
  Double_t getMomentumSt3(Double_t& px, Double_t& py, Double_t& pz) { return getMomentum(fState.back(), px, py, pz); }
  Double_t getMomentumSt3() { Double_t px, py, pz; return getMomentumSt3(px, py, pz); }
  Double_t getPositionSt1(Double_t& x, Double_t& y) { return getPosition(fState.front(), x, y); }
  Double_t getPositionSt1() { Double_t x, y; return getPositionSt1(x, y); }
  Double_t getPositionSt3(Double_t& x, Double_t& y) { return getPosition(fState.back(), x, y); }
  Double_t getPositionSt3() { Double_t x, y; return getPositionSt3(x, y); }

  Double_t getMomentum(TMatrixD& state, Double_t& px, Double_t& py, Double_t& pz);
  Double_t getPosition(TMatrixD& state, Double_t& x, Double_t& y);

  ///Comparitor
  bool operator<(const SRecTrack& elem) const;

  ///Sets
  void setChisq(Double_t chisq) { fChisq = chisq; }
  void setHodoHits(Int_t hodoHits[]) { for(Int_t i = 0; i < 3; i++) fNHodoHits[i] = hodoHits[i]; }
  void setHodoHits() { for(Int_t i = 0; i < 3; i++) fNHodoHits[i] = 1; }
  void insertHitIndex(Int_t index) { fHitIndex.push_back(index); }
  void insertStateVector(TMatrixD state) { fState.push_back(state); }
  void insertCovariance(TMatrixD covar) { fCovar.push_back(covar); }
  void insertZ(Double_t z) { fZ.push_back(z); }
  void insertChisq(Double_t chisq) { fChisqAtNode.push_back(chisq); }

  ///Vertex stuff
  bool isVertexValid();
  void setZVertex(Double_t z);

  //TLorentzVector getMomentumVertex() { return TLorentzVector(0,0,0,0); }
  Double_t getMomentumVertex(Double_t& px, Double_t& py, Double_t& pz) { return getMomentum(fStateVertex, px, py, pz); }
  Double_t getZVertex() { return fVtxPar[2]; }
  Double_t getRVertex() { return sqrt(fVtxPar[0]*fVtxPar[0] + fVtxPar[1]*fVtxPar[1]); }
  Double_t getVtxPar(Int_t i) { return fVtxPar[i]; }
  Double_t getChisqVertex() { return fChisqVertex; }

  ///Debugging output
  void print();

private:
  ///Total Chisq
  Double_t fChisq;

  ///Hit list and associated track parameters
  std::vector<Int_t> fHitIndex;
  std::vector<TMatrixD> fState;
  std::vector<TMatrixD> fCovar;
  std::vector<Double_t> fZ;
  std::vector<Double_t> fChisqAtNode;
  Int_t fNHodoHits[3];

  ///Vertex infomation
  Double_t fVtxPar[3];
  Double_t fChisqVertex;
  TMatrixD fStateVertex;
  TMatrixD fCovarVertex;

  ClassDef(SRecTrack, 2)
};

class SRecEvent: public TObject
{
public:
  SRecEvent();

  ///Set/Get event info
  void setEventInfo(Int_t runID, Int_t spillID, Int_t eventID);
  void setRawEvent(SRawEvent *rawEvent);
  Int_t getRunID() { return fRunID; }
  Int_t getSpillID() { return fSpillID; }
  Int_t getEventID() { return fEventID; }

  Int_t getLocalID(Int_t hitID) { return fLocalID[hitID]; }

  ///Get tracks
  Int_t getNTracks() { return fAllTracks.size(); }
  SRecTrack& getTrack(Int_t i) { return fAllTracks[i]; }

  ///Get track IDs
  std::vector<Int_t> getChargedTrackIDs(Int_t charge);

  ///Insert tracks
  void insertTrack(SRecTrack trk) { fAllTracks.push_back(trk); }
  void reIndex() { sort(fAllTracks.begin(), fAllTracks.end()); } 

  ///Output to MySQL database
  void outputMySQL(); 

  ///Clear everything
  void clear();

private:
  ///Basic event info.
  Int_t fRunID;
  Int_t fSpillID;
  Int_t fEventID;

  ///Container of SRecTrack
  std::vector<SRecTrack> fAllTracks;

  ///Mapping of hitID to local container ID in SRawEvent
  std::map<Int_t, Int_t> fLocalID;

  ClassDef(SRecEvent, 1)
};

#endif
