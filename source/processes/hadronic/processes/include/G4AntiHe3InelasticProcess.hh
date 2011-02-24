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
//
 // Hadronic Process: AntiHe3 Inelastic Process
 // J.Apostolakis 23 Feb 2011
 // Last modified:
 //
 // Note:  there is no .cc file
 
#ifndef G4AntiHe3InelasticProcess_h
#define G4AntiHe3InelasticProcess_h 1
 
// Class Description
// Process for AntiHe3 Inelastic scattering; 
// to be used in your physics list in case you need this physics.
// Class Description - End

#include "G4HadronInelasticProcess.hh"
#include "G4AntiHe3.hh"
#include "G4AntiAlpha.hh"

 class G4AntiHe3InelasticProcess : public G4HadronInelasticProcess
 {
 public:
    
    G4AntiHe3InelasticProcess( const G4String& processName= "G4AntiHe3InelasticProcess") \
       : G4HadronInelasticProcess( "AntiHe3Inelastic", G4AntiHe3::AntiHe3() )
    { }
    
    ~G4AntiHe3InelasticProcess()
    { }
 };
 
#endif
 

