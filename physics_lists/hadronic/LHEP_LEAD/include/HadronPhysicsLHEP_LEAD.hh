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
#ifndef HadronPhysicsLHEP_LEAD_h
#define HadronPhysicsLHEP_LEAD_h 1

#include "globals.hh"
#include "G4ios.hh"

#include "G4VPhysicsConstructor.hh"
#include "G4HadronQEDBuilder.hh"
#include "G4StoppingHadronBuilder.hh"
#include "G4MiscLHEPBuilder.hh"

#include "G4PiKBuilder.hh"
#include "G4LHEPPiKBuilder.hh"
#include "G4LEADPiKBuilder.hh"

#include "G4ProtonBuilder.hh"
#include "G4LHEPProtonBuilder.hh"
#include "G4LEADProtonBuilder.hh"

#include "G4NeutronBuilder.hh"
#include "G4LHEPNeutronBuilder.hh"
#include "G4LEADNeutronBuilder.hh"

class HadronPhysicsLHEP_LEAD : public G4VPhysicsConstructor
{
  public: 
    HadronPhysicsLHEP_LEAD(const G4String& name ="hadron");
    virtual ~HadronPhysicsLHEP_LEAD();

  public: 
    virtual void ConstructParticle();
    virtual void ConstructProcess();

  private:
    G4NeutronBuilder theNeutrons;
    G4LHEPNeutronBuilder theLHEPNeutron;
    G4LEADNeutronBuilder theLEADNeutron;
    
    G4PiKBuilder thePiK;
    G4LHEPPiKBuilder theLHEPPiK;
    G4LEADPiKBuilder theLEADPiK;
    
    G4ProtonBuilder thePro;
    G4LHEPProtonBuilder theLHEPPro;
    G4LEADProtonBuilder theLEADPro;    
    
    G4MiscLHEPBuilder theMiscLHEP;
    G4StoppingHadronBuilder theStoppingHadron;
    G4HadronQEDBuilder theHadronQED;
};

// 2002 by J.P. Wellisch

#endif

