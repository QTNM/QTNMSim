cmake_minimum_required(VERSION 3.12)
project(qtnmSim VERSION 0.1.0)

# Compiler/build settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)

# CTest if we need it
# include(CTest)

# Control building of tests
option(BUILD_TESTS "Build test applications" OFF)

# Dependencies
find_package(Geant4 11.2 REQUIRED gdml)

# Look for Boost
#TODO This should be a required package if using clang
find_package(Boost)

if(Boost_FOUND)
  add_compile_definitions(HAVE_BOOST)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

# Project ROOT directory for reading data
add_compile_definitions("SOURCE_ROOT=${CMAKE_SOURCE_DIR}")

# Build
add_library(qtnmSimlib
  src/QTDetectorConstruction.cc
  src/QTBorisScheme.cc
  src/QTBorisDriver.cc
  src/QTFieldMessenger.cc
  src/QTLarmorEMField.cc
  src/QTLarmorUniField.cc
  src/QTMagneticFieldSetup.cc
  src/QTMagneticTrap.cc
  src/QTComsolField.cc
  src/gzip_streambuf.cc
  src/QTRunAction.cc
  src/QTEquationOfMotion.cc
  src/QTGasHit.cc
  src/QTOutputManager.cc
  src/QTTrackingAction.cc
  src/QTActionInitialization.cc
  src/QTEventAction.cc
  src/QTGasSD.cc
  src/QTPrimaryGeneratorAction.cc
  src/QTTrajectory.cc
  src/QTNMeImpactIonisation.cc)
target_include_directories(qtnmSimlib PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/include/utils)
target_link_libraries(qtnmSimlib PRIVATE ${Geant4_LIBRARIES})

add_executable(qtnmSim
  qtnmSim.cc
  src/QTNMPhysicsList.cc)
target_include_directories(qtnmSim PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/include/utils)
target_link_libraries(qtnmSim
  PRIVATE ${Geant4_LIBRARIES}
  PUBLIC qtnmSimlib)

# Build Tests if requested
if (BUILD_TESTS)
   add_subdirectory(${PROJECT_SOURCE_DIR}/test/Test0)
   add_subdirectory(${PROJECT_SOURCE_DIR}/test/Test1)
endif()
