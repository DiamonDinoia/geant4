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
// $Id: G4NeutronHPKallbachMannSyst.hh,v 1.6 2002-12-12 19:18:14 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
#ifndef G4NeutronHPKallbachMannSyst_h
#define G4NeutronHPKallbachMannSyst_h 1

#include "globals.hh"

class G4NeutronHPKallbachMannSyst
{
  public:
  
   G4NeutronHPKallbachMannSyst(G4double aCompoundFraction,
                               G4double anIncidentEnergy, G4double anIncidentMass,
                               G4double aProductEnergy, G4double aProductMass,
                               G4double aResidualMass, G4int aResidualA, G4int aResidualZ,
                               G4double aTargetMass, G4int aTargetA, G4int aTargetZ)
  {
    theCompoundFraction = aCompoundFraction;
    theIncidentEnergy = anIncidentEnergy;
    theIncidentMass = anIncidentMass;
    theProductEnergy = aProductEnergy;
    theProductMass = aProductMass;
    theResidualMass = aResidualMass;
    theResidualA = aResidualA;
    theResidualZ = aResidualZ;
    theTargetMass = aTargetMass;
    theTargetA = aTargetA;
    theTargetZ = aTargetZ;
  }
  
  ~G4NeutronHPKallbachMannSyst() {};
  
  G4double Sample(G4double anEnergy);
  
  G4double Kallbach(G4double cosTh, G4double anEnergy);
  
  G4double GetKallbachZero(G4double anEnergy);
  
  G4double A(G4double anEnergy);
  
  G4double SeparationEnergy(G4int Ac, G4int Nc, G4int AA, G4int ZA);
  
  private:
  
  G4double theCompoundFraction;
  G4double theIncidentEnergy;
  G4double theIncidentMass;
  G4double theProductEnergy;
  G4double theProductMass;
  G4double theResidualMass;
  G4double theTargetMass;
  G4int theResidualA;
  G4int theResidualZ;
  G4int theTargetA;
  G4int theTargetZ;
};

#endif
