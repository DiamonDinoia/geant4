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

#ifndef G4BEChargedChannel_h
#define G4BEChargedChannel_h 1

#include "globals.hh"
#include "G4BertiniEvaporationChannel.hh"
#include "Randomize.hh"

class G4BEChargedChannel : public G4BertiniEvaporationChannel
{
public:
  G4BEChargedChannel();
  virtual ~G4BEChargedChannel(); 
  
  virtual void calculateProbability();
  virtual G4DynamicParticle * emit() = 0;
  virtual G4double coulombFactor() = 0;
  G4double coulombFactorForProton();
  G4double qmFactorForProton();
  G4double qmFactorForAlpha();
  G4double sampleKineticEnergy();
  
protected:  
  G4double A;
  G4double spin;
};


#endif
