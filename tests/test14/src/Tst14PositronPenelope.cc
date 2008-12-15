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
// $Id: Tst14PositronPenelope.cc,v 1.1 2008-12-15 10:22:59 pandola Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// Author: Luciano Pandola (pandola@lngs.infn.it)
//
// History:
// -----------
// 15 Dec 2008 Luciano Pandola     Created
//
// -------------------------------------------------------------------

#include "Tst14PositronPenelope.hh"

#include "G4ProcessManager.hh"
#include "G4Gamma.hh"
#include "G4ParticleDefinition.hh"
#include "G4MultipleScattering.hh"
#include "G4PenelopeIonisation.hh"
#include "G4PenelopeBremsstrahlung.hh"
#include "G4PenelopeAnnihilation.hh"


Tst14PositronPenelope::Tst14PositronPenelope(const G4String& name): G4VPhysicsConstructor(name)
{ }

Tst14PositronPenelope::~Tst14PositronPenelope()
{ }

void Tst14PositronPenelope::ConstructProcess()
{
  // Add processes a' la Penelope for electrons
  
  theParticleIterator->reset();

  while( (*theParticleIterator)() )
    {
      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* manager = particle->GetProcessManager();
      G4String particleName = particle->GetParticleName();
     
      if (particleName == "e+") 
	{
	  manager->AddProcess(new G4MultipleScattering,     -1, 1,1);
	  manager->AddProcess(new G4PenelopeIonisation,     -1, 2, 2);
	  manager->AddProcess(new G4PenelopeBremsstrahlung, -1,-1,3);
	  manager->AddProcess(new G4PenelopeAnnihilation,    0,-1, 4);
	}   
    }
}
