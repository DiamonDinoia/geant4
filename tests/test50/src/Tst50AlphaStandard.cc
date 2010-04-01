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
#include "Tst50AlphaStandard.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleDefinition.hh"
//#include "G4MultipleScattering.hh"
#include "G4Alpha.hh"
#include "G4GenericIon.hh"
#include "G4ionIonisation.hh"
#include "G4StepLimiter.hh"

Tst50AlphaStandard::Tst50AlphaStandard(const G4String& name): G4VPhysicsConstructor(name)
{ }

Tst50AlphaStandard::~Tst50AlphaStandard()
{ }

void Tst50AlphaStandard::ConstructProcess()
{

theParticleIterator->reset();

  while( (*theParticleIterator)() )
    {
      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* manager = particle->GetProcessManager();
      G4String particleName = particle->GetParticleName();
     
      if (particleName == "alpha")
	{
	  G4ionIonisation* ionisation = new G4ionIonisation();
          //G4VProcess*  multipleScattering= new G4MultipleScattering();  
	  manager->AddProcess(ionisation,-1,2,2);
          manager -> AddProcess(new G4StepLimiter(),-1,-1,3);
          //manager->AddProcess(multipleScattering,-1,1,1);  	
	}	
      if (particleName == "GenericIon")
	{
	  G4ionIonisation* ionisation = new G4ionIonisation();
          //G4VProcess*  multipleScattering= new G4MultipleScattering();  
	  manager->AddProcess(ionisation,-1,2,2);
          //manager->AddProcess(multipleScattering,-1,1,1);  	
	}	
    }
}
