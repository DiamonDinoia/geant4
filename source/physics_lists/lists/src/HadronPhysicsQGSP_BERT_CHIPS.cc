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
// $Id: HadronPhysicsQGSP_BERT_CHIPS.cc,v 1.2 2010-06-03 10:42:44 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//---------------------------------------------------------------------------
//
// ClassName:   HadronPhysicsQGSP_BERT_CHIPS
//
// Author: 2010 G.Folger
//   derived from HadronPhysicsQGSP_BERT
//
// Modified:
//
//----------------------------------------------------------------------------
//
#include "HadronPhysicsQGSP_BERT_CHIPS.hh"

#include "globals.hh"
#include "G4ios.hh"
#include <iomanip>   
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4HadronicProcessType.hh"

#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

#include "G4QHadronInelasticDataSet.hh"

HadronPhysicsQGSP_BERT_CHIPS::HadronPhysicsQGSP_BERT_CHIPS(G4int)
                    :  G4VPhysicsConstructor("hInelastic QGSP_BERT_CHIPS")
		     , QuasiElastic(true)
{
   ProjectileDiffraction=false;
}

HadronPhysicsQGSP_BERT_CHIPS::HadronPhysicsQGSP_BERT_CHIPS(const G4String& name, G4bool quasiElastic)
                    :  G4VPhysicsConstructor(name) , QuasiElastic(quasiElastic)
{
   ProjectileDiffraction=false;
}

void HadronPhysicsQGSP_BERT_CHIPS::CreateModels()
{
  theNeutrons=new G4NeutronBuilder;
  theNeutrons->RegisterMe(theQGSPNeutron=new G4QGSPNeutronBuilder(QuasiElastic, ProjectileDiffraction));
  theNeutrons->RegisterMe(theLEPNeutron=new G4LEPNeutronBuilder);
  theLEPNeutron->SetMinInelasticEnergy(9.5*GeV);
  theLEPNeutron->SetMaxInelasticEnergy(25*GeV);  

  theNeutrons->RegisterMe(theBertiniNeutron=new G4BertiniNeutronBuilder);
  theBertiniNeutron->SetMinEnergy(0.0*GeV);
  theBertiniNeutron->SetMaxEnergy(9.9*GeV);

  thePro=new G4ProtonBuilder;
  thePro->RegisterMe(theQGSPPro=new G4QGSPProtonBuilder(QuasiElastic, ProjectileDiffraction));
  thePro->RegisterMe(theLEPPro=new G4LEPProtonBuilder);
  theLEPPro->SetMinEnergy(9.5*GeV);
  theLEPPro->SetMaxEnergy(25*GeV);

  thePro->RegisterMe(theBertiniPro=new G4BertiniProtonBuilder);
  theBertiniPro->SetMaxEnergy(9.9*GeV);
  
  thePiK=new G4PiKBuilder;
  thePiK->RegisterMe(theQGSPPiK=new G4QGSPPiKBuilder(QuasiElastic));
  thePiK->RegisterMe(theLEPPiK=new G4LEPPiKBuilder);
  theLEPPiK->SetMaxEnergy(25*GeV);
  theLEPPiK->SetMinEnergy(9.5*GeV);

  thePiK->RegisterMe(theBertiniPiK=new G4BertiniPiKBuilder);
  theBertiniPiK->SetMaxEnergy(9.9*GeV);
  
  theMiscCHIPS=new G4MiscCHIPSBuilder;
}

HadronPhysicsQGSP_BERT_CHIPS::~HadronPhysicsQGSP_BERT_CHIPS()
{
   delete theMiscCHIPS;
   delete theQGSPNeutron;
   delete theLEPNeutron;
   delete theBertiniNeutron;
   delete theQGSPPro;
   delete theLEPPro;
   delete thePro;
   delete theBertiniPro;
   delete theQGSPPiK;
   delete theLEPPiK;
   delete theBertiniPiK;
   delete thePiK;
   delete theCHIPSInelastic;
}

void HadronPhysicsQGSP_BERT_CHIPS::ConstructParticle()
{
  G4MesonConstructor pMesonConstructor;
  pMesonConstructor.ConstructParticle();

  G4BaryonConstructor pBaryonConstructor;
  pBaryonConstructor.ConstructParticle();

  G4ShortLivedConstructor pShortLivedConstructor;
  pShortLivedConstructor.ConstructParticle();  
}

#include "G4ProcessManager.hh"
void HadronPhysicsQGSP_BERT_CHIPS::ConstructProcess()
{
  CreateModels();
  theNeutrons->Build();
  thePro->Build();
  thePiK->Build();
  // use CHIPS cross sections also for Kaons
  theCHIPSInelastic = new G4QHadronInelasticDataSet();
  
  FindInelasticProcess(G4KaonMinus::KaonMinus())->AddDataSet(theCHIPSInelastic);
  FindInelasticProcess(G4KaonPlus::KaonPlus())->AddDataSet(theCHIPSInelastic);
  FindInelasticProcess(G4KaonZeroShort::KaonZeroShort())->AddDataSet(theCHIPSInelastic);
  FindInelasticProcess(G4KaonZeroLong::KaonZeroLong())->AddDataSet(theCHIPSInelastic);

  theMiscCHIPS->Build();
}

G4HadronicProcess* 
HadronPhysicsQGSP_BERT_CHIPS::FindInelasticProcess(const G4ParticleDefinition* p)
{
  G4HadronicProcess* had = 0;
  if(p) {
     G4ProcessVector*  pvec = p->GetProcessManager()->GetProcessList();
     size_t n = pvec->size();
     if(0 < n) {
       for(size_t i=0; i<n; ++i) {
	 if(fHadronInelastic == ((*pvec)[i])->GetProcessSubType()) {
	   had = static_cast<G4HadronicProcess*>((*pvec)[i]);
	   break;
	 }
       }
     }
  }
  return had;
}
