#
# --- Setup the Geant4 kernel environment ---
. ~/ExtraSpace/geant4/env.sh-3.23-nodeb
#
# --- Setup your Geant4 application environment ---
unset G4TMP
unset G4BIN
export G4WORKDIR=~/ExtraSpace/StatAccepTest
export PATH=$PATH:$G4WORKDIR/bin/$G4SYSTEM
export G4ANALYSIS_USE=1
#
# --- Aida / PI ---
export PI_DIR=/afs/cern.ch/sw/lcg/app/releases/PI/PI_1_2_3/rh73_gcc323
export PATH=$PI_DIR/bin:$PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PI_DIR/lib
eval `aida-config --runtime sh`
#
#***LOOKHERE*** TEMPORARY LORENZO FIX: CLOUD CACHE SETS TO 10,000 .
export LD_LIBRARY_PATH=~/ExtraSpace/myPiDir/lib:$LD_LIBRARY_PATH
#export LD_LIBRARY_PATH=~/ExtraSpace/myPiDir/lib_dbg:$LD_LIBRARY_PATH
#
# --- GSL : this is needed only for  dirStat/pvalue.cpp ---
export GSL_DIR=/afs/cern.ch/sw/lcg/external/GSL/1.4/rh73_gcc323
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$GSL_DIR/lib
#

