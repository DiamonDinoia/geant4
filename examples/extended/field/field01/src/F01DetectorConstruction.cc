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
// $Id: F01DetectorConstruction.cc,v 1.6 2001-10-15 17:20:36 gcosmo Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 

#include "F01DetectorConstruction.hh"
#include "F01DetectorMessenger.hh"

#include "F01CalorimeterSD.hh"
#include "F01ElectroMagneticField.hh"

#include "G4VClusterModel.hh"
#include "G4PAIclusterModel.hh"

#include "G4Material.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4UniformMagField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"

#include "G4ios.hh"

/////////////////////////////////////////////////////////////////////////////
//
//

F01DetectorConstruction::F01DetectorConstruction()
 : solidWorld(0), logicWorld(0), physiWorld(0),
   solidAbsorber(0),logicAbsorber(0), physiAbsorber(0),
   magField(0), fEmField(0), calorimeterSD(0),
   AbsorberMaterial(0), worldchanged(false), WorldMaterial(0)
{
  // default parameter values of the calorimeter

  WorldSizeZ = 44000.*mm;
  WorldSizeR = 22000.*mm;

  AbsorberThickness = 1.0*mm;

  AbsorberRadius   = 20000.*mm;

  zAbsorber = 21990.0*mm ;

  // create commands for interactive definition of the calorimeter  

  detectorMessenger = new F01DetectorMessenger(this);
  
}

//////////////////////////////////////////////////////////////////////////
//
//

F01DetectorConstruction::~F01DetectorConstruction()
{ 
  delete detectorMessenger;
  if (fEmField) delete fEmField ;
}

//////////////////////////////////////////////////////////////////////////
//
//

G4VPhysicalVolume* F01DetectorConstruction::Construct()
{
  DefineMaterials();
  return ConstructCalorimeter();
}

//////////////////////////////////////////////////////////////////////////////
//
//

void F01DetectorConstruction::DefineMaterials()
{ 
 //This function illustrates the possible ways to define materials
 
  G4String name, symbol ;             // a=mass of a mole;
  G4double a, z, density ;            // z=mean number of protons;  
  G4int nel ;
  G4int ncomponents;
  G4double fractionmass;

  //
  // define Elements
  //

  a = 1.01*g/mole;
  G4Element* elH  = new G4Element(name="Hydrogen",symbol="H" , z= 1., a);

  a = 6.01*g/mole;
  G4Element* elC = new G4Element(name="Carbon", symbol="C", z=6., a);

  a = 14.01*g/mole;
  G4Element* elN  = new G4Element(name="Nitrogen",symbol="N" , z= 7., a);

  a = 16.00*g/mole;
  G4Element* elO  = new G4Element(name="Oxygen"  ,symbol="O" , z= 8., a);

  a = 39.948*g/mole;
  G4Element* elAr = new G4Element(name="Argon", symbol="Ar", z=18., a);

  //
  // define simple materials
  //

  // Mylar

  density = 1.39*g/cm3;
  G4Material* Mylar = new G4Material(name="Mylar", density, nel=3);
  Mylar->AddElement(elO,2);
  Mylar->AddElement(elC,5);
  Mylar->AddElement(elH,4);

  // Polypropelene

  G4Material* CH2 = new G4Material ("Polypropelene" , 0.91*g/cm3, 2);
  CH2->AddElement(elH,2);
  CH2->AddElement(elC,1);

  // Krypton as detector gas, STP

  density = 3.700*mg/cm3 ;
  a = 83.80*g/mole ;
  G4Material* Kr  = new G4Material(name="Kr",z=36., a, density );

  // Dry air (average composition)

  density = 1.7836*mg/cm3 ;       // STP
  G4Material* Argon = new G4Material(name="Argon"  , density, ncomponents=1);
  Argon->AddElement(elAr, 1);

  density = 1.25053*mg/cm3 ;       // STP
  G4Material* Nitrogen = new G4Material(name="N2"  , density, ncomponents=1);
  Nitrogen->AddElement(elN, 2);

  density = 1.4289*mg/cm3 ;       // STP
  G4Material* Oxygen = new G4Material(name="O2"  , density, ncomponents=1);
  Oxygen->AddElement(elO, 2);

  density  = 1.2928*mg/cm3 ;       // STP
  density *= 1.0e-8 ;       // pumped vacuum
  G4Material* Air = new G4Material(name="Air"  , density, ncomponents=3);
  Air->AddMaterial( Nitrogen, fractionmass = 0.7557 ) ;
  Air->AddMaterial( Oxygen,   fractionmass = 0.2315 ) ;
  Air->AddMaterial( Argon,    fractionmass = 0.0128 ) ;

  // Xenon as detector gas, STP

  density = 5.858*mg/cm3 ;
  a = 131.29*g/mole ;
  G4Material* Xe  = new G4Material(name="Xenon",z=54., a, density );

  // Carbon dioxide, STP

  density = 1.977*mg/cm3;
  G4Material* CarbonDioxide = new G4Material(name="CO2", density, nel=2);
  CarbonDioxide->AddElement(elC,1);
  CarbonDioxide->AddElement(elO,2);

  // 80% Xe + 20% CO2, STP

  density = 5.0818*mg/cm3 ;      
  G4Material* Xe20CO2 = new G4Material(name="Xe20CO2", density, ncomponents=2);
  Xe20CO2->AddMaterial( Xe,              fractionmass = 0.922 ) ;
  Xe20CO2->AddMaterial( CarbonDioxide,   fractionmass = 0.078 ) ;

  // 80% Kr + 20% CO2, STP

  density = 3.601*mg/cm3 ;      
  G4Material* Kr20CO2 = new G4Material(name="Kr20CO2", density, ncomponents=2);
  Kr20CO2->AddMaterial( Kr,              fractionmass = 0.89 ) ;
  Kr20CO2->AddMaterial( CarbonDioxide,   fractionmass = 0.11 ) ;


  G4cout << *(G4Material::GetMaterialTable()) << G4endl;

  // default materials of the calorimeter
  
  AbsorberMaterial = Air ; //  Kr20CO2 ;   // XeCO2CF4  ; 

  WorldMaterial    = Air ;
}

/////////////////////////////////////////////////////////////////////////
//
//
  
G4VPhysicalVolume* F01DetectorConstruction::ConstructCalorimeter()
{
  // complete the Calor parameters definition and Print 

  ComputeCalorParameters();
  PrintCalorParameters();
      
  // World
  
  if(solidWorld) delete solidWorld ;
  if(logicWorld) delete logicWorld ;
  if(physiWorld) delete physiWorld ;

  solidWorld = new G4Tubs("World",				// its name
                   0.,WorldSizeR,WorldSizeZ/2.,0.,twopi);       // its size
                         
  logicWorld = new G4LogicalVolume(solidWorld,		// its solid
                                   WorldMaterial,	// its material
                                   "World");		// its name
                                   
  physiWorld = new G4PVPlacement(0,			// no rotation
  				 G4ThreeVector(),	// at (0,0,0)
                                 "World",		// its name
                                 logicWorld,		// its logical volume
                                 0,			// its mother  volume
                                 false,			// no boolean operation
                                 0);			// copy number
      
  // Absorber

  if (AbsorberThickness > 0.) 
  { 
      if(solidAbsorber) delete solidAbsorber ;
      if(logicAbsorber) delete logicAbsorber ;
      if(physiAbsorber) delete physiAbsorber ;

      solidAbsorber = new G4Tubs("Absorber", 1.0*mm, 
                                  AbsorberRadius,
                                  AbsorberThickness/2., 
                                  0.0,twopi); 
                          
      logicAbsorber = new G4LogicalVolume(solidAbsorber,    
      			                  AbsorberMaterial, 
      			                  "Absorber");     
      			                  
      physiAbsorber = new G4PVPlacement(0,		   
      		          G4ThreeVector(0.,0.,zAbsorber),        
                                        "Absorber",        
                                        logicAbsorber,     
                                        physiWorld,       
                                        false,             
                                        0);                
                                        
  }
                                 
  // Sensitive Detectors: Absorber 
  
  G4SDManager* SDman = G4SDManager::GetSDMpointer();

  if(!calorimeterSD)
  {
    calorimeterSD = new F01CalorimeterSD("CalorSD",this);
    SDman->AddNewDetector( calorimeterSD );
  }
  if (logicAbsorber)  logicAbsorber->SetSensitiveDetector(calorimeterSD);

  return physiWorld;
}

////////////////////////////////////////////////////////////////////////////
//
//

void F01DetectorConstruction::PrintCalorParameters()
{
  G4cout << "\n The  WORLD   is made of " 
       << WorldSizeZ/mm << "mm of " << WorldMaterial->GetName() ;
  G4cout << ", the transverse size (R) of the world is " << WorldSizeR/mm << " mm. " << G4endl;
  G4cout << " The ABSORBER is made of " 
       << AbsorberThickness/mm << "mm of " << AbsorberMaterial->GetName() ;
  G4cout << ", the transverse size (R) is " << AbsorberRadius/mm << " mm. " << G4endl;
  G4cout << " Z position of the (middle of the) absorber " << zAbsorber/mm << "  mm." << G4endl;
  G4cout << G4endl;
}

///////////////////////////////////////////////////////////////////////////
//
//

void F01DetectorConstruction::SetAbsorberMaterial(G4String materialChoice)
{
  // get the pointer to the material table
  const G4MaterialTable* theMaterialTable = G4Material::GetMaterialTable();

  // search the material by its name   
  G4Material* pttoMaterial;
  for (size_t J=0 ; J<theMaterialTable->size() ; J++)
   { pttoMaterial = (*theMaterialTable)[J];     
     if(pttoMaterial->GetName() == materialChoice)
        {
	  AbsorberMaterial = pttoMaterial;
          logicAbsorber->SetMaterial(pttoMaterial); 
        }             
   }
}

////////////////////////////////////////////////////////////////////////////
//
//

void F01DetectorConstruction::SetWorldMaterial(G4String materialChoice)
{
  // get the pointer to the material table
  const G4MaterialTable* theMaterialTable = G4Material::GetMaterialTable();

  // search the material by its name   
  G4Material* pttoMaterial;
  for (size_t J=0 ; J<theMaterialTable->size() ; J++)
   { pttoMaterial = (*theMaterialTable)[J];     
     if(pttoMaterial->GetName() == materialChoice)
        {
	  WorldMaterial = pttoMaterial;
          logicWorld->SetMaterial(pttoMaterial); 
        }             
   }
}

///////////////////////////////////////////////////////////////////////////
//
//

void F01DetectorConstruction::SetAbsorberThickness(G4double val)
{
  // change Absorber thickness and recompute the calorimeter parameters
  AbsorberThickness = val;
  ComputeCalorParameters();
}  

/////////////////////////////////////////////////////////////////////////////
//
//

void F01DetectorConstruction::SetAbsorberRadius(G4double val)
{
  // change the transverse size and recompute the calorimeter parameters
  AbsorberRadius = val;
  ComputeCalorParameters();
}  

////////////////////////////////////////////////////////////////////////////
//
//

void F01DetectorConstruction::SetWorldSizeZ(G4double val)
{
  worldchanged=true;
  WorldSizeZ = val;
  ComputeCalorParameters();
}  

///////////////////////////////////////////////////////////////////////////
//
//

void F01DetectorConstruction::SetWorldSizeR(G4double val)
{
  worldchanged=true;
  WorldSizeR = val;
  ComputeCalorParameters();
}  

//////////////////////////////////////////////////////////////////////////////
//
//

void F01DetectorConstruction::SetAbsorberZpos(G4double val)
{
  zAbsorber  = val;
  ComputeCalorParameters();
}  

//////////////////////////////////////////////////////////////////////////////
//
//

void F01DetectorConstruction::SetMagField(G4double fieldValue)
{
  // apply a global uniform magnetic field along X axis

  /* *********************************************************

  G4FieldManager* fieldMgr 
   = G4TransportationManager::GetTransportationManager()->GetFieldManager();
    
  if(magField) delete magField;		// delete the existing magn field
  
  if(fieldValue!=0.)			// create a new one if non nul
  { 
    magField = new G4UniformMagField(G4ThreeVector(fieldValue,0.,0.));        
    fieldMgr->SetDetectorField(magField);
    fieldMgr->CreateChordFinder(magField);
  } 
  else 
  {
    magField = 0;
    fieldMgr->SetDetectorField(magField);
  }

  *************************************************************** */

}

///////////////////////////////////////////////////////////////////////////////
//
//
  
void F01DetectorConstruction::UpdateGeometry()
{
  G4RunManager::GetRunManager()->DefineWorldVolume(ConstructCalorimeter());
}

//
//
////////////////////////////////////////////////////////////////////////////
