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
//
// $Id: G4AntiLambda.cc,v 1.15 2004-09-02 01:52:31 asaim Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 
// ----------------------------------------------------------------------
//      GEANT 4 class implementation file
//
//      History: first implementation, based on object model of
//      4th April 1996, G.Cosmo
// **********************************************************************
//  New impelemenataion as an utility class  M.Asai, 26 July 2004
// ----------------------------------------------------------------------

#include "G4AntiLambda.hh"
#include "G4ParticleTable.hh"

#include "G4PhaseSpaceDecayChannel.hh"
#include "G4DecayTable.hh"

// ######################################################################
// ###                           AntiLambda                           ###
// ######################################################################

G4ParticleDefinition* G4AntiLambda::theInstance = 0;

G4ParticleDefinition* G4AntiLambda::Definition()
{
  if (theInstance !=0) return theInstance;
  const G4String name = "anti_lambda";
  // search in particle table]
  G4ParticleTable* pTable = G4ParticleTable::GetParticleTable();
  theInstance = pTable->FindParticle(name);
  if (theInstance !=0) return theInstance;

  // create particle
  //
  //    Arguments for constructor are as follows
  //               name             mass          width         charge
  //             2*spin           parity  C-conjugation
  //          2*Isospin       2*Isospin3       G-parity
  //               type    lepton number  baryon number   PDG encoding
  //             stable         lifetime    decay table
  //             shortlived      subType    anti_encoding

  theInstance = new G4ParticleDefinition(
                 name,    1.115684*GeV,  2.501e-12*MeV,         0.0, 
		    1,              +1,             0,          
		    0,               0,             0,             
	     "baryon",               0,            -1,       -3122,
		false,       0.2632*ns,          NULL,
                false,       "lambda");
 //create Decay Table 
  G4DecayTable* table = new G4DecayTable();

  // create decay channels
  G4VDecayChannel** mode = new G4VDecayChannel*[2];
  // anti_lambda -> anti_proton + pi+
  mode[0] = new G4PhaseSpaceDecayChannel("anti_lambda",0.639,2,"anti_proton","pi+");
  // anti_lambda -> anti_neutron + pi0
  mode[1] = new G4PhaseSpaceDecayChannel("anti_lambda",0.358,2,"anti_neutron","pi0");
 
  for (G4int index=0; index <2; index++ ) table->Insert(mode[index]);  
  delete [] mode;

  theInstance->SetDecayTable(table);
  return theInstance;
}

G4ParticleDefinition*  G4AntiLambda::AntiLambdaDefinition()
{
  return Definition();
}

G4ParticleDefinition*  G4AntiLambda::AntiLambda()
{
  return Definition();
}


