// This code implementation is the intellectual property of
// the RD44 GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: Tst01SteppingAction.hh,v 1.1 1999-01-08 16:34:29 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef Tst01SteppingAction_h
#define Tst01SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "Histo.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class Tst01SteppingAction : public G4UserSteppingAction
{
  public:
    Tst01SteppingAction();
   ~Tst01SteppingAction();

    void UserSteppingAction();
private:
  odHisto Steplength;
  odHisto SteplengthProfile;
};

#endif
