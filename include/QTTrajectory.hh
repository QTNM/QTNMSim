#ifndef QTTrajectory_h
#define QTTrajectory_h 1

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

class QTTrajectory : public G4VTrajectory
{
  using VTcontainer = std::vector<std::pair<double,double>>;

public:
  QTTrajectory(const G4Track* aTrack, std::vector<G4double>& ang);
  ~QTTrajectory() override;

  virtual void ShowTrajectory(std::ostream& os = G4cout) const;
  virtual void DrawTrajectory() const;
  virtual G4VTrajectoryPoint* GetPoint(G4int) const;
  virtual G4int GetPointEntries() const;
  virtual void AppendStep(const G4Step* aStep);
  virtual void MergeTrajectory(G4VTrajectory* secondTrajectory);

  inline void* operator new(size_t);
  inline void  operator delete(void*);
  inline int   operator==(const QTTrajectory& right) const { return (this == &right); }

  // access
  VTcontainer&           getVT() {return fVT;};
  std::vector<G4int>&    getAntennaID() {return fAntennaID;};
  std::vector<G4double>& getKE() {return fKE;};

  inline G4int GetTrackID() const
    { return fTrackID; }
  inline G4int GetParentID() const
    { return fParentID; }
  inline G4String GetParticleName() const
    { return ParticleName; }
  inline G4double GetCharge() const
    { return PDGCharge; }
  inline G4int GetPDGEncoding() const
    { return PDGEncoding; }
  inline G4double GetInitialEnergy() const
    { return initialEnergy; }
  inline G4ThreeVector GetInitialPosition() const
    { return initialPos; }
  inline G4ThreeVector GetInitialMomentum() const
    { return initialMomentum; }

private:
  std::pair<double,double> convertToVT(unsigned int which);
  G4double               gltime;  // global time
  G4double               fAntennaRad; // antenna radial distance from origin
  G4ThreeVector          pos;     // trajectory position
  G4ThreeVector          beta;    // trajectory velocity
  G4ThreeVector          acc;     // trajectory acceleration

  std::vector<G4double>  fAngles;    // from geometry
  std::vector<G4int>     fAntennaID; // antenna ID parallel to VTcontainer entries
  std::vector<G4double>  fKE;        // KE parallel to VTcontainer entries
  VTcontainer            fVT;        // container, Cyclotron radiation pairs, time, voltage

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

  // explicit SI units here transparent
  static constexpr G4double c_SI    = c_light/(m/s);
  static constexpr G4double eps0_SI = epsilon0 / farad * m;

};

extern G4TRACKING_DLL G4Allocator<QTTrajectory>*& myTrajectoryAllocator();

inline void* QTTrajectory::operator new(size_t)
{
  if(myTrajectoryAllocator() == nullptr)
  {
    myTrajectoryAllocator() = new G4Allocator<QTTrajectory>;
  }
  return (void*) myTrajectoryAllocator()->MallocSingle();
}

inline void QTTrajectory::operator delete(void* aTrajectory)
{
  myTrajectoryAllocator()->FreeSingle((QTTrajectory*) aTrajectory);
}

#endif
