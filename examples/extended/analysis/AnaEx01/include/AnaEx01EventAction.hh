// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: AnaEx01EventAction.hh,v 1.1.1.1 2000-09-14 11:37:21 barrand Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 

#ifndef AnaEx01EventAction_h
#define AnaEx01EventAction_h

#include "G4UserEventAction.hh"

class G4AnalysisManager;

class AnaEx01EventAction : public G4UserEventAction {
public:
  AnaEx01EventAction(G4AnalysisManager*);
  virtual ~AnaEx01EventAction();
public:
  virtual void BeginOfEventAction(const G4Event*);
  virtual void EndOfEventAction(const G4Event*);
private:
  G4AnalysisManager* fAnalysisManager;
};

#endif

    
