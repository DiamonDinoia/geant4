//
// -*- C++ -*-
//
// -----------------------------------------------------------------------
//                          HEP Random
//                       --- MixMaxRng ---
//                       class header file
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

#ifndef MixMaxRng_h
#define MixMaxRng_h 1

#include <array>
#include <cstdint>
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/mixmax.h"

namespace CLHEP {

class MixMaxRng : public HepRandomEngine, private MIXMAX::MixMaxRng17 {
public:
    MixMaxRng();

    explicit MixMaxRng(uint64_t seed);
    explicit MixMaxRng(uint64_t seed, uint64_t stream);
    MixMaxRng(uint32_t clusterId, uint32_t machineId, uint32_t runId, uint32_t streamId);
    MixMaxRng(const uint64_t *state, uint64_t sumOverNew, uint32_t counter);
    MixMaxRng(const MixMaxRng &rhs) = default;
    MixMaxRng(MixMaxRng &&rhs) = default;
    explicit MixMaxRng(const MIXMAX::MixMaxRng17 &other);
    explicit MixMaxRng(MIXMAX::MixMaxRng17 &&other);

    MixMaxRng &operator=(const MixMaxRng &rhs) = default;
    MixMaxRng &operator=(MixMaxRng &&rhs) = default;

    double flat() override;
    // Returns a pseudo random number between 0 and 1
    // (excluding the zero: in (0,1] )
    // the smallest number which it will give is approximately 10^-19

    void flatArray(const int size, double *vect) override;
    // Fills the array "vect" of specified size with flat random values.

    void setSeed(long seed, int i) override;
    // Sets the state of the algorithm according to seed.

    void setSeeds(const long *seeds, int i) override;
    //  Preferred Seeding method
    //  the values of 'Seeds' must be valid 32-bit integers
    //  Higher order bits will be ignored!!

    void saveStatus(const char *filename) const override;
    // Saves on a file the current engine status.

    void restoreStatus(const char *filename) override;
    // Reads from a file and restores the last saved engine status.

    void showStatus() const override;
    // Dumps the current engine status on the screen.

    std::string name() const override;
    // Returns the engine name.

    static std::string engineName() ;

};

}  // namespace CLHEP

#endif
