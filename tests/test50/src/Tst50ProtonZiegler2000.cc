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
// $Id: Tst50ProtonZiegler2000.cc,v 1.4 2010-04-01 09:48:30 sincerti Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// Author: Susanna Guatelli (guatelli@ge.infn.it)
//
// History:
// -----------
// 17 May     2003 SG          Designed for modular Physics List with
// Ziegler model for the Stopping Power and CSDArange and Stopping Power 
//conditions set
//
// -------------------------------------------------------------------

#include "Tst50ProtonZiegler2000.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleDefinition.hh"
//#include "G4MultipleScattering.hh"
#include "G4Proton.hh"
#include "G4hLowEnergyIonisation.hh"
#include "G4hLowEnergyLoss.hh"
#include "G4hSRIM2000p.hh" 
#include "G4StepLimiter.hh"

Tst50ProtonZiegler2000::Tst50ProtonZiegler2000(const G4String& name): G4VPhysicsConstructor(name)
{ }

Tst50ProtonZiegler2000::~Tst50ProtonZiegler2000()
{ }

void Tst50ProtonZiegler2000::ConstructProcess()
{

  theParticleIterator->reset();

  while( (*theParticleIterator)() )
    {
      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* manager = particle->GetProcessManager();
      G4String particleName = particle->GetParticleName();
     
      if (particleName == "proton") 
	{
	  // G4VProcess*  multipleScattering= new G4MultipleScattering(); 
	  G4hLowEnergyIonisation* ion = new G4hLowEnergyIonisation();
	  
	  ion -> SetElectronicStoppingPowerModel(particle, "SRIM2000p");
	  // ion -> SetNuclearStoppingPowerModel("Ziegler1985");
          ion -> SetNuclearStoppingOn() ;
	  //  manager->AddProcess(multipleScattering,-1,1,1);  	
	  ion -> SetEnlossFluc(false);
          manager -> AddProcess(new G4StepLimiter(),-1,-1,3);
          manager-> AddProcess(ion,-1,2,2);
	}
    }
}
