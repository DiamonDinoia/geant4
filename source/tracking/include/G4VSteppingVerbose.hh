// This code implementation is the intellectual property of
// the RD44 GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: G4VSteppingVerbose.hh,v 1.2 1999-10-14 05:39:47 tsasaki Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//  
//---------------------------------------------------------------
//
// G4VSteppingVerbose.hh
//
// Description:
//   This class manages the vervose outputs in G4SteppingManager. 
//   
//
// Contact:
//   Questions and comments to this code should be sent to
//     Katsuya Amako  (e-mail: Katsuya.Amako@kek.jp)
//     Takashi Sasaki (e-mail: Takashi.Sasaki@kek.jp)
//
//---------------------------------------------------------------

#ifndef G4VSteppingVerbose_h
#define G4VSteppingVerbose_h

class G4SteppingManager;

#include "globals.hh"                 // Include from 'global'
class G4Navigator;
class G4VPhysicalVolume;
class G4VSensitiveDetector;
class G4VProcess;
class G4ProcessVector;
class G4SteppingManager;              // Include from 'tracking'
class G4Track;
#include "G4TrackVector.hh"            // Include from 'tracking'
#include "G4StepStatus.hh"            // Include from 'track'
class G4UserSteppingAction;
class G4StepPoint;
class G4VTouchable;
class G4VParticleChange;
#include "G4ForceCondition.hh"  //enum 'track'
#include "G4GPILSelection.hh"   //enum 'track'


class G4VSteppingVerbose{

// Constructor/Destructor
protected:    // to force 'singleton'
  G4VSteppingVerbose();  
public:   
  virtual ~G4VSteppingVerbose();
  //
public:   
// 
  static void SetInstance(G4VSteppingVerbose* Instance)
    {
      fInstance = Instance;
    }
  static G4VSteppingVerbose* GetInstance()
    {
      return fInstance;
    }
//
  virtual void NewStep() = 0;
  void CopyState();
  void SetManager(G4SteppingManager* const);
  virtual void AtRestDoItInvoked() = 0;
  virtual void AlongStepDoItAllDone() = 0;
  virtual void PostStepDoItAllDone() = 0;
  virtual void AlongStepDoItOneByOne() = 0;
  virtual void PostStepDoItOneByOne() = 0;
  virtual void StepInfo() = 0;
  virtual void TrackingStarted() = 0;
  virtual void DPSLStarted() = 0;
  virtual void DPSLUserLimit() = 0;
  virtual void DPSLPostStep() = 0;
  virtual void DPSLAlongStep() = 0;
  virtual void VerboseTrack() = 0;
  virtual void VerboseParticleChange() = 0;
  // Member data

protected:  // pointer to the instance 
  static G4VSteppingVerbose* fInstance;
protected:
  G4SteppingManager* fManager;
  
  G4UserSteppingAction* fUserSteppingAction;
  
  G4double PhysicalStep;
  G4double GeometricalStep;
  G4double CorrectedStep;
  G4bool PreStepPointIsGeom;
  G4bool FirstStep;
  G4StepStatus fStepStatus;

  G4double TempInitVelocity;
  G4double TempVelocity;
  G4double Mass;

  G4double sumEnergyChange;

  G4VParticleChange* fParticleChange;
  G4Track* fTrack;
  G4TrackVector* fSecondary;
  G4Step* fStep;
  G4StepPoint* fPreStepPoint;
  G4StepPoint* fPostStepPoint;

  G4VPhysicalVolume* fCurrentVolume;
  G4VSensitiveDetector* fSensitive;
  G4VProcess* fCurrentProcess;
  // The pointer to the process of which DoIt or
  // GetPhysicalInteractionLength has been just executed.


  G4ProcessVector* fAtRestDoItVector;
  G4ProcessVector* fAlongStepDoItVector;
  G4ProcessVector* fPostStepDoItVector;

  G4ProcessVector* fAtRestGetPhysIntVector;
  G4ProcessVector* fAlongStepGetPhysIntVector;
  G4ProcessVector* fPostStepGetPhysIntVector;

  size_t MAXofAtRestLoops;
  size_t MAXofAlongStepLoops;
  size_t MAXofPostStepLoops;
  
  G4double currentMinimumStep;
  G4double numberOfInteractionLengthLeft;

  size_t fAtRestDoItProcTriggered;
  size_t fAlongStepDoItProcTriggered;
  size_t fPostStepDoItProcTriggered;

  G4int fN2ndariesAtRestDoIt;
  G4int fN2ndariesAlongStepDoIt;
  G4int fN2ndariesPostStepDoIt;
      // These are the numbers of secondaries generated by the process
      // just executed.

  G4Navigator *fNavigator;

  G4int verboseLevel;

  typedef RWTValOrderedVector<G4int> 
             G4SelectedAtRestDoItVector;
  typedef RWTValOrderedVector<G4int> 
             G4SelectedAlongStepDoItVector;
  typedef RWTValOrderedVector<G4int>
             G4SelectedPostStepDoItVector;
  G4SelectedAtRestDoItVector *fSelectedAtRestDoItVector;
  G4SelectedAlongStepDoItVector *fSelectedAlongStepDoItVector;
  G4SelectedPostStepDoItVector *fSelectedPostStepDoItVector;

  G4double   fPreviousStepSize;

  G4VTouchable* fTouchable1;
  G4VTouchable* fTouchable2;
  G4bool fIsTouchable1Free;
  G4bool fIsTouchable2Free;

  G4SteppingControl StepControlFlag;

  G4double physIntLength;
  G4ForceCondition fCondition;
  G4GPILSelection  fGPILSelection;
      // Above three variables are for the method 
      // DefinePhysicalStepLength(). To pass these information to
      // the method Verbose, they are kept at here. Need a more 
      // elegant mechanism.


};
#endif
