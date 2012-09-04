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

/** \file G4INCLNDFHardSphere.hh
 * \brief NDF* class for the deuteron density according to the HardSphere
 *        potential.
 *
 * Created on: 16 July 2012
 *     Author: Davide Mancusi
 */

#ifndef G4INCLNDFHARDSPHERE_HH_
#define G4INCLNDFHARDSPHERE_HH_

#include "G4INCLIFunction1D.hh"
#include "G4INCLParticleTable.hh"
#include "G4INCLDeuteronDensity.hh"

namespace G4INCL {

  namespace NuclearDensityFunctions {

    class HardSphere : public IFunction1D {
      public:
        HardSphere(const G4double xMax) :
          // We let the function go up to rMax. This can be the Fermi momentum
          // for a sphere in momentum space, or the surface radius for a
          // hard-sphere nucleus.
          IFunction1D(0., xMax),
          normalisation(3./std::pow(xMax,3.))
      {};

        inline G4double operator()(const G4double x) const {
          return x*x*normalisation;
        }
      protected:
        const G4double normalisation;
    };

  }

}

#endif // G4INCLNDFHARDSPHERE_HH_

