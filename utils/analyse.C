// plot the reconstructed antenna data for event number and antenna ID.
void reconplot(const char* fname, int eventnr, int antID, double frac) {
  ROOT::RDataFrame df("SignalRecon", fname);
  auto evCut  = [&eventnr](int id){return id==eventnr;};
  auto aidCut = [&antID](int id){return id==antID;};
  auto filtered = df.Filter(evCut, {"eventID"}).Filter(aidCut, {"antennaID"});

  auto selection = filtered.Take<std::vector<double>>("VoltageVec");
  auto px = filtered.Take<double>("posx");
  auto py = filtered.Take<double>("posy");
  auto pz = filtered.Take<double>("posz");
  auto ke = filtered.Take<double>("kinenergy");
  auto pa = filtered.Take<double>("pitchangle");
  double pxv = px->at(0); // one constant in row
  double pyv = py->at(0); // one constant in row
  double pzv = pz->at(0); // one constant in row
  double kev = ke->at(0); // one constant in row
  double pav = pa->at(0); // one constant in row
  std::cout << "vertex pos: " << pxv << ", " << pyv << ", " << pzv << std::endl;
  std::cout << "ke= " << kev << " pitch = " << pav << std::endl;
  auto sampl = filtered.Take<double>("sampling");
  double samplingTime = sampl->at(0); // one constant in row

  TGraph* output = new TGraph();
  auto rowdata = selection->at(0); // has only one row in ntuple
  std::cout << "container size = " << rowdata.size() << std::endl;
  for (int i=0;i<(int)rowdata.size()*frac;++i) 
    output->AddPoint(i*samplingTime, rowdata.at(i));

  // plot
  auto cv = new TCanvas();
  output->SetMarkerStyle(7);
  output->DrawClone("ALP");
}

// plot the raw antenna data from Geant4 output for event number and antenna ID.
void rawplot(const char* fname, int eventnr, int antID) {
  TFile fin(fname, "read");
  TTreeReader trin("ntuple/Signal", &fin);

  TTreeReaderValue<int> eventID(trin, "EventID");
  TTreeReaderValue<std::vector<int>> aID(trin, "AntennaID");
  TTreeReaderValue<std::vector<double>> tvec(trin, "TimeVec");
  TTreeReaderValue<std::vector<double>> vvec(trin, "VoltageVec");

  int eID;
  while (trin.Next()) { // read row
    ROOT::VecOps::RVec<int> tempant(*aID); // point at vector<int>
    ROOT::VecOps::RVec<double> tempt(*tvec); // point at vector<double>
    ROOT::VecOps::RVec<double> tempv(*vvec); // point at vector<double>
    // recover single value in loop
    eID = *eventID;
    if (eID==eventnr) { // selections
      TGraph* gr = new TGraph();
      auto selecttvec = tempt[tempant==antID]; // selection
      auto selectvvec = tempv[tempant==antID]; // with antenna ID
      for (unsigned int i=0;i<selecttvec.size()-1;++i) 
	gr->AddPoint(selecttvec.at(i), selectvvec.at(i));
      gr->SetMarkerStyle(7);
      gr->DrawClone("ALP");
    }
  }
}

// plot kin energy data from Geant4 output for event number and antenna ID.
void keplot(const char* fname, int eventnr, int antID) {
  TFile fin(fname, "read");
  TTreeReader trin("ntuple/Signal", &fin);

  TTreeReaderValue<int> eventID(trin, "EventID");
  TTreeReaderValue<std::vector<int>> aID(trin, "AntennaID");
  TTreeReaderValue<std::vector<double>> tvec(trin, "TimeVec");
  TTreeReaderValue<std::vector<double>> kvec(trin, "KEVec");

  int eID;
  while (trin.Next()) { // read row
    ROOT::VecOps::RVec<int> tempant(*aID); // point at vector<int>
    ROOT::VecOps::RVec<double> tempt(*tvec); // point at vector<double>
    ROOT::VecOps::RVec<double> tempk(*kvec); // point at vector<double>
    // recover single value in loop
    eID = *eventID;
    if (eID==eventnr) { // selections
      TGraph* gr = new TGraph();
      auto selecttvec = tempt[tempant==antID]; // selection
      auto selectkvec = tempk[tempant==antID]; // with antenna ID
      for (unsigned int i=0;i<selecttvec.size()-1;++i) 
	gr->AddPoint(selecttvec.at(i), selectkvec.at(i));
      gr->SetMarkerStyle(7);
      gr->DrawClone("ALP");
    }
  }
}

// print kin energy data from Geant4 output for event number and antenna ID.
void keprint(const char* fname, int eventnr, int antID) {
  TFile fin(fname, "read");
  TTreeReader trin("ntuple/Signal", &fin);

  TTreeReaderValue<int> eventID(trin, "EventID");
  TTreeReaderValue<std::vector<int>> aID(trin, "AntennaID");
  TTreeReaderValue<std::vector<double>> tvec(trin, "TimeVec");
  TTreeReaderValue<std::vector<double>> kvec(trin, "KEVec");

  int eID;
  while (trin.Next()) { // read row
    ROOT::VecOps::RVec<int> tempant(*aID); // point at vector<int>
    ROOT::VecOps::RVec<double> tempt(*tvec); // point at vector<double>
    ROOT::VecOps::RVec<double> tempk(*kvec); // point at vector<double>
    // recover single value in loop
    eID = *eventID;
    if (eID==eventnr) { // selections
      auto selecttvec = tempt[tempant==antID]; // selection
      auto selectkvec = tempk[tempant==antID]; // with antenna ID
//      for (unsigned int i=0;i<selecttvec.size()-1;++i) 
//	std::cout << "(" << selecttvec.at(i) << ", " << selectkvec.at(i) << ") ";
      std::cout << "\n >>> last-first [eV]: " << (selectkvec.at(selectkvec.size()-2)-selectkvec.at(0))*1.e6 << std::endl;
    }
  }
}
