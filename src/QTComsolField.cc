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
  setupDataStructure();
}

void
QTComsolField::readGzipCSV()
{
  std::string value; // needed for std::stod()
  std::vector<double> x, y, z, bx, by, bz; // data arrays
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
      x.push_back(std::stod(value));
      value = strtok(NULL, ","); // next
      y.push_back(std::stod(value));
      value = strtok(NULL, ","); // next
      z.push_back(std::stod(value));
      // B-field values
      value = strtok(NULL, ","); // next
      bx.push_back(std::stod(value));
      value = strtok(NULL, ","); // next
      by.push_back(std::stod(value));
      value = strtok(NULL, ","); // next
      bz.push_back(std::stod(value));
    }
  }
  delete [] buff;
  lock.unlock();
}

void QTComsolField::GetFieldValue (const G4double yIn[7],
				       G4double *B  ) const 
{

  G4double field[3];
}
