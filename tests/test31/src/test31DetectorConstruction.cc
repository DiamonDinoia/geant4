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
// -------------------------------------------------------------
//      GEANT4 test31
//
//      History: based on object model of
//      2nd December 1995, G.Cosmo
//      ---------- test31DetectorConstruction -------
//
//  Modified: 05.04.01 Vladimir Ivanchenko new design of test31
//
// -------------------------------------------------------------

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "test31DetectorConstruction.hh"
#include "test31DetectorMessenger.hh"
#include "test31EventAction.hh"
#include "test31SD.hh"
#include "test31Histo.hh"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4UniformMagField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "globals.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

test31DetectorConstruction::test31DetectorConstruction():
  AbsorberMaterial(0),
  WorldMaterial(0),
  solidWorld(0),
  logicWorld(0),
  physWorld(0),
  solidAbs(0),
  logicAbs(0),
  physAbs(0),
  magField(0),
  theEvent(0),
  myVerbose(0),
  nEvents(0),
  detIsConstructed(false),
  nAbsSaved(0),
  nFirstEvtToDebug(-1),
  nLastEvtToDebug(-1)
{
  // Default parameter values of the calorimeter
  // corresponds to water test
  nameMatAbsorber   = G4String("Water");
  AbsorberThickness = 1.0*mm;    
  SizeXY            = 1000.0*mm;
  gap               = 0.0;
  NumberOfAbsorbers = 300;
  nameMatWorld      = G4String("Air");
  WorldSizeZ        = 400.0*mm;
  maxDelta          = 10.0*MeV;

  DefineMaterials();

  // create commands for interactive definition of the calorimeter  
  detectorMessenger = new test31DetectorMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

test31DetectorConstruction::~test31DetectorConstruction()
{ 
  delete detectorMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4VPhysicalVolume* test31DetectorConstruction::Construct()
{
  return ConstructGeometry();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void test31DetectorConstruction::DefineMaterials()
{ 
  if(myVerbose > 0) {
    G4cout << "test31DetectorConstruction: DefineMaterials starts" << G4endl;  
  } 

  G4String name, symbol;             //a=mass of a mole;
  G4double a, z, density;            //z=mean number of protons;  

  G4int    ncomponents, natoms;
  G4double fractionmass;
  G4double temperature, pressure;

  //  std::vector<G4Material*> list;
  G4Material* ma = 0;

//
// define Elements
//

  a = 1.01*g/mole;
  G4Element* elH  = new G4Element(name="Hydrogen",symbol="H", z= 1., a);

  a = 14.01*g/mole;
  G4Element* elN  = new G4Element(name="Nitrogen",symbol="N" , z= 7., a);

  a = 16.00*g/mole;
  G4Element* elO  = new G4Element(name="Oxygen"  ,symbol="O" , z= 8., a);

  a = 12.00*g/mole;
  G4Element* elC  = new G4Element(name="Carbon"  ,symbol="C" , z= 6., a);

  a = 69.723*g/mole;
  G4Element* elGa  = new G4Element(name="Gallium"  ,symbol="Ga" , z= 31., a);

  a = 74.9216*g/mole;
  G4Element* elAs  = new G4Element(name="Arsenicum"  ,symbol="As" , z= 33., a);

  G4Element*  Cs  = new G4Element ("Cesium"  , "Cs", 55. , 132.905*g/mole);

  G4Element*   I  = new G4Element ("Iodide"  , "I", 53. , 126.9044*g/mole);

  
//
// define simple materials
//
  density = 1.848*g/cm3;
  a = 9.01*g/mole;
  ma = new G4Material(name="Beryllium", z=4., a, density);

  density = 2.699*g/cm3;
  a = 26.98*g/mole;
  ma = new G4Material(name="Aluminum", z=13., a, density);

  density = 2.265*g/cm3;
  a = 12.0107*g/mole;
  ma = new G4Material(name="Carbon", z=6., a, density);

  density = 2.330*g/cm3;
  a = 28.09*g/mole;
  ma = new G4Material(name="Silicon", z=14., a, density);

  density = 1.390*g/cm3;
  a = 39.95*g/mole;
  ma = new G4Material(name="LiquidArgon", z=18., a, density);

  density = 3.02*g/cm3;
  a = 131.29*g/mole;
  ma = new G4Material(name="LiquidXenon", z=54., a, density);

  density = 7.874*g/cm3;
  a = 55.85*g/mole;
  ma = new G4Material(name="Iron"   , z=26., a, density);

  density = 8.960*g/cm3;
  a = 63.55*g/mole;
  ma = new G4Material(name="Copper"   , z=29., a, density);

  density = 5.323*g/cm3;
  a = 72.61*g/mole;
  ma = new G4Material(name="Germanium", z=32., a, density);

  density = 19.32*g/cm3;
  a =196.97*g/mole;
  ma = new G4Material(name="Gold"   , z=79., a, density);

  density = 11.35*g/cm3;
  a = 207.19*g/mole;
  ma = new G4Material(name="Lead"     , z=82., a, density);

//
// define a material from elements.   case 1: chemical molecule
//

  density = 1.000*g/cm3;
  ma = new G4Material("Water", density, 2);
  ma->SetChemicalFormula("H_2O");
  ma->AddElement(elH, natoms=2);
  ma->AddElement(elO, natoms=1);
  G4double exc = ma->GetIonisation()->FindMeanExcitationEnergy("H_2O");
  ma->GetIonisation()->SetMeanExcitationEnergy(exc);

  density = 0.0006672*g/cm3;
  ma = new G4Material("Methane", density, 2);
  ma->SetChemicalFormula("CH_4");
  ma->AddElement(elH, natoms=4);
  ma->AddElement(elC, natoms=1);

  ma = new G4Material("Graphite", 2.265*g/cm3, 1);
  ma->SetChemicalFormula("Graphite");
  ma->AddElement( elC, 1 );

  density = 5.3176*g/cm3;
  ma = new G4Material("GaAs", density, ncomponents=2);
  ma->SetChemicalFormula("GaAS");
  ma->AddElement(elGa, natoms=1);
  ma->AddElement(elAs, natoms=1);

  ma = new G4Material ("Ethane" , 0.4241*g/cm3, 2);
  ma->SetChemicalFormula("C_2H_6");
  ma->AddElement(elH,6);
  ma->AddElement(elC,2);

  ma = new G4Material ("CsI" , 4.51*g/cm3, 2);
  ma->SetChemicalFormula("CsI");
  ma->AddElement(Cs,1);
  ma->AddElement(I,1);
  ma->GetIonisation()->SetMeanExcitationEnergy(415.689*eV);

  density = 19.35*g/cm3;
  a = 183.85*g/mole;
  ma = new G4Material(name="Tangsten"     , z=74., a, density);

//
// define a material from elements.   case 2: mixture by fractional mass
//

  density = 1.290*mg/cm3;
  
  ma = new G4Material("Air"  , density, ncomponents=2);
  ma->AddElement(elN, fractionmass=0.7);
  ma->AddElement(elO, fractionmass=0.3);
  
  density = 1.39*g/cm3;
  ma = new G4Material("Mylar"  , density, ncomponents=3);
  ma->AddElement(elC, natoms=10);
  ma->AddElement(elH, natoms=18);
  ma->AddElement(elO, natoms=5);
  
  density     = universe_mean_density;    //from PhysicalConstants.h
  pressure    = 3.e-18*pascal;
  temperature = 2.73*kelvin;
  a = 1.01*g/mole;
  z = 1.0;
  ma = new G4Material("Vacuum", z, a, density,
                                      kStateGas,temperature,pressure);
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
  
G4VPhysicalVolume* test31DetectorConstruction::ConstructGeometry()
{
  if(myVerbose > 0) {
    G4cout << "test31DetectorConstruction: ConstructGeometry starts" << G4endl;
  } 

  // Cleanup old geometry
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  ComputeGeomParameters();
  WorldMaterial = GetMaterial(nameMatWorld);
  AbsorberMaterial = GetMaterial(nameMatAbsorber);

  //     
  // World
  //
  solidWorld = new G4Box("World",SizeXY+1.0*mm,SizeXY+1.0*mm,WorldSizeZ);   
                         
  logicWorld = new G4LogicalVolume(solidWorld,WorldMaterial,"World");
                                   
  physWorld = new G4PVPlacement(0,G4ThreeVector(),"World",logicWorld,
                                0,false,0);
  
  //                               
  // Absorber
  // 
  solidAbs = new G4Box("Absorber",SizeXY,SizeXY,AbsorberThickness*0.5);
                          
  logicAbs = new G4LogicalVolume(solidAbs,AbsorberMaterial,"Absorber");
      			                  
  G4double z = AbsorberThickness * 0.5;

  for (G4int j=0; j<NumberOfAbsorbers; j++) {
  
    physAbs = new G4PVPlacement(0,G4ThreeVector(0.0,0.0,z),
                                "Absorber",logicAbs,physWorld,false,j);
    z += AbsorberThickness + gap; 
  }
  
  //                               
  // Sensitive Detectors: Absorber 
  //

  test31SD* calorimeterSD = new test31SD("test31");
  (G4SDManager::GetSDMpointer())->AddNewDetector( calorimeterSD );
  logicAbs->SetSensitiveDetector(calorimeterSD);

  //                                        
  // Visualization attributes
  //
  G4VisAttributes* VisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  VisAtt->SetVisibility(true);
  logicAbs->SetVisAttributes(VisAtt);

  PrintGeomParameters();  

  detIsConstructed = true;

  //
  //always return the physical World
  //

  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void test31DetectorConstruction::PrintGeomParameters()
{
  G4cout << "The  WORLD   is made of " 
         << " of " << WorldMaterial->GetName();
  G4cout << ". The transverse size (XY) of the world is " 
         << SizeXY/mm << " mm" << G4endl;
  G4cout << "The ABSORBER is made of " << NumberOfAbsorbers << " items of "
         << AbsorberThickness/mm  
         << " mm of " << AbsorberMaterial->GetName();
  G4cout << ". The transverse size (XY) is " 
         << SizeXY/mm << " mm" 
         << G4endl;
  G4cout << "The meanExc(eV)= " 
         << AbsorberMaterial->GetIonisation()->GetMeanExcitationEnergy()/eV
         << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4Material* test31DetectorConstruction::GetMaterial(const G4String& mat)
{
  // search the material by its name
  G4Material* pttoMaterial = G4Material::GetMaterial(mat);     
  if(detIsConstructed) MaterialIsChanged();
  return pttoMaterial;
}
    
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void test31DetectorConstruction::SetNumberOfAbsorbers(G4int val)
{
  // change Absorber thickness and recompute the calorimeter parameters
  NumberOfAbsorbers = val;
  if(detIsConstructed) GeometryIsChanged();
}  
    
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void test31DetectorConstruction::SetAbsorberThickness(G4double val)
{
  // change Absorber thickness and recompute the calorimeter parameters
  AbsorberThickness = val;
  if(detIsConstructed) GeometryIsChanged();
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void test31DetectorConstruction::SetAbsorberSizeXY(G4double val)
{
  // change the transverse size and recompute the calorimeter parameters
  SizeXY = val;
  if(detIsConstructed) GeometryIsChanged();
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void test31DetectorConstruction::SetWorldSizeZ(G4double val)
{
  WorldSizeZ = val;
  if(detIsConstructed) GeometryIsChanged();
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void test31DetectorConstruction::SetGap(G4double val)
{
  gap = val;
  if(detIsConstructed) GeometryIsChanged();
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void test31DetectorConstruction::SetMagField(G4double fieldValue, G4int axis)
{
  // access to the field manager
  G4FieldManager* fieldMgr 
   = G4TransportationManager::GetTransportationManager()->GetFieldManager();
    
  if(magField) delete magField;		//delete the existing magn field
  
  // Create new field if >0
  if(fieldValue!=0.0) {

    G4ThreeVector B;
    // Choose direction of the field
    if(1 == axis) {
      B = G4ThreeVector(fieldValue,0.,0.);	
    } else if(2 == axis) {
      B = G4ThreeVector(0.,fieldValue,0.);	
    } else {
      B = G4ThreeVector(0.,0.,fieldValue);	
    }

    magField = new G4UniformMagField(B);        
    fieldMgr->SetDetectorField(magField);
    fieldMgr->CreateChordFinder(magField);

  // Set zero field
  } else {
    magField = 0;
    fieldMgr->SetDetectorField(magField);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void test31DetectorConstruction::ComputeGeomParameters()
{
  // Compute derived parameters of the 1st absorber 
     
  if(WorldSizeZ < (AbsorberThickness + gap)*NumberOfAbsorbers)
     WorldSizeZ = (AbsorberThickness + gap)*NumberOfAbsorbers + 1.0*mm;

  (test31Histo::GetPointer())->SetNumberOfAbsorbers(NumberOfAbsorbers);
  (test31Histo::GetPointer())->SetAbsorberThickness(AbsorberThickness);
  (test31Histo::GetPointer())->SetNumAbsorbersSaved(nAbsSaved);
  (test31Histo::GetPointer())->SetGap(gap);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
  
void test31DetectorConstruction::UpdateGeometry()
{
  (G4RunManager::GetRunManager())->DefineWorldVolume(ConstructGeometry());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void test31DetectorConstruction::GeometryIsChanged()
{
  (G4RunManager::GetRunManager())->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void test31DetectorConstruction::MaterialIsChanged()
{
  (G4RunManager::GetRunManager())->CutOffHasBeenModified();
  (G4RunManager::GetRunManager())->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....







