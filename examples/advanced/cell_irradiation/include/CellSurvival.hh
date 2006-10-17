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
//    **************************************
//    *                                    *
//    *           CellSurvival.hh          *
//    *                                    *
//    **************************************
//
// Author: Susanna Guatelli (guatelli@ge.infn.it)
//	   Barbara Mascialino (Barbara.Mascialino@ge.infn.it)
//
//
// History:
// -----------
// 20 September 2006 S. Guatelli, B. Mascialino      first implementation
// -------------------------------------------------------------------


#ifndef CellSurvival_h
#define CellSurvival_h 1

#include "globals.hh"

class CellSurvival 
{
  public:
    CellSurvival();
   ~CellSurvival();

  public:
  void  SurvivalFormula(G4double dose);
  G4double GetSurvival();

private:
  G4double probability;
};
#endif
