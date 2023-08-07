#ifndef QTComsolField_h
#define QTComsolField_h 1

#include "G4MagneticField.hh"

class QTComsolField : public G4MagneticField
{
public:  // with description
  
  QTComsolField(G4String& s );

  virtual ~QTComsolField() = default;

  virtual void GetFieldValue(const G4double yTrack[4],
			     G4double *MagField) const ;

private:
  // reader interface for compressed (gzip) csv files 
  void readGzipCSV();
  void setupDataStructure();

  G4String fname;
};
#endif
