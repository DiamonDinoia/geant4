//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
 #include "G4FTFCPiKBuilder.hh"
 #include "G4ParticleDefinition.hh"
 #include "G4ParticleTable.hh"
 #include "G4ProcessManager.hh"

 G4FTFCPiKBuilder::
 G4FTFCPiKBuilder() 
 {
   theMin = 15*GeV;
   theModel = new G4TheoFSGenerator;
   theCascade = new G4StringChipsParticleLevelInterface;
   theStringDecay = new G4ExcitedStringDecay(&theFragmentation);
   theModel->SetTransport(theCascade);
   theModel->SetHighEnergyGenerator(&theStringModel);
   theStringModel.SetFragmentationModel(theStringDecay);
 }

 G4FTFCPiKBuilder::
 ~G4FTFCPiKBuilder() 
 {
   delete theStringDecay;
 }

 void G4FTFCPiKBuilder::
 Build(G4HadronElasticProcess & ) {}

 void G4FTFCPiKBuilder::
 Build(G4PionPlusInelasticProcess & aP)
 {
   aP.AddDataSet(&thePiData);
   theModel->SetMinEnergy(theMin);
   theModel->SetMaxEnergy(100*TeV);
   aP.RegisterMe(theModel);
 }

 void G4FTFCPiKBuilder::
 Build(G4PionMinusInelasticProcess & aP)
 {
   aP.AddDataSet(&thePiData);
   theModel->SetMinEnergy(theMin);
   theModel->SetMaxEnergy(100*TeV);
   aP.RegisterMe(theModel);
 }

 void G4FTFCPiKBuilder::
 Build(G4KaonPlusInelasticProcess & aP)
 {
   theModel->SetMinEnergy(theMin);
   theModel->SetMaxEnergy(100*TeV);
   aP.RegisterMe(theModel);
 }

 void G4FTFCPiKBuilder::
 Build(G4KaonMinusInelasticProcess & aP)
 {
   theModel->SetMinEnergy(theMin);
   theModel->SetMaxEnergy(100*TeV);
   aP.RegisterMe(theModel);
 }

 void G4FTFCPiKBuilder::
 Build(G4KaonZeroLInelasticProcess & aP)
 {
   theModel->SetMinEnergy(theMin);
   theModel->SetMaxEnergy(100*TeV);
   aP.RegisterMe(theModel);
 }

 void G4FTFCPiKBuilder::
 Build(G4KaonZeroSInelasticProcess & aP)
 {
   theModel->SetMinEnergy(theMin);
   theModel->SetMaxEnergy(100*TeV);
   aP.RegisterMe(theModel);
 }

 // 2002 by J.P. Wellisch
