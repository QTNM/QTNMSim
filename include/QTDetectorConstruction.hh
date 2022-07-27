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

/// Detector construction allowing to use the geometry read from the GDML file
class QTGasSD;

class QTDetectorConstruction : public G4VUserDetectorConstruction
{
public:
  
  QTDetectorConstruction(const G4GMDLParser&);
  
  virtual G4VPhysicalVolume* Construct();

  virtual void               ConstructSDandField();
  
  // access for questions
  const G4GDMLParser&        GetParserRef() {return fparser;}

private:
  
  const G4GDMLParser& fparser;
  G4Cache<QTGasSD*>   fSD = nullptr;
};
#endif
