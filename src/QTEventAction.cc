#include "QTEventAction.hh"
#include "QTTrajectory.hh"
#include "QTGasSD.hh"

#include <vector>

#include "G4Event.hh"
#include "G4TrajectoryContainer.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"
#include "G4AnalysisManager.hh"


QTEventAction::QTEventAction(G4int na)
  : G4UserEventAction()
  , nAntenna(na)
{
}


QTEventAction::~QTEventAction()
{
}


QTGasHitsCollection* 
QTEventAction::GetGasHitsCollection(G4int hcID,
                                    const G4Event* event) const
{
  auto hitsCollection 
    = static_cast<QTGasHitsCollection*>(
        event->GetHCofThisEvent()->GetHC(hcID));
  
  if ( ! hitsCollection ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hcID; 
    G4Exception("QTEventAction::GetGasHitsCollection()",
      "MyCode0001", FatalException, msg);
  }         

  return hitsCollection;
}    


void QTEventAction::BeginOfEventAction(const G4Event*
                                         /*event*/)
{ ; }

void QTEventAction::EndOfEventAction(const G4Event* event)
{
  // Get GAS hits collections IDs
  if(fGID < 0) 
    fGID = G4SDManager::GetSDMpointer()->GetCollectionID("GasHitsCollection");

  // Get entries from hits collections
  //
  auto GasHC     = GetGasHitsCollection(fGID, event);

  // dummy storage
  std::vector<double> tedep, ttime, tkine, px, py, pz, posx, posy, posz;
  std::vector<int> tid;

  // get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // fill Hits output from SD
  G4int GnofHits = GasHC->entries();
  G4cout << "PRINT>>> number of hits: " << GnofHits << G4endl;
  // Gas detector
  for ( G4int i=0; i<GnofHits; i++ ) 
  {
    auto hh = (*GasHC)[i];

    int    id = (hh->GetTrackID());
    double e  = (hh->GetEdep()) / G4Analysis::GetUnitValue("keV");
    double tt = (hh->GetTime()) / G4Analysis::GetUnitValue("ns");
    double k  = (hh->GetKine()) / G4Analysis::GetUnitValue("keV");
    double mx = (hh->GetPx()); // normalised momentum direction vector
    double my = (hh->GetPy());
    double mz = (hh->GetPz());
    double lx = (hh->GetPosx()); // interaction location
    double ly = (hh->GetPosy());
    double lz = (hh->GetPosz());

    tid.push_back(id);
    tedep.push_back(e);
    ttime.push_back(tt);
    tkine.push_back(k);
    px.push_back(mx);
    py.push_back(my);
    pz.push_back(mz);
    posx.push_back(lx);
    posy.push_back(ly);
    posz.push_back(lz);
  }

  // fill the ntuple - check column id?
  G4int eventID = event->GetEventID();
  for (unsigned int i=0;i<tedep.size();i++)
  {
    analysisManager->FillNtupleIColumn(0, 0, eventID); // repeat all rows
    analysisManager->FillNtupleIColumn(0, 1, tid.at(i));
    analysisManager->FillNtupleDColumn(0, 2, tedep.at(i));
    analysisManager->FillNtupleDColumn(0, 3, ttime.at(i));
    analysisManager->FillNtupleDColumn(0, 4, tkine.at(i));
    analysisManager->FillNtupleDColumn(0, 5, px.at(i));
    analysisManager->FillNtupleDColumn(0, 6, py.at(i));
    analysisManager->FillNtupleDColumn(0, 7, pz.at(i));
    analysisManager->FillNtupleDColumn(0, 8, posx.at(i));
    analysisManager->FillNtupleDColumn(0, 9, posy.at(i));
    analysisManager->FillNtupleDColumn(0, 10, posz.at(i));
    analysisManager->AddNtupleRow(0);
  }
  // next fill vectors from trajectory store, i.e. stored G4Steps

  G4cout << "PRINT>>> just before n of trajectories: " << G4endl;
  G4TrajectoryContainer* trajectoryContainer = event->GetTrajectoryContainer();
  G4int                  n_trajectories =
    (trajectoryContainer == nullptr) ? 0 : trajectoryContainer->entries();

  G4cout << "PRINT>>> number of trajectories: " << n_trajectories << G4endl;
  
  if(n_trajectories > 0) {
    for(auto* entry : *(trajectoryContainer->GetVector())) {  // vector<G4VTrajectory*>*
      //      QTTrajectory* trj = (QTTrajectory*) ((*(event->GetTrajectoryContainer()))[i]);
      QTTrajectory* trj = dynamic_cast<QTTrajectory*>(entry);
      G4int counter = 0;
      for (G4int j=0;j<nAntenna;++j) {    // nAntenna * step pairs
	for (auto entry : trj->getVT()) {  // std::pair<double,double>
	  avec.push_back((trj->getAntennaID()).at(counter));
	  tvec.push_back(entry.first);
	  vvec.push_back(entry.second);
	  counter++;
	}
      }
        
      // fill the ntuple, n antenna data for each trajectory
      analysisManager->FillNtupleIColumn(1, 0, eventID); // repeat all rows
      // Note no need to call FillNtupleDColumn for vector types
      // Filled automatically on call to AddNtupleRow
      analysisManager->AddNtupleRow(1);

      // clear antenna signals
      avec.clear();
      tvec.clear();
      vvec.clear();
    }
  }
}
