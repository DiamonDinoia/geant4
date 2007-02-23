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
// Authors: S. Guatelli and M. G. Pia, INFN Genova, Italy
// 
// Based on code developed by the undergraduate student G. Guerrieri 
// Note: this is a preliminary beta-version of the code; an improved 
// version will be distributed in the next Geant4 public release, compliant
// with the design in a forthcoming publication, and subject to a 
// design and code review.
//

#ifndef G4BasePhantomBuilder_h
#define G4BasePhantomBuilder_h 1

#include "G4VPhysicalVolume.hh"
class G4VPhysicalVolume;
class G4BasePhantomBuilder
{
public:

  G4BasePhantomBuilder();
  ~G4BasePhantomBuilder();
 
  virtual void BuildHead(const G4String&,G4bool,G4bool) {return ;};
  virtual void BuildTrunk(const G4String&,G4bool,G4bool) {return ;};
  virtual void BuildLegs(const G4String&,G4bool,G4bool) {return ;};
  virtual void BuildNeck(const G4String&,G4bool,G4bool) {return ;};

  virtual void BuildUpperSpine(const G4String&,G4bool,G4bool) {return ;}
  virtual void BuildMiddleLowerSpine(const G4String&,G4bool,G4bool) {return ;};
  virtual void BuildLeftLeg(const G4String&,G4bool,G4bool) {return ;};
  virtual void BuildRightLeg(const G4String&,G4bool,G4bool) {return ;};
  virtual void BuildLeftLegBone(const G4String&,G4bool,G4bool) {return ;};
  virtual void BuildRightLegBone(const G4String&,G4bool,G4bool) {return ;};
  virtual void BuildLeftArmBone(const G4String&,G4bool,G4bool) {return ;}
  virtual void BuildRightArmBone(const G4String&,G4bool,G4bool) {return ;}
  virtual void BuildSkull(const G4String&,G4bool,G4bool) {return ;};
  virtual void BuildRibCage(const G4String&,G4bool,G4bool) {return ;};
  virtual void BuildPelvis(const G4String&,G4bool,G4bool) {return ;};
 

 //virtual void BuildScapulae(G4bool,G4bool) = 0;
  //virtual void BuildClavicles(G4bool,G4bool) = 0;

  virtual void BuildBrain(const G4String&,G4bool,G4bool) {return ;};
  virtual void BuildHeart(const G4String&,G4bool,G4bool) {return ;};
  virtual void BuildLeftLung(const G4String&,G4bool,G4bool) {return ;};
  virtual void BuildRightLung(const G4String&,G4bool,G4bool) {return ;};
  virtual void BuildStomach(const G4String&,G4bool,G4bool) {return ;};
  virtual void BuildUpperLargeIntestine(const G4String&,G4bool,G4bool) {return ;};
  virtual void BuildLowerLargeIntestine(const G4String&,G4bool,G4bool) {return ;};
  // virtual void BuildEsophagus(G4bool,G4bool) = 0;

  virtual void BuildLeftKidney(const G4String&,G4bool,G4bool) {return ;};
  virtual void BuildRightKidney(const G4String&,G4bool,G4bool) {return ;};
  // virtual void BuildAdrenal(G4bool,G4bool) = 0;
  virtual void BuildLiver(const G4String&,G4bool,G4bool) {return ;};
  virtual void BuildPancreas(const G4String&,G4bool,G4bool) {return ;};
  virtual void BuildSpleen(const G4String&,G4bool,G4bool) {return ;};
  virtual void BuildUrinaryBladder(const G4String& ,G4bool,G4bool) {return ;};
  virtual void BuildThyroid(const G4String&,G4bool,G4bool) {return ;};

  virtual void SetModel(G4String) {return ;};
  virtual void SetMotherVolume(G4VPhysicalVolume*) {return;};
  virtual G4VPhysicalVolume* GetPhantom() {return 0;};

  virtual void BuildLeftOvary(const G4String&,G4bool,G4bool ) {return ;};
  virtual void BuildRightOvary(const G4String&,G4bool,G4bool) {return ;};
  virtual void BuildUterus(const G4String&,G4bool,G4bool){return;};
  virtual void BuildLeftBreast(const G4String&,G4bool,G4bool){return;};
  virtual void BuildRightBreast(const G4String&,G4bool,G4bool){return;};
  virtual void BuildVoxelLeftBreast(const G4String&,G4bool,G4bool){return;};
  virtual void BuildVoxelRightBreast(const G4String&,G4bool,G4bool){return;};
};
#endif
