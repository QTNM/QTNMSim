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
#include "globals.hh"

// std
#include <stdlib.h>
#include <vector>
#include <utility>

class QTTrajectory : public G4VTrajectory
{
  using VTcontainer = std::vector<std::pair<double,double>>;
  using G4TrajectoryPointContainer = std::vector<G4VTrajectoryPoint*>;

public:
  QTTrajectory(const G4Track* aTrack, std::vector<G4double>& ang);
  virtual ~QTTrajectory();

  virtual void ShowTrajectory(std::ostream& os = G4cout) const;
  virtual void DrawTrajectory() const;
  virtual void AppendStep(const G4Step* aStep);
  virtual void MergeTrajectory(G4VTrajectory* secondTrajectory);

  inline void* operator new(size_t);
  inline void  operator delete(void*);
  inline int   operator==(const QTTrajectory& right) const { return (this == &right); }

  // access
  VTcontainer   getVT(G4int iAntenna) {return fVT[iAntenna];};
  G4ThreeVector getVPosition() {return vpos;}
  G4ThreeVector getVMomDir()   {return vmom;}
  G4double      getVEnergy()   {return venergy;}

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
  inline G4ThreeVector GetInitialMomentum() const
    { return initialMomentum; }

  virtual G4int GetPointEntries() const
    { return G4int(positionRecord->size()); }
  virtual G4VTrajectoryPoint* GetPoint(G4int i) const
    { return (*positionRecord)[i]; }

private:
  std::pair<double,double> convertToVT(unsigned int which);
  G4double               gltime;  // global time
  G4double               fAntennaRad; // antenna radial distance from origin
  G4ThreeVector          fAntennaNormal; // antenna normal towards origin
  G4ThreeVector          fAntennaPos;    // antenna position
  G4ThreeVector          pos;     // trajectory position
  G4ThreeVector          mom;     // trajectory momentum
  G4ThreeVector          beta;    // trajectory velocity
  G4ThreeVector          acc;     // trajectory acceleration

  G4double               venergy; // vertex kinetic energy
  G4ThreeVector          vpos;    // Vertex position vector
  G4ThreeVector          vmom;    // Vertex momentum vector
  std::vector<G4double>& fAngles; // from geometry
  VTcontainer*           fVT;     // array, Cyclotron radiation
  G4FieldManager*        pfieldManager; // singleton for info

  G4TrajectoryPointContainer* positionRecord = nullptr;
  G4int                       fTrackID = 0;
  G4int                       fParentID = 0;
  G4int                       PDGEncoding = 0;
  G4double                    PDGCharge = 0.0;
  G4String                    ParticleName = "";
  G4ThreeVector               initialMomentum;
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
