#include <iostream>
#include <fstream>
#include "GlobalActorReader.hh"
#include <string>
#include <TLorentzVector.h>
#include "TVector3.h"
#include <stdlib.h>
#include "TH1F.h"
#include "TH2.h"
#include "TCanvas.h"

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
TH1F* timeDiff = new TH1F( "Pairs time differences", "Pairs time differences", 200, 0, 10);
TH2* pos = new TH2D(
  /* name */ "XY detected positions",
  /* title */ "XY detected positions",
  /* X-dimension */ 1200, -600, 600,
  /* Y-dimension */ 1200, -600, 600);
bool createStats = false;

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
    if (createStats) {
      pos->Fill(it->x, it->y);
    }
    for (vector<gammaTrack>::iterator it2 = data.begin() ; it2 != data.end(); ++it2) {
      if (createStats) {
        timeDiff->Fill(abs(it->time - it2->time));
      }
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
  if (argc != 3) {
    cerr << "Invalid number of variables." << endl;
  } else {
    string file_name( argv[1] );
    createStats = argv[2];

    try {
      GlobalActorReader gar;

      if (gar.LoadFile(file_name)) {
        while (gar.Read())
          readEntry(gar);  
      } else {
        cerr << "Loading file failed." << endl;
      }
      createCSVFile();
      if (createStats) {
        TCanvas c1("c", "c", 2000, 2000);
        timeDiff->SetLineColor(kBlack);
        timeDiff->Draw();
        c1.SaveAs("timeDiff.png");
        TCanvas c2("c", "c", 2000, 2000);
        pos->SetLineColor(kBlack);
        pos->Draw();
        c2.SaveAs("posXY.png");
      }
    } catch (const logic_error& e ) {
      cerr << e.what() << endl;
    } catch (...) {
      cerr << "Udefined exception" << endl;
    }
  }
  return 0;
}
