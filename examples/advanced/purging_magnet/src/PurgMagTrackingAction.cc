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
// ********************************************************************
// Code developed by:
//  S.Larsson
//
//    ***********************************
//    *                                 *
//    *    PurgMagTrackingAction.cc     *
//    *                                 *
//    ***********************************
//
// $Id: PurgMagTrackingAction.cc,v 1.1 2003-12-09 14:13:24 sussi Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//

#include "PurgMagTrackingAction.hh"
#include "PurgMagRunAction.hh"

#include "G4TrackingManager.hh"
#include "G4Track.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

PurgMagTrackingAction::PurgMagTrackingAction(PurgMagRunAction* run)
:PurgMagRun(run)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void PurgMagTrackingAction::PostUserTrackingAction(const G4Track*)
{   
}










