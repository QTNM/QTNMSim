#ifndef QTComsolField_h
#define QTComsolField_h 1

#include "G4MagneticField.hh"
#include "kdtree.hh"

// from kdtree.hh
typedef point<double, 3> point3d;
typedef kdtree<double, 3> tree3d;

class QTComsolField : public G4MagneticField
{
public:  // with description
  
  QTComsolField(G4String& s );

  ~QTComsolField();

  virtual void GetFieldValue(const G4double yTrack[4],
			     G4double *MagField) const ;

private:
  // reader interface for compressed (gzip) csv files 
  void readGzipCSV();

  tree3d* ftree = nullptr;
  std::vector<point3d> fBfieldMap; // data array

  G4String fname;
};
#endif
