#include "QTComsolField.hh"

// us
#include "streams.hh"

// std
#include <iostream>
#include <string>
#include <cstring>
// #include <algorithm>

// G4
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4AutoLock.hh"
#include "G4ThreeVector.hh"

namespace{
  G4Mutex myComsolFieldLock = G4MUTEX_INITIALIZER;
}

QTComsolField::QTComsolField(G4String& name)
  : fname(name)
{
  readGzipCSV();

}


QTComsolField::~QTComsolField()
{
  delete ftree; ftree = nullptr;
}


void
QTComsolField::readGzipCSV()
{
  std::string value; // needed for std::stod()
  double x, y, z, bx, by, bz; // data items
  std::vector<point3d> coords; // data array
  constexpr std::size_t BUFFSIZE = 256;
  //  std::vector<G4ThreeVector> cdummy;

  //This is a thread-local class and we have to avoid that all workers open the 
  //file at the same time; from PurgingMag example in G4 Advanced
  G4AutoLock lock(&myComsolFieldLock);

  char * buff = new char[BUFFSIZE]();
  
  gzip_streambuf gzbuf{fname.data()};
  std::istream is{&gzbuf};
  
  while(is.getline(buff, BUFFSIZE)) {
    value = strtok(buff, ", "); // can be , or space here

    // header ignore, COMSOL specific
    if (value=="%") {
      //      std::cout << "header line " << std::endl;
      continue;
    }
    else {
      // convert to double, structure from COMSOL csv line
      // coordinates
      x = std::stod(value);
      value = strtok(NULL, ","); // next
      y = std::stod(value);
      value = strtok(NULL, ","); // next
      z = std::stod(value);
      coords.push_back(point3d({x,y,z}));
      //      cdummy.push_back(G4ThreeVector(x*m,y*m,z*m));
      // B-field values
      value = strtok(NULL, ","); // next
      bx= std::stod(value);
      value = strtok(NULL, ","); // next
      by = std::stod(value);
      value = strtok(NULL, ","); // next
      bz = std::stod(value);
      // set to Tesla for G4 internal units
      fBfieldMap.push_back(G4ThreeVector(bx*tesla,by*tesla,bz*tesla));
    }
  }
  delete [] buff;
  lock.unlock();

  G4cout << "read values: " << coords.size() << G4endl;
  // G4cout << "max c: " << *std::max_element(cdummy.begin(), cdummy.end())/mm << G4endl;
  // G4cout << "min c: " << *std::min_element(cdummy.begin(), cdummy.end())/mm << G4endl;
  // G4cout << "max B: " << *std::max_element(fBfieldMap.begin(), fBfieldMap.end())/tesla << G4endl;
  // G4cout << "min B: " << *std::min_element(fBfieldMap.begin(), fBfieldMap.end())/tesla << G4endl;
  ftree = new tree3d(coords); // kd-tree construction
}

void QTComsolField::GetFieldValue (const G4double yIn[7],
				       G4double *B  ) const 
{
  // get distance and ID from kd-tree, 8 nearest
  // distance in COMSOL units from file, same for BfieldMap
  point3d pt({yIn[0]/m, yIn[1]/m, yIn[2]/m}); // Comsol unit [m] from G4 unit [mm]
  std::vector<std::pair<double, int> > di = ftree->kn_distance_id(pt, 8);
  std::vector<G4ThreeVector> allBvectors;
  G4ThreeVector genvec;
  G4double dsum = 0.0;
  for (auto entry : di) {
    int idx = entry.second;
    genvec.set(fBfieldMap[idx].x(),
	       fBfieldMap[idx].y(),
	       fBfieldMap[idx].z());
    allBvectors.push_back(genvec);
    dsum += entry.first;
  }
  G4double denom = 0.0;
  for (auto entry : di) denom += (1.0-entry.first / dsum);
    
  G4ThreeVector weightedvec;
  for (unsigned int j=0;j<allBvectors.size();++j) {
    G4double weight = 1.0-di[j].first / dsum;
    genvec = allBvectors.at(j)*(weight / denom);
    weightedvec += genvec;
  }
  B[0] = weightedvec.x();
  B[1] = weightedvec.y();
  B[2] = weightedvec.z();

}
