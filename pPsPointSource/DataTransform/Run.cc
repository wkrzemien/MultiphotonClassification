#include <iostream>
#include <fstream>
#include "GlobalActorReader.hh"
#include <string>
#include <TLorentzVector.h>
#include "TVector3.h"
#include <stdlib.h>

using namespace std;

struct gammaTrack {
  int eventID;
  int trackID;
  Double_t energy;
  Double_t x;
  Double_t y;
  Double_t z;
  Double_t time;
  bool pPs;
};

vector<gammaTrack> data;

void readEntry(const GlobalActorReader& gar)
{
  auto hitPosition = gar.GetProcessPosition();
  gammaTrack newOne;

  newOne.eventID = gar.GetEventID();
  newOne.trackID = gar.GetTrackID();
  newOne.x = hitPosition.X();
  newOne.y = hitPosition.Y();
  newOne.z = hitPosition.Z();
  newOne.time = gar.GetGlobalTime() + gar.GetLocalTime();
  newOne.energy = gar.GetEnergyLossDuringProcess();
  newOne.pPs = gar.GetEnergyBeforeProcess() == 511;

  data.push_back(newOne);
}

void createCSVFile() {
  ofstream outputFile;
  outputFile.open("data.csv");
  vector<pair <int,int>> events;
  bool isPPsEvent = false;

  for (vector<gammaTrack>::iterator it = data.begin() ; it != data.end(); ++it) {
    for (vector<gammaTrack>::iterator it2 = data.begin() ; it2 != data.end(); ++it2) {

      // Condition stand for small time difference applies only to point source
      if (abs(it->time - it2->time) <= 0.1 && it != it2 &&
        (it->trackID != it2->trackID || it->eventID != it2->eventID)) {
        pair <int,int> ex = make_pair(it->eventID, it->trackID);

        // Condition stand for pPs kind
        if (it->eventID == it2->eventID && it->pPs && it2->pPs && 
            it->trackID != it2->trackID &&
            find(events.begin(), events.end(), ex) == events.end() ) {
          events.push_back(ex);
          isPPsEvent = true;
        }

        outputFile  << it->eventID << ","
                << it->x << ","
                << it->y << ","
                << it->z << ","
                << it2->x << "," 
                << it2->y << ","
                << it2->z << "," 
                << it->energy << "," 
                << it2->energy << ","
                << it->time - it2->time << ","
                << it->time << ","
                << it2->time << ","
                << int(isPPsEvent) << endl; 
        isPPsEvent = false;
      }
    }
  }   
  outputFile.close();
  cout << "Output file created!" << endl;
}

int main(int argc, char* argv[])
{
  if (argc != 2) {
    cerr << "Invalid number of variables." << endl;
  } else {
    string file_name( argv[1] );

    try {
      GlobalActorReader gar;

      if (gar.LoadFile(file_name)) {
        while (gar.Read())
          readEntry(gar);  
      } else {
        cerr << "Loading file failed." << endl;
      }
      createCSVFile();
    } catch (const logic_error& e ) {
      cerr << e.what() << endl;
    } catch (...) {
      cerr << "Udefined exception" << endl;
    }
  }
  return 0;
}
