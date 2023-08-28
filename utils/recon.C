#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "Math/Interpolator.h"

// have 2 configurable parameter: regular time step in [ns] for reconstruction
// and the number of antenna in G4 simulation output file.
struct config {
  double sampletime = 0.004; // [ns]=1/250 GHz
  int nantenna = 2;
} conf;


// this re-samples the irregularly sampled time points to regular steps with conf.sampletime
void interpolatePerTraj(const std::vector<double>& t, const std::vector<double>& volts, std::vector<double>& resampled) {
  double interval = t.back() - t.front(); // time in ns
  if (interval<=conf.sampletime) return; // too short, empty

  int maxpoints = (int)floor(interval / conf.sampletime);
  // Spline here
  ROOT::Math::Interpolator ip(t,volts);
  for (int i=1;i<=maxpoints;++i) 
    resampled.push_back(ip.Eval(i*conf.sampletime)); // volts at time step
}

// Read and create a new reconstructed output file with a regularly time-sampled voltage values
// by interpolation. Should yield fewer (downsample) values than raw data with interpolation
// but ideally not by much. File size should shrink since only the voltage value vector is 
// written in the output file; all other values are single values, not containers.
void reconstruct(const char* fname, const char* foutname) {
  TFile fin(fname, "read");
  TTreeReader trin("ntuple/Signal", &fin);
  // collect all info for disentangling
  TTreeReaderValue<int> eventID(trin, "EventID");
  TTreeReaderValue<int> trackID(trin, "TrackID");
  TTreeReaderValue<double> px(trin, "Posx");
  TTreeReaderValue<double> py(trin, "Posy");
  TTreeReaderValue<double> pz(trin, "Posz");
  TTreeReaderValue<double> ke(trin, "KinEnergy");
  TTreeReaderValue<double> pangle(trin, "PitchAngle");
  TTreeReaderValue<std::vector<int>> aID(trin, "AntennaID");
  TTreeReaderValue<std::vector<double>> tvec(trin, "TimeVec");
  TTreeReaderValue<std::vector<double>> vvec(trin, "VoltageVec");

  // disentangle
  int eID;
  int tID;
  int antID;
  double npx;
  double npy;
  double npz;
  double nke;
  double npa;
  double sampling;
  std::vector<double> resampled; // resampling

  // output
  TFile* fout = new TFile(foutname, "recreate");
  TTree* trout = new TTree("SignalRecon", "recon signal");
  trout->SetDirectory(fout);
  trout->Branch("eventID", &eID);
  trout->Branch("trackID", &tID);
  trout->Branch("antennaID", &antID);
  trout->Branch("posx", &npx);
  trout->Branch("posy", &npy);
  trout->Branch("posz", &npz);
  trout->Branch("kinenergy", &nke);
  trout->Branch("pitchangle", &npa);
  trout->Branch("sampling", &sampling);
  trout->Branch("VoltageVec", &resampled);

  while (trin.Next()) { // read row
    ROOT::VecOps::RVec<int> tempant(*aID); // point at vector<int>
    ROOT::VecOps::RVec<double> tempt(*tvec); // point at vector<double>
    ROOT::VecOps::RVec<double> tempv(*vvec); // point at vector<double>
    // recover single values in loop
    eID = *eventID;
    tID = *trackID;
    npx = *px;
    npy = *py;
    npz = *pz;
    nke = *ke;
    npa = *pangle;
    sampling = conf.sampletime; // const from config

    for (int i=0;i<conf.nantenna; ++i) {
      antID = i;
      auto selecttvec = tempt[tempant==i]; // selection
      auto selectvvec = tempv[tempant==i]; // with antenna ID
      // temporary vector for function call
      std::vector<double> tt(selecttvec.begin(), selecttvec.end());
      std::vector<double> tv(selectvvec.begin(), selectvvec.end());
      // function call
      interpolatePerTraj(tt, tv, resampled);
      trout->Fill(); // have all data, stream a row for output.
      // clear storage
      resampled.clear();
    }
  }
  // done all input values
  fout->Write();
  fout->Close();

  fin.Close();
}

#include <iostream>
#include <fstream>
#include <string>
void steer(const char* filelist) {
  int counter=0;
  std::string line, fin, fout;

  ifstream myfile(filelist);
  if (myfile.is_open()) {
    while (getline(myfile, line)) {
      fin = line.substr(0,line.rfind("."));
      fout = fin + "_recon_" + std::to_string(counter) + ".root";
      reconstruct(line.data(), fout.data());
      std::cout << fout << std::endl;
      counter++;
    }
    myfile.close();
  }
  else
    std::cout << "file open error." << std::endl;
}

