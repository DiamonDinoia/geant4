// This code implementation is the intellectual property of
// the RD44 GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: T07EventAction.hh,v 1.1 1999-01-08 16:35:09 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef T07EventAction_h
#define T07EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

#ifdef MAKEHBOOK
#include "HbookHistogram.hh"
#else
#include "Histo.hh"
#endif

class T07EventActionMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class T07EventAction : public G4UserEventAction
{
  public:
    T07EventAction();
   ~T07EventAction();

  public:
    void BeginOfEventAction();
    void EndOfEventAction();
    
    void SetDrawFlag(G4String val)  {drawFlag = val;};
    
  private:
    G4int    calorimeterCollID;                // Hits collection ID
    G4String drawFlag;                         // control the drawing of event
    T07EventActionMessenger*  eventMessenger;
#ifdef MAKEBOOK
    HbookHistogram *EnergyAbsorber;
    HbookHistogram *EnergyGap;    
    HbookHistogram *TrackLengthAbsorber;
    HbookHistogram *TrackLengthGap;
#else
    odHisto EnergyAbsorber;
    odHisto EnergyGap;
    odHisto TrackLengthAbsorber;
    odHisto TrackLengthGap;
#endif    
};

#endif

    
