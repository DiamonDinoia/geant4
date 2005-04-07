#include "StatAccepTestAnalysis.hh"
#include <string>
#include "G4RunManager.hh" 

#include <AIDA/AIDA.h>

//***TEMPORARY WORK-AROUND*** : need  AIDA_Dev/  subdirectory at the
//                              same level as  AIDA/  in PI/PI_1_3_0/include .
//                              But it does not work! 
// // //#include <AIDA_Dev/IDevHistogram1D.h>


StatAccepTestAnalysis* StatAccepTestAnalysis::instance = 0;

 
StatAccepTestAnalysis::StatAccepTestAnalysis() : 
  analysisFactory( 0 ), tree( 0 ), tuple( 0 ), histoFactory( 0 ), 
  numberOfReplicas( 0 ), numberOfRadiusBins( 0 ), numberOfEvents( 0 ),
  radiusBin( 0.0 ), 
  longitudinalProfileHisto( 0 ), transverseProfileHisto( 0 ) {

  analysisFactory = AIDA_createAnalysisFactory();
  if ( analysisFactory ) {    
    AIDA::ITreeFactory* treeFactory = analysisFactory->createTreeFactory();
    if ( treeFactory ) {
      // Trees in memory: create a "tree" associated to an hbook file,
      // which will be filled with an ntuple, and several other trees
      // associated to an XML file, for data sets.
      bool readOnly = false;  // we want to write.
      bool createNew = true ; // create file if it doesn't exist.
      tree = treeFactory->create( "ntuple.hbook", "hbook", readOnly, createNew );
      if ( tree ) {
	// Get a tuple factory :
	AIDA::ITupleFactory* tupleFactory = analysisFactory->createTupleFactory( *tree );
	if ( tupleFactory ) {
	  // Create a tuple :
	  std::string description = "float ID, E, EDEP_ACT, EDEP_CAL; "; 
          description += "int nLayers, nBinR; ";
          description += "Tuple{ float L} L; ";
          description += "Tuple{ float R} R"; 
	  std::string option = "nLayers[0,100] L(nLayers) nBinR[0,30] R(nBinR)";
	  tuple = tupleFactory->create("1","Event info", description, option);
	  assert( tuple );
	  delete tupleFactory;
	}
        // Create a factory for histograms :
        histoFactory = analysisFactory->createHistogramFactory( *tree );  
      }
      delete treeFactory; // It will not delete the ITree.
    }
  }

}


StatAccepTestAnalysis::~StatAccepTestAnalysis() {}


void StatAccepTestAnalysis::close() {
  if ( tree ) {
    tree->commit();
    tree->close();
  }
  if ( tree ) {
    delete tree;
    tree = 0;
  }
  if ( analysisFactory ) {
    delete analysisFactory;
    analysisFactory = 0;
  }
  if ( histoFactory ) {
    delete histoFactory;
    histoFactory = 0;
  }
}


StatAccepTestAnalysis* StatAccepTestAnalysis::getInstance() {
  if ( instance == 0 ) instance = new StatAccepTestAnalysis();
  return instance;
}


void StatAccepTestAnalysis::init() {
  G4cout << " StatAccepTestAnalysis::init() : Cleaning up..." << G4endl;

  // We need to reset the content of the tuple and the
  // profile containers at the beginning of a new Run,
  // because they can be incompatible between two different
  // Runs. Notice that we never execute jobs with more than
  // one Run to really get data, so in practice it is not
  // a problem to loose the data of the previous runs.
  // In practice, jobs with more runs are used only sometimes
  // to check that there is no crashes and the parameters are
  // properly set, but not when you are really interested in
  // the result of the simulation, i.e. the content of the
  // ntuple in the file ntuple.hbook.

  if ( tuple ) tuple->reset();
  longitudinalProfile.clear();
  sumEdepAct  = 0.0;
  sumEdepAct2 = 0.0;
  sumEdepTot  = 0.0;
  sumEdepTot2 = 0.0;
  sumL.clear();
  sumL2.clear();
  for ( int layer = 0; layer < numberOfReplicas; layer++ ) {
    longitudinalProfile.push_back( 0.0 );
    sumL.push_back( 0.0 );
    sumL2.push_back( 0.0 );
  }
  transverseProfile.clear();
  sumR.clear();
  sumR2.clear();
  for ( int ir = 0; ir < numberOfRadiusBins; ir++ ) {
    transverseProfile.push_back( 0.0 );
    sumR.push_back( 0.0 );
    sumR2.push_back( 0.0 );
  } 
  numberOfEvents = 0;
}                       


void StatAccepTestAnalysis::init( const G4int numberOfReplicasIn, 
				  const G4int numberOfRadiusBinsIn,
				  const G4double radiusBinIn ) {
  if ( numberOfReplicasIn > 0 )    numberOfReplicas   = numberOfReplicasIn;
  if ( numberOfRadiusBinsIn > 0 )  numberOfRadiusBins = numberOfRadiusBinsIn;
  if ( radiusBinIn > 0 )           radiusBin = radiusBinIn;
  G4cout << " StatAccepTestAnalysis::init( , , , ) : DEBUG Info " << G4endl
         << "\t numberOfReplicas   = " << numberOfReplicas << G4endl
         << "\t numberOfRadiusBins = " << numberOfRadiusBins << G4endl
         << "\t radiusBin          = " << radiusBin/mm << " mm" 
	 << G4endl;  //***DEBUG***

  // Create two histograms: one for the longitudinal shower profile,
  // and one for the transverse shower profile.
  assert( histoFactory );
  if ( histoFactory ) {
      // // // if ( ! tree->find( "50" ) ) {
      longitudinalProfileHisto = 
	histoFactory->createHistogram1D("50", "Longitudinal shower profile", 
					numberOfReplicas, 0.0, 1.0*numberOfReplicas );
      assert( longitudinalProfileHisto );
      // G4cout << " Created longitudinalProfileHisto " << G4endl;
      // // // if ( ! tree->find( "60" ) ) {
      transverseProfileHisto = 
	histoFactory->createHistogram1D("60", "Transverse shower profile", 
					numberOfRadiusBins, 0.0, 1.0*numberOfRadiusBins );
      assert( transverseProfileHisto );
      // G4cout << " Created transverseProfileHisto " << G4endl;
  }
  
}                       


void StatAccepTestAnalysis::finish() {

  // Notice that the errors that are calculated here are not the
  // precisely correct ones, which would need also the pure weights
  // besides the values  x * weight . Only the latter are used 
  // below to calculate approximate errors, using the standard
  // formulas, as if the values  x * weight  were pure values
  // without a weight.

  // For the summary histograms for the longitudinal and transverse 
  // shower shapes (longitudinalProfileHisto, transverseProfileHisto
  // respectively), for the time being, in AIDA is not yet possible
  // to set the errors, so the errors are automatically set to the 
  // square root of the y-value of that bin (which is what is called 
  // "mu" below, rather than "mu_sigma"), which does not make any
  // sense! So, ignore eventual failures in the statistical tests
  // only for these two variables.

  // Print results. 
  G4double n = static_cast< G4double >( numberOfEvents );
  // G4cout << " n=" << n << G4endl;                             //***DEBUG***
  double sum, sum2, mu, sigma, mu_sigma;
  sum  = sumEdepAct;
  sum2 = sumEdepAct2;
  mu       = sum / n;
  sigma    = sqrt( ( sum2 - sum*sum/n ) / (n - 1.0) );
  mu_sigma = sigma / sqrt( n );
  G4cout << " Average <E> [MeV] deposited in all active layers = " 
         << mu << " +/- " << mu_sigma << G4endl;
  sum  = sumEdepTot;
  sum2 = sumEdepTot2;
  mu       = sum / n;
  sigma    = sqrt( ( sum2 - sum*sum/n ) / (n - 1.0) );
  mu_sigma = sigma / sqrt( n );
  G4cout << " Average <E> [MeV] deposited in the whole calorimeter = " 
         << mu << " +/- " << mu_sigma << G4endl;
  G4cout << " Average <E> [MeV] in each Layer " << G4endl; 
  for ( int iLayer = 0; iLayer < numberOfReplicas; iLayer++ ) {
    sum  = sumL[ iLayer ];
    sum2 = sumL2[ iLayer ];
    mu       = sum / n;
    sigma    = sqrt( ( sum2 - sum*sum/n ) / (n - 1.0) );
    mu_sigma = sigma / sqrt( n );
    if ( mu > 1.0E-06 ) {
      G4cout << "\t layer = " << iLayer << "\t <E> = " 
	     << mu << " +/- " << mu_sigma << G4endl;
    }
    if ( longitudinalProfileHisto ) {
      //***LOOKHERE*** :  mu_sigma  should be set as error.
      longitudinalProfileHisto->fill( 1.0*iLayer, mu );
    }
  }
  // // // std::vector<double> rmsTransverseProfile;   //***TEMPORARY WORK-AROUND***
  G4cout << " Average <E> [MeV] in each Radius bin " << G4endl; 
  for ( int iBinR = 0; iBinR < numberOfRadiusBins; iBinR++ ) {
    double sum  = sumR[ iBinR ];
    double sum2 = sumR2[ iBinR ];
    mu       = sum / n;
    sigma    = sqrt( ( sum2 - sum*sum/n ) / (n - 1.0) );
    mu_sigma = sigma / sqrt( n );
    if ( mu > 1.0E-06 ) {
      G4cout << "\t iBinR = " << iBinR << "\t <E> = " 
	     << mu << " +/- " << mu_sigma << G4endl;
    }
    if ( transverseProfileHisto ) {
      //***LOOKHERE*** :  mu_sigma  should be set as error.
      transverseProfileHisto->fill( 1.0*iBinR, mu );
    }
    // // // rmsTransverseProfile.push_back( mu_sigma );   //***TEMPORARY WORK-AROUND***
  }  

  // ***TEMPORARY WORK-AROUND*** in order to set properly the error bars
  // of a weighted histogram, until this possibility will be added 
  // in AIDA::IHistogram1D . But it does not work!
  // // // AIDA::Dev::IDevHistogram1D * devTransverseProfileHisto = 
  // // //   dynamic_cast< AIDA::Dev::IDevHistogram1D * > ( transverseProfileHisto ); 
  // // // assert( devTransverseProfileHisto ); 
  // // // for (int iBinR = 0; iBinR < devTransverseProfileHisto->axis().bins() ; iBinR++ ) { 
  // // //   devTransverseProfileHisto->setBinContents( iBinR, 
  // // // 					       transverseProfileHisto->binEntries( iBinR ),
  // // // 					       transverseProfileHisto->binHeight( iBinR ),
  // // // 					       rmsTransverseProfile[ iBinR ], 
  // // // 					       transverseProfileHisto->binMean( iBinR ) );
  // // // }

  if ( tree ) tree->commit();
}


void StatAccepTestAnalysis::fillNtuple( float incidentParticleId, 
					float incidentParticleEnergy, 
					float totalEnergyDepositedInActiveLayers,
					float totalEnergyDepositedInCalorimeter ) {
  if (tuple) {
    // G4cout << " StatAccepTestAnalysis::fillNtuple : DEBUG Info " << G4endl
    //        << "\t incidentParticleId = " << incidentParticleId << G4endl
    //        << "\t incidentParticleEnergy = " << incidentParticleEnergy << G4endl
    //        << "\t totalEnergyDepositedInActiveLayers = " 
    //        << totalEnergyDepositedInActiveLayers << G4endl
    //        << "\t totalEnergyDepositedInCalorimeter = " 
    //        << totalEnergyDepositedInCalorimeter << G4endl;       // ***DEBUG***

    tuple->fill( tuple->findColumn( "ID" ), incidentParticleId );
    tuple->fill( tuple->findColumn( "E" ), incidentParticleEnergy );
    tuple->fill( tuple->findColumn( "EDEP_ACT" ), totalEnergyDepositedInActiveLayers );
    tuple->fill( tuple->findColumn( "EDEP_CAL" ), totalEnergyDepositedInCalorimeter );
    tuple->fill( tuple->findColumn( "nLayers" ), numberOfReplicas );
    tuple->fill( tuple->findColumn( "nBinR" ), numberOfRadiusBins );
    sumEdepAct  += totalEnergyDepositedInActiveLayers;
    sumEdepAct2 += 
      totalEnergyDepositedInActiveLayers * totalEnergyDepositedInActiveLayers;
    sumEdepTot  += totalEnergyDepositedInCalorimeter;
    sumEdepTot2 += 
      totalEnergyDepositedInCalorimeter * totalEnergyDepositedInCalorimeter;
    AIDA::ITuple* tpL = tuple->getTuple( tuple->findColumn( "L" ) );
    AIDA::ITuple* tpR = tuple->getTuple( tuple->findColumn( "R" ) );
    for ( int iLayer = 0; iLayer < numberOfReplicas; iLayer++ ) {
      tpL->fill( 0, longitudinalProfile[ iLayer ] );
      tpL->addRow();
      sumL[ iLayer ]  += longitudinalProfile[ iLayer ];
      sumL2[ iLayer ] += longitudinalProfile[ iLayer ] * longitudinalProfile[ iLayer ];  
    }
    for ( int iBinR = 0; iBinR < numberOfRadiusBins; iBinR++ ) {
      tpR->fill( 0, transverseProfile[ iBinR ] );
      tpR->addRow();
      sumR[ iBinR ]  += transverseProfile[ iBinR ];
      sumR2[ iBinR ] += transverseProfile[ iBinR ] * transverseProfile[ iBinR ];  
    }
    tuple->addRow();
  }

  // Reset the longitudinal and transverse profiles, for the next event.
  for ( int layer = 0; layer < numberOfReplicas; layer++ ) {
    // G4cout << " StatAccepTestAnalysis::fillNtuple : DEBUG Info " << G4endl
    //        << "\t Longitudinal profile: layer = " << layer
    //        << "   energy = " << longitudinalProfile[ layer ] / MeV 
    //        << " MeV " << G4endl;                                 //***DEBUG***
    longitudinalProfile[ layer ] = 0.0;
  }
  for ( int ir = 0; ir < numberOfRadiusBins; ir++ ) {
    // G4cout << " StatAccepTestAnalysis::fillNtuple : DEBUG Info " << G4endl
    //        << "\t Transverse profile: iBinRadius = " << ir / mm
    //        << " mm   energy = " << transverseProfile[ ir ] / MeV 
    //        << " MeV " << G4endl;                                 //***DEBUG***
    transverseProfile[ ir ] = 0.0;
  }

  // This method is called at each event, so it is useful to commit
  // the tree from time to time, for instance every 10 events, in
  // such a way that it is possible to see the ntuple  ntuple.hbook
  // while the job is running, for instance for a quick check that
  // it makes sense. Or, if the job crashes after many events, we
  // have anyhow some data already stored in the ntuple to be checked.
  // Remember that when you look the ntuple with PAW, while is running,
  // you need to close the session and open a new one to see the updates.
  numberOfEvents++;
  if ( numberOfEvents % 1000 == 0 ) {
    if ( tree ) {
      tree->commit();
      // G4cout << " tree commit ,  at event=" << numberOfEvents-1 
      //       << G4endl; //***DEBUG***
    }
  }
}


void StatAccepTestAnalysis::
fillShowerProfile( G4int replica, G4double radius, G4double edep ) {

  if ( replica >= numberOfReplicas ) {
    G4cout << " StatAccepTestAnalysis::fillShowerProfile : ***ERROR*** " << G4endl
           << "\t replica=" << replica 
	   << "  >=  numberOfReplicas=" << numberOfReplicas << G4endl; 
    replica = numberOfReplicas - 1;  // Just to avoid a crash
  }

  longitudinalProfile[ replica ] += edep;

  // The last bin of the transverse profile includes all the hits with 
  // remaining radius. 
  // The bins are not constants: the specified radius refers to the first
  // (narrow) bin, then the second one has a width which is double the first
  // one, then the third has a width which is three time the first time,
  // and so on. The reason for this is to keep a reasonable statistics
  // in each bin, given the fast decrease as the radius increases.
  int iBinRadius = 0;
  double currentRadius = radiusBin;
  while ( radius > currentRadius  &&  iBinRadius < numberOfRadiusBins-1 ) {
    iBinRadius++;
    currentRadius += iBinRadius*radiusBin;
  }

  transverseProfile[ iBinRadius ] += edep;

  //G4cout << " StatAccepTestAnalysis::fillShowerProfile : DEBUG Info " << G4endl
  //       << " \t replica = " << replica  << "   radius = " << radius / mm 
  //       << " mm   iBinRadius = " << iBinRadius << "   edep = " << edep 
  //       << G4endl;  //***DEBUG***

}
