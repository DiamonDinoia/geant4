//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
// The code was written by :
//	*Louis Archambault louis.archambault@phy.ulaval.ca,
//      *Luc Beaulieu beaulieu@phy.ulaval.ca
//      +Vincent Hubert-Tremblay at tigre.2@sympatico.ca
//
//
// *Centre Hospitalier Universitaire de Quebec (CHUQ),
// Hotel-Dieu de Quebec, departement de Radio-oncologie
// 11 cote du palais. Quebec, QC, Canada, G1R 2J6
// tel (418) 525-4444 #6720
// fax (418) 691 5268
//
// + Universit� Laval, Qu�bec (QC) Canada
//*******************************************************//
//
//---------------------------------------------------------------------------
///  Octree encapsulates a volumetric compressed representation of the
/// vector field used for the reconstruction of a 3D model from multiple
/// range images or curves.
/// 
/// It is assumed that the space occupied by octree is a cube whose
/// bottom-left-front corner is at origin (0,0,0) while the opposite corner
/// (top,right,back) is at ( mSize,mSize,mSize). Resolution of an octree
/// denotes the number of voxels along each axis such
/// that the resolution = 2^mNoLevels.
// -------------------------------------------------------------------------

#ifndef DICOMOCTREE_HH
#define DICOMOCTREE_HH

#include "globals.hh"
#include "DicomOctreeNode.hh"
#include "DicomOctreeMiddleNode.hh"
#include "DicomOctreeTerminalNode.hh"

class DicomOctreeNode;
class DicomOctreeMiddleNode;
class DicomTerminalNode;
class DicomOctree;

class DicomOctree
{
public:
  
  DicomOctree( G4int noLevels, G4double size );
  ~DicomOctree();

  DicomOctreeNode* CreateNode( G4double i, G4double j, G4double k, G4int level );
  DicomOctreeNode* operator()( G4double nodeX, G4double nodeY, 
                          G4double nodeZ, G4int level );
  void DeleteTree(); 
  
  G4int CountMemory( G4int rMiddle, G4int rTerminal );
  
  DicomOctreeNode* Root()   { return mRoot; } 
  G4double Size()      { return mSize; }
  G4int NoLevels()     { return mNoLevels; }
  //G4bool Resolution()   { return ( 1 << mNoLevels ); }
  G4int GetIndexChild() { return indexChild;}
private:
  void CountRecursive( DicomOctreeNode* pNode, G4int rMiddle, G4int rTerminal );
  // Root node of the tree
  DicomOctreeMiddleNode* mRoot;
  
  // In an octree, size denotes physical size of the cube,
  // ie length of its sides (which are assumed equal).
  G4double mSize;
  
  // Number of levels denotes the maximal number of
  // nodes in a single branch, starting from the root node.
  G4int mNoLevels; 
  G4int indexChild;
};
#endif 

