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
// Implementation of the HETC88 code into Geant4.
// Evaporation and De-excitation parts
// T. Lampen, Helsinki Institute of Physics, May-2000

#ifndef G4BEGammaDeexcitation_h
#define G4BEGammaDeexcitation_h 1

#include "globals.hh"

class G4BEGammaDeexcitation
{
public:
  G4BEGammaDeexcitation();
  virtual ~G4BEGammaDeexcitation();

  void setVerboseLevel( G4int verbose ); 

  void setNucleusA( G4int inputA );
  void setNucleusZ( G4int inputZ );
  void setExcitationEnergy( G4double inputE );

  G4DynamicParticle * emit();

private:  
  G4double sampleKineticEnergy();
  G4int verboseLevel;
  G4int nucleusA;
  G4int nucleusZ;
  G4double excitationEnergy;
  void isotropicCosines( G4double&,
			 G4double&,
			 G4double& );
};


#endif
