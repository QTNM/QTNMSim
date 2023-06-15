#ifndef QTPrimaryGeneratorAction_h
#define QTPrimaryGeneratorAction_h 1

// std lib
#include <random>

// G4
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GenericMessenger.hh"
#include "Randomize.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;


/// Primary generator
///
/// A single particle is generated.
/// macro commands can change primary properties.

class QTPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  QTPrimaryGeneratorAction();
  virtual ~QTPrimaryGeneratorAction();

  virtual void GeneratePrimaries(G4Event*);

private:

  void DefineCommands();

  G4ParticleGun*      fParticleGun;
  G4GenericMessenger* fMessenger;

  G4bool              fTestElectron;
  G4bool              fGunType;
  // electron gun parameter
  G4double            fMean;
  G4double            fStdev;
  G4double            fSpot;
  // tritium beta decay parameter
  G4bool              fOrder;
  G4double            fNumass;
  G4double            fSterilemass;
  G4double            fSterilemixing;

  std::ranlux24       generator;
  std::random_device  rd; // for random seeds
};

// us
#include "TBetaGenerator.hh"

// build generation functor
// operator calls eactly one 
// distribution function.
// Make different functor for 
// alternative distribution.
class betaGenerator {
private:
  bool order_;
  double munu_;
  double mNu_;
  double mixing_;

public:
  // fix parameter at construction
  betaGenerator(bool o, double ml, double mn, double eta) :
    order_(o),
    munu_(ml),
    mNu_(mn),
    mixing_(eta) {} // with parameter constructor

  double operator() (double x) {
    return TBeta::dGammadE(order_, munu_, mNu_, mixing_, x);
  }
};

#endif
