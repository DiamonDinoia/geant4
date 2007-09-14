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
// $Id: G4ScoringBox.cc,v 1.32 2007-09-14 13:57:36 asaim Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//

#include "G4ScoringBox.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4PVParameterised.hh"
#include "G4VisAttributes.hh"
#include "G4ScoringBoxParameterisation.hh"
#include "G4VVisManager.hh"

#include "G4MultiFunctionalDetector.hh"
#include "G4SDParticleFilter.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit3D.hh"

#include <map>

G4ScoringBox::G4ScoringBox(G4String wName)
  :G4VScoringMesh(wName), fSegmentDirection(-1),
   fMeshElementLogical(0)
{
  fShape = boxMesh;
}

G4ScoringBox::~G4ScoringBox()
{
}

void G4ScoringBox::Construct(G4VPhysicalVolume* fWorldPhys)
{
  if(fConstructed) {

    G4cerr << fWorldPhys->GetName() << G4endl;
//////////////////////////////////////    G4Exception(fWorldName+" has already been built.");
    ResetScore();

  } else {
    fConstructed = true;

    SetupGeometry(fWorldPhys);
  }
}



void G4ScoringBox::SetupGeometry(G4VPhysicalVolume * fWorldPhys) {

  if(verboseLevel > 10) G4cout << "G4ScoringBox::SetupGeometry() ..." << G4endl;

  // World
  G4VPhysicalVolume * scoringWorld = fWorldPhys;
  G4LogicalVolume * worldLogical = scoringWorld->GetLogicalVolume();

  // Scoring Mesh
  if(verboseLevel > 10) G4cout << fWorldName << G4endl;
  G4String boxName = fWorldName;

  if(verboseLevel > 10) G4cout << fSize[0] << ", " << fSize[1] << ", " << fSize[2] << G4endl;
  G4VSolid * boxSolid = new G4Box(boxName+"0", fSize[0], fSize[1], fSize[2]);
  G4LogicalVolume *  boxLogical = new G4LogicalVolume(boxSolid, 0, boxName);
  new G4PVPlacement(fRotationMatrix, fCenterPosition,
		    boxLogical, boxName+"0", worldLogical, false, 0);

  //G4double fsegment[3][3];
  //G4int segOrder[3];
  //GetSegmentOrder(fSegmentDirection, fNSegment, segOrder, fsegment);
  //EAxis axis[3] = {kXAxis, kYAxis, kZAxis};



  G4String layerName[2] = {boxName + "1",  boxName + "2"};
  G4VSolid * layerSolid[2]; 
  G4LogicalVolume * layerLogical[2];

  // fisrt nested layer (replicated to x direction)
  if(verboseLevel > 10) G4cout << "layer 1 :" << G4endl;
  layerSolid[0] = new G4Box(layerName[0],
			    fSize[0]/fNSegment[0],
			    fSize[1],
			    fSize[2]);
  layerLogical[0] = new G4LogicalVolume(layerSolid[0], 0, layerName[0]);
  if(fNSegment[0] > 1) {
    if(verboseLevel > 9) G4cout << "G4ScoringBox::Construct() : Replicate to x direction" << G4endl;
    new G4PVReplica(layerName[0], layerLogical[0], boxLogical, kXAxis,
		    fNSegment[0], fSize[0]/fNSegment[0]*2.);
  } else if(fNSegment[0] == 1) {
    if(verboseLevel > 9) G4cout << "G4ScoringBox::Construct() : Placement" << G4endl;
    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), layerLogical[0], layerName[0], boxLogical, false, 0);
  } else
    G4cerr << "G4ScoringBox::SetupGeometry() : invalid parameter ("
	   << fNSegment[0] << ") "
	   << "in placement of the first nested layer." << G4endl;

  if(verboseLevel > 10) {
    G4cout << fSize[0]/fNSegment[0] << ", "
	   << fSize[1] << ", "
	   << fSize[2] << G4endl;
    G4cout << layerName[0] << ": kXAxis, "
	   << fNSegment[0] << ", "
	   << 2.*fSize[0]/fNSegment[0] << G4endl;
  }

  // second nested layer (replicated to y direction)
  if(verboseLevel > 10) G4cout << "layer 2 :" << G4endl;
  layerSolid[1] = new G4Box(layerName[1],
			    fSize[0]/fNSegment[0],
			    fSize[1]/fNSegment[1],
			    fSize[2]);
  layerLogical[1] = new G4LogicalVolume(layerSolid[1], 0, layerName[1]);
  if(fNSegment[1] > 1) {
    if(verboseLevel > 9) G4cout << "G4ScoringBox::Construct() : Replicate to y direction" << G4endl;
    new G4PVReplica(layerName[1], layerLogical[1], layerLogical[0], kYAxis,
		    fNSegment[1], fSize[1]/fNSegment[1]*2.);
  } else if(fNSegment[1] == 1) {
    if(verboseLevel > 9) G4cout << "G4ScoringBox::Construct() : Placement" << G4endl;
    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), layerLogical[1], layerName[1], layerLogical[0], false, 0);
  } else
    G4cerr << "G4ScoringBox::SetupGeometry() : invalid parameter ("
	   << fNSegment[1] << ") "
	   << "in placement of the second nested layer." << G4endl;

  if(verboseLevel > 10) {
    G4cout << fSize[0]/fNSegment[0] << ", "
	   << fSize[1]/fNSegment[1] << ", "
	   << fSize[2] << G4endl;
    G4cout << layerName[1] << ": kYAxis, "
	   << fNSegment[1] << ", "
	   << 2.*fSize[1]/fNSegment[1] << G4endl;
  }

  // mesh elements (replicated to z direction)
  if(verboseLevel > 10) G4cout << "mesh elements :" << G4endl;
  G4String elementName = boxName +"3";
  G4VSolid * elementSolid = new G4Box(elementName,
				      fSize[0]/fNSegment[0],
				      fSize[1]/fNSegment[1],
				      fSize[2]/fNSegment[2]);
  fMeshElementLogical = new G4LogicalVolume(elementSolid, 0, elementName);
  if(fNSegment[2] > 1) 
    if(fSegmentPositions.size() > 0) {
      if(verboseLevel > 9) G4cout << "G4ScoringBox::Construct() : Parameterise to z direction" << G4endl;
      G4double motherDims[3] ={fSize[0]/fNSegment[0],
			       fSize[1]/fNSegment[1],
			       fSize[2]/fNSegment[2]};
      G4int nelement = fNSegment[2];
      fSegmentPositions.push_back(fSize[2]*2.);
      //G4ScoringBoxParameterisation * param =
      G4VPVParameterisation * param =
	new G4ScoringBoxParameterisation(kZAxis, motherDims, fSegmentPositions);
      new G4PVParameterised(elementName,
			    fMeshElementLogical,
			    layerLogical[1],
			    kZAxis,
			    nelement,
			    param);

      if(verboseLevel > 10) {
	G4cout << motherDims[0] << ", " << motherDims[1] << ", " << motherDims[2] << G4endl;
	for(int i = 0; i < (int)fSegmentPositions.size(); i++)
	  G4cout << fSegmentPositions[i] << ", ";
	G4cout << G4endl;
      }

    } else {
      if(verboseLevel > 9) G4cout << "G4ScoringBox::Construct() : Replicate to z direction" << G4endl;

      new G4PVReplica(elementName, fMeshElementLogical, layerLogical[1], kZAxis,
		      fNSegment[2], 2.*fSize[2]/fNSegment[2]);
    }
  else if(fNSegment[2] == 1) {
    if(verboseLevel > 9) G4cout << "G4ScoringBox::Construct() : Placement" << G4endl;
    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), fMeshElementLogical, elementName, layerLogical[1], false, 0);
  } else
    G4cerr << "G4ScoringBox::SetupGeometry() : "
	   << "invalid parameter (" << fNSegment[2] << ") "
	   << "in mesh element placement." << G4endl;

  if(verboseLevel > 10) {
    G4cout << fSize[0]/fNSegment[0] << ", "
	   << fSize[1]/fNSegment[1] << ", "
	   << fSize[2]/fNSegment[2] << G4endl;
    G4cout << elementName << ": kZAxis, "
	   << fNSegment[2] << ", "
	   << 2.*fSize[2]/fNSegment[2] << G4endl;
  }


  // set the sensitive detector
  fMeshElementLogical->SetSensitiveDetector(fMFD);
  

  // vis. attributes
  G4VisAttributes * visatt = new G4VisAttributes(G4Colour(.5,.5,.5));
  visatt->SetVisibility(false);
  layerLogical[0]->SetVisAttributes(visatt);
  layerLogical[1]->SetVisAttributes(visatt);
  visatt->SetVisibility(true);
  fMeshElementLogical->SetVisAttributes(visatt);
}


void G4ScoringBox::List() const {
  G4cout << "G4ScoringBox : " << fWorldName << " --- Shape: Box mesh" << G4endl;

  G4VScoringMesh::List();

//////  G4cout << "# of G4THitsMap : " << fMap.size() << G4endl;

//////  std::map<G4String, G4THitsMap<G4double>* >::const_iterator itr = fMap.begin();
//////  for(; itr != fMap.end(); itr++) {
//////    G4cout << "[" << itr->first << "]" << G4endl;
//////  }
}

void G4ScoringBox::Draw(std::map<G4int, G4double*> * map) {

  G4VVisManager * pVisManager = G4VVisManager::GetConcreteInstance();
  if(pVisManager) {
    
    // edep vectors
    std::vector<std::vector<std::vector<double> > > edep; // edep[X][Y][Z]
    std::vector<double> ez;
    for(int z = 0; z < fNSegment[2]; z++) ez.push_back(0.);
    std::vector<std::vector<double> > eyz;
    for(int y = 0; y < fNSegment[1]; y++) eyz.push_back(ez);
    for(int x = 0; x < fNSegment[0]; x++) edep.push_back(eyz);

    std::vector<std::vector<double> > xyedep; // xyedep[X][Y]
    std::vector<double> ey;
    for(int y = 0; y < fNSegment[1]; y++) ey.push_back(0.);
    for(int x = 0; x < fNSegment[0]; x++) xyedep.push_back(ey);

    std::vector<std::vector<double> > yzedep; // yzedep[Y][Z]
    for(int y = 0; y < fNSegment[1]; y++) yzedep.push_back(ez);

    std::vector<std::vector<double> > xzedep; // xzedep[X][Z]
    for(int x = 0; x < fNSegment[0]; x++) xzedep.push_back(ez);

    G4double xymax = 0., yzmax = 0., xzmax = 0.;
    G4int q[3];
    std::map<G4int, G4double*>::iterator itr = map->begin();
    for(; itr != map->end(); itr++) {
      GetXYZ(itr->first, q);

      xyedep[q[0]][q[1]] += *(itr->second);
      if(xymax < xyedep[q[0]][q[1]]) xymax = xyedep[q[0]][q[1]];

      yzedep[q[1]][q[2]] += *(itr->second);
      if(yzmax < yzedep[q[1]][q[2]]) yzmax = yzedep[q[1]][q[2]];

      xzedep[q[0]][q[2]] += *(itr->second);
      if(xzmax < xzedep[q[0]][q[2]]) xzmax = xzedep[q[0]][q[2]];
    }  
    
    G4VisAttributes att;
    att.SetForceSolid(true);
    att.SetForceAuxEdgeVisible(true);


    //G4VSolid * drawBox = new G4Box("drawBox", fSize[0], fSize[1], fSize[2]);
    //G4LogicalVolume * drawBoxLV = new G4LogicalVolume(drawBox, 0 ,"drawBoxLV");

    G4Scale3D scale;
    // xy plane
    G4ThreeVector zhalf(0., 0., fSize[2]/fNSegment[2]*0.98);
    G4Box xyplate("xy", fSize[0]/fNSegment[0], fSize[1]/fNSegment[1], fSize[2]/fNSegment[2]*0.01);
    for(int x = 0; x < fNSegment[0]; x++) {
      for(int y = 0; y < fNSegment[1]; y++) {
	//G4ThreeVector pos(GetReplicaPosition(x, y, 0) + fCenterPosition - zhalf);
        //G4ThreeVector pos2(GetReplicaPosition(x, y, fNSegment[2]-1) + fCenterPosition + zhalf);
	G4ThreeVector pos(GetReplicaPosition(x, y, 0) - zhalf);
        G4ThreeVector pos2(GetReplicaPosition(x, y, fNSegment[2]-1) + zhalf);
        G4Transform3D trans, trans2;
        if(fRotationMatrix) {
          trans = G4Rotate3D(*fRotationMatrix).inverse()*G4Translate3D(pos);
	  trans = G4Translate3D(fCenterPosition)*trans;
          trans2 = G4Rotate3D(*fRotationMatrix).inverse()*G4Translate3D(pos2);
	  trans2 = G4Translate3D(fCenterPosition)*trans2;
        } else {
          trans = G4Translate3D(pos)*G4Translate3D(fCenterPosition);
          trans2 = G4Translate3D(pos2)*G4Translate3D(fCenterPosition);
        }
	G4double c[4];
	GetMapColor(xyedep[x][y]/xymax, c);
	att.SetColour(c[0], c[1], c[2]);//, c[3]);
	pVisManager->Draw(xyplate, att, trans);
	pVisManager->Draw(xyplate, att, trans2);

      }
    }

    // yz plane
    G4ThreeVector xhalf(fSize[0]/fNSegment[0]*0.98, 0., 0.);
    G4Box yzplate("yz", fSize[0]/fNSegment[0]*0.01, fSize[1]/fNSegment[1], fSize[2]/fNSegment[2]);
    for(int y = 0; y < fNSegment[1]; y++) {
      for(int z = 0; z < fNSegment[2]; z++) {
        //G4ThreeVector pos(GetReplicaPosition(0, y, z) + fCenterPosition - xhalf);
        //G4ThreeVector pos2(GetReplicaPosition(fNSegment[0]-1, y, z) + fCenterPosition + xhalf);
        G4ThreeVector pos(GetReplicaPosition(0, y, z) - xhalf);
        G4ThreeVector pos2(GetReplicaPosition(fNSegment[0]-1, y, z) + xhalf);
        G4Transform3D trans, trans2;
        if(fRotationMatrix) {
          trans = G4Rotate3D(*fRotationMatrix).inverse()*G4Translate3D(pos);
	  trans = G4Translate3D(fCenterPosition)*trans;
          trans2 = G4Rotate3D(*fRotationMatrix).inverse()*G4Translate3D(pos2);
	  trans2 = G4Translate3D(fCenterPosition)*trans2;
        } else {
          trans = G4Translate3D(pos)*G4Translate3D(fCenterPosition);
          trans2 = G4Translate3D(pos2)*G4Translate3D(fCenterPosition);
        }
	G4double c[4];
	GetMapColor(yzedep[y][z]/yzmax, c);
	att.SetColour(c[0], c[1], c[2]);//, c[3]);
	pVisManager->Draw(yzplate, att, trans);
	pVisManager->Draw(yzplate, att, trans2);

      }
    }

    // xz plane
    G4ThreeVector yhalf(0., fSize[1]/fNSegment[1]*0.98, 0.);
    G4Box xzplate("xz", fSize[0]/fNSegment[0], fSize[1]/fNSegment[1]*0.01, fSize[2]/fNSegment[2]);
    for(int x = 0; x < fNSegment[0]; x++) {
      for(int z = 0; z < fNSegment[2]; z++) {
        //G4ThreeVector pos(GetReplicaPosition(x, 0, z) + fCenterPosition - yhalf);
        //G4ThreeVector pos2(GetReplicaPosition(x, fNSegment[1]-1, z) + fCenterPosition + yhalf);
        G4ThreeVector pos(GetReplicaPosition(x, 0, z) - yhalf);
        G4ThreeVector pos2(GetReplicaPosition(x, fNSegment[1]-1, z) + yhalf);
        G4Transform3D trans, trans2;
        if(fRotationMatrix) {
          trans = G4Rotate3D(*fRotationMatrix).inverse()*G4Translate3D(pos);
	  trans = G4Translate3D(fCenterPosition)*trans;
          trans2 = G4Rotate3D(*fRotationMatrix).inverse()*G4Translate3D(pos2);
	  trans2 = G4Translate3D(fCenterPosition)*trans2;
        } else {
          trans = G4Translate3D(pos)*G4Translate3D(fCenterPosition);
          trans2 = G4Translate3D(pos2)*G4Translate3D(fCenterPosition);
	}
	G4double c[4];
	GetMapColor(xzedep[x][z]/xzmax, c);
	att.SetColour(c[0], c[1], c[2]);//, c[3]);
	pVisManager->Draw(xzplate, att, trans);
	pVisManager->Draw(xzplate, att, trans2);

      }
    }
  }
}

G4ThreeVector G4ScoringBox::GetReplicaPosition(G4int x, G4int y, G4int z) {
  G4ThreeVector width(fSize[0]/fNSegment[0], fSize[1]/fNSegment[1], fSize[2]/fNSegment[2]);
  G4ThreeVector pos(-fSize[0] + 2*(x+0.5)*width.x(),
		    -fSize[1] + 2*(y+0.5)*width.y(),
		    -fSize[2] + 2*(z+0.5)*width.z());

  return pos;
}

void G4ScoringBox::GetXYZ(G4int index, G4int q[3]) const {

  q[0] = index/(fNSegment[2]*fNSegment[1]);
  q[1] = (index - q[0]*fNSegment[2]*fNSegment[1])/fNSegment[2];
  q[2] = index - q[1]*fNSegment[2] - q[0]*fNSegment[2]*fNSegment[1];

  //G4cout << "GetXYZ: " << index << ": "
  //<< q[0] << ", " << q[1] << ", " << q[2] << G4endl;
}

void G4ScoringBox::GetMapColor(G4double value, G4double color[4]) {


  if(value > 1.) {
    G4cerr << "Error : G4ScoringBox::GetMapColor() : 'value' argument needs to be <= 1." << G4endl;
    color[0] = 1.;
    color[1] = color[2] = color[3] = 0.;
    return;
  }

  // color map
  const int NCOLOR = 6;
  struct ColorMap {
    G4double val;
    G4double rgb[4];
  } colormap[NCOLOR] = {{0.0, 1., 1., 1., 1.}, // value, r, g, b, alpha
			{0.2, 0., 0., 1., 1.},
			{0.4, 0., 1., 1., 1.},
			{0.6, 0., 1., 0., 1.},
			{0.8, 1., 1., 0., 1.},
			{1.0, 1., 0., 0., 1.}};
  
  // search
  G4int during[2] = {0, 0};
  for(int i = 1; i < NCOLOR; i++) {
    if(colormap[i].val >= value) {
      during[0] = i-1;
      during[1] = i;
      break;
    }
  }

  // interpolate
  G4double a = std::fabs(value - colormap[during[0]].val);
  G4double b = std::fabs(value - colormap[during[1]].val);
  for(int i = 0; i < 4; i++) {
    color[i] = (b*colormap[during[0]].rgb[i] + a*colormap[during[1]].rgb[i])
      /(colormap[during[1]].val - colormap[during[0]].val);
  } 

}
