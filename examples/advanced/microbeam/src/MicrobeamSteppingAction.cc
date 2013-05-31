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
// -------------------------------------------------------------------
// $Id$
// -------------------------------------------------------------------

#include "G4SystemOfUnits.hh"
#include "G4SteppingManager.hh"

#include "MicrobeamSteppingAction.hh"
#include "MicrobeamRunAction.hh"
#include "MicrobeamDetectorConstruction.hh"
#include "MicrobeamHistoManager.hh"

#include "G4Alpha.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

MicrobeamSteppingAction::MicrobeamSteppingAction(MicrobeamRunAction* run,MicrobeamDetectorConstruction* det)
:Run(run),Detector(det)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

MicrobeamSteppingAction::~MicrobeamSteppingAction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void MicrobeamSteppingAction::UserSteppingAction(const G4Step* aStep)
  
{ 
  G4AnalysisManager* man = G4AnalysisManager::Instance();

// COUNT GAS DETECTOR HITS

if (       ((aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume() == Detector->GetLogicalCollDetYoke())
        &&  (aStep->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume() == Detector->GetLogicalIsobutane())
        &&  (aStep->GetTrack()->GetDynamicParticle()->GetDefinition() == G4Alpha::AlphaDefinition() ))
	
        || 
	   ((aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume() == Detector->GetLogicalCollDetGap4())
        &&  (aStep->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume() == Detector->GetLogicalIsobutane())
        &&  (aStep->GetTrack()->GetDynamicParticle()->GetDefinition() == G4Alpha::AlphaDefinition() ))

	||

    	   ((aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume() == Detector->GetLogicalCollDetGap4())
        &&  (aStep->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume() == Detector->GetLogicalIsobutane())
        &&  (aStep->GetTrack()->GetDynamicParticle()->GetDefinition() == G4Alpha::AlphaDefinition() ))

    )
	{
	  Run->AddNbOfHitsGas();	
	}
	
// STOPPING POWER AND BEAM SPOT SIZE AT CELL ENTRANCE

if (       ((aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume() == Detector->GetLogicalPolyprop())
        &&  (aStep->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume() == Detector->GetLogicalKgm())
        &&  (aStep->GetTrack()->GetDynamicParticle()->GetDefinition() == G4Alpha::AlphaDefinition() ))
	
        || 
	   ((aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume() == Detector->GetLogicalPolyprop())
        &&  (aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "physicalCytoplasm")
        &&  (aStep->GetTrack()->GetDynamicParticle()->GetDefinition() == G4Alpha::AlphaDefinition() ))
    )
	{
	
	 if( (aStep->GetPreStepPoint()->GetKineticEnergy() - aStep->GetPostStepPoint()->GetKineticEnergy() ) >0) 
	 {
	   //Fill ntupleid=1 
	   man->FillNtupleDColumn(1,0,aStep->GetPreStepPoint()->GetKineticEnergy()/keV);
	   man->FillNtupleDColumn(1,1,
				  (aStep->GetPreStepPoint()->GetKineticEnergy() -
				   aStep->GetPostStepPoint()->GetKineticEnergy())/
				  keV/(aStep->GetStepLength()/micrometer));
	   man->AddNtupleRow(1);
	 }

         // Average dE over step suggested by Michel Maire

	 G4StepPoint* p1 = aStep->GetPreStepPoint();
         G4ThreeVector coord1 = p1->GetPosition();
         const G4AffineTransform transformation1 = p1->GetTouchable()->GetHistory()->GetTopTransform();
         G4ThreeVector localPosition1 = transformation1.TransformPoint(coord1);

	 G4StepPoint* p2 = aStep->GetPostStepPoint();
         G4ThreeVector coord2 = p2->GetPosition();
         const G4AffineTransform transformation2 = p2->GetTouchable()->GetHistory()->GetTopTransform();
         G4ThreeVector localPosition2 = transformation2.TransformPoint(coord2);

         G4ThreeVector localPosition = localPosition1 + G4UniformRand()*(localPosition2-localPosition1);
	 
	 // end
	 //Fill ntupleid=2
	 man->FillNtupleDColumn(2,0,localPosition.x()/micrometer);
	 man->FillNtupleDColumn(2,1,localPosition.y()/micrometer);
	 man->AddNtupleRow(2);
	}

// ALPHA RANGE

if (

	(aStep->GetTrack()->GetDynamicParticle()->GetDefinition() == G4Alpha::AlphaDefinition())
	
	&&
	
	(aStep->GetTrack()->GetKineticEnergy()<1e-6)
	
	&&
			
          ( (aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "physicalCytoplasm")
	||  (aStep->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume() == Detector->GetLogicalKgm())	
	||  (aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "physicalNucleus") )	
		
   )
	
	{
	   //Fill ntupleid=3
	  man->FillNtupleDColumn(3,0,
				 aStep->GetPostStepPoint()->GetPosition().x()/micrometer);
	  man->FillNtupleDColumn(3,1,
				 aStep->GetPostStepPoint()->GetPosition().y()/micrometer);
	  man->FillNtupleDColumn(3,2,
				 aStep->GetPostStepPoint()->GetPosition().z()/micrometer);
	  man->AddNtupleRow(3);
 	}

// TOTAL DOSE DEPOSIT AND DOSE DEPOSIT WITHIN A PHANTOM VOXEL
// FOR ALL PARTICLES

if (aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName()  == "physicalNucleus")

	{ 
   	 G4double dose = (aStep->GetTotalEnergyDeposit()/joule)/(Run->GetMassNucleus()/kg);
   	 Run->AddDoseN(dose);

	 G4ThreeVector v;
    	 Run->AddDoseBox(aStep->GetPreStepPoint()->GetTouchableHandle()->GetReplicaNumber(),
	  aStep->GetTotalEnergyDeposit()/eV);
	}


if (aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName()  == "physicalCytoplasm")

	{ 
   	 G4double dose = (aStep->GetTotalEnergyDeposit()/joule)/(Run->GetMassCytoplasm()/kg);
   	 Run->AddDoseC(dose);

	 G4ThreeVector v;
    	 Run->AddDoseBox(aStep->GetPreStepPoint()->GetTouchableHandle()->GetReplicaNumber(),
	  aStep->GetTotalEnergyDeposit()/eV);
 	}
}
