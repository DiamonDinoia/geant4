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
// * authors in the GEANT4 collaboration.                             *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: G4QParticleVector.hh,v 1.3 2001-08-01 17:03:38 hpw Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
#ifndef G4QParticleVector_h
#define G4QParticleVector_h 1

// ------------------------------------------------------------
//      GEANT 4 class header file
//
//      ---------------- G4QCandidateVector ----------------
//             by Mikhail Kossov, Sept 1999.
// Type defenition for Decay Channel Vector in CHIPS model
// ---------------------------------------------------------------

#include "G4QParticle.hh"
#include "g4rw/tpordvec.h"

typedef G4RWTPtrOrderedVector<G4QParticle> G4QParticleVector;

#endif


