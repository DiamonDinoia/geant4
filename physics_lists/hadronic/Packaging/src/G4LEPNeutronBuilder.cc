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
// GEANT4 tag $Name: not supported by cvs2svn $
//
//---------------------------------------------------------------------------
//
// ClassName:   G4LEPNeutronBuilder
//
// Author: 2002 J.P. Wellisch
//
// Modified:
// 16.11.2005 G.Folger: don't  keep processes as data members, but new these
// 13.06.2006 G.Folger: (re)move elastic scatterring 
//
//----------------------------------------------------------------------------
//
#include "G4LEPNeutronBuilder.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"

G4LEPNeutronBuilder::
G4LEPNeutronBuilder() 
{
  theMin = 0;
  theIMin = theMin;
  theMax = 20*TeV;
  theIMax = 55*GeV;
}

G4LEPNeutronBuilder::
~G4LEPNeutronBuilder() 
{
  delete theNeutronFissionModel;
  delete theNeutronCaptureModel;
  delete theLENeutronModel;
}

void G4LEPNeutronBuilder::
Build(G4HadronElasticProcess *)
{
     G4cout << "Info - G4LEPNeutronBuilder::Build() not adding elastic" << G4endl;
}

void G4LEPNeutronBuilder::
Build(G4HadronFissionProcess * aP)
{
  theNeutronFissionModel = new G4LFission();
  theNeutronFissionModel->SetMinEnergy(theMin);
  theNeutronFissionModel->SetMaxEnergy(theMax);
  aP->RegisterMe(theNeutronFissionModel);
}

void G4LEPNeutronBuilder::
Build(G4HadronCaptureProcess * aP)
{
  theNeutronCaptureModel = new G4LCapture();
  theNeutronCaptureModel->SetMinEnergy(theMin);
  theNeutronCaptureModel->SetMaxEnergy(theMax);
  aP->RegisterMe(theNeutronCaptureModel);
}

void G4LEPNeutronBuilder::
Build(G4NeutronInelasticProcess * aP)
{
  theLENeutronModel = new G4LENeutronInelastic();
  theLENeutronModel->SetMinEnergy(theIMin);
  theLENeutronModel->SetMaxEnergy(theIMax);
  aP->RegisterMe(theLENeutronModel);
}

// 2002 by J.P. Wellisch
