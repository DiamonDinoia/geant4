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
// $Id: G4ITSteppingMessenger.hh 60427 2012-07-11 16:34:35Z matkara $
//
// Author: Mathieu Karamitros (kara (AT) cenbg . in2p3 . fr)
//
// WARNING : This class is released as a prototype.
// It might strongly evolve or even disapear in the next releases.
//

#ifndef G4ITSTEPPINGMESSENGER_H
#define G4ITSTEPPINGMESSENGER_H

class G4ITStepManager;
class G4UIdirectory;
class G4UIcmdWithoutParameter;
class G4UIcmdWithAnInteger;
class G4UIcommand;
class G4UIcmdWithADoubleAndUnit;

#include "G4UImessenger.hh"
#include "globals.hh"

class G4ITSteppingMessenger: public G4UImessenger
{
  public:
    G4ITSteppingMessenger(G4ITStepManager* runMgr);
    ~G4ITSteppingMessenger();

  public:
    void SetNewValue(G4UIcommand * command,G4String newValues);
    G4String GetCurrentValue(G4UIcommand * command);

  private:
    G4ITStepManager * fITStepManager;

  private: //commands
    G4UIdirectory*              fITDirectory;

    G4UIcmdWithADoubleAndUnit*  fEndTime;
    G4UIcmdWithAnInteger*       fVerboseCmd;
    G4UIcmdWithAnInteger*       fMaxStepNumber;
    G4UIcmdWithoutParameter*    fInitCmd;
    G4UIcmdWithoutParameter*    fProcessCmd;
    G4UIcmdWithAnInteger*       fMaxNULLTimeSteps;
};

#endif // G4ITSTEPPINGMESSENGER_H
