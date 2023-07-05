#include "QTEventAction.hh"
#include "QTOutputManager.hh"
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


QTEventAction::QTEventAction(G4int na, QTOutputManager* out)
  : G4UserEventAction()
  , fOutput(out)
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
{ 
}

void QTEventAction::EndOfEventAction(const G4Event* event)
{
  // Get GAS hits collections IDs
  if(fGID < 0) 
    fGID = G4SDManager::GetSDMpointer()->GetCollectionID("GasHitsCollection");

  // Get entries from hits collections
  //
  auto GasHC     = GetGasHitsCollection(fGID, event);

  // dummy hit storage
  std::vector<double> tedep, ttime, tkine, px, py, pz, posx, posy, posz;
  std::vector<int> tid;

  // fill Hits output from SD
  G4int GnofHits = GasHC->entries();
  G4cout << "PRINT>>> number of hits: " << GnofHits << G4endl;
  // Gas detector
  for ( G4int i=0; i<GnofHits; i++ ) 
  {
    //    auto hh = (*GasHC)[i];
    G4cout << "PRINT>>> access QTGasHit: " << G4endl;
    auto hh = dynamic_cast<QTGasHit*>(GasHC->GetHit(i));
    G4cout << "PRINT>>> got QTGasHit: access id " << hh->GetTrackID() << G4endl;

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
  G4cout << "PRINT>>> Before filling ntuple. " << G4endl;
  for (unsigned int i=0;i<tedep.size();i++)
  {
    fOutput->FillNtupleI(0, 0, eventID); // repeat all rows
    G4cout << "PRINT>>> filled first IColumn " << G4endl;
    fOutput->FillNtupleI(0, 1, tid.at(i));
    G4cout << "PRINT>>> filled tid IColumn " << G4endl;
    fOutput->FillNtupleD(0, 2, tedep.at(i));
    fOutput->FillNtupleD(0, 3, ttime.at(i));
    fOutput->FillNtupleD(0, 4, tkine.at(i));
    fOutput->FillNtupleD(0, 5, px.at(i));
    fOutput->FillNtupleD(0, 6, py.at(i));
    fOutput->FillNtupleD(0, 7, pz.at(i));
    fOutput->FillNtupleD(0, 8, posx.at(i));
    fOutput->FillNtupleD(0, 9, posy.at(i));
    fOutput->FillNtupleD(0, 10, posz.at(i));
    fOutput->AddNtupleRow(0);
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
	for (auto values : trj->getVT()) {  // std::pair<double,double>
	  fOutput->FillTIDVec(trj->GetTrackID()); // same for every trajectory
	  fOutput->FillAntennaVec((trj->getAntennaID()).at(counter)); // same size as
	  fOutput->FillTimeVec(values.first);                         // VT container
	  fOutput->FillVoltageVec(values.second);
	  counter++;
	}
      }
        
      // fill the ntuple, n antenna data for each trajectory
      fOutput->FillNtupleI(1, 0, eventID); // repeat all rows
      // Note no need to call FillNtupleDColumn for vector types
      // Filled automatically on call to AddNtupleRow
      // assumes clearing of storage vectors after streaming.
      fOutput->AddNtupleRow(1);
      
    }
  }
}
