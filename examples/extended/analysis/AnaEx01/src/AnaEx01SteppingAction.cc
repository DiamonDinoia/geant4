// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: AnaEx01SteppingAction.cc,v 1.1.1.1 2000-09-14 11:37:21 barrand Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 

#include "G4AnalysisManager.hh"

#include "AnaEx01SteppingAction.hh"

AnaEx01SteppingAction::AnaEx01SteppingAction(
 G4AnalysisManager* aAnalysisManager
):fAnalysisManager(aAnalysisManager){}

AnaEx01SteppingAction::~AnaEx01SteppingAction(){}
void AnaEx01SteppingAction::UserSteppingAction(const G4Step* aStep){
  if(fAnalysisManager) fAnalysisManager->Step(aStep);
}



