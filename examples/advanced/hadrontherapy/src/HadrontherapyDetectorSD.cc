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
// $Id: HadrontherapyDetectorSD.cc; 
// See more at: http://g4advancedexamples.lngs.infn.it/Examples/hadrontherapy

#include "HadrontherapyDetectorSD.hh"
#include "HadrontherapyAnalysisManager.hh"
#include "HadrontherapyDetectorHit.hh"
#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4SDManager.hh"
#include "HadrontherapyMatrix.hh"
#include "HadrontherapyLet.hh"

/////////////////////////////////////////////////////////////////////////////
HadrontherapyDetectorSD::HadrontherapyDetectorSD(G4String name):G4VSensitiveDetector(name)
{ 
  G4String HCname;
  collectionName.insert(HCname="HadrontherapyDetectorHitsCollection");
  HitsCollection = NULL; 
  G4String sensitiveDetectorName = name;

}

/////////////////////////////////////////////////////////////////////////////
HadrontherapyDetectorSD::~HadrontherapyDetectorSD()
{ 
}

/////////////////////////////////////////////////////////////////////////////
void HadrontherapyDetectorSD::Initialize(G4HCofThisEvent*)
{
  HitsCollection = new HadrontherapyDetectorHitsCollection(sensitiveDetectorName,
							   collectionName[0]);
}

/////////////////////////////////////////////////////////////////////////////
G4bool HadrontherapyDetectorSD::ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist)
{
  //The code doesn't seem to get here if we use the IAEA geometry. FIXME
  if(!ROhist)
    return false;

  if (aStep -> GetPreStepPoint() -> GetPhysicalVolume() -> GetName() != "DetectorPhys")
    return false;
  // Get kinetic energy
  G4Track * theTrack = aStep  ->  GetTrack();
  G4double kineticEnergy =  theTrack -> GetKineticEnergy();     

  G4ParticleDefinition *particleDef = theTrack -> GetDefinition();
  //Get particle name  
  G4String particleName =  particleDef -> GetParticleName();  
  // G4cout << particleDef -> GetParticleType() << '\n';  
  // Get unique track_id (in an event)
  G4int trackID = theTrack -> GetTrackID();

  G4double energyDeposit = aStep -> GetTotalEnergyDeposit();

  G4int Z = particleDef-> GetAtomicNumber();
  G4int A = particleDef-> GetAtomicMass();

  // Read voxel indexes: i is the x index, k is the z index
  G4int k  = ROhist -> GetReplicaNumber(0);
  G4int i  = ROhist -> GetReplicaNumber(2);
  G4int j  = ROhist -> GetReplicaNumber(1);

#ifdef ANALYSIS_USE
  static HadrontherapyAnalysisManager* analysis = HadrontherapyAnalysisManager::getInstance();
#endif

  static HadrontherapyMatrix* matrix = HadrontherapyMatrix::GetInstance();
  static HadrontherapyLet* let = HadrontherapyLet::GetInstance();

  if (matrix && let)
    {

      // Increment Fluences & accumulate energy spectra
      // Hit voxels are marked with track_id throught hitTrack matrix
      // XXX TrackID sequences inside a voxel are NOT decrescent (?)  XXX
      G4int* hitTrack = matrix -> GetHitTrack(i,j,k); // hitTrack MUST BE cleared at every eventAction!
      if ( *hitTrack != trackID )
	{
	  //G4cout << "TrackID " << TrackID << " Voxel " << i << '\t' << j << '\t' << k << G4endl;
	  *hitTrack = trackID;

	  // Fill FLUENCE data for every single nuclide 
	  if ( Z>= 1)    //  exclude e-, neutrons, gamma, ...
	    matrix-> Fill(particleDef, i, j, k, 0, true);


	  // Fill LET data for every single nuclide (kinetic energy spectrum taken from first step in voxel).
	  // Let will be calculated at endOfRunAction.
	  // A data ASCII file will be generated in main through the command let -> StoreData(filename)
	  // Put this command at endOfRunAction in case of multiple runs!
	  //
	  //if (A==12 && Z==6) // C12
	  //if (A==4 && Z==2)  // Helium
	  //if (A==1 && Z==1)  // Protons 
	  if ( Z>=1 )          // Exclude e-, neutrons, gamma, ...
	    let -> FillEnergySpectrum(particleDef, kineticEnergy/MeV, i, j, k);
    

	}	 
      if(energyDeposit != 0)                       
	{  
	  // Energy deposit.
	  // This method will fill a dose matrix for every single nuclide. 
	  // A data ASCII file can be generated through the method StoreData(filename) 
	  // into the matrix class.
	  // In case of multiple runs, remember to: 
	  // a) Initialize() the matrix and the Let classes (Initialize() method for both) 
	  // B) Put the StoreData(filename) method at endOfRunAction
	
	  // if (A==1 && Z==1) // primary and sec. protons 
	  if ( Z>=1 )    //  exclude e-, neutrons, gamma, ...
	    matrix -> Fill(particleDef, i, j, k, energyDeposit/MeV);

	  // Create a hit with the information of position is in the detector     
	  HadrontherapyDetectorHit* detectorHit = new HadrontherapyDetectorHit();       
	  detectorHit -> SetEdepAndPosition(i, j, k, energyDeposit); 
	  HitsCollection -> insert(detectorHit);
	}
    }

#ifdef G4ANALYSIS_USE_ROOT
  // First step kinetic energy (Ntuple!)
  if (Z>=1) 
    analysis -> FillKineticFragmentTuple(i, j, k, A, Z, kineticEnergy/MeV);
#endif

#ifdef ANALYSIS_USE
  if(energyDeposit != 0)                       
    {  
      if(trackID != 1)
	{
	  if (particleName == "proton")
	    analysis -> SecondaryProtonEnergyDeposit(i, energyDeposit/MeV);

	  else if (particleName == "neutron")
	    analysis -> SecondaryNeutronEnergyDeposit(i, energyDeposit/MeV);

	  else if (particleName == "alpha")
	    analysis -> SecondaryAlphaEnergyDeposit(i, energyDeposit/MeV);

	  else if (particleName == "gamma")
	    analysis -> SecondaryGammaEnergyDeposit(i, energyDeposit/MeV);

	  else if (particleName == "e-")
	    analysis -> SecondaryElectronEnergyDeposit(i, energyDeposit/MeV);

	  else if (particleName == "triton")
	    analysis -> SecondaryTritonEnergyDeposit(i, energyDeposit/MeV);

	  else if (particleName == "deuteron")
	    analysis -> SecondaryDeuteronEnergyDeposit(i, energyDeposit/MeV);

	  else if (particleName == "pi+" || particleName == "pi-" ||  particleName == "pi0")
	    analysis -> SecondaryPionEnergyDeposit(i, energyDeposit/MeV);   	
	}
    }
#endif

  return true;
}

/////////////////////////////////////////////////////////////////////////////
void HadrontherapyDetectorSD::EndOfEvent(G4HCofThisEvent* HCE)
{
  static G4int HCID = -1;
  if(HCID < 0)
    { 
      HCID = GetCollectionID(0); 
    }

  HCE -> AddHitsCollection(HCID,HitsCollection);
}

