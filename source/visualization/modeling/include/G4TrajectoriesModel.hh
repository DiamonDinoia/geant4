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
//
// $Id: G4TrajectoriesModel.hh,v 1.8 2002-11-11 18:39:28 johna Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 
// John Allison  26th August 1998.
//
// Class Description:
//
// Model which knows how to draw GEANT4 trajectories.
//
// For access to base class information, e.g., modeling parameters,
// use GetModelingParameters() inherited from G4VModel.  See Class
// Description of the base class G4VModel.

#ifndef G4TRAJECTORIESMODEL_HH
#define G4TRAJECTORIESMODEL_HH

#include "G4VModel.hh"

class G4TrajectoriesModel: public G4VModel {

public: // With description

  G4TrajectoriesModel (G4int drawingMode = 0);
   
  virtual ~G4TrajectoriesModel ();

  virtual void DescribeYourselfTo (G4VGraphicsScene&);
  // The main task of a model is to describe itself to the graphics scene.

  virtual G4String GetCurrentDescription () const;
  // A description which depends on the current state of the model.

  virtual G4String GetCurrentTag () const;
  // A tag which depends on the current state of the model.

  G4int GetDrawingMode() const { return fDrawingMode;}
  void SetDrawingMode(G4int drawingMode) {fDrawingMode = drawingMode;}

private:

  G4int fDrawingMode;

};

#include "G4TrajectoriesModel.icc"

#endif
