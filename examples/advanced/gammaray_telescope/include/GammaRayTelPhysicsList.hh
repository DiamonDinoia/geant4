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
#ifndef GammaRayTelPhysicsList_h
#define GammaRayTelPhysicsList_h 1

#include "G4VModularPhysicsList.hh"
#include "globals.hh"

class GammaRayTelPhysicsList: public G4VModularPhysicsList
{
public:
  GammaRayTelPhysicsList();
  virtual ~GammaRayTelPhysicsList();
  
public:
  // SetCuts() 
  virtual void SetCuts();


};


#endif



