/// Definition of the QTDetectorConstruction class
//
//
//
//

#ifndef _QTDETECTORCONSTRUCTION_H_
#define _QTDETECTORCONSTRUCTION_H_

#include "G4VUserDetectorConstruction.hh"
#include "G4Cache.hh"
#include "G4GDMLParser.hh"
#include "G4GenericMessenger.hh"


/// Detector construction allowing to use the geometry read from the GDML file
class QTGasSD;
class QTMagneticFieldSetup;
// add a user limit to world = maximum time limit for transport
class G4UserLimits;

class QTDetectorConstruction : public G4VUserDetectorConstruction
{
public:
  
  QTDetectorConstruction(const G4GDMLParser&);
  ~QTDetectorConstruction() override;
  
  virtual G4VPhysicalVolume* Construct();

  virtual void               ConstructSDandField();
  
  // access for questions
  const G4GDMLParser&        GetParserRef() {return fparser;}

private:

  void DefineCommands();
  
  G4UserLimits*       fUserLimit = nullptr; // max time and max step limit
  G4GenericMessenger* fMessenger = nullptr;
  G4double            fMaxTime;

  const G4GDMLParser& fparser;
  G4Cache<QTGasSD*>   fSD = nullptr;
  G4Cache<QTMagneticFieldSetup*>  fEmFieldSetup = nullptr;

};
#endif
