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
//
// $Id$
//
// 
// class G4PVReplica Implementation
//
// ----------------------------------------------------------------------

#include "G4PVReplica.hh"
#include "G4LogicalVolume.hh"

//01.25.2009 Xin Dong: Phase II change for Geant4 multi-threading.
//This static member is thread local. For each thread, it points to the     
//array of ReplicaPrivateSubclass instances.
template <class ReplicaPrivateSubclass> __thread ReplicaPrivateSubclass* G4MTPrivateSubInstanceManager<ReplicaPrivateSubclass>::offset = 0;

//01.25.2009 Xin Dong: Phase II change for Geant4 multi-threading.          
//This new field helps to use the class G4PVReplicaSubInstanceManager        
//introduced in the "G4PVReplica.hh" file.
G4PVReplicaSubInstanceManager G4PVReplica::g4pvreplicaSubInstanceManager;

//01.25.2009 Xin Dong: Phase II change for Geant4 multi-threading.
//This method is similar to the constructor. It is used by each worker
//thread to achieve the same effect as that of the master thread exept
//to register the new created instance. This method is invoked explicitly.
//It does not create a new G4PVReplica instance. It only assigns the value
//for the fields encapsulated by the class ReplicaPrivateSubclass.
void G4PVReplica::SlaveG4PVReplica(G4PVReplica *pMasterObject)
{

  SlaveG4VPhysicalVolume( pMasterObject, 0, G4ThreeVector());
  g4pvreplicaSubInstanceManager.SlaveCopySubInstanceArray();
  fcopyNoG4MTThreadPrivate = -1;
  CheckAndSetParameters (faxis, fnReplicas, fwidth, foffset);
} 

//01.25.2009 Xin Dong: Phase II change for Geant4 multi-threading.
//This method is similar to the destructor. It is used by each worker
//thread to achieve the partial effect as that of the master thread.
//For G4PVReplica instances, it destories the rotation matrix.
void G4PVReplica::DestroySlaveG4PVReplica(G4PVReplica* /*pMasterObject*/)
{
  if ( faxis==kPhi )
  {
    delete GetRotation();
  }
}

G4PVReplica::G4PVReplica( const G4String& pName,
                                G4LogicalVolume* pLogical,
                                G4VPhysicalVolume* pMother,
                          const EAxis pAxis,
                          const G4int nReplicas,
                          const G4double width,
                          const G4double offset )
  : G4VPhysicalVolume(0, G4ThreeVector(), pName, pLogical, pMother), fRegularVolsId(0)
{

  g4pvreplicaInstanceID = g4pvreplicaSubInstanceManager.CreateSubInstance();

  fcopyNoG4MTThreadPrivate = -1;

  if ((!pMother) || (!pMother->GetLogicalVolume()))
  {
    std::ostringstream message;
    message << "NULL pointer specified as mother volume." << G4endl
            << "The world volume cannot be sliced or parameterised !";
    G4Exception("G4PVReplica::G4PVReplica()", "GeomVol0002",
                FatalException, message);
    return;
  }
  G4LogicalVolume* motherLogical = pMother->GetLogicalVolume();
  if (pLogical == motherLogical)
  {
    G4Exception("G4PVReplica::G4PVReplica()", "GeomVol0002",
                FatalException, "Cannot place a volume inside itself!");
    return;
  }
  SetMotherLogical(motherLogical);
  motherLogical->AddDaughter(this);
  if (motherLogical->GetNoDaughters() != 1)
  {
    std::ostringstream message;
    message << "Replica or parameterised volume must be the only daughter !"
            << G4endl
            << "     Mother physical volume: " << pMother->GetName() << G4endl
            << "     Replicated volume: " << pName;
    G4Exception("G4PVReplica::G4PVReplica()", "GeomVol0002",
                FatalException, message);
    return;
  }
  CheckAndSetParameters (pAxis, nReplicas, width, offset);
}

G4PVReplica::G4PVReplica( const G4String& pName,
                                G4LogicalVolume* pLogical,
                                G4LogicalVolume* pMotherLogical,
                          const EAxis pAxis,
                          const G4int nReplicas,
                          const G4double width,
                          const G4double offset )
  : G4VPhysicalVolume(0,G4ThreeVector(),pName,pLogical,0), fRegularVolsId(0)
{

  g4pvreplicaInstanceID = g4pvreplicaSubInstanceManager.CreateSubInstance();
  fcopyNoG4MTThreadPrivate = -1; 

  if (!pMotherLogical)
  {
    std::ostringstream message;
    message << "NULL pointer specified as mother volume for "
            << pName << ".";
    G4Exception("G4PVReplica::G4PVReplica()", "GeomVol0002",
                FatalException, message);
    return;
  }
  if (pLogical == pMotherLogical)
  {
    G4Exception("G4PVReplica::G4PVReplica()", "GeomVol0002",
                FatalException, "Cannot place a volume inside itself!");
    return;
  }
  pMotherLogical->AddDaughter(this);
  SetMotherLogical(pMotherLogical);
  if (pMotherLogical->GetNoDaughters() != 1)
  {
    std::ostringstream message;
    message << "Replica or parameterised volume must be the only daughter !"
            << G4endl
            << "     Mother logical volume: " << pMotherLogical->GetName()
            << G4endl
            << "     Replicated volume: " << pName;
    G4Exception("G4PVReplica::G4PVReplica()", "GeomVol0002",
                FatalException, message);
    return;
  }
  CheckAndSetParameters (pAxis, nReplicas, width, offset);
}

void G4PVReplica::CheckAndSetParameters( const EAxis pAxis,
                                         const G4int nReplicas,
                                         const G4double width,
                                         const G4double offset)
{    
  if (nReplicas<1)
  {
    G4Exception("G4PVReplica::CheckAndSetParameters()", "GeomVol0002",
                FatalException, "Illegal number of replicas.");
  }
  fnReplicas=nReplicas;
  if (width<0)
  {
    G4Exception("G4PVReplica::CheckAndSetParameters()", "GeomVol0002",
                FatalException, "Width must be positive.");
  }
  fwidth  = width;
  foffset = offset;
  faxis   = pAxis;

  // Create rotation matrix for phi axis case & check axis is valid
  //
  G4RotationMatrix* pRMat=0;
  switch (faxis)
  {
    case kPhi:
      pRMat=new G4RotationMatrix();
      if (!pRMat)
      {
        G4Exception("G4PVReplica::CheckAndSetParameters()", "GeomVol0003",
                    FatalException, "Rotation matrix allocation failed.");
      }
      SetRotation(pRMat);
      break;
    case kRho:
    case kXAxis:
    case kYAxis:
    case kZAxis:
    case kUndefined:
      break;
    default:
      G4Exception("G4PVReplica::CheckAndSetParameters()", "GeomVol0002",
                  FatalException, "Unknown axis of replication.");
      break;
  }
}

G4PVReplica::G4PVReplica( __void__& a )
  : G4VPhysicalVolume(a), faxis(kZAxis), fnReplicas(0), fwidth(0.),
    foffset(0.), fRegularStructureCode(0), fRegularVolsId(0)
{
  g4pvreplicaInstanceID = g4pvreplicaSubInstanceManager.CreateSubInstance();
  fcopyNoG4MTThreadPrivate = -1; 
}

G4PVReplica::~G4PVReplica()
{
  if ( faxis==kPhi )
  {
    delete GetRotation();
  }
}

G4bool G4PVReplica::IsMany() const
{
  return false; 
}

G4int G4PVReplica::GetCopyNo() const
{
  return fcopyNoG4MTThreadPrivate;
}

void  G4PVReplica::SetCopyNo(G4int newCopyNo)
{
  fcopyNoG4MTThreadPrivate = newCopyNo;
}

G4bool G4PVReplica::IsReplicated() const
{
  return true;
}

G4bool G4PVReplica::IsParameterised() const
{
  return false;
}

G4VPVParameterisation* G4PVReplica::GetParameterisation() const
{
  return 0;
}

G4int G4PVReplica::GetMultiplicity() const
{
  return fnReplicas;
}



void G4PVReplica::GetReplicationData( EAxis& axis,
                                      G4int& nReplicas,
                                      G4double& width,
                                      G4double& offset,
                                      G4bool& consuming ) const
{
  axis = faxis;
  nReplicas = fnReplicas;
  width = fwidth;
  offset = foffset;
  consuming = true;
}

G4bool G4PVReplica::IsRegularStructure() const
{
  return (fRegularVolsId!=0); 
}

G4int  G4PVReplica::GetRegularStructureId() const
{
  return fRegularVolsId; 
}

void   G4PVReplica::SetRegularStructureId( G4int Code )
{
  fRegularVolsId= Code; 
} 
