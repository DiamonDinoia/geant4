// This code implementation is the intellectual property of
// the RD44 GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: Tst09PrimaryGeneratorAction.hh,v 1.1 1999-01-08 16:35:24 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//

#ifndef Tst09PrimaryGeneratorAction_h
#define Tst09PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"

class G4ParticleGun;
class G4Event;

class Tst09PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    Tst09PrimaryGeneratorAction();
    ~Tst09PrimaryGeneratorAction();

  public:
    void GeneratePrimaries(G4Event* anEvent);

  private:
    G4ParticleGun* particleGun;
};

#endif


