//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: NistMaterialTest1.cc,v 1.1 2005-02-11 17:30:26 maire Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 
// ------------------------------------------------------------
//
//
//  Test the construction of materials from the NIST data base
//
#include "G4NistElementBuilder.hh"
#include "G4NistMaterialBuilder.hh"

#include "globals.hh"
#include "G4UnitsTable.hh"

int main() {

G4int z;
G4bool buildIsotopes;
G4int verbose;

G4UnitDefinition::BuildUnitsTable();

// define Elements
//
G4NistElementBuilder nistElm(verbose=0);

nistElm.FindOrBuildElement (z=1);
nistElm.FindOrBuildElement (z=6);
nistElm.FindOrBuildElement (z=7, buildIsotopes=false);
nistElm.FindOrBuildElement ("O");
nistElm.FindOrBuildElement ("Si", buildIsotopes=false);
nistElm.FindOrBuildElement ("Fe");
nistElm.FindOrBuildElement ("U");

G4cout << *(G4Isotope::GetIsotopeTable()) << G4endl;
G4cout << *(G4Element::GetElementTable()) << G4endl;


// define Materials
//
G4NistMaterialBuilder nistMat(0, &nistElm, verbose=0);

//nistMat.ListNistSimpleMaterials();

G4Material* Al =
nistMat.FindOrBuildMaterial ("G4_Al");
nistMat.FindOrBuildMaterial ("G4_lAr", buildIsotopes=false);
nistMat.FindOrBuildMaterial ("G4_Cu" , buildIsotopes=false);
G4Material* Pb =
nistMat.FindOrBuildMaterial ("G4_Pb");

//nistMat.ListNistCompoundMaterials();

G4Material* H2O =
nistMat.FindOrBuildMaterial ("G4_WATER");
nistMat.FindOrBuildMaterial ("G4_POLYSTYRENE");
nistMat.FindOrBuildMaterial ("G4_SILICON_DIOXIDE");
nistMat.FindOrBuildMaterial ("G4_AIR", buildIsotopes=false);

// print G4MaterialTable
//
G4cout << *(G4Material::GetMaterialTable()) << G4endl;

// print additional informations
//
G4cout << " Nuclear interaction length of Aluminium: " 
       << Al->GetNuclearInterLength()/cm << " cm" << G4endl;
       
G4cout << " Nuclear interaction length of Lead: " 
       << Pb->GetNuclearInterLength()/cm << " cm" << G4endl;
              
G4cout << " Nuclear interaction length of Water: " 
       << H2O->GetNuclearInterLength()/cm << " cm" << G4endl;
                    
return EXIT_SUCCESS;
}
