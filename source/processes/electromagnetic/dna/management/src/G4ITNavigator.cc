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
// Id: From G4ITNavigator.cc,v 1.46 2010-11-15 14:03:27 gcosmo Exp
// $Id: G4ITNavigator.cc 60495 2012-07-12 14:49:48Z gcosmo $
// 
// class G4ITNavigator Implementation
//
// Original author: Paul Kent, July 95/96
//
// --------------------------------------------------------------------

#include <iomanip>

#include "G4ITNavigator.hh"
#include "G4ios.hh"
#include "G4SystemOfUnits.hh"
#include "G4GeometryTolerance.hh"
#include "G4VPhysicalVolume.hh"

#define G4DEBUG_NAVIGATION 1

// ********************************************************************
// Constructor
// ********************************************************************
//
G4ITNavigator::G4ITNavigator()
  : fWasLimitedByGeometry(false), fVerbose(0),
    fTopPhysical(0), fCheck(false), fPushed(false), fWarnPush(true)
{
  fActive= false; 
  fLastTriedStepComputation= false;
  ResetStackAndState();

  fActionThreshold_NoZeroSteps  = 10; 
  fAbandonThreshold_NoZeroSteps = 25; 

  kCarTolerance = G4GeometryTolerance::GetInstance()->GetSurfaceTolerance();
  fregularNav.SetNormalNavigation( &fnormalNav );

  fStepEndPoint = G4ThreeVector( kInfinity, kInfinity, kInfinity ); 
  fLastStepEndPointLocal = G4ThreeVector( kInfinity, kInfinity, kInfinity ); 

  fpSaveState = 0;

  // this->SetVerboseLevel(3);
  // this->CheckMode(true);
}

// !>

G4ITNavigator::G4SaveNavigatorState::G4SaveNavigatorState() : G4ITNavigatorState_Lock()
{
    sWasLimitedByGeometry  = false;
    sEntering              = false;
    sExiting               = false;
    sLocatedOnEdge         = false;
    sLastStepWasZero       = false;
    sEnteredDaughter       = false;
    sExitedMother          = false;
    sPushed                = false;

    sValidExitNormal       = false;
    sExitNormal            = G4ThreeVector(0,0,0);

    sPreviousSftOrigin     = G4ThreeVector(0,0,0);
    sPreviousSafety        = 0.0;

    sNumberZeroSteps       = 0;

    spBlockedPhysicalVolume = 0;
    sBlockedReplicaNo      = -1;

    sLastLocatedPointLocal = G4ThreeVector( kInfinity, -kInfinity, 0.0 );
    sLocatedOutsideWorld   = false;
}

// <!

// ********************************************************************
// Destructor
// ********************************************************************
//
G4ITNavigator::~G4ITNavigator()
{;}

// ********************************************************************
// ResetHierarchyAndLocate
// ********************************************************************
//
G4VPhysicalVolume*
G4ITNavigator::ResetHierarchyAndLocate(const G4ThreeVector &p,
                                     const G4ThreeVector &direction,
                                     const G4TouchableHistory &h)
{
  ResetState();
  fHistory = *h.GetHistory();
  SetupHierarchy();
  fLastTriedStepComputation= false;  // Redundant, but best
  return LocateGlobalPointAndSetup(p, &direction, true, false);
}

// ********************************************************************
// LocateGlobalPointAndSetup
//
// Locate the point in the hierarchy return 0 if outside
// The direction is required 
//    - if on an edge shared by more than two surfaces 
//      (to resolve likely looping in tracking)
//    - at initial location of a particle
//      (to resolve potential ambiguity at boundary)
// 
// Flags on exit: (comments to be completed)
// fEntering         - True if entering `daughter' volume (or replica)
//                     whether daughter of last mother directly 
//                     or daughter of that volume's ancestor.
// ********************************************************************
//
G4VPhysicalVolume* 
G4ITNavigator::LocateGlobalPointAndSetup( const G4ThreeVector& globalPoint,
                                        const G4ThreeVector* pGlobalDirection,
                                        const G4bool relativeSearch,
                                        const G4bool ignoreDirection )
{
  G4bool notKnownContained=true, noResult;
  G4VPhysicalVolume *targetPhysical;
  G4LogicalVolume *targetLogical;
  G4VSolid *targetSolid=0;
  G4ThreeVector localPoint, globalDirection;
  EInside insideCode;
  
  G4bool considerDirection = (!ignoreDirection) || fLocatedOnEdge;
  fLastTriedStepComputation= false;   

  if( considerDirection && pGlobalDirection != 0 )
  {
    globalDirection=*pGlobalDirection;
  }

#ifdef G4VERBOSE
  if( fVerbose > 2 )
  {
    G4int oldcoutPrec = G4cout.precision(8);
    G4cout << "*** G4ITNavigator::LocateGlobalPointAndSetup: ***" << G4endl;
    G4cout << "    Called with arguments: " << G4endl
           << "    Globalpoint = " << globalPoint << G4endl
           << "    RelativeSearch = " << relativeSearch  << G4endl;
    if( fVerbose == 4 )
    {
      G4cout << "    ----- Upon entering:" << G4endl;
      PrintState();
    }
    G4cout.precision(oldcoutPrec);
  }
#endif

  if ( !relativeSearch )
  {
    ResetStackAndState();
  }
  else
  {
    if ( fWasLimitedByGeometry )
    {
      fWasLimitedByGeometry = false;
      fEnteredDaughter = fEntering;   // Remember
      fExitedMother = fExiting;       // Remember
      if ( fExiting )
      {
        if ( fHistory.GetDepth() )
        {
          fBlockedPhysicalVolume = fHistory.GetTopVolume();
          fBlockedReplicaNo = fHistory.GetTopReplicaNo();
          fHistory.BackLevel();
        }
        else
        {
          fLastLocatedPointLocal = localPoint;
          fLocatedOutsideWorld = true;
          return 0;           // Have exited world volume
        }
        // A fix for the case where a volume is "entered" at an edge
        // and a coincident surface exists outside it.
        //  - This stops it from exiting further volumes and cycling
        //  - However ReplicaNavigator treats this case itself
        //
        if ( fLocatedOnEdge && (VolumeType(fBlockedPhysicalVolume)!=kReplica ))
        { 
          fExiting= false;
        }
      }
      else
        if ( fEntering )
        {
          switch (VolumeType(fBlockedPhysicalVolume))
          {
            case kNormal:
              fHistory.NewLevel(fBlockedPhysicalVolume, kNormal,
                                fBlockedPhysicalVolume->GetCopyNo());
              break;
            case kReplica:
              freplicaNav.ComputeTransformation(fBlockedReplicaNo,
                                                fBlockedPhysicalVolume);
              fHistory.NewLevel(fBlockedPhysicalVolume, kReplica,
                                fBlockedReplicaNo);
              fBlockedPhysicalVolume->SetCopyNo(fBlockedReplicaNo);
              break;
            case kParameterised:
              if( fBlockedPhysicalVolume->GetRegularStructureId() == 0 )
              {
                G4VSolid *pSolid;
                G4VPVParameterisation *pParam;
                G4TouchableHistory parentTouchable( fHistory );
                pParam = fBlockedPhysicalVolume->GetParameterisation();
                pSolid = pParam->ComputeSolid(fBlockedReplicaNo,
                                              fBlockedPhysicalVolume);
                pSolid->ComputeDimensions(pParam, fBlockedReplicaNo,
                                          fBlockedPhysicalVolume);
                pParam->ComputeTransformation(fBlockedReplicaNo,
                                              fBlockedPhysicalVolume);
                fHistory.NewLevel(fBlockedPhysicalVolume, kParameterised,
                                  fBlockedReplicaNo);
                fBlockedPhysicalVolume->SetCopyNo(fBlockedReplicaNo);
                //
                // Set the correct solid and material in Logical Volume
                //
                G4LogicalVolume *pLogical;
                pLogical = fBlockedPhysicalVolume->GetLogicalVolume();
                pLogical->SetSolid( pSolid );
                pLogical->UpdateMaterial(pParam ->
                  ComputeMaterial(fBlockedReplicaNo,
                                  fBlockedPhysicalVolume, 
                                  &parentTouchable));
              }
              break;
          }
          fEntering = false;
          fBlockedPhysicalVolume = 0;
          localPoint = fHistory.GetTopTransform().TransformPoint(globalPoint);
          notKnownContained = false;
        }
    }
    else
    {
      fBlockedPhysicalVolume = 0;
      fEntering = false;
      fEnteredDaughter = false;  // Full Step was not taken, did not enter
      fExiting = false;
      fExitedMother = false;     // Full Step was not taken, did not exit
    }
  }
  //
  // Search from top of history up through geometry until
  // containing volume found:
  // If on 
  // o OUTSIDE - Back up level, not/no longer exiting volumes
  // o SURFACE and EXITING - Back up level, setting new blocking no.s
  // else
  // o containing volume found
  //
  while (notKnownContained)
  {
    if ( fHistory.GetTopVolumeType()!=kReplica )
    {
      targetSolid = fHistory.GetTopVolume()->GetLogicalVolume()->GetSolid();
      localPoint = fHistory.GetTopTransform().TransformPoint(globalPoint);
      insideCode = targetSolid->Inside(localPoint);
#ifdef G4VERBOSE
      if(( fVerbose == 1 ) && ( fCheck ))
      {
         G4String solidResponse = "-kInside-";
         if (insideCode == kOutside)
           solidResponse = "-kOutside-";
         else if (insideCode == kSurface)
           solidResponse = "-kSurface-";
         G4cout << "*** G4ITNavigator::LocateGlobalPointAndSetup(): ***" << G4endl
                << "    Invoked Inside() for solid: " << targetSolid->GetName()
                << ". Solid replied: " << solidResponse << G4endl
                << "    For local point p: " << localPoint << G4endl;
      }
#endif
    }
    else
    {
      insideCode = freplicaNav.BackLocate(fHistory, globalPoint, localPoint,
                                          fExiting, notKnownContained);
      // !CARE! if notKnownContained returns false then the point is within
      // the containing placement volume of the replica(s). If insidecode
      // will result in the history being backed up one level, then the
      // local point returned is the point in the system of this new level
    }
    if ( insideCode==kOutside )
    {
      if ( fHistory.GetDepth() )
      {
        fBlockedPhysicalVolume = fHistory.GetTopVolume();
        fBlockedReplicaNo = fHistory.GetTopReplicaNo();
        fHistory.BackLevel();
        fExiting = false;
      }
      else
      {
        fLastLocatedPointLocal = localPoint;
        fLocatedOutsideWorld = true;
        return 0;         // Have exited world volume
      }
    }
    else
      if ( insideCode==kSurface )
      {
        G4bool isExiting = fExiting;
        if( (!fExiting)&&considerDirection )
        {
          // Figure out whether we are exiting this level's volume
          // by using the direction
          //
          G4bool directionExiting = false;
          G4ThreeVector localDirection =
              fHistory.GetTopTransform().TransformAxis(globalDirection);
          if ( fHistory.GetTopVolumeType()!=kReplica )
          {
            G4ThreeVector normal = targetSolid->SurfaceNormal(localPoint);
            directionExiting = normal.dot(localDirection) > 0.0;
            isExiting = isExiting || directionExiting;
          }
        }
        if( isExiting )
        {
          if ( fHistory.GetDepth() )
          {
            fBlockedPhysicalVolume = fHistory.GetTopVolume();
            fBlockedReplicaNo = fHistory.GetTopReplicaNo();
            fHistory.BackLevel();
            //
            // Still on surface but exited volume not necessarily convex
            //
            fValidExitNormal = false;
          } 
          else
          {
            fLastLocatedPointLocal = localPoint;
            fLocatedOutsideWorld = true;
            return 0;          // Have exited world volume
          }
        }
        else
        {
          notKnownContained=false;
        }
      }
      else
      {
        notKnownContained=false;
      }
  }  // END while (notKnownContained)
  //
  // Search downwards until deepest containing volume found,
  // blocking fBlockedPhysicalVolume/BlockedReplicaNum
  //
  // 3 Cases:
  //
  // o Parameterised daughters
  //   =>Must be one G4PVParameterised daughter & voxels
  // o Positioned daughters & voxels
  // o Positioned daughters & no voxels

  noResult = true;  // noResult should be renamed to 
                    // something like enteredLevel, as that is its meaning.
  do
  {
    // Determine `type' of current mother volume
    //
    targetPhysical = fHistory.GetTopVolume();
    if (!targetPhysical) { break; }
    targetLogical = targetPhysical->GetLogicalVolume();
    switch( CharacteriseDaughters(targetLogical) )
    {
      case kNormal:
        if ( targetLogical->GetVoxelHeader() )  // use optimised navigation
        {
          noResult = fvoxelNav.LevelLocate(fHistory,
                                           fBlockedPhysicalVolume,
                                           fBlockedReplicaNo,
                                           globalPoint,
                                           pGlobalDirection,
                                           considerDirection,
                                           localPoint);
        }
        else                       // do not use optimised navigation
        {
          noResult = fnormalNav.LevelLocate(fHistory,
                                            fBlockedPhysicalVolume,
                                            fBlockedReplicaNo,
                                            globalPoint,
                                            pGlobalDirection,
                                            considerDirection,
                                            localPoint);
        }
        break;
      case kReplica:
        noResult = freplicaNav.LevelLocate(fHistory,
                                           fBlockedPhysicalVolume,
                                           fBlockedReplicaNo,
                                           globalPoint,
                                           pGlobalDirection,
                                           considerDirection,
                                           localPoint);
        break;
      case kParameterised:
        if( GetDaughtersRegularStructureId(targetLogical) != 1 )
        {
          noResult = fparamNav.LevelLocate(fHistory,
                                           fBlockedPhysicalVolume,
                                           fBlockedReplicaNo,
                                           globalPoint,
                                           pGlobalDirection,
                                           considerDirection,
                                           localPoint);
        }
        else  // Regular structure
        {
          noResult = fregularNav.LevelLocate(fHistory,
                                             fBlockedPhysicalVolume,
                                             fBlockedReplicaNo,
                                             globalPoint,
                                             pGlobalDirection,
                                             considerDirection,
                                             localPoint);
        }
        break;
    }

    // LevelLocate returns true if it finds a daughter volume 
    // in which globalPoint is inside (or on the surface).

    if ( noResult )
    {
      // Entering a daughter after ascending
      //
      // The blocked volume is no longer valid - it was for another level
      //
      fBlockedPhysicalVolume = 0;
      fBlockedReplicaNo = -1;

      // fEntering should be false -- else blockedVolume is assumed good.
      // fEnteredDaughter is used for ExitNormal
      //
      fEntering = false;
      fEnteredDaughter = true;
#ifdef G4DEBUG_NAVIGATION
      if( fVerbose > 2 )
      { 
         G4VPhysicalVolume* enteredPhysical = fHistory.GetTopVolume();
         G4cout << "*** G4ITNavigator::LocateGlobalPointAndSetup() ***" << G4endl;
         G4cout << "    Entering volume: " << enteredPhysical->GetName()
                << G4endl;
      }
#endif
    }
  } while (noResult);

  fLastLocatedPointLocal = localPoint;

#ifdef G4VERBOSE
  if( fVerbose == 4 )
  {
    G4int oldcoutPrec = G4cout.precision(8);
    G4String curPhysVol_Name("None");
    if (targetPhysical)  { curPhysVol_Name = targetPhysical->GetName(); }
    G4cout << "    Return value = new volume = " << curPhysVol_Name << G4endl;
    G4cout << "    ----- Upon exiting:" << G4endl;
    PrintState();
#ifdef G4DEBUG_NAVIGATION
    G4cout << "Upon exiting LocateGlobalPointAndSetup():" << G4endl;
    G4cout << "    History = " << G4endl << fHistory << G4endl << G4endl;
#endif
    G4cout.precision(oldcoutPrec);
  }
#endif

  fLocatedOutsideWorld= false;

  return targetPhysical;
}

// ********************************************************************
// LocateGlobalPointWithinVolume
//
// -> the state information of this Navigator and its subNavigators
//    is updated in order to start the next step at pGlobalpoint
// -> no check is performed whether pGlobalpoint is inside the 
//    original volume (this must be the case).
//
// Note: a direction could be added to the arguments, to aid in future
//       optional checking (via the old code below, flagged by OLD_LOCATE). 
//       [ This would be done only in verbose mode ]
// ********************************************************************
//
void
G4ITNavigator::LocateGlobalPointWithinVolume(const G4ThreeVector& pGlobalpoint)
{  
   fLastLocatedPointLocal = ComputeLocalPoint(pGlobalpoint);
   fLastTriedStepComputation= false;

#ifdef G4DEBUG_NAVIGATION
   if( fVerbose > 2 )
   { 
     G4cout << "Entering LocateGlobalWithinVolume(): History = " << G4endl;
     G4cout << fHistory << G4endl;
   }
#endif

   // For the case of Voxel (or Parameterised) volume the respective 
   // Navigator must be messaged to update its voxel information etc

   // Update the state of the Sub Navigators 
   // - in particular any voxel information they store/cache
   //
   G4VPhysicalVolume*  motherPhysical = fHistory.GetTopVolume();
   G4LogicalVolume*    motherLogical  = motherPhysical->GetLogicalVolume();
   G4SmartVoxelHeader* pVoxelHeader   = motherLogical->GetVoxelHeader();

   if ( fHistory.GetTopVolumeType()!=kReplica )
   {
     switch( CharacteriseDaughters(motherLogical) )
     {
       case kNormal:
         if ( pVoxelHeader )
         {
           fvoxelNav.VoxelLocate( pVoxelHeader, fLastLocatedPointLocal );
         }
         break;
       case kParameterised:
         if( GetDaughtersRegularStructureId(motherLogical) != 1 )
         {
           // Resets state & returns voxel node
           //
           fparamNav.ParamVoxelLocate( pVoxelHeader, fLastLocatedPointLocal );
         }
         break;
       case kReplica:
         G4Exception("G4ITNavigator::LocateGlobalPointWithinVolume()",
                     "GeomNav0001", FatalException,
                     "Not applicable for replicated volumes.");
         break;
     }
   }

   // Reset the state variables 
   //   - which would have been affected
   //     by the 'equivalent' call to LocateGlobalPointAndSetup
   //   - who's values have been invalidated by the 'move'.
   //
   fBlockedPhysicalVolume = 0; 
   fBlockedReplicaNo = -1;
   fEntering = false;
   fEnteredDaughter = false;  // Boundary not encountered, did not enter
   fExiting = false;
   fExitedMother = false;     // Boundary not encountered, did not exit
}

// !>
G4ITNavigatorState_Lock* G4ITNavigator::GetNavigatorState()
{
    SetSavedState();
    return fpSaveState;
}

void G4ITNavigator::SetNavigatorState(G4ITNavigatorState_Lock* navState)
{
    fpSaveState = (G4SaveNavigatorState*) navState;
    if(navState) RestoreSavedState();
}

void G4ITNavigator::NewNavigatorState()
{
    fpSaveState = new G4SaveNavigatorState();
    ResetState();
}


// ********************************************************************
// SetSavedState
//
// Save the state, in case this is a parasitic call
// Save fValidExitNormal, fExitNormal, fExiting, fEntering,
//      fBlockedPhysicalVolume, fBlockedReplicaNo, fLastStepWasZero;
// ********************************************************************
//
void G4ITNavigator::SetSavedState()
{
    // !>
    // This check can be avoid if instead, at every first step of a track,
    // the IT tracking uses NewNavigatorSate
    // The normal tracking would just call once NewNavigatorState() before tracking

//    if(fpSaveState == 0)
//        fpSaveState = new G4SaveNavigatorState;
    // <!

  // fSaveExitNormal = fExitNormal;
  fpSaveState->sExitNormal = fExitNormal;
  fpSaveState->sValidExitNormal = fValidExitNormal;
  fpSaveState->sExiting = fExiting;
  fpSaveState->sEntering = fEntering;

  fpSaveState->spBlockedPhysicalVolume = fBlockedPhysicalVolume;
  fpSaveState->sBlockedReplicaNo = fBlockedReplicaNo,

  fpSaveState->sLastStepWasZero = fLastStepWasZero;

  // !>
  fpSaveState->sPreviousSftOrigin = fPreviousSftOrigin;
  fpSaveState->sPreviousSafety = fPreviousSafety;
  fpSaveState->sNumberZeroSteps = fNumberZeroSteps;
  fpSaveState->sLocatedOnEdge = fLocatedOnEdge;
  fpSaveState->sWasLimitedByGeometry= fWasLimitedByGeometry;
  fpSaveState->sPushed=fPushed;
  fpSaveState->sNumberZeroSteps=fNumberZeroSteps;
  fpSaveState->sEnteredDaughter = fEnteredDaughter;
  fpSaveState->sExitedMother = fExitedMother;

  fpSaveState->sLastLocatedPointLocal = fLastLocatedPointLocal;
  fpSaveState->sLocatedOutsideWorld = fLocatedOutsideWorld;
  // <!
}

// ********************************************************************
// RestoreSavedState
//
// Restore the state (in Compute Step), in case this is a parasitic call
// ********************************************************************
//
void G4ITNavigator::RestoreSavedState()
{
  fExitNormal = fpSaveState->sExitNormal;
  fValidExitNormal = fpSaveState->sValidExitNormal;
  fExiting = fpSaveState->sExiting;
  fEntering = fpSaveState->sEntering;

  fBlockedPhysicalVolume = fpSaveState->spBlockedPhysicalVolume;
  fBlockedReplicaNo = fpSaveState->sBlockedReplicaNo,

  fLastStepWasZero = fpSaveState->sLastStepWasZero;

  // !>
  fPreviousSftOrigin = fpSaveState->sPreviousSftOrigin ;
  fPreviousSafety = fpSaveState->sPreviousSafety ;
  fNumberZeroSteps = fpSaveState->sNumberZeroSteps ;
  fLocatedOnEdge = fpSaveState->sLocatedOnEdge ;
  fWasLimitedByGeometry = fpSaveState->sWasLimitedByGeometry;
  fPushed = fpSaveState->sPushed;
  fNumberZeroSteps = fpSaveState->sNumberZeroSteps;
  fEnteredDaughter= fpSaveState->sEnteredDaughter ;
  fExitedMother = fpSaveState->sExitedMother ;

  fLastLocatedPointLocal = fpSaveState->sLastLocatedPointLocal ;
  fLocatedOutsideWorld = fpSaveState->sLocatedOutsideWorld;
  // <!
}
// <!

// ********************************************************************
// ComputeStep
//
// Computes the next geometric Step: intersections with current
// mother and `daughter' volumes.
//
// NOTE:
//
// Flags on entry:
// --------------
// fValidExitNormal  - Normal of exited volume is valid (convex, not a 
//                     coincident boundary)
// fExitNormal       - Surface normal of exited volume
// fExiting          - True if have exited solid
//
// fBlockedPhysicalVolume - Ptr to exited volume (or 0)
// fBlockedReplicaNo - Replication no of exited volume
// fLastStepWasZero  - True if last Step size was zero.
//
// Flags on exit:
// -------------
// fValidExitNormal  - True if surface normal of exited volume is valid
// fExitNormal       - Surface normal of exited volume rotated to mothers
//                    reference system
// fExiting          - True if exiting mother
// fEntering         - True if entering `daughter' volume (or replica)
// fBlockedPhysicalVolume - Ptr to candidate (entered) volume
// fBlockedReplicaNo - Replication no of candidate (entered) volume
// fLastStepWasZero  - True if this Step size was zero.
// ********************************************************************
//
G4double G4ITNavigator::ComputeStep( const G4ThreeVector &pGlobalpoint,
                                   const G4ThreeVector &pDirection,
                                   const G4double pCurrentProposedStepLength,
                                         G4double &pNewSafety)
{
  G4ThreeVector localDirection = ComputeLocalAxis(pDirection);
  G4double Step = kInfinity;
  G4VPhysicalVolume  *motherPhysical = fHistory.GetTopVolume();
  G4LogicalVolume *motherLogical = motherPhysical->GetLogicalVolume();

  static G4int sNavCScalls=0;
  sNavCScalls++;

  fLastTriedStepComputation= true; 

#ifdef G4VERBOSE
  if( fVerbose > 0 )
  {
    G4cout << "*** G4ITNavigator::ComputeStep: ***" << G4endl;
    G4cout << "    Volume = " << motherPhysical->GetName() 
           << " - Proposed step length = " << pCurrentProposedStepLength
           << G4endl; 
#ifdef G4DEBUG_NAVIGATION
    if( fVerbose >= 4 ) 
    {
      G4cout << "  Called with the arguments: " << G4endl
             << "  Globalpoint = " << std::setw(25) << pGlobalpoint << G4endl
             << "  Direction   = " << std::setw(25) << pDirection << G4endl;
      G4cout << "  ---- Upon entering :" << G4endl;
      PrintState();
    }
#endif
  }
#endif

  G4ThreeVector newLocalPoint = ComputeLocalPoint(pGlobalpoint);
  if( newLocalPoint != fLastLocatedPointLocal )
  {
    // Check whether the relocation is within safety
    //
    G4ThreeVector oldLocalPoint = fLastLocatedPointLocal;
    G4double moveLenSq = (newLocalPoint-oldLocalPoint).mag2();

    if ( moveLenSq >= kCarTolerance*kCarTolerance )
    {
#ifdef G4VERBOSE
      ComputeStepLog(pGlobalpoint, moveLenSq);
#endif
      // Relocate the point within the same volume
      //
      LocateGlobalPointWithinVolume( pGlobalpoint );
      fLastTriedStepComputation= true;     // Ensure that this is set again !!
    }
  }
  if ( fHistory.GetTopVolumeType()!=kReplica )
  {
    switch( CharacteriseDaughters(motherLogical) )
    {
      case kNormal:
        if ( motherLogical->GetVoxelHeader() )
        {
          Step = fvoxelNav.ComputeStep(fLastLocatedPointLocal,
                                       localDirection,
                                       pCurrentProposedStepLength,
                                       pNewSafety,
                                       fHistory,
                                       fValidExitNormal,
                                       fExitNormal,
                                       fExiting,
                                       fEntering,
                                       &fBlockedPhysicalVolume,
                                       fBlockedReplicaNo);
      
        }
        else
        {
          if( motherPhysical->GetRegularStructureId() == 0 )
          {
            Step = fnormalNav.ComputeStep(fLastLocatedPointLocal,
                                          localDirection,
                                          pCurrentProposedStepLength,
                                          pNewSafety,
                                          fHistory,
                                          fValidExitNormal,
                                          fExitNormal,
                                          fExiting,
                                          fEntering,
                                          &fBlockedPhysicalVolume,
                                          fBlockedReplicaNo);
          }
          else  // Regular (non-voxelised) structure
          {
            LocateGlobalPointAndSetup( pGlobalpoint, &pDirection, true, true );
            fLastTriedStepComputation= true;     // Ensure that this is set again !!
            //
            // if physical process limits the step, the voxel will not be the
            // one given by ComputeStepSkippingEqualMaterials() and the local
            // point will be wrongly calculated.

            // There is a problem: when msc limits the step and the point is
            // assigned wrongly to phantom in previous step (while it is out
            // of the container volume). Then LocateGlobalPointAndSetup() has
            // reset the history topvolume to world.
            //
            if(fHistory.GetTopVolume()->GetRegularStructureId() == 0 )
            { 
              G4Exception("G4ITNavigator::ComputeStep()",
                          "GeomNav1001", JustWarning,
                "Point is relocated in voxels, while it should be outside!");
              Step = fnormalNav.ComputeStep(fLastLocatedPointLocal,
                                            localDirection,
                                            pCurrentProposedStepLength,
                                            pNewSafety,
                                            fHistory,
                                            fValidExitNormal,
                                            fExitNormal,
                                            fExiting,
                                            fEntering,
                                            &fBlockedPhysicalVolume,
                                            fBlockedReplicaNo);
            }
            else
            {
              Step = fregularNav.
                   ComputeStepSkippingEqualMaterials(fLastLocatedPointLocal,
                                                     localDirection,
                                                     pCurrentProposedStepLength,
                                                     pNewSafety,
                                                     fHistory,
                                                     fValidExitNormal,
                                                     fExitNormal,
                                                     fExiting,
                                                     fEntering,
                                                     &fBlockedPhysicalVolume,
                                                     fBlockedReplicaNo,
                                                     motherPhysical);
            }
          }
        }
        break;
      case kParameterised:
        if( GetDaughtersRegularStructureId(motherLogical) != 1 )
        {
          Step = fparamNav.ComputeStep(fLastLocatedPointLocal,
                                       localDirection,
                                       pCurrentProposedStepLength,
                                       pNewSafety,
                                       fHistory,
                                       fValidExitNormal,
                                       fExitNormal,
                                       fExiting,
                                       fEntering,
                                       &fBlockedPhysicalVolume,
                                       fBlockedReplicaNo);
        }
        else  // Regular structure
        {
          Step = fregularNav.ComputeStep(fLastLocatedPointLocal,
                                         localDirection,
                                         pCurrentProposedStepLength,
                                         pNewSafety,
                                         fHistory,
                                         fValidExitNormal,
                                         fExitNormal,
                                         fExiting,
                                         fEntering,
                                         &fBlockedPhysicalVolume,
                                         fBlockedReplicaNo);
        }
        break;
      case kReplica:
        G4Exception("G4ITNavigator::ComputeStep()", "GeomNav0001",
                    FatalException, "Not applicable for replicated volumes.");
        break;
    }
  }
  else
  {
    // In the case of a replica, it must handle the exiting
    // edge/corner problem by itself
    //
    G4bool exitingReplica = fExitedMother;
    Step = freplicaNav.ComputeStep(pGlobalpoint,
                                   pDirection,
                                   fLastLocatedPointLocal,
                                   localDirection,
                                   pCurrentProposedStepLength,
                                   pNewSafety,
                                   fHistory,
                                   fValidExitNormal,
                                   fExitNormal,
                                   exitingReplica,
                                   fEntering,
                                   &fBlockedPhysicalVolume,
                                   fBlockedReplicaNo);
    fExiting= exitingReplica;                          // still ok to set it ??
  }

  // Remember last safety origin & value.
  //
  fPreviousSftOrigin = pGlobalpoint;
  fPreviousSafety = pNewSafety; 

  // Count zero steps - one can occur due to changing momentum at a boundary
  //                  - one, two (or a few) can occur at common edges between
  //                    volumes
  //                  - more than two is likely a problem in the geometry
  //                    description or the Navigation 

  // Rule of thumb: likely at an Edge if two consecutive steps are zero,
  //                because at least two candidate volumes must have been
  //                checked
  //
  fLocatedOnEdge   = fLastStepWasZero && (Step==0.0);
  fLastStepWasZero = (Step==0.0);
  if (fPushed)  fPushed = fLastStepWasZero;

  // Handle large number of consecutive zero steps
  //
  if ( fLastStepWasZero )
  {
    fNumberZeroSteps++;
#ifdef G4DEBUG_NAVIGATION
    if( fNumberZeroSteps > 1 )
    {
       G4cout << "G4ITNavigator::ComputeStep(): another zero step, # "
              << fNumberZeroSteps
              << " at " << pGlobalpoint
              << " in volume " << motherPhysical->GetName()
              << " nav-comp-step calls # " << sNavCScalls
              << G4endl;
    }
#endif
    if( fNumberZeroSteps > fActionThreshold_NoZeroSteps-1 )
    {
       // Act to recover this stuck track. Pushing it along direction
       //
       Step += 100*kCarTolerance;
#ifdef G4VERBOSE
       if ((!fPushed) && (fWarnPush))
       {
         std::ostringstream message;
         message << "Track stuck or not moving." << G4endl
                 << "          Track stuck, not moving for " 
                 << fNumberZeroSteps << " steps" << G4endl
                 << "          in volume -" << motherPhysical->GetName()
                 << "- at point " << pGlobalpoint << G4endl
                 << "          direction: " << pDirection << "." << G4endl
                 << "          Potential geometry or navigation problem !"
                 << G4endl
                 << "          Trying pushing it of " << Step << " mm ...";
         G4Exception("G4ITNavigator::ComputeStep()", "GeomNav1002",
                     JustWarning, message, "Potential overlap in geometry!");
       }
#endif
       fPushed = true;
    }
    if( fNumberZeroSteps > fAbandonThreshold_NoZeroSteps-1 )
    {
      // Must kill this stuck track
      //
      std::ostringstream message;
      message << "Stuck Track: potential geometry or navigation problem."
              << G4endl
              << "        Track stuck, not moving for " 
              << fNumberZeroSteps << " steps" << G4endl
              << "        in volume -" << motherPhysical->GetName()
              << "- at point " << pGlobalpoint << G4endl
              << "        direction: " << pDirection << ".";
      motherPhysical->CheckOverlaps(5000, false);
      G4Exception("G4ITNavigator::ComputeStep()", "GeomNav0003",
                  EventMustBeAborted, message);
    }
  }
  else
  {
    if (!fPushed)  fNumberZeroSteps = 0;
  }

  fEnteredDaughter = fEntering;   // I expect to enter a volume in this Step
  fExitedMother = fExiting;

  fStepEndPoint = pGlobalpoint + Step * pDirection; 
  fLastStepEndPointLocal = fLastLocatedPointLocal + Step * localDirection; 

  if( fExiting )
  {
#ifdef G4DEBUG_NAVIGATION
    if( fVerbose > 2 )
    { 
      G4cout << " At G4Nav CompStep End - if(exiting) - fExiting= " << fExiting 
             << " fValidExitNormal = " << fValidExitNormal  << G4endl;
      G4cout << " fExitNormal= " << fExitNormal << G4endl;
    }
#endif

    if(fValidExitNormal)
    {
      // Convention: fExitNormal is in the 'grand-mother' coordinate system
      //
      fGrandMotherExitNormal= fExitNormal;
    }
    else
    {  
      // We must calculate the normal anyway (in order to have it if requested)
      //
      G4ThreeVector finalLocalPoint =
        fLastLocatedPointLocal + localDirection*Step;

      // Now fGrandMotherExitNormal is in the 'grand-mother' coordinate system
      //
      fGrandMotherExitNormal =
        motherLogical->GetSolid()->SurfaceNormal(finalLocalPoint);

      const G4RotationMatrix* mRot = motherPhysical->GetRotation();
      if( mRot )
      { 
        fGrandMotherExitNormal *= (*mRot).inverse();
      }
      //  Do not set fValidExitNormal -- this signifies that the solid is convex!
    }
  }
  fStepEndPoint= pGlobalpoint+Step*pDirection; 

  if( (Step == pCurrentProposedStepLength) && (!fExiting) && (!fEntering) )
  {
    // This if Step is not really limited by the geometry.
    // The Navigator is obliged to return "infinity"
    //
    Step = kInfinity;
  }

#ifdef G4VERBOSE
  if( fVerbose > 1 )
  {
    if( fVerbose >= 4 )
    {
      G4cout << "    ----- Upon exiting :" << G4endl;
      PrintState();
    }
    G4cout <<"    Returned step = " << Step << G4endl;
    if( Step == kInfinity )
    {
      G4cout << "    Original proposed step = "
             << pCurrentProposedStepLength << G4endl;
    }
    G4cout << "    Safety = " << pNewSafety << G4endl;
  }
#endif

  return Step;
}

// ********************************************************************
// CheckNextStep
//
// Compute the step without altering the navigator state
// ********************************************************************
//
G4double G4ITNavigator::CheckNextStep( const G4ThreeVector& pGlobalpoint,
                                     const G4ThreeVector& pDirection,
                                     const G4double pCurrentProposedStepLength,
                                           G4double& pNewSafety)
{
  G4double step;

  // Save the state, for this parasitic call
  //
  SetSavedState();

  step = ComputeStep ( pGlobalpoint, 
                       pDirection,
                       pCurrentProposedStepLength, 
                       pNewSafety ); 

  // If a parasitic call, then attempt to restore the key parts of the state
  //
  RestoreSavedState(); 

  return step; 
}

// ********************************************************************
// ResetState
//
// Resets stack and minimum of navigator state `machine'
// ********************************************************************
//
void G4ITNavigator::ResetState()
{
  fWasLimitedByGeometry  = false;
  fEntering              = false;
  fExiting               = false;
  fLocatedOnEdge         = false;
  fLastStepWasZero       = false;
  fEnteredDaughter       = false;
  fExitedMother          = false;
  fPushed                = false;

  fValidExitNormal       = false;
  fExitNormal            = G4ThreeVector(0,0,0);

  fPreviousSftOrigin     = G4ThreeVector(0,0,0);
  fPreviousSafety        = 0.0; 

  fNumberZeroSteps       = 0;
    
  fBlockedPhysicalVolume = 0;
  fBlockedReplicaNo      = -1;

  fLastLocatedPointLocal = G4ThreeVector( kInfinity, -kInfinity, 0.0 ); 
  fLocatedOutsideWorld   = false;
}

// ********************************************************************
// SetupHierarchy
//
// Renavigates & resets hierarchy described by current history
// o Reset volumes
// o Recompute transforms and/or solids of replicated/parameterised volumes
// ********************************************************************
//
void G4ITNavigator::SetupHierarchy()
{
  G4int i;
  const G4int cdepth = fHistory.GetDepth();
  G4VPhysicalVolume *current;
  G4VSolid *pSolid;
  G4VPVParameterisation *pParam;

  for ( i=1; i<=cdepth; i++ )
  {
    current = fHistory.GetVolume(i);
    switch ( fHistory.GetVolumeType(i) )
    {
      case kNormal:
        break;
      case kReplica:
        freplicaNav.ComputeTransformation(fHistory.GetReplicaNo(i), current);
        break;
      case kParameterised:
        G4int replicaNo;
        pParam = current->GetParameterisation();
        replicaNo = fHistory.GetReplicaNo(i);
        pSolid = pParam->ComputeSolid(replicaNo, current);

        // Set up dimensions & transform in solid/physical volume
        //
        pSolid->ComputeDimensions(pParam, replicaNo, current);
        pParam->ComputeTransformation(replicaNo, current);

        G4TouchableHistory touchable( fHistory );
        touchable.MoveUpHistory();  // move up to the parent level
      
        // Set up the correct solid and material in Logical Volume
        //
        G4LogicalVolume *pLogical = current->GetLogicalVolume();
        pLogical->SetSolid( pSolid );
        pLogical->UpdateMaterial( pParam ->
          ComputeMaterial(replicaNo, current, &touchable) );
        break;
    }
  }
}

// ********************************************************************
// GetLocalExitNormal
//
// Obtains the Normal vector to a surface (in local coordinates)
// pointing out of previous volume and into current volume
// ********************************************************************
//
G4ThreeVector G4ITNavigator::GetLocalExitNormal( G4bool* valid )
{
  G4ThreeVector    ExitNormal(0.,0.,0.);
  G4VSolid        *currentSolid=0;
  G4LogicalVolume *candidateLogical;
  if ( fLastTriedStepComputation ) 
  {
    // use fLastLocatedPointLocal
    // and next candidate volume
    G4ThreeVector nextSolidExitNormal(0.,0.,0.);

    if( fEntering && (fBlockedPhysicalVolume!=0) ) 
    { 
      candidateLogical= fBlockedPhysicalVolume->GetLogicalVolume();
      if( candidateLogical ) 
      {
        // fLastStepEndPointLocal is in the coordinates of the mother
        // we need it in the daughter's coordinate system.

        if( CharacteriseDaughters(candidateLogical) != kReplica )
        {
          // First transform fLastLocatedPointLocal to the new daughter
          // coordinates
          G4AffineTransform MotherToDaughterTransform=
            GetMotherToDaughterTransform( fBlockedPhysicalVolume, 
                                          fBlockedReplicaNo,
                                          VolumeType(fBlockedPhysicalVolume) ); 
          G4ThreeVector daughterPointOwnLocal= 
            MotherToDaughterTransform.TransformPoint( fLastStepEndPointLocal ); 

          // OK if it is a parameterised volume
          //
          EInside  inSideIt; 
          G4bool   onSurface;
          G4double safety= -1.0; 
          currentSolid= candidateLogical->GetSolid(); 
          inSideIt  =  currentSolid->Inside(daughterPointOwnLocal); 
          onSurface =  (inSideIt == kSurface); 
          if( ! onSurface ) 
          {
            if( inSideIt == kOutside )
            { 
              safety = (currentSolid->DistanceToIn(daughterPointOwnLocal)); 
              onSurface = safety < 100.0 * kCarTolerance; 
            }
            else if (inSideIt == kInside ) 
            {
              safety = (currentSolid->DistanceToOut(daughterPointOwnLocal)); 
              onSurface = safety < 100.0 * kCarTolerance; 
            }
          }

          if( onSurface ) 
          {
            nextSolidExitNormal =
              currentSolid->SurfaceNormal(daughterPointOwnLocal); 
 
            // Entering the solid ==> opposite
            //
            ExitNormal = -nextSolidExitNormal;
          }
          else
          {
#ifdef G4VERBOSE
            if(( fVerbose == 1 ) && ( fCheck ))
            {
              std::ostringstream message;
              message << "Point not on surface ! " << G4endl
                      << "  Point           = "
                      << daughterPointOwnLocal << G4endl 
                      << "  Physical volume = "
                      << fBlockedPhysicalVolume->GetName() << G4endl
                      << "  Logical volume  = "
                      << candidateLogical->GetName() << G4endl
                      << "  Solid           = " << currentSolid->GetName() 
                      << "  Type            = "
                      << currentSolid->GetEntityType() << G4endl
                      << *currentSolid << G4endl;
              if( inSideIt == kOutside )
              { 
                message << "Point is Outside. " << G4endl
                        << "  Safety (from outside) = " << safety << G4endl;
              }
              else // if( inSideIt == kInside ) 
              {
                message << "Point is Inside. " << G4endl
                        << "  Safety (from inside) = " << safety << G4endl;              
              }
              G4Exception("G4ITNavigator::GetLocalExitNormal()", "GeomNav1001",
                          JustWarning, message);
            }
#endif
          }
          *valid = onSurface;   //   was =true;
        }
        else
        {
          *valid = false;  // TODO: Need Separate code for replica!!!!
#ifdef G4DEBUG_NAVIGATION
          G4Exception("G4ITNavigator::GetLocalExitNormal()", "GeomNav0001",
                      FatalException, 
                      "Local normal not (yet) available for replica volumes.");
#endif 
        }
      }
    }
    else if ( fExiting ) 
    {
        ExitNormal = fGrandMotherExitNormal;
        *valid = true;
    }
    else  // ie  ( fBlockedPhysicalVolume == 0 )
    {
      *valid = false;
    }
  }
  else 
  {
    if ( EnteredDaughterVolume() )
    {
      ExitNormal= -(fHistory.GetTopVolume()->GetLogicalVolume()->
                    GetSolid()->SurfaceNormal(fLastLocatedPointLocal));
      *valid = true;
    }
    else
    {
      if( fExitedMother )
      {
        ExitNormal = fGrandMotherExitNormal;
        *valid = true;
      }
      else  // We are not at a boundary. ExitNormal remains (0,0,0)
      {
        *valid = false;
      }
    }
  }
  return ExitNormal;
}

// ********************************************************************
// GetMotherToDaughterTransform
//
// Obtains the mother to daughter affine transformation
// ********************************************************************
//
G4AffineTransform
G4ITNavigator::GetMotherToDaughterTransform( G4VPhysicalVolume *pEnteringPhysVol,   // not Const
                                           G4int   enteringReplicaNo,
                                           EVolume enteringVolumeType ) 
{
  switch (enteringVolumeType)
  {
    case kNormal:  // Nothing is needed to prepare the transformation
      break;       // It is stored already in the physical volume (placement)
    case kReplica: // Sets the transform in the Replica - tbc
      G4Exception("G4ITNavigator::GetMotherToDaughterTransform()",
                  "GeomNav0001", FatalException,
                  "Method NOT Implemented yet for replica volumes.");
      break;
    case kParameterised:
      if( pEnteringPhysVol->GetRegularStructureId() == 0 )
      {
        G4VPVParameterisation *pParam =
          pEnteringPhysVol->GetParameterisation();
        G4VSolid* pSolid =
          pParam->ComputeSolid(enteringReplicaNo, pEnteringPhysVol);
        pSolid->ComputeDimensions(pParam, enteringReplicaNo, pEnteringPhysVol);

        // Sets the transform in the Parameterisation
        //
        pParam->ComputeTransformation(enteringReplicaNo, pEnteringPhysVol);

        // Set the correct solid and material in Logical Volume
        //
        G4LogicalVolume* pLogical = pEnteringPhysVol->GetLogicalVolume();
        pLogical->SetSolid( pSolid );
      }
      break;
  }
  return G4AffineTransform(pEnteringPhysVol->GetRotation(), 
                           pEnteringPhysVol->GetTranslation()).Invert(); 
}

// ********************************************************************
// GetLocalExitNormalAndCheck
//
// Obtains the Normal vector to a surface (in local coordinates)
// pointing out of previous volume and into current volume, and
// checks the current point against expected 'local' value.
// ********************************************************************
//
G4ThreeVector G4ITNavigator::
GetLocalExitNormalAndCheck(const G4ThreeVector& ExpectedBoundaryPointGlobal,
                                 G4bool*        pValid)
{
  G4ThreeVector ExpectedBoundaryPointLocal;

  // Check Current point against expected 'local' value
  //
  if ( fLastTriedStepComputation ) 
  {
     const G4AffineTransform& GlobalToLocal= GetGlobalToLocalTransform(); 
     ExpectedBoundaryPointLocal =
       GlobalToLocal.TransformPoint( ExpectedBoundaryPointGlobal ); 
  }

  return GetLocalExitNormal( pValid); 
}

// ********************************************************************
// GetGlobalExitNormal
//
// Obtains the Normal vector to a surface (in global coordinates)
// pointing out of previous volume and into current volume
// ********************************************************************
//
G4ThreeVector 
G4ITNavigator::GetGlobalExitNormal(const G4ThreeVector& IntersectPointGlobal,
                                       G4bool*        pValidNormal)
{
  G4bool         validNormal;
  G4ThreeVector  localNormal, globalNormal;

  localNormal = GetLocalExitNormalAndCheck( IntersectPointGlobal, &validNormal);
  *pValidNormal = validNormal; 
  G4AffineTransform localToGlobal = GetLocalToGlobalTransform(); 
  globalNormal = localToGlobal.TransformAxis( localNormal );
  
  return globalNormal;
}

// ********************************************************************
// ComputeSafety
//
// It assumes that it will be 
//  i) called at the Point in the same volume as the EndPoint of the
//     ComputeStep.
// ii) after (or at the end of) ComputeStep OR after the relocation.
// ********************************************************************
//
G4double G4ITNavigator::ComputeSafety( const G4ThreeVector &pGlobalpoint,
                                     const G4double pMaxLength,
                                     const G4bool keepState)
{
  G4double newSafety = 0.0;

#ifdef G4DEBUG_NAVIGATION
  G4int oldcoutPrec = G4cout.precision(8);
  if( fVerbose > 0 )
  {
    G4cout << "*** G4ITNavigator::ComputeSafety: ***" << G4endl
           << "    Called at point: " << pGlobalpoint << G4endl;

    G4VPhysicalVolume  *motherPhysical = fHistory.GetTopVolume();
    G4cout << "    Volume = " << motherPhysical->GetName() 
           << " - Maximum length = " << pMaxLength << G4endl; 
    if( fVerbose >= 4 )
    {
       G4cout << "    ----- Upon entering Compute Safety:" << G4endl;
       PrintState();
    }
  }
#endif

  if (keepState)  { SetSavedState(); }
  //  fLastTriedStepComputation= true;   -- this method is NOT computing the Step size

  G4double distEndpointSq = (pGlobalpoint-fStepEndPoint).mag2(); 
  G4bool   stayedOnEndpoint  = distEndpointSq < kCarTolerance*kCarTolerance; 
  G4bool   endpointOnSurface = fEnteredDaughter || fExitedMother;

  if( !(endpointOnSurface && stayedOnEndpoint) )
  {
    // Pseudo-relocate to this point (updates voxel information only)
    //
    LocateGlobalPointWithinVolume( pGlobalpoint );
      // --->> Danger: Side effects on sub-navigator voxel information <<---
      //       Could be replaced again by 'granular' calls to sub-navigator
      //       locates (similar side-effects, but faster.  
      //       Solutions:
      //        1) Re-locate (to where?)
      //        2) Insure that the methods using (G4ComputeStep?)
      //           does a relocation (if information is disturbed only ?)

#ifdef G4DEBUG_NAVIGATION
    if( fVerbose >= 2 )
    {
      G4cout << "  G4ITNavigator::ComputeSafety() relocates-in-volume to point: "
             << pGlobalpoint << G4endl;
    }
#endif 
    G4VPhysicalVolume *motherPhysical = fHistory.GetTopVolume();
    G4LogicalVolume *motherLogical = motherPhysical->GetLogicalVolume();
    G4SmartVoxelHeader* pVoxelHeader = motherLogical->GetVoxelHeader();
    G4ThreeVector localPoint = ComputeLocalPoint(pGlobalpoint);

    if ( fHistory.GetTopVolumeType()!=kReplica )
    {
      switch(CharacteriseDaughters(motherLogical))
      {
        case kNormal:
          if ( pVoxelHeader )
          {
            newSafety=fvoxelNav.ComputeSafety(localPoint,fHistory,pMaxLength);
          }
          else
          {
            newSafety=fnormalNav.ComputeSafety(localPoint,fHistory,pMaxLength);
          }
          break;
        case kParameterised:
          if( GetDaughtersRegularStructureId(motherLogical) != 1 )
          {
            newSafety = fparamNav.ComputeSafety(localPoint,fHistory,pMaxLength);
          }
          else  // Regular structure
          {
            newSafety = fregularNav.ComputeSafety(localPoint,fHistory,pMaxLength);
          }
          break;
        case kReplica:
          G4Exception("G4ITNavigator::ComputeSafety()", "NotApplicable",
                      FatalException, "Not applicable for replicated volumes.");
          break;
      }
    }
    else
    {
      newSafety = freplicaNav.ComputeSafety(pGlobalpoint, localPoint,
                                            fHistory, pMaxLength);
    }
  }
  else // if( endpointOnSurface && stayedOnEndpoint )
  {
#ifdef G4DEBUG_NAVIGATION
    if( fVerbose >= 2 )
    {
      G4cout << "    G4ITNavigator::ComputeSafety() finds that point - "
             << pGlobalpoint << " - is on surface " << G4endl; 
      if( fEnteredDaughter ) { G4cout << "   entered new daughter volume"; }
      if( fExitedMother )    { G4cout << "   and exited previous volume."; }
      G4cout << G4endl;
      G4cout << " EndPoint was = " << fStepEndPoint << G4endl;
    } 
#endif
    newSafety = 0.0; 
  }

  // Remember last safety origin & value
  //
  fPreviousSftOrigin = pGlobalpoint;
  fPreviousSafety = newSafety; 

  if (keepState)  { RestoreSavedState(); }

#ifdef G4DEBUG_NAVIGATION
  if( fVerbose > 1 )
  {
    G4cout << "   ---- Exiting ComputeSafety  " << G4endl;
    if( fVerbose > 2 )  { PrintState(); }
    G4cout << "    Returned value of Safety = " << newSafety << G4endl;
  }
  G4cout.precision(oldcoutPrec);
#endif

  return newSafety;
}

// ********************************************************************
// CreateTouchableHistoryHandle
// ********************************************************************
//
G4TouchableHistoryHandle G4ITNavigator::CreateTouchableHistoryHandle() const
{
  return G4TouchableHistoryHandle( CreateTouchableHistory() );
}

// ********************************************************************
// PrintState
// ********************************************************************
//
void  G4ITNavigator::PrintState() const
{
  G4int oldcoutPrec = G4cout.precision(4);
  if( fVerbose == 4 )
  {
    G4cout << "The current state of G4ITNavigator is: " << G4endl;
    G4cout << "  ValidExitNormal= " << fValidExitNormal << G4endl
           << "  ExitNormal     = " << fExitNormal      << G4endl
           << "  Exiting        = " << fExiting         << G4endl
           << "  Entering       = " << fEntering        << G4endl
           << "  BlockedPhysicalVolume= " ;
    if (fBlockedPhysicalVolume==0)
      G4cout << "None";
    else
      G4cout << fBlockedPhysicalVolume->GetName();
    G4cout << G4endl
           << "  BlockedReplicaNo     = " <<  fBlockedReplicaNo       << G4endl
           << "  LastStepWasZero      = " <<   fLastStepWasZero       << G4endl
           << G4endl;   
  }
  if( ( 1 < fVerbose) && (fVerbose < 4) )
  {
    G4cout << std::setw(30) << " ExitNormal "  << " "     
           << std::setw( 5) << " Valid "       << " "     
           << std::setw( 9) << " Exiting "     << " "      
           << std::setw( 9) << " Entering"     << " " 
           << std::setw(15) << " Blocked:Volume "  << " "   
           << std::setw( 9) << " ReplicaNo"        << " "  
           << std::setw( 8) << " LastStepZero  "   << " "   
           << G4endl;   
    G4cout << "( " << std::setw(7) << fExitNormal.x() 
           << ", " << std::setw(7) << fExitNormal.y()
           << ", " << std::setw(7) << fExitNormal.z() << " ) "
           << std::setw( 5)  << fValidExitNormal  << " "   
           << std::setw( 9)  << fExiting          << " "
           << std::setw( 9)  << fEntering         << " ";
    if ( fBlockedPhysicalVolume==0 )
      G4cout << std::setw(15) << "None";
    else
      G4cout << std::setw(15)<< fBlockedPhysicalVolume->GetName();
      G4cout << std::setw( 9)  << fBlockedReplicaNo  << " "
             << std::setw( 8)  << fLastStepWasZero   << " "
             << G4endl;   
  }
  if( fVerbose > 2 ) 
  {
    G4cout.precision(8);
    G4cout << " Current Localpoint = " << fLastLocatedPointLocal << G4endl;
    G4cout << " PreviousSftOrigin  = " << fPreviousSftOrigin << G4endl;
    G4cout << " PreviousSafety     = " << fPreviousSafety << G4endl; 
  }
  G4cout.precision(oldcoutPrec);
}

// ********************************************************************
// ComputeStepLog
// ********************************************************************
//
void G4ITNavigator::ComputeStepLog(const G4ThreeVector& pGlobalpoint,
                                       G4double moveLenSq) const
{
  //  The following checks only make sense if the move is larger
  //  than the tolerance.

  static const G4double fAccuracyForWarning   = kCarTolerance,
                        fAccuracyForException = 1000*kCarTolerance;

  G4ThreeVector OriginalGlobalpoint = fHistory.GetTopTransform().Inverse().
                                      TransformPoint(fLastLocatedPointLocal); 

  G4double shiftOriginSafSq = (fPreviousSftOrigin-pGlobalpoint).mag2();

  // Check that the starting point of this step is 
  // within the isotropic safety sphere of the last point
  // to a accuracy/precision  given by fAccuracyForWarning.
  //   If so give warning.
  //   If it fails by more than fAccuracyForException exit with error.
  //
  if( shiftOriginSafSq >= sqr(fPreviousSafety) )
  {
    G4double shiftOrigin = std::sqrt(shiftOriginSafSq);
    G4double diffShiftSaf = shiftOrigin - fPreviousSafety;

    if( diffShiftSaf > fAccuracyForWarning )
    {
      G4int oldcoutPrec= G4cout.precision(8);
      G4int oldcerrPrec= G4cerr.precision(10);
      std::ostringstream message, suggestion;
      message << "Accuracy error or slightly inaccurate position shift."
              << G4endl
              << "     The Step's starting point has moved " 
              << std::sqrt(moveLenSq)/mm << " mm " << G4endl
              << "     since the last call to a Locate method." << G4endl
              << "     This has resulted in moving " 
              << shiftOrigin/mm << " mm " 
              << " from the last point at which the safety " 
              << "     was calculated " << G4endl
              << "     which is more than the computed safety= " 
              << fPreviousSafety/mm << " mm  at that point." << G4endl
              << "     This difference is " 
              << diffShiftSaf/mm << " mm." << G4endl
              << "     The tolerated accuracy is "
              << fAccuracyForException/mm << " mm.";

      suggestion << " ";
      static G4int warnNow = 0;
      if( ((++warnNow % 100) == 1) )
      {
        message << G4endl
               << "  This problem can be due to either " << G4endl
               << "    - a process that has proposed a displacement"
               << " larger than the current safety , or" << G4endl
               << "    - inaccuracy in the computation of the safety";
        suggestion << "We suggest that you " << G4endl
                   << "   - find i) what particle is being tracked, and "
                   << " ii) through what part of your geometry " << G4endl
                   << "      for example by re-running this event with "
                   << G4endl
                   << "         /tracking/verbose 1 "  << G4endl
                   << "    - check which processes you declare for"
                   << " this particle (and look at non-standard ones)"
                   << G4endl
                   << "   - in case, create a detailed logfile"
                   << " of this event using:" << G4endl
                   << "         /tracking/verbose 6 ";
      }
      G4Exception("G4ITNavigator::ComputeStep()",
                  "GeomNav1002", JustWarning,
                  message, G4String(suggestion.str()));
      G4cout.precision(oldcoutPrec);
      G4cerr.precision(oldcerrPrec);
    }
#ifdef G4DEBUG_NAVIGATION
    else
    {
      G4cerr << "WARNING - G4ITNavigator::ComputeStep()" << G4endl
             << "          The Step's starting point has moved "
             << std::sqrt(moveLenSq) << "," << G4endl
             << "          which has taken it to the limit of"
             << " the current safety. " << G4endl;
    }
#endif
  }
  G4double safetyPlus = fPreviousSafety + fAccuracyForException;
  if ( shiftOriginSafSq > sqr(safetyPlus) )
  {
    std::ostringstream message;
    message << "May lead to a crash or unreliable results." << G4endl
            << "        Position has shifted considerably without"
            << " notifying the navigator !" << G4endl
            << "        Tolerated safety: " << safetyPlus << G4endl
            << "        Computed shift  : " << shiftOriginSafSq;
    G4Exception("G4ITNavigator::ComputeStep()", "GeomNav1002",
                JustWarning, message);
  }
}

// ********************************************************************
// Operator <<
// ********************************************************************
//
std::ostream& operator << (std::ostream &os,const G4ITNavigator &n)
{
  os << "Current History: " << G4endl << n.fHistory;
  return os;
}
