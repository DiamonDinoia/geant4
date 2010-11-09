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
// class G4PartialPhantomParameterisation implementation
//
// May 2007 Pedro Arce,   first version
//
// --------------------------------------------------------------------

#include "G4PartialPhantomParameterisation.hh"

#include "globals.hh"
#include "G4VSolid.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4VVolumeMaterialScanner.hh"
#include "G4GeometryTolerance.hh"

//------------------------------------------------------------------
G4PartialPhantomParameterisation::G4PartialPhantomParameterisation() 
{
}


//------------------------------------------------------------------
G4PartialPhantomParameterisation::~G4PartialPhantomParameterisation()
{
}


//------------------------------------------------------------------
void G4PartialPhantomParameterisation::
ComputeTransformation(const G4int copyNo, G4VPhysicalVolume *physVol ) const
{
  // Voxels cannot be rotated, return translation
  //
  G4ThreeVector trans = GetTranslation( copyNo );

#ifndef NO_REGNAV_VERB
  G4cout << "REGNAV G4PartialPhantomParameterisation::ComputeTransformation " << copyNo << " = " << trans << G4endl;
#endif
  physVol->SetTranslation( trans );
}


//------------------------------------------------------------------
G4ThreeVector G4PartialPhantomParameterisation::
GetTranslation(const G4int copyNo ) const
{
  CheckCopyNo( copyNo );

  size_t nx;
  size_t ny;
  size_t nz;
  ComputeVoxelIndices( copyNo, nx, ny, nz );

  G4ThreeVector trans( (2*nx+1)*fVoxelHalfX - fContainerWallX,
                       (2*ny+1)*fVoxelHalfY - fContainerWallY,
                       (2*nz+1)*fVoxelHalfZ - fContainerWallZ);
#ifndef NO_REGNAV_VERB
  G4cout << "REGNAV G4PartialPhantomParameterisation::GetTranslation " << trans << " copyNo " << copyNo << G4endl;
#endif

  return trans;
}


//------------------------------------------------------------------
       
#include "G4Material.hh"

//------------------------------------------------------------------
G4Material* G4PartialPhantomParameterisation::
ComputeMaterial(const G4int copyNo, G4VPhysicalVolume *, const G4VTouchable *) 
{ 
  CheckCopyNo( copyNo );
  size_t matIndex = GetMaterialIndex(copyNo);

#ifndef NO_REGNAV_VERB
  G4cout << "REGNAV G4PartialPhantomParameterisation::ComputeMaterial " << copyNo << " matIndex " << matIndex << " = " << fMaterials[ matIndex ]->GetName() << G4endl;
#endif
  return fMaterials[ matIndex ];
}


//------------------------------------------------------------------
size_t G4PartialPhantomParameterisation::
GetMaterialIndex( size_t copyNo ) const
{
  CheckCopyNo( copyNo );

  if( !fMaterialIndices ) { return 0; }
#ifndef NO_REGNAV_VERB
  G4cout << " REGNAV G4PartialPhantomParameterisation::GetMaterialIndex = " << *(fMaterialIndices+copyNo) << " copyNo " << copyNo << G4endl;
#endif

  return *(fMaterialIndices+copyNo);
}


//------------------------------------------------------------------
size_t G4PartialPhantomParameterisation::
GetMaterialIndex( size_t nx, size_t ny, size_t nz ) const
{
  size_t copyNo = nx + fNoVoxelX*ny + fNoVoxelXY*nz;
#ifndef NO_REGNAV_VERB
  G4cout << " REGNAV G4PartialPhantomParameterisation::GetMaterialIndex " << copyNo << " nx " << nx << " ny " << ny << " nz " << nz << G4endl;
#endif
  return GetMaterialIndex( copyNo );
}


//------------------------------------------------------------------
G4Material* G4PartialPhantomParameterisation::GetMaterial( size_t nx, size_t ny, size_t nz) const
{
#ifndef NO_REGNAV_VERB
  G4cout << " REGNAV G4PartialPhantomParameterisation::GetMaterial nxnynz " << fMaterials[GetMaterialIndex(nx,ny,nz)]->GetName() << G4endl; 
#endif
  return fMaterials[GetMaterialIndex(nx,ny,nz)];
}

//------------------------------------------------------------------
G4Material* G4PartialPhantomParameterisation::GetMaterial( size_t copyNo ) const
{
#ifndef NO_REGNAV_VERB
  G4cout << " REGNAV G4PartialPhantomParameterisation::GetMaterial copyNo " << fMaterials[GetMaterialIndex(copyNo)]->GetName() << G4endl;
#endif
  return fMaterials[GetMaterialIndex(copyNo)];
}
#include <list>
//------------------------------------------------------------------
void G4PartialPhantomParameterisation::
ComputeVoxelIndices(const G4int copyNo, size_t& nx,
                            size_t& ny, size_t& nz ) const
{
  CheckCopyNo( copyNo );

  std::multimap<G4int,G4int>::const_iterator ite = fFilledIDs.lower_bound(size_t(copyNo));
  //  G4cout << " ite " << (*ite).first << " " << (*ite).second << G4endl;
  G4int dist = std::distance( fFilledIDs.begin(), ite );
  nz = size_t(dist/fNoVoxelY);
  ny = size_t( dist%fNoVoxelY );

  G4int ifmin = (*ite).second;
  //  G4int nvoxX = (*ite).first;
  G4int nvoxXprev;
  if( dist != 0 ) {
    ite--;
    nvoxXprev = (*ite).first;
  } else {
    nvoxXprev = -1;
  } 

  nx = ifmin+copyNo-nvoxXprev-1;

#ifndef NO_REGNAV_VERB
  G4cout << " REGNAV ComputeVoxelIndices " << copyNo << " dist " << dist << " nx " << nx << " ny " << ny << " nz " << nz << " ifmin " << ifmin << " nvoxX " << nvoxX << " nvoxXprev " << nvoxXprev << G4endl;
#endif
}


//------------------------------------------------------------------
G4int G4PartialPhantomParameterisation::
GetReplicaNo( const G4ThreeVector& localPoint, const G4ThreeVector& localDir )
{

  // Check first that point is really inside voxels
  //
  /*  if( fContainerSolid->Inside( localPoint ) == kOutside )
  {
    G4cerr << "ERROR - G4PartialPhantomParameterisation::GetReplicaNo()" << G4endl
           << "        localPoint - " << localPoint
           << " - is outside container solid: "
           << fContainerSolid->GetName() << G4endl;
    G4Exception("G4PartialPhantomParameterisation::GetReplicaNo()", "InvalidSetup",
                FatalErrorInArgument, "Point outside voxels!");
		}*/
  
  // Check the voxel numbers corresponding to localPoint
  // When a particle is on a surface, it may be between -kCarTolerance and
  // +kCartolerance. By a simple distance as:
  //   G4int nx = G4int( (localPoint.x()+)/fVoxelHalfX/2.);
  // those between -kCartolerance and 0 will be placed on voxel N-1 and those
  // between 0 and kCarTolerance on voxel N.
  // To avoid precision problems place the tracks that are on the surface on
  // voxel N-1 if they have negative direction and on voxel N if they have
  // positive direction.
  // Add +kCarTolerance so that they are first placed on voxel N, and then
  // if the direction is negative substract 1

  G4double fx = (localPoint.x()+fContainerWallX+kCarTolerance)/(fVoxelHalfX*2.);
  G4int nx = G4int(fx);

  G4double fy = (localPoint.y()+fContainerWallY+kCarTolerance)/(fVoxelHalfY*2.);
  G4int ny = G4int(fy);

  G4double fz = (localPoint.z()+fContainerWallZ+kCarTolerance)/(fVoxelHalfZ*2.);
  G4int nz = G4int(fz);

  // If it is on the surface side, check the direction: if direction is
  // negative place it on the previous voxel (if direction is positive it is
  // already in the next voxel...). 
  // Correct also cases where n = -1 or n = fNoVoxel. It is always traced to be
  // due to multiple scattering: track is entering a voxel but multiple
  // scattering changes the angle towards outside
  //
  if( fx - nx < kCarTolerance/fVoxelHalfX )
  {
    if( localDir.x() < 0 )
    {
      if( nx != 0 )
      {
        nx -= 1;
      }
    }
    else
    {
      if( nx == G4int(fNoVoxelX) )  
      {
        nx -= 1;       
      }
    }
  }
  if( fy - ny < kCarTolerance/fVoxelHalfY )
  {
    if( localDir.y() < 0 )
    {
      if( ny != 0 )
      {
        ny -= 1;
      }
    }
    else
    {
      if( ny == G4int(fNoVoxelY) )  
      {
        ny -= 1;       
      }
    }
  }
  if( fz - nz < kCarTolerance/fVoxelHalfZ )
  {
    if( localDir.z() < 0 )
    {
      if( nz != 0 )
      {
        nz -= 1;
      }
    }
    else
    {
      if( nz == G4int(fNoVoxelZ) )  
      {
        nz -= 1;       
      }
    }
  }
  
  // Check if there are still errors 
  //
  G4bool isOK = true;
  if( nx < 0 )
  {
    nx = 0;
    isOK = false;
  }
  else if( nx >= G4int(fNoVoxelX) )
  {
    nx = fNoVoxelX-1;
    isOK = false;
  }
  if( ny < 0 )
  {
    ny = 0;
    isOK = false;
  }
  else if( ny >= G4int(fNoVoxelY) )
  {
    ny = fNoVoxelY-1;
    isOK = false;
  }
  if( nz < 0 )
  {
    nz = 0;
    isOK = false;
  }
  else if( nz >= G4int(fNoVoxelZ) )
  {
    nz = fNoVoxelZ-1;
    isOK = false;
  }
  if( !isOK )
  {
    G4cerr << "WARNING - G4PartialPhantomParameterisation::GetReplicaNo()" << G4endl
           << "          LocalPoint: " << localPoint << G4endl
           << "          LocalDir: " << localDir << G4endl
           << "          Voxel container size: " << fContainerWallX
           << " " << fContainerWallY << " " << fContainerWallZ << G4endl
           << "          LocalPoint - wall: "
           << localPoint.x()-fContainerWallX << " "
           << localPoint.y()-fContainerWallY << " "
           << localPoint.z()-fContainerWallZ << G4endl;
    G4Exception("G4PartialPhantomParameterisation::GetReplicaNo()",
                "Wrong-copy-number", JustWarning,
                "Corrected the copy number! It was negative or too big");
  }

  G4int nyz = nz*fNoVoxelY+ny;
  std::multimap<G4int,G4int>::iterator ite = fFilledIDs.begin();
  for( ite = fFilledIDs.begin(); ite != fFilledIDs.end(); ite++ ){
    G4cout << " G4PartialPhantomParameterisation::GetReplicaNo filled " << (*ite).first << " , " << (*ite).second << std::endl;

  }
  ite = fFilledIDs.begin();

  advance(ite,nyz);
  //    for( G4int ii = 0; ii < nyz; ii++, ite++ );
  std::multimap<G4int,G4int>::iterator iteant = ite; iteant--;
  //  G4int nVoxelsInXLine = (*ite).first - (*iteant).first;
  G4int copyNo = (*iteant).first + 1 + ( nx - (*ite).second );
  G4cout << " G4PartialPhantomParameterisation::GetReplicaNo getting copyNo " << copyNo << "  nyz " << nyz << "  (*iteant).first " << (*iteant).first << "  (*ite).second " <<  (*ite).second << G4endl;

  //  G4int copyNo = nx + fNoVoxelX*ny + fNoVoxelXY*nz;

  // CheckCopyNo( copyNo ); // not needed, just for debugging code

  G4cout << " G4PartialPhantomParameterisation::GetReplicaNo " << copyNo << " nx " << nx << " ny " << ny << " nz " << nz << " localPoint " << localPoint << " localDir " << localDir << G4endl;

  return copyNo;
}


//------------------------------------------------------------------
void G4PartialPhantomParameterisation::CheckCopyNo( const G4int copyNo ) const
{ 
  if( copyNo < 0 || copyNo >= G4int(fNoVoxel) )
  {
    G4cerr << "ERROR - G4PartialPhantomParameterisation::CheckCopyNo()" << G4endl
           << "        Copy number: " << copyNo << G4endl
           << "        Total number of voxels: " << fNoVoxel << G4endl;
    G4Exception("G4PartialPhantomParameterisation::CheckCopyNo()",
                "Wrong-copy-number", FatalErrorInArgument,
                "Copy number is negative or too big!");
  }
}

//------------------------------------------------------------------
void G4PartialPhantomParameterisation::BuildContainerWalls()
{
  fContainerWallX = fNoVoxelX * fVoxelHalfX;
  fContainerWallY = fNoVoxelY * fVoxelHalfY;
  fContainerWallZ = fNoVoxelZ * fVoxelHalfZ;

#ifndef NO_REGNAV_VERB
  G4cout << " REGNAV G4PartialPhantomParameterisation::BuildContainerWalls " << fContainerWallX << " " << fContainerWallY << " " << fContainerWallZ << G4endl;
#endif
}
