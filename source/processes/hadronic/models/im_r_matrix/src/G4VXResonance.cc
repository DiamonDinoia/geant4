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
// -------------------------------------------------------------------
//      GEANT4 Class file
//
//      For information related to this code contact:
//
//      File name:     G4VXResonance
//
//      Author:        
// 
//      Creation date: 15 April 1999
//
//      Modifications: 
//      
// -------------------------------------------------------------------

#include "globals.hh"
#include "G4ios.hh"
#include "G4KineticTrack.hh"
#include "G4VXResonance.hh"
#include "Randomize.hh"
#include "G4Proton.hh"

G4VXResonance::G4VXResonance()
{ }


G4VXResonance::~G4VXResonance() 
{  }


G4bool G4VXResonance::operator==(const G4VXResonance &right) const
{
  return (this == (G4VXResonance *) &right);
}


G4bool G4VXResonance::operator!=(const G4VXResonance &right) const
{
  return (this != (G4VXResonance *) &right);
}


G4double G4VXResonance::IsospinCorrection(const G4KineticTrack& trk1, 
					  const G4KineticTrack& trk2,
					  G4int isoOut1, G4int isoOut2,
					  G4double /*iSpinOut1*/, G4double /*iSpinOut2*/) const
{
  G4double result = 0.;
 
  G4ParticleDefinition* in1 = trk1.GetDefinition();
  G4ParticleDefinition* in2 = trk2.GetDefinition();

  G4int isoIn1  = in1->GetPDGiIsospin();
  G4int iso3In1 = in1->GetPDGiIsospin3();
  G4int isoIn2  = in2->GetPDGiIsospin();
  G4int iso3In2 = in2->GetPDGiIsospin3();

  G4int isoProton = G4Proton::ProtonDefinition()->GetPDGiIsospin();
  G4int iso3Proton = G4Proton::ProtonDefinition()->GetPDGiIsospin3();
  
  G4double pWeight = clebsch.Weight(isoProton,iso3Proton, isoProton,iso3Proton, isoOut1,isoOut2);
  if (pWeight == 0.) G4Exception ("G4VXResonance::IsospinCorrection, no resonances - pWeight is zero");

  if (in1->IsShortLived() || in2->IsShortLived())
  {
    // Resonances in the initial state
    G4int iSpinProton = G4Proton::ProtonDefinition()->GetPDGiSpin();
    G4double degeneracyFactor = DegeneracyFactor(trk1,trk2,iSpinProton,iSpinProton);

    G4double factor = degeneracyFactor * pWeight;
    if (factor > DBL_MIN)
    {
      // Randomly select the Isospin3 of the initial state resonances
      std::vector<G4double> iso = clebsch.GenerateIso3(isoIn1,iso3In1, 
						  isoIn2,iso3In2, 
						  isoProton,isoProton);
      G4int isoA = lrint(iso[0]);
      G4int isoB = lrint(iso[1]);
      G4double rWeight =  clebsch.Weight(isoProton,isoA,
					 isoProton,isoB,
					 isoOut1,isoOut2);
      result = rWeight / pWeight;
    }
  }
  else
  {
    G4double weight = clebsch.Weight(isoIn1,iso3In1, isoIn2,iso3In2, isoOut1,isoOut2);
    result = weight / pWeight;
  }
  
  return result;
}


#include "G4DetailedBalancePhaseSpaceIntegral.hh"

G4double G4VXResonance::DetailedBalance(const G4KineticTrack& trk1, 
					const G4KineticTrack& trk2,
					G4int isoOut1, G4int isoOut2,
					G4double iSpinOut1, G4double iSpinOut2,
					G4double mOut1, G4double mOut2) const
{
  // To handle the cases when resonances are involved the modified
  // detailed balance of P. Danielewicz and G.F. Bertsch, Nucl. Phys. A533(1991) 712
  // is used; in other words, the width of the resonances are folded to get the
  // mean square of the final state momentum.

  G4ParticleDefinition* in1 = trk1.GetDefinition();
  G4ParticleDefinition* in2 = trk2.GetDefinition();
  if(in1->IsShortLived() && in2->IsShortLived())
  {
    G4Exception("Detailed balance for resonance scattering still on the schedule.");
  }

  G4double result = 0.;

  G4int isoIn1 = in1->GetPDGiIsospin();
  G4int iso3In1 = in1->GetPDGiIsospin3();
  G4int isoIn2 = in2->GetPDGiIsospin();
  G4int iso3In2 = in2->GetPDGiIsospin3();  
  G4double weight = clebsch.Weight(isoIn1, iso3In1, isoIn2, iso3In2, isoOut1, isoOut2);
  
  if (weight > 00001)
  {
    // adding spin counting here ...... does not look quite consistent, but is correct anyway. 
    // revisit in the next design iteration @@
    G4double degeneracy = DegeneracyFactor(trk1,trk2,iSpinOut1,iSpinOut2);
    G4double factor = degeneracy * weight;

    // now the phase-space
    G4double s = (trk1.Get4Momentum() + trk2.Get4Momentum()).mag2();
    G4double m1 = in1->GetPDGMass();
    G4double m2 = in2->GetPDGMass();
    
    // on-shell
    G4double pinitial2 = (s - (m1+m2) * (m1+m2)) * (s - (m1-m2) * (m1-m2)) / (4.0*s);
    G4double pfinal2 = (s - (mOut1+mOut2) * (mOut1+mOut2)) * (s - ( mOut1-mOut2) * (mOut1-mOut2)) / (4.0*s);
    G4double relativeMomsquared = pfinal2/pinitial2;

    // resonance-nucleon scattering - inverse channel
    if(in1->IsShortLived())
    {
      G4DetailedBalancePhaseSpaceIntegral theI(in1);
      relativeMomsquared = 1./theI.GetPhaseSpaceIntegral(sqrt(s));
    }
    else if(in2->IsShortLived())
    {
      G4DetailedBalancePhaseSpaceIntegral theI(in2);
      relativeMomsquared = 1./theI.GetPhaseSpaceIntegral(sqrt(s));
    }

    result = factor * relativeMomsquared;
  }

  return result;
}
  

G4double G4VXResonance::DegeneracyFactor(const G4KineticTrack& trk1, 
					 const G4KineticTrack& trk2,
					 G4double iSpinOut1, G4double iSpinOut2) const 
{
  G4double value = 0.;

  G4ParticleDefinition* in1 = trk1.GetDefinition();
  G4ParticleDefinition* in2 = trk2.GetDefinition();

  G4double sIn1 =  in1->GetPDGiSpin()  + 1.;
  G4double sIn2 =  in2->GetPDGiSpin()  + 1.;
  
  G4double denom = sIn1 * sIn2;
  if (denom > 0.)
  {
    value = (iSpinOut1+1) * (iSpinOut2+1) / denom;
  }
  return value;
}


