#include "QTComsolField.hh"

// us
#include "streams.hh"

// std
#include <iostream>
#include <cstring>

// G4
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4AutoLock.hh"

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

  //This is a thread-local class and we have to avoid that all workers open the 
  //file at the same time; from PurgingMag example in G4 Advanced
  G4AutoLock lock(&myComsolFieldLock);

  char * buff = new char[BUFFSIZE]();
  
  gzip_streambuf gzbuf{fname.c_str()};
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

      // B-field values
      value = strtok(NULL, ","); // next
      bx= std::stod(value);
      value = strtok(NULL, ","); // next
      by = std::stod(value);
      value = strtok(NULL, ","); // next
      bz = std::stod(value);
      fBfieldMap.push_back(point3d({bx,by,bz}));
    }
  }
  delete [] buff;
  lock.unlock();

  ftree = new tree3d(coords); // kd-tree construction
}

void QTComsolField::GetFieldValue (const G4double yIn[7],
				       G4double *B  ) const 
{
  G4double field[3];
  // get distance and ID from kd-tree, 8 nearest
  point3d pt({yIn[0], yIn[1], yIn[2]});
  std::vector<std::pair<double, int> > di = ftree->kn_distance_id(pt, 8);


}
