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
// $Id: G4ProcessPlacer.hh,v 1.2 2002-04-09 17:40:14 gcosmo Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// ----------------------------------------------------------------------
// Class G4ProcessPlacer
//
// Class description:
//
// <<insert the description here>>

// Author: Michael Dressel (Michael.Dressel@cern.ch)
// ----------------------------------------------------------------------
#ifndef G4ProcessPlacer_hh
#define G4ProcessPlacer_hh

#include "globals.hh"
#include "G4VProcessPlacer.hh"

class G4ProcessManager;
class G4ProcessVector;

class G4ProcessPlacer : public G4VProcessPlacer
{

public:  // with description

  G4ProcessPlacer(const G4String &particlename);

  void AddProcessAsLastDoIt(G4VProcess *process);
  void AddProcessAsSecondDoIt(G4VProcess *process);

private:

  G4ProcessManager &GetProcessManager();

  enum SecondOrLast
  {
    eSecond = 1,            
    eLast = 0
  };

  void AddProcessAs(G4VProcess *process, SecondOrLast);
  void PrintProcVec(G4ProcessVector* processVec);  

private:

  G4String fParticleName;

};

#endif
