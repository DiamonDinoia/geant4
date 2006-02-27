// $Id: pyExN03geom.cc,v 1.1 2006-02-27 09:47:41 kmura Exp $
// $Name: not supported by cvs2svn $
// ====================================================================
//   pyExN03geom.cc
//
//   [ExN03geom]
//   a site-module of Geant4Py
//
//   geometry presented in ExN03 of Geant4 example
//
//                                         2005 Q
// ====================================================================
#include <boost/python.hpp>
#include "G4RunManager.hh"
#include "ExN03DetectorConstruction.hh"

using namespace boost::python;

typedef ExN03DetectorConstruction XXX;

// ====================================================================
// thin wrappers
// ====================================================================
namespace pyExN03geom {

void Construct()
{
  G4RunManager* runMgr= G4RunManager::GetRunManager();
  runMgr-> SetUserInitialization(new ExN03DetectorConstruction);
}

};

using namespace pyExN03geom;

// ====================================================================
//   Expose to Python
// ====================================================================

BOOST_PYTHON_MODULE(ExN03geom) {
  class_<ExN03DetectorConstruction, ExN03DetectorConstruction*,
    bases<G4VUserDetectorConstruction> >
    ("ExN03DetectorConstruction", "ExN03 detector")
    // ---
    .def("SetAbsorberMaterial",   &XXX::SetAbsorberMaterial)
    .def("SetAbsorberThickness",  &XXX::SetAbsorberThickness)
    .def("SetGapMaterial",        &XXX::SetGapMaterial)
    .def("SetGapThickness",       &XXX::SetGapThickness)
    .def("SetCalorSizeYZ",        &XXX::SetCalorSizeYZ)
    .def("SetNbOfLayers",         &XXX::SetNbOfLayers)
    .def("SetMagField",           &XXX::SetMagField)
    // ---
    .def("GetWorldSizeX",         &XXX::GetWorldSizeX)
    .def("GetWorldSizeYZ",        &XXX::GetWorldSizeYZ)
    .def("GetCalorThickness",     &XXX::GetCalorThickness)
    .def("GetCalorSizeYZ",        &XXX::GetCalorSizeYZ)
    .def("GetNbOfLayers",         &XXX::GetNbOfLayers)
    .def("GetAbsorberMaterial",   &XXX::GetAbsorberMaterial,
	 return_value_policy<reference_existing_object>())
    .def("GetAbsorberThickness",  &XXX::GetAbsorberThickness)
    .def("GetGapMaterial",        &XXX::GetGapMaterial,
	 return_value_policy<reference_existing_object>())    
    .def("GetGapThickness",       &XXX::GetGapThickness)
    .def("GetphysiWorld",         &XXX::GetphysiWorld,
	 return_value_policy<reference_existing_object>())
    .def("GetAbsorber",           &XXX::GetAbsorber,
	 return_value_policy<reference_existing_object>())
    .def("GetGap",                &XXX::GetGap,
	 return_value_policy<reference_existing_object>())
    // ---
    .def("UpdateGeometry",        &XXX::UpdateGeometry)
    .def("PrintCalorParameters",  &XXX::PrintCalorParameters)
    ;

  // ---
  def("Construct",  Construct);
}

