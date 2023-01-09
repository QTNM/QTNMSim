"""
test script
"""
import pyg4ometry
import BespokeMag as cs

det = cs.QTNMBaseline() # default gas density 5.e-12 g/cm3

print('All logical volumes:')
print(det.reg.logicalVolumeDict.keys())

print('All physical volumes:')
print(det.reg.physicalVolumeDict.keys())

# check overlaps
#lv = det.reg.logicalVolumeDict['worldLV']
#lv.checkOverlaps(recursive=True,coplanar=False,debugIO=True)

#det.drawGeometry()
det.writeGDML("test.gdml")

# visualise geometry
#v = pyg4ometry.visualisation.VtkViewerColoured(defaultColour='random')
#wl = det.reg.logicalVolumeDict['pipeLV']
#v.addLogicalVolume(wl)
#v.setWireframe()
#v.addAxes(length=20.0) # 2 cm axes
#v.view()
