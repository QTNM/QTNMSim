#ifndef QTComsolField_h
#define QTComsolField_h 1

#include "QTLarmorEMField.hh"
#include "G4ThreeVector.hh"
#include "kdtree.hh"

// from kdtree.hh
typedef point<double, 3> point3d;
typedef kdtree<double, 3> tree3d;

class QTComsolField : public QTLarmorEMField
{
public:  // with description
  
  QTComsolField(G4String& s );

  ~QTComsolField() override;

  void GetFieldValue(const G4double yTrack[4],
		     G4double *MagField) const override;

private:
  // reader interface for compressed (gzip) csv files 
  void readGzipCSV();

  tree3d* ftree = nullptr;
  std::vector<G4ThreeVector> fBfieldMap; // data array

  G4String fname;
};
#endif
