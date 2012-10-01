#------------------------------------------------------------------------------
# sources.cmake
# Module : G4hadronic_bert_cascade
# Package: Geant4.src.G4processes.G4hadronic.G4hadronic_models.G4had_hetcpp.G4hadronic_bert_cascade
#
# Sources description for a library.
# Lists the sources and headers of the code explicitely.
# Lists include paths needed.
# Lists the internal granular and global dependencies of the library.
# Source specific properties should be added at the end.
#
# Generated on : 30/9/2010
#
# $Id: sources.cmake,v 1.4 2010-09-30 12:02:28 bmorgan Exp $
#
# 20110725  M. Kelsey -- Update HEADERS and SOURCES with new code
# 20110919  M. Kelsey -- Add G4CascadeCoalescence
# 20110922  M. Kelsey -- Add G4ExitonConfiguration, G4FissionConfiguration
# 20110923  M. Kelsey -- Add G4CascadeChannel.cc
# 20111007  M. Kelsey -- Add new gamma-nucleon tables
# 20120822  M. Kelsey -- Add G4CascadeParameters
# 20120831  M. Kelsey -- Replace G4CascadeT1NNChannel with T1pp and T1nn
# 20120907  M. Kelsey -- Renamed *T1xxChannel.cc to *XXChannel.cc (PP,NN,NP)
# 20120912  M. Kelsey -- Add G4CascadeParamMessenger, dependencies
#------------------------------------------------------------------------------

# List external includes needed.
include_directories(${CLHEP_INCLUDE_DIRS})

# List internal includes needed.
include_directories(${CMAKE_SOURCE_DIR}/source/geometry/management/include)
include_directories(${CMAKE_SOURCE_DIR}/source/geometry/volumes/include)
include_directories(${CMAKE_SOURCE_DIR}/source/global/HEPGeometry/include)
include_directories(${CMAKE_SOURCE_DIR}/source/global/HEPNumerics/include)
include_directories(${CMAKE_SOURCE_DIR}/source/global/HEPRandom/include)
include_directories(${CMAKE_SOURCE_DIR}/source/global/management/include)
include_directories(${CMAKE_SOURCE_DIR}/source/intercoms/include)
include_directories(${CMAKE_SOURCE_DIR}/source/materials/include)
include_directories(${CMAKE_SOURCE_DIR}/source/particles/bosons/include)
include_directories(${CMAKE_SOURCE_DIR}/source/particles/hadrons/barions/include)
include_directories(${CMAKE_SOURCE_DIR}/source/particles/hadrons/ions/include)
include_directories(${CMAKE_SOURCE_DIR}/source/particles/hadrons/mesons/include)
include_directories(${CMAKE_SOURCE_DIR}/source/particles/leptons/include)
include_directories(${CMAKE_SOURCE_DIR}/source/particles/management/include)
include_directories(${CMAKE_SOURCE_DIR}/source/particles/shortlived/include)
include_directories(${CMAKE_SOURCE_DIR}/source/processes/hadronic/cross_sections/include)
include_directories(${CMAKE_SOURCE_DIR}/source/processes/hadronic/management/include)
include_directories(${CMAKE_SOURCE_DIR}/source/processes/hadronic/models/cascade/cascade/include)
include_directories(${CMAKE_SOURCE_DIR}/source/processes/hadronic/models/cascade/utils/include)
include_directories(${CMAKE_SOURCE_DIR}/source/processes/hadronic/models/de_excitation/evaporation/include)
include_directories(${CMAKE_SOURCE_DIR}/source/processes/hadronic/models/de_excitation/fermi_breakup/include)
include_directories(${CMAKE_SOURCE_DIR}/source/processes/hadronic/models/de_excitation/handler/include)
include_directories(${CMAKE_SOURCE_DIR}/source/processes/hadronic/models/de_excitation/management/include)
include_directories(${CMAKE_SOURCE_DIR}/source/processes/hadronic/models/de_excitation/multifragmentation/include)
include_directories(${CMAKE_SOURCE_DIR}/source/processes/hadronic/models/de_excitation/photon_evaporation/include)
include_directories(${CMAKE_SOURCE_DIR}/source/processes/hadronic/models/de_excitation/util/include)
include_directories(${CMAKE_SOURCE_DIR}/source/processes/hadronic/models/management/include)
include_directories(${CMAKE_SOURCE_DIR}/source/processes/hadronic/models/pre_equilibrium/exciton_model/include)
include_directories(${CMAKE_SOURCE_DIR}/source/processes/hadronic/models/util/include)
include_directories(${CMAKE_SOURCE_DIR}/source/processes/hadronic/processes/include)
include_directories(${CMAKE_SOURCE_DIR}/source/processes/hadronic/util/include)
include_directories(${CMAKE_SOURCE_DIR}/source/processes/management/include)
include_directories(${CMAKE_SOURCE_DIR}/source/track/include)

#
# Define the Geant4 Module.
#
include(Geant4MacroDefineModule)
GEANT4_DEFINE_MODULE(NAME G4hadronic_bert_cascade
    HEADERS
        G4Analyser.hh
        G4BertiniElasticCollision.hh
        G4BertiniHydrogenCollision.hh
        G4BigBanger.hh
        G4CascadeChannel.hh
        G4CascadeChannelTables.hh
        G4CascadeCheckBalance.hh
	G4CascadeCoalescence.hh
        G4CascadeColliderBase.hh
        G4CascadeData.hh
        G4CascadeData.icc
        G4CascadeDeexcitation.hh
        G4CascadeFunctions.hh
        G4CascadeFunctions.icc
        G4CascadeInterface.hh
        G4CascadeInterpolator.hh
        G4CascadeInterpolator.icc
        G4CascadeGamNChannel.hh
        G4CascadeGamPChannel.hh
        G4CascadeKminusNChannel.hh
        G4CascadeKminusPChannel.hh
        G4CascadeKplusNChannel.hh
        G4CascadeKplusPChannel.hh
        G4CascadeKzeroBarNChannel.hh
        G4CascadeKzeroBarPChannel.hh
        G4CascadeKzeroNChannel.hh
        G4CascadeKzeroPChannel.hh
        G4CascadeLambdaNChannel.hh
        G4CascadeLambdaPChannel.hh
        G4CascadeNNChannel.hh
        G4CascadeNPChannel.hh
	G4CascadeParameters.hh
	G4CascadeParamMessenger.hh
	G4CascadeParamMessenger.icc
        G4CascadePiMinusNChannel.hh
        G4CascadePiMinusPChannel.hh
        G4CascadePiPlusNChannel.hh
        G4CascadePiPlusPChannel.hh
        G4CascadePiZeroNChannel.hh
        G4CascadePiZeroPChannel.hh
        G4CascadePPChannel.hh
        G4CascadeRecoilMaker.hh
        G4CascadeSampler.hh
        G4CascadeSampler.icc
        G4CascadeSigmaMinusNChannel.hh
        G4CascadeSigmaMinusPChannel.hh
        G4CascadeSigmaPlusNChannel.hh
        G4CascadeSigmaPlusPChannel.hh
        G4CascadeSigmaZeroNChannel.hh
        G4CascadeSigmaZeroPChannel.hh
        G4CascadeXiMinusNChannel.hh
        G4CascadeXiMinusPChannel.hh
        G4CascadeXiZeroNChannel.hh
        G4CascadeXiZeroPChannel.hh
        G4CascadParticle.hh
        G4CollisionOutput.hh
        G4Dineutron.hh
        G4Diproton.hh
        G4ElementaryParticleCollider.hh
        G4EquilibriumEvaporator.hh
        G4EvaporationInuclCollider.hh
        G4ExitonConfiguration.hh
        G4FissionConfiguration.hh
        G4Fissioner.hh
        G4FissionStore.hh
        G4InteractionCase.hh
        G4IntraNucleiCascader.hh
        G4InuclCollider.hh
        G4InuclElementaryParticle.hh
        G4InuclEvaporation.hh
        G4InuclNuclei.hh
        G4InuclParticle.hh
        G4InuclParticleNames.hh
        G4InuclSpecialFunctions.hh
        G4KaonHypSampler.hh
        G4LorentzConvertor.hh
        G4NonEquilibriumEvaporator.hh
        G4NucleiModel.hh
        G4NuclWatcher.hh
        G4ParticleLargerBeta.hh
        G4ParticleLargerEkin.hh
        G4PionNucSampler.hh
        G4PreCompoundDeexcitation.hh
        G4RegionModel.hh
        G4UnboundPN.hh
        G4VCascadeCollider.hh
        G4VCascadeDeexcitation.hh
        G4WatcherGun.hh
    SOURCES
	G4CascadeCoalescence.cc
	G4CascadeParameters.cc
	G4CascadeParamMessenger.cc
        G4Analyser.cc
        G4BigBanger.cc
        G4CascadParticle.cc
        G4CascadeChannel.cc
        G4CascadeChannelTables.cc
        G4CascadeCheckBalance.cc
        G4CascadeColliderBase.cc
        G4CascadeDeexcitation.cc
        G4CascadeInterface.cc
        G4CascadeKminusNChannel.cc
        G4CascadeKminusPChannel.cc
        G4CascadeKplusNChannel.cc
        G4CascadeKplusPChannel.cc
        G4CascadeKzeroBarNChannel.cc
        G4CascadeKzeroBarPChannel.cc
        G4CascadeKzeroNChannel.cc
        G4CascadeKzeroPChannel.cc
        G4CascadeLambdaNChannel.cc
        G4CascadeLambdaPChannel.cc
        G4CascadeNNChannel.cc
        G4CascadeNPChannel.cc
        G4CascadePPChannel.cc
        G4CascadeRecoilMaker.cc
        G4CascadeSigmaMinusNChannel.cc
        G4CascadeSigmaMinusPChannel.cc
        G4CascadeSigmaPlusNChannel.cc
        G4CascadeSigmaPlusPChannel.cc
        G4CascadeSigmaZeroNChannel.cc
        G4CascadeSigmaZeroPChannel.cc
        G4CascadeT11pizNChannel.cc
        G4CascadeT1GamNChannel.cc
        G4CascadeT31piNChannel.cc
        G4CascadeT33piNChannel.cc
        G4CascadeXiMinusNChannel.cc
        G4CascadeXiMinusPChannel.cc
        G4CascadeXiZeroNChannel.cc
        G4CascadeXiZeroPChannel.cc
        G4CollisionOutput.cc
        G4Dineutron.cc
        G4Diproton.cc
        G4ElementaryParticleCollider.cc
        G4EquilibriumEvaporator.cc
        G4EvaporationInuclCollider.cc
        G4ExitonConfiguration.cc
        G4FissionConfiguration.cc
        G4FissionStore.cc
        G4Fissioner.cc
        G4InteractionCase.cc
        G4IntraNucleiCascader.cc
        G4InuclCollider.cc
        G4InuclElementaryParticle.cc
        G4InuclEvaporation.cc
        G4InuclNuclei.cc
        G4InuclParticle.cc
        G4InuclSpecialFunctions.cc
        G4KaonHypSampler.cc
        G4LorentzConvertor.cc
        G4NonEquilibriumEvaporator.cc
        G4NuclWatcher.cc
        G4NucleiModel.cc
        G4PionNucSampler.cc
        G4PreCompoundDeexcitation.cc
        G4RegionModel.cc
        G4UnboundPN.cc
        G4VCascadeCollider.cc
        G4WatcherGun.cc
        bindingEnergy.cc
        bindingEnergyAsymptotic.cc
        nucleiLevelDensity.cc
        paraMaker.cc
    GRANULAR_DEPENDENCIES
        G4baryons
        G4bosons
        G4geometrymng
        G4globman
        G4had_mod_man
        G4had_mod_util
        G4had_preequ_exciton
        G4hadronic_bert_cascade
        G4hadronic_deex_evaporation
        G4hadronic_deex_fermi_breakup
        G4hadronic_deex_handler
        G4hadronic_deex_management
        G4hadronic_deex_multifragmentation
        G4hadronic_deex_photon_evaporation
        G4hadronic_deex_util
        G4hadronic_hetcpp_utils
        G4hadronic_mgt
        G4hadronic_proc
        G4hadronic_util
        G4hadronic_xsect
        G4hepnumerics
	G4intercoms
        G4ions
        G4leptons
        G4materials
        G4mesons
        G4partman
        G4procman
        G4shortlived
        G4track
        G4volumes
    GLOBAL_DEPENDENCIES
        G4geometry
        G4global
	G4intercoms
        G4materials
        G4particles
        G4track
    LINK_LIBRARIES
)

# List any source specific properties here

