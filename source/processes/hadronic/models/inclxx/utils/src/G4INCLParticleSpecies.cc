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

/*
 * G4INCLParticleSpecies.cc
 *
 *  Created on: Nov 25, 2011
 *      Author: Davide Mancusi
 */

#include "G4INCLParticleSpecies.hh"
#include "G4INCLParticleTable.hh"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace G4INCL {

  ParticleSpecies::ParticleSpecies(std::string const &pS) {
    // Normalise the string to lower case
    std::string pSNorm = pS;
    std::transform(pSNorm.begin(), pSNorm.end(), pSNorm.begin(), ::tolower);
    if(pSNorm=="p" || pSNorm=="proton") {
      theA = 1;
      theZ = 1;
      theType = G4INCL::Proton;
    } else if(pSNorm=="n" || pSNorm=="neutron") {
      theA = 1;
      theZ = 0;
      theType = G4INCL::Neutron;
    } else if(pSNorm=="delta++" || pSNorm=="deltaplusplus") {
      theA = 1;
      theZ = 2;
      theType = G4INCL::DeltaPlusPlus;
    } else if(pSNorm=="delta+" || pSNorm=="deltaplus") {
      theA = 1;
      theZ = 1;
      theType = G4INCL::DeltaPlus;
    } else if(pSNorm=="delta0" || pSNorm=="deltazero") {
      theA = 1;
      theZ = 0;
      theType = G4INCL::DeltaZero;
    } else if(pSNorm=="delta-" || pSNorm=="deltaminus") {
      theA = 1;
      theZ = -1;
      theType = G4INCL::DeltaMinus;
    } else if(pSNorm=="pi+" || pSNorm=="pion+" || pSNorm=="piplus" || pSNorm=="pionplus") {
      theA = 0;
      theZ = 1;
      theType = G4INCL::PiPlus;
    } else if(pSNorm=="pi0" || pSNorm=="pion0" || pSNorm=="pizero" || pSNorm=="pionzero") {
      theA = 0;
      theZ = 0;
      theType = G4INCL::PiZero;
    } else if(pSNorm=="pi-" || pSNorm=="pion-" || pSNorm=="piminus" || pSNorm=="pionminus") {
      theA = 0;
      theZ = -1;
      theType = G4INCL::PiMinus;
    } else if(pSNorm=="d" || pSNorm=="deuteron") {
      theA = 2;
      theZ = 1;
      theType = G4INCL::Composite;
    } else if(pSNorm=="t" || pSNorm=="triton") {
      theA = 3;
      theZ = 1;
      theType = G4INCL::Composite;
    } else if(pSNorm=="a" || pSNorm=="alpha") {
      theA = 4;
      theZ = 2;
      theType = G4INCL::Composite;
    } else
      parseNuclide(pSNorm);
  }

  ParticleSpecies::ParticleSpecies(ParticleType const t) :
    theType(t),
    theA(ParticleTable::getMassNumber(theType)),
    theZ(ParticleTable::getChargeNumber(theType))
  {}

  void ParticleSpecies::parseNuclide(std::string const &pS) {
    theType = Composite;

    // Allowed characters
    const std::string separators("-_");
    std::string allowed("0123456789abcdefghijklmnopqrstuvwxyz");
    allowed += separators;

    // There must be at least two characters
    if(pS.find_first_not_of(allowed)!=std::string::npos) {
      // Malformed input string
      // Setting unknown particle species
      (*this) = ParticleSpecies(UnknownParticle);
      return;
    }

    // There must be at least two characters
    if(pS.size()<2) {
      // Malformed input string
      // Setting unknown particle species
      (*this) = ParticleSpecies(UnknownParticle);
      return;
    }

    std::size_t firstSeparator = pS.find_first_of(separators);
    std::size_t lastSeparator = pS.find_last_of(separators);
    if(firstSeparator!=std::string::npos && firstSeparator!=lastSeparator) {
      // Several separators in malformed input string
      // Setting unknown particle species
      (*this) = ParticleSpecies(UnknownParticle);
      return;
    }

    // Identify the type of the first character
    G4int (*predicate)(G4int);
    G4bool startsWithAlpha = std::isalpha(pS.at(0));
    if(startsWithAlpha) {
      predicate=std::isalpha;
    } else if(std::isdigit(pS.at(0))) {
      predicate=std::isdigit;
    } else {
      // Non-alphanumeric character in string
      // Setting unknown particle species
      (*this) = ParticleSpecies(UnknownParticle);
      return;
    }

    size_t endFirstSection, beginSecondSection;
    if(firstSeparator==std::string::npos) {
      // No separator, Fe56 or 56Fe style
      // Identify the end of the first section

      // Find the first character that is not of the same type as the first one
      for(beginSecondSection=1; beginSecondSection<pS.size(); ++beginSecondSection) {
        if(!predicate(pS.at(beginSecondSection)))
          break;
      }

      if(beginSecondSection>=pS.size()) {
        // Only one type of characters (numeric or alphabetic) in the string
        // Setting unknown particle species
        (*this) = ParticleSpecies(UnknownParticle);
        return;
      }

      endFirstSection = beginSecondSection;

    } else {
      // One separator, Fe-56 or 56-Fe style
      endFirstSection = firstSeparator;
      beginSecondSection = firstSeparator+1;
    }

    std::string firstSection(pS.substr(0,endFirstSection));
    std::string secondSection(pS.substr(beginSecondSection,std::string::npos));
    std::stringstream parsingStream;

    // Parse the sections
    G4bool success;
    if(startsWithAlpha) {
      parsingStream.str(secondSection);
      success = parseElement(firstSection);
    } else {
      parsingStream.str(firstSection);
      success = parseElement(secondSection);
    }
    if(!success) {
      // Couldn't parse the element section
      // Setting unknown particle species
      (*this) = ParticleSpecies(UnknownParticle);
      return;
    }

    parsingStream >> theA;
    if(parsingStream.fail()) {
      // Couldn't parse the mass section
      // Setting unknown particle species
      (*this) = ParticleSpecies(UnknownParticle);
      return;
    }

    // Check that Z<=A
    if(theZ>theA) {
      // Setting unknown particle species
      (*this) = ParticleSpecies(UnknownParticle);
      return;
    }

    // Special particle type for protons
    if(theZ==1 && theA==1)
      theType = Proton;
  }

  G4bool ParticleSpecies::parseElement(std::string const &s) {
    for(theZ=1; theZ<ParticleTable::elementTableSize; ++theZ) {
      std::string elementName = ParticleTable::getElementName(theZ);
      // Normalize the element name
      std::transform(elementName.begin(), elementName.end(), elementName.begin(), ::tolower);
      if(s.compare(elementName)==0)
        return true;
    }
    return parseIUPACElement(s);
  }

  G4bool ParticleSpecies::parseIUPACElement(std::string const &s) {
    theZ = ParticleTable::parseIUPACElement(s);
    if(theZ==0)
      return false;
    else
      return true;
  }
}

