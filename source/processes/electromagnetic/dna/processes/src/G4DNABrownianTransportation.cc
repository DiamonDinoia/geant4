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
// $Id$
//
// Author: Mathieu Karamitros (kara (AT) cenbg . in2p3 . fr) 
//
// WARNING : This class is released as a prototype.
// It might strongly evolve or even disapear in the next releases.
//
// History:
// -----------
// 10 Oct 2011 M.Karamitros created
//
// -------------------------------------------------------------------

/// \brief { The transportation method implemented is the one from
///         Ermak-McCammon : J. Chem. Phys. 69, 1352 (1978)}

#include <CLHEP/Random/Stat.h>

#include "G4DNABrownianTransportation.hh"

#include <G4VScheduler.hh>
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4Molecule.hh"
#include "G4RandomDirection.hh"
#include "G4ParticleTable.hh"
#include "G4SafetyHelper.hh"
#include "G4TransportationManager.hh"
#include "G4UnitsTable.hh"
#include "G4NistManager.hh"
#include "G4DNAMolecularMaterial.hh"
#include "G4ITNavigator.hh"
#include "G4ITSafetyHelper.hh" // Not used yet

using namespace std;

#ifndef State
#define State(theXInfo) (GetState<G4ITBrownianState>()->theXInfo)
#endif

//#ifndef State
//#define State(theXInfo) (fTransportationState->theXInfo)
//#endif

//COLOR FOR DEBUGING
//#define RED_ON_WHITE  "\033[0;31m"
//#define GREEN "\033[32;40m"
#define GREEN_ON_BLUE "\033[1;32;44m"
#define RESET "\033[0m"

//#define DEBUG_MEM 1

#ifdef DEBUG_MEM
#include "G4MemStat.hh"
using namespace G4MemStat;
using G4MemStat::MemStat;
#endif

static double InvErf(double x)
{
  return CLHEP::HepStat::inverseErf(x);
}

static double InvErfc(double x)
{
  return CLHEP::HepStat::inverseErf(1. - x);
}

#ifndef State
#define State(theXInfo) (GetState<G4ITTransportationState>()->theXInfo)
#endif

G4DNABrownianTransportation::G4DNABrownianTransportation(const G4String& aName,
                                                         G4int verbosity) :
    G4ITTransportation(aName, verbosity)
{

  fpState.reset(new G4ITBrownianState());

  //ctor
  SetProcessSubType(61);

  fNistWater = G4NistManager::Instance()->FindOrBuildMaterial("G4_WATER");
  fpWaterDensity = 0;


  fUseMaximumTimeBeforeReachingBoundary = true;
  fForceLimitOnMinTimeSteps = true;

  // Once the small distances sent back from the navigator system are understood
  // and corrected, the above flag should be put by default
//  fUseMaximumTimeBeforeReachingBoundary = false;
//  fForceLimitOnMinTimeSteps = false;

  //	fMinTimeStep = 100*picosecond;
}

G4DNABrownianTransportation::~G4DNABrownianTransportation()
{
  ;
}

G4DNABrownianTransportation::G4DNABrownianTransportation(const G4DNABrownianTransportation& right) :
    G4ITTransportation(right)
{
  //copy ctor
  SetProcessSubType(61);
  fUseMaximumTimeBeforeReachingBoundary = right
      .fUseMaximumTimeBeforeReachingBoundary;
  fForceLimitOnMinTimeSteps = right.fForceLimitOnMinTimeSteps;
  fNistWater = right.fNistWater;
  fpWaterDensity = right.fpWaterDensity;
  //fMinTimeStep = right.fMinTimeStep;
}

G4DNABrownianTransportation& G4DNABrownianTransportation::operator=(const G4DNABrownianTransportation& rhs)
{
  if (this == &rhs) return *this; // handle self assignment
  //assignment operator
  return *this;
}

G4DNABrownianTransportation::G4ITBrownianState::G4ITBrownianState() :
    G4ITTransportationState()
{
  fPathLengthWasCorrected = false;
  fTimeStepReachedLimit = false;
}

void G4DNABrownianTransportation::StartTracking(G4Track* track)
{
  fpState.reset(new G4ITBrownianState());
//	G4cout << "G4DNABrownianTransportation::StartTracking : "
//  "Initialised track State" << G4endl;
  SetInstantiateProcessState(false);
  G4ITTransportation::StartTracking(track);
}

void G4DNABrownianTransportation::BuildPhysicsTable(const G4ParticleDefinition& particle)
{
  if (verboseLevel > 0)
  {
    G4cout << G4endl<< GetProcessName() << ":   for  "
    << setw(24) << particle.GetParticleName()
    << "\tSubType= " << GetProcessSubType() << G4endl;
  }
  // Initialize water density pointer
  fpWaterDensity = G4DNAMolecularMaterial::Instance()->
  GetDensityTableFor(G4Material::GetMaterial("G4_WATER"));

  fpSafetyHelper->InitialiseHelper();
  G4ITTransportation::BuildPhysicsTable(particle);
}

void G4DNABrownianTransportation::ComputeStep(const G4Track& track,
                                              const G4Step& step,
                                              const double timeStep,
                                              double& spaceStep)
{
  // G4cout << "G4ITBrownianTransportation::ComputeStep" << G4endl;

  /* If this method is called, this step
   * cannot be geometry limited.
   * In case the step is limited by the geometry,
   * this method should not be called.
   * The fTransportEndPosition calculated in
   * the method AlongStepIL should be taken
   * into account.
   * In order to do so, the flag IsLeadingStep
   * is on. Meaning : this track has the minimum
   * interaction length over all others.
   */
  if (GetIT(track)->GetTrackingInfo()->IsLeadingStep())
  {
    const G4VITProcess* ITProc = ((const G4VITProcess*) step.GetPostStepPoint()
        ->GetProcessDefinedStep());
    bool makeException = true;

    if (ITProc && ITProc->ProposesTimeStep()) makeException = false;

    if (makeException)
    {

      G4ExceptionDescription exceptionDescription;
      exceptionDescription << "ComputeStep is called while the track has"
                           "the minimum interaction time";
      exceptionDescription << " so it should not recompute a timeStep ";
      G4Exception("G4DNABrownianTransportation::ComputeStep",
                  "G4DNABrownianTransportation001", FatalErrorInArgument,
                  exceptionDescription);
    }
  }

  State(fGeometryLimitedStep) = false;
  // TODO : generalize this process to all kind of brownian objects
  // G4ITBrownianObject* ITBrown = GetITBrownianObject(track) ;
  // G4double diffCoeff = ITBrown->GetDiffusionCoefficient(track.GetMaterial());
  G4Molecule* molecule = GetMolecule(track);
  G4double diffCoeff = molecule->GetDiffusionCoefficient();

  if (timeStep > 0)
  {
    spaceStep = DBL_MAX;

    // G4cout << "EndpointDistance = " << State(endpointDistance) << G4endl;
    //  while(spaceStep > State(endpointDistance))
    // Probably inefficient when the track is close to boundaries
    // it goes with fUserMaximumTimeBeforeReachingBoundary == false
    // fUserMaximumTimeBeforeReachingBoundary == true, it should never loop
    {
      G4double x = G4RandGauss::shoot(0,sqrt(2*diffCoeff*timeStep));
      G4double y = G4RandGauss::shoot(0,sqrt(2*diffCoeff*timeStep));
      G4double z = G4RandGauss::shoot(0,sqrt(2*diffCoeff*timeStep));

      spaceStep = sqrt(x*x + y*y + z*z);
    }

    if(fUseMaximumTimeBeforeReachingBoundary == false ||
        fForceLimitOnMinTimeSteps == false)
    {
      if(spaceStep > State(endpointDistance))
      {
        State(fGeometryLimitedStep) = true;
        spaceStep = State(endpointDistance);
      }
    }

    State(fTransportEndPosition)= spaceStep*step.GetPostStepPoint()->
    GetMomentumDirection() + track.GetPosition();

//    G4cout << "!!! Final position:"
//    << G4BestUnit(State(fTransportEndPosition), "Length")
//    << "  --- time step = " << G4BestUnit(timeStep, "Time")
//    << G4endl;
  }
  else
  {
    spaceStep = 0.;
    State(fTransportEndPosition) = track.GetPosition();
  }

  State(fCandidateEndGlobalTime) = step.GetPreStepPoint()->GetGlobalTime()
      + timeStep;
  State(fEndGlobalTimeComputed) = true;

#ifdef G4VERBOSE
  //    DEBUG
  if (fVerboseLevel > 1)
  {
    G4cout << GREEN_ON_BLUE << "G4ITBrownianTransportation::ComputeStep() : "
    << " trackID : " << track.GetTrackID() << " : Molecule name: "
    << molecule->GetName()
    << G4endl
    << "Diffusion length : "
    << G4BestUnit(spaceStep, "Length")
    << " within time step : " << G4BestUnit(timeStep,"Time")
    << RESET
    << G4endl<< G4endl;
  }
#endif
}

G4VParticleChange* G4DNABrownianTransportation::PostStepDoIt(const G4Track& track,
                                                             const G4Step& step)
{
  bool killTrack = false;
  if (State(fTimeStepReachedLimit))
  {
    //fLinearNavigator->LocateGlobalPointAndSetup(track.GetPosition());
    fLinearNavigator->LocateGlobalPointAndUpdateTouchable(
        State(fTransportEndPosition), (State(fCurrentTouchableHandle))(), true);

    if ( State(fCurrentTouchableHandle)->GetVolume() == 0)
    {
#ifdef G4VERBOSE
      if (fVerboseLevel)
      {
        G4cout << "Track position : "
        << track.GetPosition() / nanometer<< " [nm]"
        << " Track ID : " << track.GetTrackID() << G4endl;
        G4cout << "G4DNABrownianTransportation will killed the track because "
        "State(fCurrentTouchableHandle)->GetVolume() == 0"
        << G4endl;
      }
#endif
      killTrack = true;
    }
  }

  if (!killTrack)
  {
    G4ITTransportation::PostStepDoIt(track, step);
  }
  else
  {
    fParticleChange.Initialize(track);
    fParticleChange.ProposeTrackStatus(fStopAndKill);
    return &fParticleChange;
  }

#ifdef G4VERBOSE
  //    DEBUG
  if (fVerboseLevel > 1)
  {
    G4cout << GREEN_ON_BLUE << "G4ITBrownianTransportation::PostStepDoIt() :"
    << " trackID : " << track.GetTrackID() << " Molecule name: "
    << GetMolecule(track)->GetName() << G4endl;
    G4cout << "Diffusion length : "
    << G4BestUnit(step.GetStepLength(), "Length") <<" within time step : "
    << G4BestUnit(step.GetDeltaTime(),"Time") << "\t"
    << " Current global time : " << G4BestUnit(track.GetGlobalTime(),"Time")
    << RESET
    << G4endl<< G4endl;
  }
#endif
  return &fParticleChange;
}

void G4DNABrownianTransportation::Diffusion(const G4Track& track)
{

#ifdef DEBUG_MEM
  MemStat mem_first, mem_second, mem_diff;
#endif

#ifdef DEBUG_MEM
  mem_first = MemoryUsage();
#endif

#ifdef G4VERBOSE
  // DEBUG
  if (fVerboseLevel > 1)
  {
    G4cout << GREEN_ON_BLUE
    << setw(18) << "G4DNABrownianTransportation::Diffusion :" << setw(8)
    << GetIT(track)->GetName() << "\t trackID:" << track.GetTrackID()
    << "\t" << " Global Time = "
    << G4BestUnit(track.GetGlobalTime(), "Time") << RESET
    << G4endl<< G4endl;
  }
#endif

  G4Material* material = track.GetMaterial();
  //    if (material != fNistWater && material->GetBaseMaterial() != fNistWater)

  G4double waterDensity = (*fpWaterDensity)[material->GetIndex()];

  if (waterDensity == 0.0)
  //  if (material == nistwater || material->GetBaseMaterial() == nistwater)
  {
    G4cout << "A track is outside water material : trackID"
    << track.GetTrackID() << " (" << GetMolecule(track)->GetName() <<")"
    << G4endl;
    G4cout << "Local Time : " << (track.GetLocalTime()) / s << G4endl;
    G4cout << "Step Number :" << track.GetCurrentStepNumber() << G4endl;

    fParticleChange.ProposeEnergy(0.);
    fParticleChange.ProposeTrackStatus(fStopAndKill); //(fStopButAlive);

    // Got pb with :
    // fParticleChange.ProposeTrackStatus(fStopAndKill);
    // It makes the tracks going straight without killing them

    return;// &fParticleChange is the final returned object
  }

  /*
   #ifdef DEBUG_MEM
   MemStat mem_intermediaire = MemoryUsage();
   mem_diff = mem_intermediaire-mem_first;
   G4cout << "\t\t\t >> || MEM || In G4DNABrownianTransportation::Diffusion "
   "after dealing with waterDensity for "<< track.GetTrackID()
   << ", diff is : " << mem_diff << G4endl;
   #endif

   G4double costheta = (2*G4UniformRand()-1);

   #ifdef DEBUG_MEM
   mem_intermediaire = MemoryUsage();
   mem_diff = mem_intermediaire-mem_first;
   G4cout << "\t\t\t >> || MEM || In G4DNABrownianTransportation::Diffusion "
   "after using G4UniformRand for "<< track.GetTrackID()
   << ", diff is : " << mem_diff << G4endl;
   #endif

   G4double theta = acos (costheta);

   #ifdef DEBUG_MEM
   mem_intermediaire = MemoryUsage();
   mem_diff = mem_intermediaire-mem_first;
   G4cout << "\t\t\t >> || MEM || In G4DNABrownianTransportation::Diffusion "
   "after calling acos for "<< track.GetTrackID()
   << ", diff is : " << mem_diff << G4endl;
   #endif

   G4double phi = 2*pi*G4UniformRand();
   //
   //#ifdef DEBUG_MEM
   //    mem_intermediaire = MemoryUsage();
   //    mem_diff = mem_intermediaire-mem_first;
   //    G4cout << "\t\t\t >> || MEM || In G4DNABrownianTransportation::"
   //           "Diffusion after using G4UniformRand for "
   //           << track.GetTrackID() << ", diff is : " << mem_diff << G4endl;
   //#endif
   //
   //
   G4double xMomentum = cos(phi)* sin(theta);
   G4double yMomentum = sin(theta)*sin(phi);
   G4double zMomentum = costheta;
   //
   //#ifdef DEBUG_MEM
   //    mem_intermediaire = MemoryUsage();
   //    mem_diff = mem_intermediaire-mem_first;
   //    G4cout << "\t\t\t >> || MEM || In G4DNABrownianTransportation::"
   //           "Diffusion before using fParticleChange for "
   //           << track.GetTrackID() << ", diff is : " << mem_diff << G4endl;
   //#endif

   fParticleChange.ProposeMomentumDirection(xMomentum, yMomentum, zMomentum);
   */

  fParticleChange.ProposeMomentumDirection(G4RandomDirection());
  State(fMomentumChanged) = true;
  fParticleChange.SetMomentumChanged(true);

  //    G4cout << "BROWN : Propose new direction :" << G4ThreeVector(xMomentum,
  // yMomentum, zMomentum) << G4endl;

  // Alternative
  //fParticleChange.ProposeMomentumDirection(G4RandomDirection());

  return;// &fParticleChange is the final returned object
}

G4double G4DNABrownianTransportation::ComputeGeomLimit(const G4Track& track,
                                                       G4double& presafety,
                                                       G4double limit)
{
  G4double res = DBL_MAX;
  if(track.GetVolume() != fpSafetyHelper->GetWorldVolume())
  {
    G4TrackStateManager& trackStateMan = GetIT(track)->GetTrackingInfo()
    ->GetTrackStateManager();
    fpSafetyHelper->LoadTrackState(trackStateMan);
    res = fpSafetyHelper->CheckNextStep(
        track.GetStep()->GetPreStepPoint()->GetPosition(),
        track.GetMomentumDirection(),
        limit, presafety);
    fpSafetyHelper->ResetTrackState();
  }
  return res;
}

G4double G4DNABrownianTransportation::AlongStepGetPhysicalInteractionLength(const G4Track& track,
                                                                            G4double previousStepSize,
                                                                            G4double currentMinimumStep,
                                                                            G4double& currentSafety,
                                                                            G4GPILSelection* selection)
{
  G4double geometryStepLength =
      G4ITTransportation::AlongStepGetPhysicalInteractionLength(
          track, previousStepSize, currentMinimumStep, currentSafety,
          selection);

  assert(State(endpointDistance) == geometryStepLength);

  G4double diffusionCoefficient = GetMolecule(track)->GetDiffusionCoefficient();

  State(fComputeLastPosition) = false;

  if (State(fGeometryLimitedStep))
  {
    State(fTimeStepReachedLimit) = false;

    // 99 % of the space step distribution is lower than
    // d_99 = 8 * sqrt(D*t)
    // where t is the corresponding time step
    // so by inversion :
    if (fUseMaximumTimeBeforeReachingBoundary)
    {
      State(theInteractionTimeLeft) = (geometryStepLength * geometryStepLength)
          / (8 * diffusionCoefficient);
      State(fComputeLastPosition) = true;
    }
    else
    // Will use a random time - this is precise but long to compute in certain
    // circumstances (many particles - small volumes)
    {
      State(theInteractionTimeLeft) = 1 / (4 * diffusionCoefficient)
          * pow(geometryStepLength / InvErfc(G4UniformRand()),2);
//      G4cout << "Current Position: "
//      << G4BestUnit(track.GetPosition(), "Length")
//      << G4endl;
//      G4cout << "Proposed Position: "
//      << G4BestUnit(State(fTransportEndPosition), "Length")
//      << G4endl;
//      G4cout << "-- geometryStepLength :"
//      << G4BestUnit(geometryStepLength,"Length")
//      << " --  currentSafety :"
//      << G4BestUnit(currentSafety,"Length")
//      << " --  currentMinimumStep :"
//      << G4BestUnit(currentMinimumStep,"Length")
//      << " -- time : "
//      << G4BestUnit(State(theInteractionTimeLeft), "Time")
//      << G4endl;

//      double spaceStep = DBL_MAX;
//      double geomlimit = DBL_MAX;
//      double presafety = DBL_MAX;
//      geomlimit = ComputeGeomLimit(track, presafety, spaceStep);
//
//      G4cout << "CheckNextStep: " << G4BestUnit(geomlimit, "Length") << G4endl;
//
//      G4cout << G4endl;

//      if(State(fTransportEndPosition).x() / nm < 90) abort();
    }

    double minTimeStepAllowed = G4VScheduler::Instance()->GetLimitingTimeStep();
    double currentMinTimeStep = G4VScheduler::Instance()->GetTimeStep();

    if (fForceLimitOnMinTimeSteps && State(theInteractionTimeLeft)
        < minTimeStepAllowed
        && State(theInteractionTimeLeft) < currentMinTimeStep)
    {
      State(theInteractionTimeLeft) = minTimeStepAllowed;
      State(fTimeStepReachedLimit) = true;
      State(fComputeLastPosition) = true;
    }

//    if (State(fComputeLastPosition))
//    {
//      double spaceStep = DBL_MAX;
//
//      G4double x = G4RandGauss::shoot(
//          0, sqrt(2 * diffusionCoefficient * State(theInteractionTimeLeft)));
//      G4double y = G4RandGauss::shoot(
//          0, sqrt(2 * diffusionCoefficient * State(theInteractionTimeLeft)));
//      G4double z = G4RandGauss::shoot(
//          0, sqrt(2 * diffusionCoefficient * State(theInteractionTimeLeft)));
//
//      spaceStep = sqrt(x * x + y * y + z * z);
//
//      if (fForceLimitOnMinTimeSteps == false
//          && spaceStep > State(endpointDistance))
//      {
//        spaceStep = State(endpointDistance);
//      }
//      else
//      {
//        State(endpointDistance) = spaceStep;
//      }
//
//      // Calculate final position
//      //
//      State(fTransportEndPosition) = track.GetPosition()
//          + spaceStep * track.GetMomentumDirection();
//    }

    State(fCandidateEndGlobalTime) =
        track.GetGlobalTime() + State(theInteractionTimeLeft);

    State(fEndGlobalTimeComputed) = true; // MK: ADDED ON 20/11/2014

    State(fPathLengthWasCorrected) = false;
  }
  else
  {
    geometryStepLength = 2
        * sqrt(diffusionCoefficient * State(theInteractionTimeLeft))
        * InvErf(G4UniformRand());
    State(fPathLengthWasCorrected) = true;
    State(endpointDistance) = geometryStepLength;
  }

  return geometryStepLength;
}

//////////////////////////////////////////////////////////////////////////
//
//   Initialize ParticleChange  (by setting all its members equal
//                               to corresponding members in G4Track)
G4VParticleChange*
G4DNABrownianTransportation::AlongStepDoIt(const G4Track& track,
                                           const G4Step& step)
{
#ifdef DEBUG_MEM
  MemStat mem_first, mem_second, mem_diff;
#endif

#ifdef DEBUG_MEM
  mem_first = MemoryUsage();
#endif

  if (GetIT(track)->GetTrackingInfo()->IsLeadingStep()
      && State(fComputeLastPosition))
  {
    double spaceStep = DBL_MAX;

    G4double diffusionCoefficient = GetMolecule(track)->GetDiffusionCoefficient();
    G4double x = G4RandGauss::shoot(
        0, sqrt(2 * diffusionCoefficient * State(theInteractionTimeLeft)));
    G4double y = G4RandGauss::shoot(
        0, sqrt(2 * diffusionCoefficient * State(theInteractionTimeLeft)));
    G4double z = G4RandGauss::shoot(
        0, sqrt(2 * diffusionCoefficient * State(theInteractionTimeLeft)));

    spaceStep = sqrt(x * x + y * y + z * z);

    if (fForceLimitOnMinTimeSteps == false
        && spaceStep > State(endpointDistance))
    {
      spaceStep = State(endpointDistance);
    }
    else
    {
      State(endpointDistance) = spaceStep;
    }

    // Calculate final position
    //
    State(fTransportEndPosition) = track.GetPosition()
        + spaceStep * track.GetMomentumDirection();
  }

  G4ITTransportation::AlongStepDoIt(track, step);

#ifdef DEBUG_MEM
  MemStat mem_intermediaire = MemoryUsage();
  mem_diff = mem_intermediaire-mem_first;
  G4cout << "\t\t\t >> || MEM || After calling G4ITTransportation::"
  "AlongStepDoIt for "<< track.GetTrackID() << ", diff is : "
  << mem_diff << G4endl;
#endif

  Diffusion(track);

#ifdef DEBUG_MEM
  mem_intermediaire = MemoryUsage();
  mem_diff = mem_intermediaire-mem_first;
  G4cout << "\t\t\t >> || MEM || After calling G4DNABrownianTransportation::"
  "Diffusion for "<< track.GetTrackID() << ", diff is : "
  << mem_diff << G4endl;
#endif

  return &fParticleChange;
}
