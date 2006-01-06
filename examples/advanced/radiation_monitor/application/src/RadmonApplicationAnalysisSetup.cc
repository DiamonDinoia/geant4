//
// File name:     RadmonApplicationAnalysisSetup.cc
// Creation date: Sep 2005
// Main author:   Riccardo Capra <capra@ge.infn.it>
//
// Id:            $Id: RadmonApplicationAnalysisSetup.cc,v 1.4 2006-01-06 12:52:31 guatelli Exp $
// Tag:           $Name: not supported by cvs2svn $
//

// Include files
#include "RadmonApplicationAnalysisSetup.hh"
#include "RadmonApplicationOptions.hh"

#ifdef    G4ANALYSIS_USE
 #include "RadmonDataAnalysisDepositedEnergy.hh"

 #include "RadmonDataAnalysisWithLabelFactory.hh"


 #define DECLARE_ANALYSIS_CONSTRUCTOR(name)     constructor=new name();                                                                  \
                                                if (constructor==0)                                                                      \
                                                {                                                                                        \
                                                 G4cerr << currentOptions.ApplicationName() << ": Cannot allocate " #name "." << G4endl; \
                                                 return false;                                                                           \
                                                }                                                                                        \
                                                factory->AppendDataAnalysisWithLabel(constructor)

 G4bool RadmonApplicationAnalysisSetup :: CreateDataAnalysis(RadmonDataAnalysisWithLabelFactory * factory)
 {
  RadmonVDataAnalysisWithLabel * constructor;

  DECLARE_ANALYSIS_CONSTRUCTOR(RadmonDataAnalysisDepositedEnergy);

  return true;
 }
#else  /* G4ANALYSIS_USE */
 G4bool RadmonApplicationAnalysisSetup :: CreateDataAnalysis(RadmonDataAnalysisWithLabelFactory * /* factory */)
 {
  return false;
 }
#endif /* G4ANALYSIS_USE */
