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
// $Id: G4ImportancePostStepDoIt.hh,v 1.2 2002-04-09 17:40:13 gcosmo Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// ----------------------------------------------------------------------
// Class G4ImportancePostStepDoIt
//
// Class description:
//
// <<insert the description here>>

// Author: Michael Dressel (Michael.Dressel@cern.ch)
// ----------------------------------------------------------------------
#ifndef G4ImportancePostStepDoIt_hh
#define G4ImportancePostStepDoIt_hh

class G4VImportanceSampler;
class G4ParticleChange;
class G4Track;
class G4Step;
class G4Nsplit_Weight;

class G4ImportancePostStepDoIt
{

public:  // with description

  G4ImportancePostStepDoIt();
  ~G4ImportancePostStepDoIt();
  
  void DoIt(const G4Track& aTrack, 
	    G4ParticleChange *aParticleChange, 
	    const G4Nsplit_Weight nw);
  
private:

  void Split(const G4Track &aTrack,
	     const G4Nsplit_Weight &nw,
	     G4ParticleChange *aParticleChange);
};

#endif
