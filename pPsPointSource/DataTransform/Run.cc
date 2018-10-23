#include "GlobalActorReader.hh"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2.h"
#include "TVector3.h"
#include <TLorentzVector.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>

using namespace std;

struct gammaTrack {
  int eventID;
  int trackID;
  Double_t energy;
  Double_t x;
  Double_t y;
  Double_t z;
  Double_t time;
  Double_t globalTime;
  string volume;
  bool pPs;
};

// WK My version
TVector3 recoEmissionPoint(double x1, double y1, double z1, double time1,
                            double x2, double y2, double z2, double time2) {
  TVector3 hit1;
  TVector3 hit2;
  double t1 = 0;
  double t2 = 0;
  if (time1 < time2) {
    hit1.SetXYZ(x1, y1, z1);
    hit2.SetXYZ(x2, y2, z2);
    t1 = time1;
    t2 = time2;
  } else {
    hit2.SetXYZ(x1, y1, z1);
    hit1.SetXYZ(x2, y2, z2);
    t2 = time1;
    t1 = time2;
  }
  TVector3 vCenter = (hit1 + hit2);
  vCenter = 0.5 * vCenter;
  TVector3 vVect = hit2 - hit1;
  TVector3 vVersor = vVect.Unit();
  double tDiff = (t1 - t2) / 2;
  double speedOfLight = 300; /// mm/ns
  double shift = tDiff * speedOfLight;
  TVector3 vect;
  vect.SetXYZ(vCenter.X() + shift * std::abs(vVersor.X()),
              vCenter.Y() + shift * std::abs(vVersor.Y()),
              vCenter.Z() + shift * std::abs(vVersor.Z()));
  return vect;
}

vector<gammaTrack> data;
TH1F *timeDiff = new TH1F("Pairs time differences", "Pairs time differences",
                          200, 0, 10000000);
TH1F *htimeDiff2 =
    new TH1F("htimeDiff2", "Pairs time differences", 1000, -5, 5);
TH1F *hTimeDiffPPsSameLayer =
    new TH1F("hTimeDiffPPsSameLayer", "hTimeDiffPPsSameLayer", 1000, -1, 1);
TH2 *hRecoEmissionPointTrue =
    new TH2D("hRecoEmissionPointTrue", "hRecoEmissionPointTrue", 1000, -5, 5,
             1000, -5, 5);
TH2 *hRecoEmissionPointAll = new TH2D(
    "hRecoEmissionPointAll", "hRecoEmissionPointAll", 1000, -5, 5, 1000, -5, 5);
TH2 *pos = new TH2D("XY", "XY", 1200, -600, 600, 1200, -600, 600);
bool createStats = false;

void saveEntry(gammaTrack &gt1, gammaTrack &gt2, bool isPPsEvent,
               ofstream &outputFile) {
  outputFile << gt1.eventID << "," << gt2.eventID << "," << gt1.trackID << ","
             << gt2.trackID << "," << setprecision(20) << gt1.x << "," << gt1.y
             << "," << gt1.z << "," << gt2.x << "," << gt2.y << "," << gt2.z
             << "," << gt1.energy << "," << gt2.energy << ","
             << gt1.globalTime - gt2.globalTime << "," << gt1.time << "," << gt2.time << ","
             << gt1.volume << "," << gt2.volume << "," << int(isPPsEvent)
             << endl;
}

void readEntry(const GlobalActorReader &gar) {
  auto hitPosition = gar.GetProcessPosition();
  gammaTrack newOne;

  newOne.eventID = gar.GetEventID();
  newOne.trackID = gar.GetTrackID();
  newOne.x = hitPosition.X();
  newOne.y = hitPosition.Y();
  newOne.z = hitPosition.Z();
  newOne.time = gar.GetLocalTime();
  newOne.globalTime = gar.GetGlobalTime();
  newOne.energy = gar.GetEnergyLossDuringProcess();
  newOne.pPs = (gar.GetEnergyBeforeProcess() == 511);
  newOne.volume = gar.GetVolumeName();

  data.push_back(newOne);
}

void createCSVFile() {
  ofstream outputFile;
  outputFile.open("data.csv");
  bool isPPsEvent = false;
  int dataSize = data.size();
  int loops = dataSize*dataSize/2 - dataSize;
  int counter = 0;

  for (vector<gammaTrack>::iterator it = data.begin(); it != data.end(); ++it) {
    if (createStats) {
      pos->Fill(it->x, it->y);
    }
    for (vector<gammaTrack>::iterator it2 = next(it); it2 != data.end(); ++it2) {
      // Process log
      cout << "\r" << (counter*100)/loops << "%";
      counter++;
      // Time cut
      const double kTimeCut = 0.1;
      bool areIntimeCut = (abs(it->globalTime - it2->globalTime) <= kTimeCut);
      bool areSameEvents = (it->eventID == it2->eventID);
      bool areDifferentTracks = (it->trackID != it2->trackID);
      bool areSameLayers = (it->volume == it2->volume);
      if (!areIntimeCut) continue;

      if (createStats) {
        auto recoEPoint = recoEmissionPoint(it->x, it->y, it->z, it->time, it2->x,
                                          it2->y, it2->z, it2->time);
        timeDiff->Fill(std::abs(it->time - it2->time));
        htimeDiff2->Fill(it->time - it2->time);
        hRecoEmissionPointAll->Fill(recoEPoint.X(), recoEPoint.Y());
      }
      if (areSameEvents && areDifferentTracks && areSameLayers && it->pPs &&
          it2->pPs) {
        isPPsEvent = true;
        if (createStats) {
          hTimeDiffPPsSameLayer->Fill(it->time - it2->time);
          pos->Fill((it->x * it2->time + it2->x * it->time) /
                        (it->time + it2->time), // x
                    (it->y * it2->time + it2->y * it->time) /
                        (it->time + it2->time) // y
                    );
          auto recoEPoint = recoEmissionPoint(it->x, it->y, it->z, it->time, it2->x,
                                          it2->y, it2->z, it2->time);
          hRecoEmissionPointTrue->Fill(recoEPoint.X(), recoEPoint.Y());
        }
      }

      saveEntry(*it, *it2, isPPsEvent, outputFile);
      // Symmetrical case
      saveEntry(*it2, *it, isPPsEvent, outputFile);
      isPPsEvent = false;
    }
  }
  outputFile.close();
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    cerr << "Invalid number of variables." << endl;
  } else {
    string file_name(argv[1]);
    createStats = argv[2];

    try {
      GlobalActorReader gar;

      int counter = 0;
      if (gar.LoadFile(file_name)) {
        while (gar.Read()) {
          if (counter > 10000)
            break;
          readEntry(gar);
          counter++;
        }
      } else {
        cerr << "Loading file failed." << endl;
      }
      createCSVFile();
      if (createStats) {
        // Time differences
        TCanvas c1("c", "c", 2000, 2000);
        timeDiff->SetLineColor(kBlack);
        timeDiff->Draw();
        c1.SaveAs("timeDiff.png");
        // Positions
        TCanvas c2("c", "c", 2000, 2000);
        pos->SetLineColor(kBlack);
        pos->Draw();
        c2.SaveAs("posXY.png");
        // Time differences part 2
        TCanvas c3("c", "c", 2000, 2000);
        c3.Divide(2, 1);
        c3.cd(1);
        htimeDiff2->SetLineColor(kBlack);
        htimeDiff2->Draw();
        c3.cd(2);
        hTimeDiffPPsSameLayer->Draw();
        c3.SaveAs("timeDiff.png");
        c3.SaveAs("timeDiff.root");
        TCanvas c4("c", "c", 2000, 2000);
        c4.Divide(2, 1);
        c4.cd(1);
        hRecoEmissionPointAll->Draw();
        c4.cd(2);
        hRecoEmissionPointTrue->Draw();
        c4.SaveAs("RecoXY.png");
        c4.SaveAs("RecoXY.root");
      }
    } catch (const logic_error &e) {
      cerr << e.what() << endl;
    } catch (...) {
      cerr << "Udefined exception" << endl;
    }
  }
  return 0;
}
