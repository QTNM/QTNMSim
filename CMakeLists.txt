cmake_minimum_required(VERSION 3.12)
project(qtnmSim VERSION 0.1.0)

# Compiler/build settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# CTest if we need it
# include(CTest)

# Dependencies
find_package(Geant4 11.1 REQUIRED gdml)

# Build
add_executable(qtnmSim
  qtnmSim.cc
  src/BorisStepper.cc
  src/QTDetectorConstruction.cc
  src/QTFieldMessenger.cc
  src/QTMagneticFieldSetup.cc
  src/QTRunAction.cc
  src/BorisStepperRadiation.cc
  src/QTEquationOfMotion.cc
  src/QTGasHit.cc
  src/QTNMPhysicsList.cc
  src/QTTrackingAction.cc
  src/QTActionInitialization.cc
  src/QTEventAction.cc
  src/QTGasSD.cc
  src/QTPrimaryGeneratorAction.cc
  src/QTTrajectory.cc
  src/QTNMPhysicsList.cc)
target_include_directories(qtnmSim PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(qtnmSim PRIVATE ${Geant4_LIBRARIES})

# Test
#if(BUILD_TESTING)
#  add_subdirectory(test)
#endif()
