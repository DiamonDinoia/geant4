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
// $Id: G4VShortLivedParticle.cc,v 1.7 2004-09-02 01:52:42 asaim Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 
// --------------------------------------------------------------
//      GEANT 4 class implementation file 
//
//      History: first implementation, based on object model of
//      28 June 1998 H.Kurashige
// --------------------------------------------------------------

#include "G4VShortLivedParticle.hh"

G4VShortLivedParticle::G4VShortLivedParticle(const G4String&  aName,  
               G4double         mass,     
               G4double         width,
               G4double         charge,   
               G4int            iSpin,
               G4int            iParity,
               G4int            iConjugation,
               G4int            iIsospin,   
               G4int            iIsospinZ, 
               G4int            gParity,
               const G4String&  pType,
               G4int            lepton,
               G4int            baryon,
               G4int            encoding,
               G4bool           stable,
               G4double         lifetime,
	       G4DecayTable     *decaytable)
  :G4ParticleDefinition( aName,mass,width,charge,iSpin,iParity,
           iConjugation,iIsospin,iIsospinZ,gParity,pType,
           lepton,baryon,encoding,stable,lifetime,decaytable, true)
{
   
}

G4VShortLivedParticle::~G4VShortLivedParticle() {}

G4int G4VShortLivedParticle::operator==(const G4VShortLivedParticle &right) const
{
  return (this->GetParticleName() == right.GetParticleName());
}

G4int G4VShortLivedParticle::operator!=(const G4VShortLivedParticle &right) const
{
  return (this->GetParticleName() != right.GetParticleName());
}
