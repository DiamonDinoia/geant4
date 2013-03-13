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
#include "QGSPCMS_FTFP_BERT_EML.hh"
#include "CMSEmStandardPhysics92.hh"
#include "G4EmStandardPhysics_option1.hh"

#include "G4SystemOfUnits.hh"
#include "G4DecayPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4IonPhysics.hh"
#include "G4StoppingPhysics.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4NeutronTrackingCut.hh"

#include "G4DataQuestionaire.hh"
#include "G4HadronPhysicsQGSP_FTFP_BERT.hh"

QGSPCMS_FTFP_BERT_EML::QGSPCMS_FTFP_BERT_EML()
{
  int ver = 1;

  G4DataQuestionaire it(photon);
  defaultCutValue = 0.7*mm;  
  SetVerboseLevel(ver);
  
  G4cout << "You are using " << "QGSP_FTFP_BERT_EML "
	 << G4endl;

  // EM Physics
  //RegisterPhysics( new CMSEmStandardPhysics92(ver));
  RegisterPhysics( new G4EmStandardPhysics_option1(ver));

  // Synchroton Radiation & GN Physics
  RegisterPhysics( new G4EmExtraPhysics(ver));

  // Decays
  RegisterPhysics( new G4DecayPhysics(ver));

  // Hadron Elastic scattering
  RegisterPhysics( new G4HadronElasticPhysics(ver));

  // Hadron Physics
  //G4bool quasiElastic=true;
  //RegisterPhysics( new HadronPhysicsQGSPCMS_FTFP_BERT("hadron",quasiElastic)); 
  RegisterPhysics( new G4HadronPhysicsQGSP_FTFP_BERT(ver)); 
  
  // Stopping Physics
  RegisterPhysics( new G4StoppingPhysics(ver));

  // Ion Physics
  RegisterPhysics( new G4IonPhysics(ver));

  // Neutron tracking cut
  RegisterPhysics( new G4NeutronTrackingCut(ver));
}

QGSPCMS_FTFP_BERT_EML::~QGSPCMS_FTFP_BERT_EML()
{}

void QGSPCMS_FTFP_BERT_EML::SetCuts()
{
  SetCutsWithDefault();   
}
