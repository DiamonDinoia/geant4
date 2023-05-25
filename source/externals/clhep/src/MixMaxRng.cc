//
// -*- C++ -*-
//
// -----------------------------------------------------------------------
//                          HEP Random
//                       --- MixMaxRng ---
//                     class implementation file
// -----------------------------------------------------------------------
//
// This file interfaces the MixMax PseudoRandom Number Generator
// proposed by:
//
// G.K.Savvidy and N.G.Ter-Arutyunian,
//   On the Monte Carlo simulation of physical systems,
//   J.Comput.Phys. 97, 566 (1991);
//   Preprint EPI-865-16-86, Yerevan, Jan. 1986
//   http://dx.doi.org/10.1016/0021-9991(91)90015-D
//
// K.Savvidy
//   "The MIXMAX random number generator"
//   Comp. Phys. Commun. (2015)
//   http://dx.doi.org/10.1016/j.cpc.2015.06.003
//
// K.Savvidy and G.Savvidy
//   "Spectrum and Entropy of C-systems. MIXMAX random number generator"
//   Chaos, Solitons & Fractals, Volume 91, (2016) pp. 33-38
//   http://dx.doi.org/10.1016/j.chaos.2016.05.003
//


#include "CLHEP/Random/MixMaxRng.h"
#include "CLHEP/Random/engineIDulong.h"
#include "CLHEP/Utility/atomic_int.h"

#include <atomic>
#include <cmath>
#include <iostream>
#include <string.h>        // for strcmp
#include <vector>

const unsigned long MASK32=0xffffffff;

namespace CLHEP {

namespace {
  // Number of instances with automatic seed selection
  CLHEP_ATOMIC_INT_TYPE numberOfEngines(0);
}

    MixMaxRng::MixMaxRng() : MixMaxEngine(++numberOfEngines){}

    MixMaxRng::MixMaxRng(uint64_t seed) : MixMaxEngine(seed) {}

    MixMaxRng::MixMaxRng(uint32_t clusterId, uint32_t machineId, uint32_t runId, uint32_t streamId) : MixMaxEngine(
            clusterId, machineId, runId, streamId) {}

    MixMaxRng::MixMaxRng(uint64_t seed, uint64_t stream) : MixMaxEngine(seed, stream) {}

    MixMaxRng::MixMaxRng(const uint64_t *state, uint64_t sumOverNew, uint32_t counter) : MixMaxEngine(state, sumOverNew,
                                                                                                      counter) {}

    MixMaxRng::MixMaxRng(const MIXMAX::MixMaxRng17 &other) : MixMaxEngine(other) {}

    MixMaxRng::MixMaxRng(MIXMAX::MixMaxRng17 &&other) : MixMaxEngine(other) {}

    double MixMaxRng::flat() {
        return Uniform();
    }

    void MixMaxRng::flatArray(const int size, double *vect) {
        for (int i = 0; i < size; ++i) {
            vect[i] = Uniform();
        }
    }

    void MixMaxRng::setSeed(long seed, int) {
        seedLCG(seed);
    }

    void MixMaxRng::setSeeds(const long *seeds, int i) {
        if(i <= 0) {
            throw std::invalid_argument("MixMaxRng::setSeeds: i must be > 0");
        }

        std::uint32_t seed0 = seeds[0] & MASK32, seed1 = 0, seed2 = 0, seed3 = 0;

        if (i > 1) {
            seed1 = seeds[1] & MASK32;
        }
        if (i > 2) {
            seed2 = seeds[2] & MASK32;
        }
        if (i > 3) {
            seed3 = seeds[3] & MASK32;
        }
        applyBigSkip(seed3, seed2, seed1, seed0);
    }

    void MixMaxRng::saveStatus(const char *filename) const {
        std::ofstream outFile(filename, std::ios::out);
        if (!outFile.good()) {
            throw std::runtime_error("MixMaxRng::saveStatus(): cannot open file " + std::string(filename));
        }
        outFile << "MixMaxRng " << std::endl;
        outFile << static_cast<const MIXMAX::MixMaxRng17*>(this) << std::endl;
    }

    void MixMaxRng::restoreStatus(const char *filename) {
        std::ifstream inFile(filename, std::ios::in);
        if (!inFile.good()) {
            throw std::runtime_error("MixMaxRng::restoreStatus(): cannot open file " + std::string(filename));
            return;
        }
        std::string line;
        std::getline(inFile, line);
        if (line != "MixMaxRng ") {
            throw std::runtime_error("MixMaxRng::restoreStatus(): wrong engine type " + line);
        }
        std::getline(inFile, line);
        std::istringstream iss(line);
        iss >> *static_cast<MIXMAX::MixMaxRng17*>(this);
    }

    void MixMaxRng::showStatus() const {
        std::cout << "----- MixMaxRng engine status -----" << std::endl;
        std::cout << static_cast<const MIXMAX::MixMaxRng17*>(this) << std::endl;
        std::cout << "------------------------------------" << std::endl;
    }

    std::string MixMaxRng::name() const {
        return "MixMaxRng";
    }

    std::string MixMaxRng::engineName() {
        return "MixMaxRng";
    }

}  // namespace CLHEP
