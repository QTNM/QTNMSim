#include "QTGasSD.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4VProcess.hh"
#include "G4ios.hh"

QTGasSD::QTGasSD(const G4String& name,
                 const G4String& hitsCollectionName) 
 : G4VSensitiveDetector(name),
   fHitsCollection(NULL)
{
  collectionName.insert(hitsCollectionName);
}

QTGasSD::~QTGasSD() = default;


void QTGasSD::Initialize(G4HCofThisEvent* hce)
{
  // Create hits collection

  fHitsCollection 
    = new QTGasHitsCollection(SensitiveDetectorName, collectionName[0]); 

  // Add this collection in hce

  G4int hcID 
    = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection( hcID, fHitsCollection ); 
}

G4bool QTGasSD::ProcessHits(G4Step* aStep, 
                            G4TouchableHistory*)
{  
// process filter
  auto* vprocess = aStep->GetPostStepPoint()->GetProcessDefinedStep();
  if (vprocess->GetProcessName()=="Transportation") return false;

  // pitch angle change condition < 0.057 degrees
  G4ThreeVector premom  = aStep->GetPreStepPoint()->GetMomentumDirection();
  G4ThreeVector postmom = aStep->GetPostStepPoint()->GetMomentumDirection();
  if (fabs(premom.theta()-postmom.theta()) < 1.0e-3) return false;
  
  G4double postkine = aStep->GetPostStepPoint()->GetKineticEnergy();
  if (postkine/CLHEP::eV < 1.0) {
    G4cout << ">>>SD >>> no post step kinetic energy left, not stored" << G4endl;
    return false; // stopped electron by max time cut, not interested
  }
  QTGasHit* newHit = new QTGasHit();
  G4ThreeVector preloc = aStep->GetPreStepPoint()->GetPosition();

  newHit->SetTrackID(aStep->GetTrack()->GetTrackID());
  newHit->SetEdep(aStep->GetTotalEnergyDeposit());
  newHit->SetTime(aStep->GetTrack()->GetGlobalTime());
  newHit->SetPreKine(aStep->GetPreStepPoint()->GetKineticEnergy());
  newHit->SetPostKine(aStep->GetPostStepPoint()->GetKineticEnergy());
  newHit->SetPreTheta(premom.theta());
  newHit->SetPostTheta(postmom.theta());
  newHit->SetPosx(preloc.x());
  newHit->SetPosy(preloc.y());
  newHit->SetPosz(preloc.z());

  fHitsCollection->insert( newHit );

  return true;
}

void QTGasSD::EndOfEvent(G4HCofThisEvent*)
{
  if ( verboseLevel>1 ) { 
     G4int nofHits = fHitsCollection->entries();
     G4cout << G4endl
            << "-------->Hits Collection: in this event there are " << nofHits 
            << " hits in the gas: " << G4endl;
  }
}


