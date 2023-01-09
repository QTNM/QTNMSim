"""
QTNM bespoke magnet setup file

Build the setup of the QTNM experiment.
"""

# Third-party imports
import numpy as np
import pyg4ometry as pg4

# Our imports
from QMaterials import QMaterials


class QTNMBaseline(object):
    
    def __init__(self, targetDensity=5.e-12):
        '''
        Construct QTNM Baseline

        Parameters
        ----------
        targetDensity : optional, double
            forward gas atom cloud density to material definition;
            unit [g/cm^3], enters all gas material definitions.


        Returns
        -------
        None.
        
        After construction, drawGeometry() or writeGDML()
        or continue with the stored pyg4ometry registry (self.reg).

        '''
        self.fdensity = targetDensity

        # registry to store gdml data
        self.reg  = pg4.geant4.Registry()

        # dictionary of materials
        lm = QMaterials(self.reg, self.fdensity)
        # lm.PrintAll()
        self.materials = lm.getMaterialsDict()
        
        # build the geometry
        self._buildWorld()


    def __repr__(self):
        ''' Print the object purpose. '''
        s = "QTNM baseline object: build the complete setup.\n"
        return s
    


    def _buildWorld(self):
        '''
        Build the entire geometry using module objects, see imports.

        Parameters
        ----------
        None.

        Returns
        -------
        None. Stores geometry in registry.

        '''
        # default length unit [mm]
        # world solid and logical
        origin  = [0.0, 0.0, 0.0]

        zeroRad = 0.0
        outerRad = 40.0  # [mm] bore hole in HTS-110
        wheight = 200.0  # 20 cm bore hole
        worldSolid = pg4.geant4.solid.Tubs("ws",zeroRad,outerRad,
                                           wheight+1.0,
                                           0,2*np.pi,self.reg)
        self.worldLV = pg4.geant4.LogicalVolume(worldSolid,
                                                self.materials['vacuum'],
                                                "worldLV", self.reg)

        # build the steel pipe and heat shields
        pipeInside  = 38.0 # [mm] inner pipe radius
        pipeOutside = 39.0 # [mm] outer pipe radius

        distance    = 5.0  # [mm] radial gap between cylinders
        thin        = 0.5  # [mm] shield thickness
        hs30kInner  = pipeInside - distance
        hs30kOuter  = pipeInside - distance + thin
        hs4kInner   = pipeInside - 2*distance
        hs4kOuter   = pipeInside - 2*distance + thin
        aRingInner  = hs4kInner - 2*thin
        aRingOuter  = hs4kInner - thin
        aRingHeight = 10.0 # 1 cm antenna ring cylinder in z

        # DN63
        pipeSolid  = pg4.geant4.solid.Tubs("dn63",pipeInside,pipeOutside,
                                           wheight,
                                           0,2*np.pi,self.reg)

        pipeLV     = pg4.geant4.LogicalVolume(pipeSolid,
                                              self.materials["steel"],
                                              "pipeLV", self.reg)
        pg4.geant4.PhysicalVolume(origin,origin,pipeLV,"pipePV",
                                  self.worldLV,self.reg)

        # 30 K
        hs30kSolid  = pg4.geant4.solid.Tubs("30K",hs30kInner,hs30kOuter,
                                           wheight-1.0,
                                           0,2*np.pi,self.reg)

        hs30kLV     = pg4.geant4.LogicalVolume(hs30kSolid,
                                              self.materials["Cu"],
                                              "hs30kLV", self.reg)
        pg4.geant4.PhysicalVolume(origin,origin,hs30kLV,"hs30kPV",
                                  self.worldLV,self.reg)

        # 4 K
        hs4kSolid  = pg4.geant4.solid.Tubs("4K",hs4kInner,hs4kOuter,
                                           wheight-2.0,
                                           0,2*np.pi,self.reg)

        hs4kLV     = pg4.geant4.LogicalVolume(hs4kSolid,
                                              self.materials["Cu"],
                                              "hs4kLV", self.reg)
        pg4.geant4.PhysicalVolume(origin,origin,hs4kLV,"hs4kPV",
                                  self.worldLV,self.reg)

        # Antenna ring
        antennaSolid  = pg4.geant4.solid.Tubs("AntennaRing",aRingInner,aRingOuter,
                                           aRingHeight,
                                           0,2*np.pi,self.reg)

        antennaLV     = pg4.geant4.LogicalVolume(antennaSolid,
                                              self.materials["vacuum"],
                                              "AntennaLV", self.reg)
        pg4.geant4.PhysicalVolume(origin,origin,antennaLV,"AntennaPV",
                                  self.worldLV,self.reg)

        # gas clouds
        gasCylRadius = 2.0   # [mm] to be refined, eventually
        gasCylHeight = 100.0 # [mm] assumed 10 cm
        gasSolid  = pg4.geant4.solid.Tubs("Gas",zeroRad, gasCylRadius,
                                           gasCylHeight,
                                           0,2*np.pi,self.reg)
        
        gasLV     = pg4.geant4.LogicalVolume(gasSolid,
                                             self.materials["T"],
                                             "Gas_log", self.reg)

        # source at origin
        pg4.geant4.PhysicalVolume(origin,origin,gasLV,"gasPV",
                                  self.worldLV,self.reg)


        # Add auxiliary information as required for Geant4 app.
        # Antenna information first; type: angle; value: theta [deg] around cylinder
        aux1 = pg4.gdml.Defines.Auxiliary("angle","0.0",self.reg)
        aux2 = pg4.gdml.Defines.Auxiliary("angle","90.0",self.reg)
        antennaLV.addAuxiliaryInfo(aux1) # antenna 1
        antennaLV.addAuxiliaryInfo(aux2) # antenna 2

        det = pg4.gdml.Defines.Auxiliary("SensDet","cloud",self.reg)
        gasLV.addAuxiliaryInfo(det) # register for hits


    def drawGeometry(self):
        '''
        Draw the geometry held in the World volume.
        '''
        v = pg4.visualisation.VtkViewer()
        v.addLogicalVolume(self.worldLV)
        v.view()


    def writeGDML(self, filename):
        '''
        Create GDML file from stored pyg4ometry registry.

        Parameters
        ----------
        filename : string
            Full GDML file name for storing data, append '.gdml'.

        Returns
        -------
        None.

        '''
        self.reg.setWorld(self.worldLV)
        w = pg4.gdml.Writer()
        w.addDetector(self.reg)
        w.write(filename)
