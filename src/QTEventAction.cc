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


QTEventAction::QTEventAction(QTOutputManager* out)
  : G4UserEventAction()
  , fOutput(out)
{
}


QTEventAction::~QTEventAction() = default;



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
  std::vector<double> tedep, ttime, tkine1, tkine2, px, py, posx, posy, posz;
  std::vector<int> tid;

  // fill Hits output from SD
  G4int GnofHits = GasHC->entries();
  //  G4cout << "PRINT>>> number of hits: " << GnofHits << G4endl;
  // Gas detector
  for ( G4int i=0; i<GnofHits; i++ ) 
  {
    auto hh = dynamic_cast<QTGasHit*>(GasHC->GetHit(i));

    int    id = (hh->GetTrackID());
    double e  = (hh->GetEdep()) / G4Analysis::GetUnitValue("keV");
    double tt = (hh->GetTime()) / G4Analysis::GetUnitValue("ns");
    double k1  = (hh->GetPreKine()) / G4Analysis::GetUnitValue("keV");
    double k2  = (hh->GetPostKine()) / G4Analysis::GetUnitValue("keV");
    double mx = (hh->GetPreTheta()); // pitch angle from momentum
    double my = (hh->GetPostTheta());
    double lx = (hh->GetPosx()); // interaction location
    double ly = (hh->GetPosy());
    double lz = (hh->GetPosz());

    tid.push_back(id);
    tedep.push_back(e);
    ttime.push_back(tt);
    tkine1.push_back(k1);
    tkine2.push_back(k2);
    px.push_back(mx);
    py.push_back(my);
    posx.push_back(lx);
    posy.push_back(ly);
    posz.push_back(lz);
  }

  // fill the ntuple - check column id?
  G4int eventID = event->GetEventID();
  for (unsigned int i=0;i<tedep.size();i++)
  {
    fOutput->FillNtupleI(0, 0, eventID); // repeat all rows
    fOutput->FillNtupleI(0, 1, tid.at(i));
    fOutput->FillNtupleD(0, 2, tedep.at(i));
    fOutput->FillNtupleD(0, 3, ttime.at(i));
    fOutput->FillNtupleD(0, 4, tkine1.at(i));
    fOutput->FillNtupleD(0, 5, tkine2.at(i));
    fOutput->FillNtupleD(0, 6, px.at(i));
    fOutput->FillNtupleD(0, 7, py.at(i));
    fOutput->FillNtupleD(0, 8, posx.at(i));
    fOutput->FillNtupleD(0, 9, posy.at(i));
    fOutput->FillNtupleD(0, 10, posz.at(i));
    fOutput->AddNtupleRow(0);
  }
  // next fill vectors from trajectory store, i.e. stored G4Steps

  G4TrajectoryContainer* trajectoryContainer = event->GetTrajectoryContainer();
  G4int                  n_trajectories =
    (trajectoryContainer == nullptr) ? 0 : trajectoryContainer->entries();

  //  G4cout << "PRINT>>> number of trajectories: " << n_trajectories << G4endl;
  
  if(n_trajectories > 0) {
    for(auto* entry : *(trajectoryContainer->GetVector())) {  // vector<G4VTrajectory*>*
      QTTrajectory* trj = dynamic_cast<QTTrajectory*>(entry);
      G4int counter = 0;
      for (auto value : trj->getKE()) {
	fOutput->FillKEVec(value / G4Analysis::GetUnitValue("keV"));
	fOutput->FillTimeVec((trj->getTime()).at(counter));
	fOutput->FillXVec((trj->getXpos()).at(counter) / G4Analysis::GetUnitValue("m"));
	fOutput->FillYVec((trj->getYpos()).at(counter) / G4Analysis::GetUnitValue("m"));
	fOutput->FillZVec((trj->getZpos()).at(counter) / G4Analysis::GetUnitValue("m"));
	fOutput->FillBetaXVec((trj->getBetaX()).at(counter));
	fOutput->FillBetaYVec((trj->getBetaY()).at(counter));
	fOutput->FillBetaZVec((trj->getBetaZ()).at(counter));
	fOutput->FillAccXVec((trj->getAccX()).at(counter));
	fOutput->FillAccYVec((trj->getAccY()).at(counter));
	fOutput->FillAccZVec((trj->getAccZ()).at(counter));
	counter++;
      }
        
      // fill the ntuple, n antenna data for each trajectory
      fOutput->FillNtupleI(1, 0, eventID); // repeat all rows
      fOutput->FillNtupleI(1, 1, trj->GetTrackID()); // trajectory specific
      // vertex info first in row
      G4ThreeVector p = trj->GetInitialPosition();
      fOutput->FillNtupleD(1, 2, p.x()); // [mm] default
      fOutput->FillNtupleD(1, 3, p.y());
      fOutput->FillNtupleD(1, 4, p.z());
      G4ThreeVector mom = trj->GetInitialMomentum();
      fOutput->FillNtupleD(1, 5, mom.theta()); // angle to z-axis
      fOutput->FillNtupleD(1, 6, trj->GetInitialEnergy() / G4Analysis::GetUnitValue("keV"));
      
      // Note no need to call FillNtupleDColumn for vector types
      // Filled automatically on call to AddNtupleRow
      // assumes clearing of storage vectors after streaming.
      fOutput->AddNtupleRow(1);
      
    }
  }
}
