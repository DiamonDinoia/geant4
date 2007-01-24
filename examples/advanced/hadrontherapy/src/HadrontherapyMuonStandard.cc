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
// $Id: HadrontherapyMuonStandard.cc; May 2005
// ----------------------------------------------------------------------------
//                 GEANT 4 - Hadrontherapy example
// ----------------------------------------------------------------------------
// Code developed by:
//
// G.A.P. Cirrone(a)*, F. Di Rosa(a), S. Guatelli(b), G. Russo(a)
// 
// (a) Laboratori Nazionali del Sud 
//     of the National Institute for Nuclear Physics, Catania, Italy
// (b) National Institute for Nuclear Physics Section of Genova, genova, Italy
// 
// * cirrone@lns.infn.it
// ----------------------------------------------------------------------------

#include "HadrontherapyMuonStandard.hh"
#include "G4ProcessManager.hh"

//muon:
#include "G4hIonisation.hh"
#include "G4MultipleScattering.hh"
#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"
#include "G4MuonMinusCaptureAtRest.hh" 
#include "G4StepLimiter.hh"
#include "G4EmProcessOptions.hh"
HadrontherapyMuonStandard::HadrontherapyMuonStandard(const G4String& name): G4VPhysicsConstructor(name)
{ }

HadrontherapyMuonStandard::~HadrontherapyMuonStandard()
{ }

void HadrontherapyMuonStandard::ConstructProcess()
{
  G4EmProcessOptions emProcessOptions;
  emProcessOptions.SetDEDXBinning(480);

  // Add standard processes for muons
  
  theParticleIterator -> reset();

  while( (*theParticleIterator)() )
    {
      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* pmanager = particle -> GetProcessManager();
      G4String particleName = particle -> GetParticleName();
     
      if(( particleName == "mu+")|| (particleName == "mu-" ))
	{
           //muon
	  G4VProcess* aMultipleScattering = new G4MultipleScattering();
	  G4VProcess* aBremsstrahlung     = new G4MuBremsstrahlung();
	  G4VProcess* aPairProduction     = new G4MuPairProduction();
	  G4VProcess* anIonisation        = new G4MuIonisation();
	  //
	  // add processes
	  pmanager -> AddProcess(anIonisation);
	  pmanager -> AddProcess(aMultipleScattering);
	  pmanager -> AddProcess(aBremsstrahlung);
	  pmanager -> AddProcess(aPairProduction);
	  //
	  // set ordering for AlongStepDoIt
	  pmanager -> SetProcessOrdering(aMultipleScattering, idxAlongStep,1);
	  pmanager -> SetProcessOrdering(anIonisation,        idxAlongStep,2);
	  pmanager -> SetProcessOrdering(aBremsstrahlung,     idxAlongStep,3);
	  pmanager -> SetProcessOrdering(aPairProduction,     idxAlongStep,4);      
	  //
	  // set ordering for PostStepDoIt
	  pmanager -> SetProcessOrdering(aMultipleScattering, idxPostStep,1);
	  pmanager -> SetProcessOrdering(anIonisation,        idxPostStep,2);
	  pmanager -> SetProcessOrdering(aBremsstrahlung,     idxPostStep,3);
	  pmanager -> SetProcessOrdering(aPairProduction,     idxPostStep,4);
          pmanager -> AddProcess(new G4StepLimiter(),      -1,-1, 3);

	  if( particleName == "mu-" )
	    pmanager -> AddProcess(new G4MuonMinusCaptureAtRest(), 0,-1,-1);
	}
    }
  // G4cout << "Muon processes are active!"<<G4endl;
}
