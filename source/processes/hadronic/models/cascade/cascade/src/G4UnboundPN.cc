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
// $Id: G4UnboundPN.cc,v 1.3 2010-04-13 05:30:10 mkelsey Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// ------------------------------------------------------------
//      Bertini Cascade unboundPN class implementation file
//
//      History: first implementation, inspired by G4Proton
//      17 Nov 2009:  Michael Kelsey
//	06 Apr 2010:  Do G4Ions initialization in ctor.
//	13 Apr 2010:  Per Kurashige, inherit from G4VShortLivedParticle.
// ----------------------------------------------------------------

#include "G4UnboundPN.hh"

#include "globals.hh"
#include "G4ios.hh"
#include "G4Ions.hh"

// ######################################################################
// ###                          UNBOUNDPN                             ###
// ######################################################################
G4UnboundPN* G4UnboundPN::theInstance = 0;

//    Arguments for constructor are as follows
//               name             mass          width         charge
//             2*spin           parity  C-conjugation
//          2*Isospin       2*Isospin3       G-parity
//               type    lepton number  baryon number   PDG encoding
//             stable         lifetime    decay table
G4UnboundPN::G4UnboundPN()
  : G4VShortLivedParticle("unboundPN",
			  (proton_mass_c2+neutron_mass_c2), 0.0*MeV, +1.*eplus, 
			  2,       +1,       0,          
			  2,        0,       0,             
			  "nucleus",        0,      +2, 0, /* ? 100010020 */
			  true,       0.,    NULL) {}

G4UnboundPN* G4UnboundPN::Definition() {
  if (0 == theInstance) theInstance = new G4UnboundPN;	// There can be only one
  return theInstance;
}

// Simple call-throughs
G4UnboundPN* G4UnboundPN::UnboundPNDefinition() { return Definition(); }
G4UnboundPN* G4UnboundPN::UnboundPN()           { return Definition(); }
