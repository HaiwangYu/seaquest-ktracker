/*
IO manager to handle fast extraction of data from database or upload data to database

Author: Kun Liu, liuk@fnal.gov
Created: 2013.9.29
*/

#ifndef _MYSQLSVC_H
#define _MYSQLSVC_H

#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <algorithm>

#include <TSQLServer.h>
#include <TSQLResult.h>
#include <TSQLRow.h>
#include <TRandom.h>
#include <TClonesArray.h>
#include <TVector3.h>
#include <TLorentzVector.h>

#include "GeomSvc.h"
#include "SRawEvent.h"
#include "FastTracklet.h"

//#define OUT_TO_SCREEN
//#define USE_M_TABLES

class MySQLSvc
{
public:
  MySQLSvc();
  ~MySQLSvc();
  static MySQLSvc* instance();
  
  //Connect to the server
  bool connect(std::string sqlServer = MYSQL_SERVER);

  //check if the run is stopped
  bool isRunStopped();

  //Get run info
  int getNEventsFast();
  int getNEvents();

  //Gets
  bool getEvent(SRawEvent* rawEvent, int eventID);
  bool getLatestEvt(SRawEvent* rawEvent);
  bool getRandomEvt(SRawEvent* rawEvent);
  bool getNextEvent(SRawEvent* rawEvent);
  bool getNextEvent(SRawMCEvent* rawEvent);

  //Check if the event has been loaded
  bool isEventLoaded(int eventID) { return std::find(eventIDs.begin(), eventIDs.end(), eventID) != eventIDs.end(); } 

  //Get the event header
  bool getEventHeader(SRawEvent* rawEvent, int eventID);
  bool getEventHeader(SRawMCEvent* mcEvent, int eventID);

  //Output to database/txt file/screen
  void bookOutputTables();
  void writeTrackingRes(SRecEvent* recEvent, TClonesArray* tracklets);
  void writeTrackTable(int trackID, SRecTrack* recTrack);
  void writeTrackHitTable(int trackID, Tracklet* tracklet);
  void writeDimuonTable(int dimuonID, int idx_positive, int idx_negative);

  //Set the data schema
  void setWorkingSchema(std::string schema);
  void setLoggingSchema(std::string schema) { logSchema = schema; } 

  //Memory-safe sql queries
  int makeQuery();
  bool nextEntry();
  
  int getInt(const char* field, int default_val = 0) { return field == NULL ? default_val : atoi(field); }
  double getDouble(const char* field, double default_val = 0.) { return field == NULL ? default_val : atof(field); }

private:
  //pointer to the only instance
  static MySQLSvc* p_mysqlSvc;

  //pointer to the geometry service
  GeomSvc* p_geomSvc;

  //SQL server
  TSQLServer* server;
  TSQLResult* res;
  TSQLRow* row;

  //Random generator
  TRandom rndm;

  //Last eventID used
  int eventID_last;

  //run-related info
  int runID;
  int spillID;
  int nEvents;
  std::list<int> eventIDs;

  //Query string used in all clause
  char query[2000];

  //name of the production schema working on
  std::string dataSchema;
  std::string logSchema;

  //Internal counter of tracks and dimuons
  int nTracks;
  int nDimuons;

  std::vector<TLorentzVector> mom_vertex;
  std::vector<TVector3> pos_vertex;
};

#endif
