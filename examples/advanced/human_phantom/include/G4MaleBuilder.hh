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
#ifndef G4MaleBuilder_h
#define G4MaleBuilder_h 1

//#include "G4VPhysicalVolume.hh"

#include "G4PhantomBuilder.hh"

class G4PhantomBuilder;
class G4MaleBuilder: public G4PhantomBuilder
{
public:
  G4MaleBuilder();
  ~G4MaleBuilder();

  void BuildMaleGenitalia(G4bool);
  void BuildTestes(G4bool);
  void BuildUterus(G4bool){;};
  void BuildRightOvary(G4bool){;}; 
  void BuildLeftOvary(G4bool){;};
};
#endif
