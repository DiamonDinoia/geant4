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
// INCL++ intra-nuclear cascade model
// Pekka Kaitaniemi, CEA and Helsinki Institute of Physics
// Davide Mancusi, CEA
// Alain Boudard, CEA
// Sylvie Leray, CEA
// Joseph Cugnon, University of Liege
//
// INCL++ revision: v5.1.3
//
#define INCLXX_IN_GEANT4_MODE 1

#include "globals.hh"

#include "G4INCLReflectionChannel.hh"
#include "G4INCLFinalState.hh"
#include "G4INCLRandom.hh"
#include "G4INCLINuclearPotential.hh"

#include <cmath>

namespace G4INCL {
  const G4double ReflectionChannel::sinMinReflectionAngleSquaredOverFour = std::pow(std::sin(2.*Math::pi/200.),2.);
  const G4double ReflectionChannel::positionScalingFactor = 0.99;

  ReflectionChannel::ReflectionChannel(Nucleus *n, Particle *p)
    :theNucleus(n),theParticle(p)
  {
  }

  ReflectionChannel::~ReflectionChannel()
  {
  }

  FinalState* ReflectionChannel::getFinalState()
  {
    FinalState *fs = new FinalState(); // Create final state for the output
    fs->setTotalEnergyBeforeInteraction(theParticle->getEnergy() - theParticle->getPotentialEnergy());

    const ThreeVector &oldMomentum = theParticle->getMomentum();
    G4double pspr = theParticle->getPosition().dot(oldMomentum);
    if(pspr>=0) { // This means that the particle is trying to leave; perform a reflection
      const G4double x2cour = theParticle->getPosition().mag2();
      const ThreeVector newMomentum = oldMomentum - (theParticle->getPosition() * (2.0 * pspr/x2cour));
      const G4double deltaP2 = (newMomentum-oldMomentum).mag2();
      theParticle->setMomentum(newMomentum);
      const G4double minDeltaP2 = sinMinReflectionAngleSquaredOverFour * newMomentum.mag2();
      if(deltaP2 < minDeltaP2) { // Avoid extremely small reflection angles
        theParticle->setPosition(theParticle->getPosition() * positionScalingFactor);
        DEBUG("Reflection angle for particle " << theParticle->getID() << " was too tangential: " << std::endl
            << "  " << deltaP2 << "=deltaP2<minDeltaP2=" << minDeltaP2 << std::endl
            << "  Resetting the particle position to ("
            << theParticle->getPosition().getX() << ", "
            << theParticle->getPosition().getY() << ", "
            << theParticle->getPosition().getZ() << ")" << std::endl);
      }
      theNucleus->updatePotentialEnergy(theParticle);
    } else { // The particle momentum is already directed towards the inside of the nucleus; do nothing
      // ...but make sure this only happened because of the frozen propagation
// assert(theParticle->getPosition().dot(theParticle->getPropagationVelocity())>0.);
    }

    theParticle->thawPropagation();
    fs->addModifiedParticle(theParticle);
    return fs;
  }
}

