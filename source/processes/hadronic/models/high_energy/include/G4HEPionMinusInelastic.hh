// This code implementation is the intellectual property of
// the RD44 GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: G4HEPionMinusInelastic.hh,v 1.2 1999-06-16 04:19:11 kurasige Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//
// G4 Gheisha High Energy model class -- header file
// H. Fesefeldt, RWTH Aachen 23-October-1996
// Last modified: 10-December-1996

// A prototype of the Gheisha High Energy collision model.

#ifndef G4HEPionMinusInelastic_h
#define G4HEPionMinusInelastic_h 1

#include "G4HEInelastic.hh"

class G4HEPionMinusInelastic : public G4HEInelastic  
{
 public: 
        G4HEPionMinusInelastic() : G4HEInelastic()
           {
             G4int    vecLen       = 0;
           }

        ~G4HEPionMinusInelastic(){ };
         
        G4int vecLength;
        
        G4VParticleChange * ApplyYourself( const G4Track &aTrack, G4Nucleus &targetNucleus );

        G4int  GetNumberOfSecondaries()
               { return vecLength; }         

 private:

        void   FirstIntInCasPionMinus(G4bool &inElastic, const G4double availableEnergy,
                                     G4HEVector pv[],
                                     G4int &vecLen, 
                                     G4HEVector incidentParticle,
                                     G4HEVector targetParticle);
};
#endif                     
                                         

