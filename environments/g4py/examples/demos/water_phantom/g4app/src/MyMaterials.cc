// $Id: MyMaterials.cc,v 1.1 2006-02-27 09:44:35 kmura Exp $
// $Name: not supported by cvs2svn $
// ====================================================================
//   MyMaterials.cc
//
//                                         2005 Q
// ====================================================================
#include "MyMaterials.hh"
#include "G4Material.hh"

// ====================================================================
//
// class description
//
// ====================================================================

//////////////////////////
MyMaterials::MyMaterials()
//////////////////////////
{
}


///////////////////////////
MyMaterials::~MyMaterials()
///////////////////////////
{
}


/////////////////////////////
void MyMaterials::Construct()
/////////////////////////////
{
  G4double A, Z;

  // ------------------------------------------------------------------------
  // Elements
  // ------------------------------------------------------------------------
  G4Element* elH  = new G4Element("Hydrogen","H",  Z=1.,  A=1.00794*g/mole);
  G4Element* elC  = new G4Element("Carbon",  "C",  Z=6.,  A= 12.011 *g/mole);
  G4Element* elN  = new G4Element("Nitrogen","N",  Z=7.,  A= 14.00674*g/mole);
  G4Element* elO  = new G4Element("Oxygen",  "O",  Z=8.,  A= 15.9994*g/mole);
  
  // ------------------------------------------------------------------------
  // Materials
  // ------------------------------------------------------------------------
  G4double density, massfraction;
  G4int natoms, nel;

  // temperature of experimental hall is controlled at 20 degree.
  const G4double expTemp= STP_Temperature+20.*kelvin; 

  // vacuum
  density= universe_mean_density;
  G4Material* Vacuum= new G4Material("Vacuum", density, nel=2);
  Vacuum-> AddElement(elN, .7);
  Vacuum-> AddElement(elO, .3);

  // air
  density= 1.2929e-03 *g/cm3;  // at 20 degree
  G4Material* Air= new G4Material("Air", density, nel=2, 
				  kStateGas, expTemp);
  G4double ttt= 75.47+23.20;
  Air-> AddElement(elN,  massfraction= 75.47/ttt);
  Air-> AddElement(elO,  massfraction= 23.20/ttt);

  // water
  density= 1.000*g/cm3;
  G4Material* H2O= new G4Material("Water", density, nel=2);
  H2O-> AddElement(elH, natoms=2);
  H2O-> AddElement(elO, natoms=1);

  // alminium
  A= 26.98 *g/mole;
  density= 2.70 *g/cm3;
  G4Material* Al= new G4Material("Al", Z=13., A, density);

  // iron
  A= 55.847 *g/mole;
  density= 7.87 *g/cm3;
  G4Material* Fe= new G4Material("Iron", Z=26., A, density);

  // lead
  A= 207.2 *g/mole;
  density= 11.35 *g/cm3;
  G4Material* Pb= new G4Material("Lead", Z=82., A, density);

  // scintillator (Polystyene(C6H5CH=CH2))
  density= 1.032 *g/cm3;
  G4Material* Scinti= new G4Material("Scinti", density, nel=2);
  Scinti-> AddElement(elC, natoms=8);
  Scinti-> AddElement(elH, natoms=8);

}

