//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// GEANT4 tag $Name: not supported by cvs2svn $
//
//
// GEANT4 physics class: G4ChipsAntiBaryonInelasticXS -- header file
// Created: M.V. Kossov, CERN/ITEP(Moscow), 20-Dec-01
// The last update: M.V. Kossov, CERN/ITEP (Moscow) 17-May-02
//
// -------------------------------------------------------------------------------------
// Short description: Cross-sections extracted (by W.Pokorski) from the CHIPS package for 
// anti-baryoninteractions. Original author: M. Kossov
// -------------------------------------------------------------------------------------
//

#ifndef G4ChipsAntiBaryonInelasticXS_h
#define G4ChipsAntiBaryonInelasticXS_h 1

#include "G4ParticleTable.hh"
#include "G4NucleiProperties.hh"
#include <vector>
#include "G4VCrossSectionDataSet.hh"
#include "G4ChipsBaseXS.hh"

class G4ChipsAntiBaryonInelasticXS :   public G4VCrossSectionDataSet, public G4ChipsBaseXS
{
public:

  G4ChipsAntiBaryonInelasticXS();

  ~G4ChipsAntiBaryonInelasticXS();

  virtual G4bool IsIsoApplicable(const G4DynamicParticle* Pt, G4int Z, G4int A,    
				 const G4Element* elm,
				 const G4Material* mat );

  // At present momentum (pMom) in MeV/c, CS in mb (@@ Units)
  virtual G4double GetIsoCrossSection(const G4DynamicParticle*, G4int tgZ, G4int A,  
				      const G4Isotope* iso = 0,
				      const G4Element* elm = 0,
				      const G4Material* mat = 0);

  virtual G4double GetChipsCrossSection(G4double momentum, G4int Z, G4int N, G4int pdg);

private:
  G4double CalculateCrossSection(G4int F, G4int I, G4int PDG, G4int Z,
                                 G4int N, G4double Momentum);

  G4double CrossSectionLin(G4int targZ, G4int targN, G4double P);
  G4double CrossSectionLog(G4int targZ, G4int targN, G4double lP);
  G4double CrossSectionFormula(G4int targZ, G4int targN, G4double P, G4double lP);
// Body
private:
  G4int pPDG;

  static G4double  lastSig; // Last value of the Cross Section
  static G4double* lastLEN; // Pointer to the last array of LowEnergy cross sections
  static G4double* lastHEN; // Pointer to the last array of HighEnergy cross sections
  static G4double  lastE;   // Last used in the cross section Energy
  static G4int     lastPDG; // The last projectile PDG
  static G4int     lastN;   // The last N of calculated nucleus
  static G4int     lastZ;   // The last Z of calculated nucleus
  static G4double  lastP;   // Last used in the cross section Momentum
  static G4double  lastTH;  // Last value of the Momentum Threshold
  static G4double  lastCS;  // Last value of the Cross Section
  static G4int     lastI;   // The last position in the DAMDB
  static std::vector<G4double*>* LEN;  // Vector of pointers to LowEnProtonCrossSection
  static std::vector<G4double*>* HEN;  // Vector of pointers to HighEnProtonCrossSection
};

#endif