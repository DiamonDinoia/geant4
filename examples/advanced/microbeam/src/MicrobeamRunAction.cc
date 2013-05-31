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

#include "G4UImanager.hh"
#include "Randomize.hh"

#include "MicrobeamRunAction.hh"
#include "MicrobeamHistoManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

MicrobeamRunAction::MicrobeamRunAction(MicrobeamDetectorConstruction* det) 
:Detector(det)
{   
  saveRndm = 0;  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

MicrobeamRunAction::~MicrobeamRunAction()
{
  delete[] dose3DDose;
  delete[] mapVoxels;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void MicrobeamRunAction::BeginOfRunAction(const G4Run* /*aRun*/)
{  
 
  // Histograms
  // Get/create analysis manager
  G4AnalysisManager* man = G4AnalysisManager::Instance();
  
  // Open an output file
  man->OpenFile("microbeam");
  man->SetFirstNtupleId(1);

  //Declare ntuples
  //
  // Create 1st ntuple (id = 1)
  //
  man->CreateNtuple("ntuple0", "Stopping power");
  man->CreateNtupleDColumn("e");
  man->CreateNtupleDColumn("sp");
  man->FinishNtuple();
  //G4cout << "Ntuple-1 created" << G4endl;

  //
  // Create 2nd ntuple (id = 2)
  //
  man->CreateNtuple("ntuple1", "Beam position");
  man->CreateNtupleDColumn("x");
  man->CreateNtupleDColumn("y");
  man->FinishNtuple();
  //G4cout << "Ntuple-2 created" << G4endl;

  //
  // Create 3rd ntuple (id = 3)
  //
  man->CreateNtuple("ntuple2", "Range");
  man->CreateNtupleDColumn("x");
  man->CreateNtupleDColumn("y");
  man->CreateNtupleDColumn("z");
  man->FinishNtuple();
  //G4cout << "Ntuple-3 created" << G4endl;

  //
  // Create 4th ntuple (id = 4)
  //
  man->CreateNtuple("ntuple3", "Doses");
  man->CreateNtupleDColumn("doseN");
  man->CreateNtupleDColumn("doseC");
  man->FinishNtuple();
  //G4cout << "Ntuple-4 created" << G4endl;

  //
  // Create 5th ntuple (id = 5)
  //
  man->CreateNtuple("ntuple4", "3D");
  man->CreateNtupleDColumn("x");
  man->CreateNtupleDColumn("y");
  man->CreateNtupleDColumn("z");
  man->CreateNtupleDColumn("doseV");
  man->FinishNtuple();
  //G4cout << "Ntuple-3 created" << G4endl;

  G4cout << "All Ntuples have been created " << G4endl;

  // save Rndm status
  if (saveRndm > 0)
    { 
      CLHEP::HepRandom::showEngineStatus();
      CLHEP::HepRandom::saveEngineStatus("beginOfRun.rndm");
    }
 
  numEvent = 0;
  nbOfHitsGas = 0;
    
  // ABSORBED DOSES INITIALIZATION
  DoseN = 0;
  DoseC = 0;
    
  massCytoplasm = Detector->GetMassCytoplasm();
  massNucleus   = Detector->GetMassNucleus();
  nbOfPixels = Detector->GetNbOfPixelsInPhantom();
  
  mapVoxels = new G4ThreeVector[nbOfPixels];
  dose3DDose = new G4float[nbOfPixels];

  for (G4int i=0; i<nbOfPixels; i++)
  {
  	mapVoxels [i]=myMicrobeamPhantomConfiguration.GetVoxelThreeVector(i);
  	dose3DDose[i]=0;
	G4ThreeVector v=mapVoxels[i];
  }
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void MicrobeamRunAction::EndOfRunAction(const G4Run* /*aRun*/)
{     
  G4AnalysisManager* man = G4AnalysisManager::Instance();
  // save Rndm status
  if (saveRndm == 1)
  { 
    CLHEP::HepRandom::showEngineStatus();
    CLHEP::HepRandom::saveEngineStatus("endOfRun.rndm");
  }   
  
  for (G4int i=0; i<nbOfPixels; i++) 
  {  
    G4ThreeVector v;
    v = mapVoxels[i];
    if ( (GetNumEvent()+1) !=0) 
      {
	//Fill ntuple #5
	man->FillNtupleDColumn(5,0,v.x());
	man->FillNtupleDColumn(5,1,v.y());
	man->FillNtupleDColumn(5,2,v.z());
	man->FillNtupleDColumn(5,3,dose3DDose[i]/(GetNumEvent()+1));
	man->AddNtupleRow(5);
      }
  }
   
  G4cout << "-> Total number of particles detected by the gas detector : " << GetNbOfHitsGas() << G4endl;  
  G4cout << G4endl;    
  
  //save histograms      
  man->Write();
  man->CloseFile();
  // Complete clean-up
  delete G4AnalysisManager::Instance();

}
