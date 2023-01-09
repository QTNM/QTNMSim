"""
Build all materials for QTNM Geometry structures.
"""
import pyg4ometry as pg4

class QMaterials(object):
    '''
    Define all QTNM Geometry materials.
    '''
    
    def __init__(self, reg=None, targetDensity=5.e-12):
        '''
        Build materials dictionary

        Parameters
        ----------
        reg : pg4.geant4.Registry, optional
            if None, standalone construction
        targetDensity : optional, double
            forward gas atom cloud density to material definition;
            unit [g/cm^3], enters all gas material definitions.

        Returns
        -------
        None.

        '''
        self.store = {}
        self.fdensity = targetDensity
        self.defineMaterials(reg)
        

    def getMaterialsDict(self):
        '''
        get hold of materials dictionary

        Returns
        -------
        Dict
            Materials dictionary

        '''
        return self.store


    def PrintAll(self):
        '''
        Print out all keys in dictionary.

        Returns
        -------
        None.

        '''
        print('List of keys in Materials dictionary:')
        for key in self.store.keys():
            print('key: ',key)


    def defineMaterials(self, reg):
        '''
        Define all materials required to build the geometry
        in one place for look-up. Predefined materials could be
        avoided here but prevents use of materials commands
        elsewhere.

        Parameters
        ----------
        reg : pg4.geant4.Registry or None (standalone construction)

        Returns
        -------
        store: dict
            Dictionary of materials objects as values and
            simple strings as key. Once constructed, should be
            data member for geometry construction.

        '''
        if reg is None:
            reg = pg4.geant4.Registry()
             
        # add predefined materials first - convenience
        vac    = pg4.geant4.MaterialPredefined("G4_Galactic")
        self.store['vacuum'] = vac
        steel  = pg4.geant4.MaterialPredefined("G4_STAINLESS-STEEL")
        self.store['steel'] = steel
        copper = pg4.geant4.MaterialPredefined("G4_Cu")
        self.store['Cu'] = copper

        # low density gas, H as element, others as isotopes
        hydrogen   = pg4.geant4.MaterialSingleElement("H", 1, 1.008, self.fdensity, reg)
        self.store['H'] = hydrogen

        deut  = pg4.geant4.Isotope("D", 1, 2, 2.0141)
        trit  = pg4.geant4.Isotope("T", 1, 3, 3.016)
        elD   = pg4.geant4.ElementIsotopeMixture("Deuterium", "D", 1)
        elT   = pg4.geant4.ElementIsotopeMixture("Tritium", "T", 1)
        elD.add_isotope(deut, 1.0)
        elT.add_isotope(trit, 1.0)

        matdeut = pg4.geant4.MaterialCompound("matD", self.fdensity, 1, reg)
        matdeut.add_element_massfraction(elD, 1)
        self.store['D'] = matdeut

        mattrit = pg4.geant4.MaterialCompound("matT", self.fdensity, 1, reg)
        mattrit.add_element_massfraction(elT, 1)
        self.store['T'] = mattrit
