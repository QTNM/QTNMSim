#ifndef NATrajectory_h
#define NATrajectory_h 1

#include "trkgdefs.hh"
#include "G4Allocator.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4VTrajectory.hh"
#include "G4ios.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "globals.hh"

// us
#include "QTEquationOfMotion.hh"

// std
#include <stdlib.h>
#include <vector>
#include <utility>

class NATrajectory : public G4VTrajectory
{
public:
  NATrajectory(const G4Track* aTrack);
  ~NATrajectory() override;

  virtual void ShowTrajectory(std::ostream& os = G4cout) const override;
  virtual void DrawTrajectory() const override;
  virtual G4VTrajectoryPoint* GetPoint(G4int) const override;
  virtual G4int GetPointEntries() const override;
  virtual void AppendStep(const G4Step* aStep) override;
  virtual void MergeTrajectory(G4VTrajectory* secondTrajectory) override;

  inline void* operator new(size_t);
  inline void  operator delete(void*);
  inline int   operator==(const NATrajectory& right) const { return (this == &right); }

  // access
  std::vector<G4double>& getOm()    {return fOm;};
  std::vector<G4double>& getKE()    {return fKE;};
  std::vector<G4double>& getTime()  {return ft;};
  std::vector<G4double>& getXpos()  {return xp;};
  std::vector<G4double>& getYpos()  {return yp;};
  std::vector<G4double>& getZpos()  {return zp;};
  std::vector<G4double>& getBetaX() {return betax;};
  std::vector<G4double>& getBetaY() {return betay;};
  std::vector<G4double>& getBetaZ() {return betaz;};
  std::vector<G4double>& getAccX()  {return accx;};
  std::vector<G4double>& getAccY()  {return accy;};
  std::vector<G4double>& getAccZ()  {return accz;};

  inline G4int GetTrackID() const override
    { return fTrackID; }
  inline G4int GetParentID() const override
    { return fParentID; }
  inline G4String GetParticleName() const override
    { return ParticleName; }
  inline G4double GetCharge() const override
    { return PDGCharge; }
  inline G4int GetPDGEncoding() const override
    { return PDGEncoding; }
  inline G4double GetInitialEnergy() const
    { return initialEnergy; }
  inline G4ThreeVector GetInitialPosition() const
    { return initialPos; }
  inline G4ThreeVector GetInitialMomentum() const override
    { return initialMomentum; }

private:
  G4ThreeVector          getAcceleration();
  G4ThreeVector          getOmega();
  G4double               gltime;  // global time
  G4ThreeVector          pos;     // trajectory position
  G4ThreeVector          beta;    // trajectory velocity
  G4ThreeVector          acc;     // trajectory acceleration

  std::vector<G4double>  fOm;        // Omega
  std::vector<G4double>  fKE;        // KE
  std::vector<G4double>  ft;         // time 
  std::vector<G4double>  xp;         // pos.x
  std::vector<G4double>  yp;         // pos.y
  std::vector<G4double>  zp;         // pos.z
  std::vector<G4double>  betax;      // beta.x
  std::vector<G4double>  betay;      // beta.y
  std::vector<G4double>  betaz;      // beta.z
  std::vector<G4double>  accx;       // acc.x
  std::vector<G4double>  accy;       // acc.y
  std::vector<G4double>  accz;       // acc.z


  G4FieldManager*        pfieldManager; // singleton for info
  QTEquationOfMotion*    pEqn;          // info on particle

  G4int                       fTrackID = 0;
  G4int                       fParentID = 0;
  G4int                       PDGEncoding = 0;
  G4double                    PDGCharge = 0.0;
  G4String                    ParticleName = "";
  G4double                    initialEnergy = 0.0;
  G4ThreeVector               initialMomentum;
  G4ThreeVector               initialPos;

};

extern G4TRACKING_DLL G4Allocator<NATrajectory>*& myTrajectoryAllocator();

inline void* NATrajectory::operator new(size_t)
{
  if(myTrajectoryAllocator() == nullptr)
  {
    myTrajectoryAllocator() = new G4Allocator<NATrajectory>;
  }
  return (void*) myTrajectoryAllocator()->MallocSingle();
}

inline void NATrajectory::operator delete(void* aTrajectory)
{
  myTrajectoryAllocator()->FreeSingle((NATrajectory*) aTrajectory);
}

#endif
