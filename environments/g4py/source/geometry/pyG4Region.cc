// $Id: pyG4Region.cc,v 1.1 2006-02-27 09:56:05 kmura Exp $
// $Name: not supported by cvs2svn $
// ====================================================================
//   pyG4Region.cc
//
//                                         2005 Q
// ====================================================================
#include <boost/python.hpp>
#include "pyG4Version.hh"
#include "G4Region.hh"
#include "G4LogicalVolume.hh"
#include "G4ProductionCuts.hh"
#include "G4VUserRegionInformation.hh"
#include "G4UserLimits.hh"
#include "G4MaterialCutsCouple.hh"
#include "G4FastSimulationManager.hh"

using namespace boost::python;

// ====================================================================
// module definition
// ====================================================================
void export_G4Region()
{
  class_<G4Region, G4Region*, boost::noncopyable>
    ("G4Region", "region class", no_init)
    // constructors
    .def(init<const G4String&>())
    // ---
    .def("AddRootLogicalVolume",    &G4Region::AddRootLogicalVolume)
    .def("RemoveRootLogicalVolume", &G4Region::RemoveRootLogicalVolume)
    .def("SetName",                 &G4Region::SetName)
    .def("GetName",                 &G4Region::GetName,
	 return_value_policy<return_by_value>())
    .def("RegionModified",          &G4Region::RegionModified)
    .def("IsModified",              &G4Region::IsModified)
    .def("SetProductionCuts",       &G4Region::SetProductionCuts)
    .def("GetProductionCuts",       &G4Region::GetProductionCuts,
         return_internal_reference<>())
    .def("GetNumberOfMaterials",    &G4Region::GetNumberOfMaterials)
    .def("GetNumberOfRootVolumes",  &G4Region::GetNumberOfRootVolumes)
    .def("UpdateMaterialList",      &G4Region::UpdateMaterialList)
    .def("ClearMaterialList",       &G4Region::ClearMaterialList)
    .def("ScanVolumeTree",          &G4Region::ScanVolumeTree)
    .def("SetUserInformation",      &G4Region::SetUserInformation)
    .def("GetUserInformation",      &G4Region::GetUserInformation,
         return_internal_reference<>())
#if G4VERSION_NUMBER >= 710
    .def("SetUserLimits",           &G4Region::SetUserLimits)
    .def("GetUserLimits",           &G4Region::GetUserLimits,
         return_internal_reference<>())
#endif
    .def("ClearMap",                &G4Region::ClearMap)
    .def("RegisterMaterialCouplePair", &G4Region::RegisterMaterialCouplePair)
    .def("FindCouple",              &G4Region::FindCouple,
         return_value_policy<reference_existing_object>())   
#if G4VERSION_NUMBER >= 800
    .def("SetFastSimulationManager", &G4Region::SetFastSimulationManager)
    .def("GetFastSimulationManager", &G4Region::GetFastSimulationManager,
         return_internal_reference<>())
    .def("ClearFastSimulationManager", &G4Region::ClearFastSimulationManager)
    .def("GetWorldPhysical",        &G4Region::GetWorldPhysical,
         return_internal_reference<>())
    .def("SetWorld",                &G4Region::SetWorld)
    .def("BelongsTo",               &G4Region::BelongsTo)
    .def("GetParentRegion",         &G4Region::GetParentRegion,
         return_value_policy<reference_existing_object>())
#endif
    ;
 }
