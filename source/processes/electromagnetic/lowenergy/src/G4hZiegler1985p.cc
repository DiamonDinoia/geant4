// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// -------------------------------------------------------------------
//
// GEANT4 Class file
//
// For information related to this code contact:
// Geant4 Collaboration
//
// File name:     G4hZiegler1985p
//
// Author:        V.Ivanchenko (Vladimir.Ivanchenko@cern.ch)
// 
// Creation date: 20 July 2000
//
// Modifications: 
// 20/07/2000  V.Ivanchenko First implementation
// 18/09/2000  V.Ivanchenko clean up - all variable are the same as in ICRU
//
// Class Description: 
//
// Electronic stopping power parametrised according to
// J.F.Ziegler, J.P.Biersack, U.Littmark The Stoping and
// Range of Ions in Solids, Vol.1, Pergamon Press, 1985
//
// Class Description: End 
//
// -------------------------------------------------------------------
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "G4hZiegler1985p.hh"
#include "G4UnitsTable.hh"
#include "globals.hh"
#include "G4Material.hh"

G4double G4hZiegler1985p::a[92][8] = {
   0.0091827, 0.0053496, 0.69741, 0.48493, 316.07 , 1.0143, 9329.3, 0.053989,
   0.11393,   0.0051984, 1.0822,  0.39252, 1081.0 , 1.0645, 4068.5, 0.017699,
   0.85837,   0.0050147, 1.6044,  0.38844, 1337.3 , 1.047,  2659.2, 0.01898,
   0.8781,    0.0051049, 5.4232,  0.2032 , 1200.6 , 1.0211, 1401.8, 0.038529,
   1.4608,    0.0048836, 2.338,   0.44249, 1801.3 , 1.0352, 1784.1, 0.02024,
   3.2579,    0.0049148, 2.7156,  0.36473, 2092.2 , 1.0291, 2643.6, 0.018237,
   0.59674,   0.0050837, 4.2073,  0.30612, 2394.2 , 1.0255, 4892.1, 0.016006,
   0.75253,   0.0050314, 4.0824,  0.30067, 2455.8 , 1.0181, 5069.7, 0.017426,
   1.226,     0.0051385, 3.2246,  0.32703, 2525.0 , 1.0142, 7563.6, 0.019469,
   1.0332,    0.0051645, 3.004,   0.33889, 2338.6 ,0.99997, 6991.2, 0.021799,

   6.0972,    0.0044292, 3.1929,  0.45763, 1363.3 , 0.95182, 2380.6, 0.081835,
   14.013,    0.0043646, 2.2641,  0.36326, 2187.4 , 0.99098, 6264.8, 0.0462,
   0.039001,  0.0045415, 5.5463,  0.39562, 1589.2 , 0.95316, 816.16, 0.047484,
   2.072,     0.0044516, 3.5585,  0.53933, 1515.2 , 0.93161, 1790.3, 0.035189,
   17.575,    0.0038346, 0.078694,1.2388,  2806.0 , 0.97284, 1037.6, 0.012879,
   16.126,    0.0038315, 0.054164,1.3104,  2813.3 , 0.96587, 1251.4, 0.011847,
   3.217,     0.0044579, 3.6696,  0.5091,  2734.6 , 0.96253, 2187.5, 0.016907,
   2.0379,    0.0044775, 3.0743,  0.54773, 3505.0 , 0.96575, 1714.0, 0.011701,
   0.74171,   0.0043051, 1.1515,  0.95083, 917.21 , 0.8782,  389.93, 0.18926,
   9.1316,    0.0043809, 5.4611,  0.31327, 3891.8 , 0.97933, 6267.9, 0.015196,

   7.2247,    0.0043718, 6.1017,  0.37511, 2829.2 , 0.95218, 6376.1, 0.020398,
   0.147,     0.0048456, 6.3485,  0.41057, 2164.1 , 0.94028, 5292.6, 0.050263,
   5.0611,    0.0039867, 2.6174,  0.57957, 2218.9 , 0.92361, 6323.0, 0.025669,
   0.53267,   0.0042968, 0.39005, 1.2725,  1872.7 , 0.90776, 64.166, 0.030107,
   0.47697,   0.0043038, 0.31452, 1.3289,  1920.5 , 0.90649, 45.576, 0.027469,
   0.027426,  0.0035443, 0.031563,2.1755,  1919.5 , 0.90099, 23.902, 0.025363,
   0.16383,   0.0043042, 0.073454,1.8592,  1918.4 , 0.89678, 27.61,  0.023184,
   4.2562,    0.0043737, 1.5606,  0.72067, 1546.8 , 0.87958, 302.02, 0.040944,
   2.3508,    0.0043237, 2.882,   0.50113, 1837.7 , 0.89992, 2377.0, 0.04965,
   3.1095,    0.0038455, 0.11477, 1.5037,  2184.7 , 0.89309, 67.306, 0.016588,

  15.322,     0.0040306, 0.65391, 0.67668, 3001.7 , 0.92484, 3344.2, 0.016366,
   3.6932,    0.0044813, 8.608,   0.27638, 2982.7 , 0.9276,  3166.6, 0.030874,
   7.1373,    0.0043134, 9.4247,  0.27937, 2725.8 , 0.91597, 3166.1, 0.025008,
   4.8979,    0.0042937, 3.7793,  0.50004, 2824.5 , 0.91028, 1282.4, 0.017061,
   1.3683,    0.0043024, 2.5679,  0.60822, 6907.8 , 0.9817,  628.01, 0.0068055,
   1.8301,    0.0042983, 2.9057,  0.6038,  4744.6 , 0.94722, 936.64, 0.0092242,
   0.42056,   0.0041169, 0.01695, 2.3616,  2252.7 , 0.89192, 39.752, 0.027757,
   30.78,     0.0037736, 0.55813, 0.76816, 7113.2 , 0.97697, 1604.4, 0.0065268,
   11.576,    0.0042119, 7.0244,  0.37764, 4713.5 , 0.94264, 2493.2, 0.01127,
   6.2406,    0.0041916, 5.2701,  0.49453, 4234.6 , 0.93232, 2063.9, 0.011844,

   0.33073,   0.0041243, 1.7246,  1.1062,  1930.2 , 0.86907, 27.416, 0.038208,
   0.017747,  0.0041715, 0.14586, 1.7305,  1803.6 , 0.86315, 29.669, 0.032123,
   3.7229,    0.0041768, 4.6286,  0.56769, 1678.0 , 0.86202, 3094.0, 0.06244,
   0.13998,   0.0041329, 0.25573, 1.4241,  1919.3 , 0.86326, 72.797, 0.032235,
   0.2859,    0.0041386, 0.31301, 1.3424,  1954.8 , 0.86175, 115.18, 0.029342,
   0.76002,   0.0042179, 3.386,   0.76285, 1867.4 , 0.85805, 69.994, 0.036448,
   6.3957,    0.0041935, 5.4689,  0.41378, 1712.6 , 0.85397, 18493., 0.056471,
   3.4717,    0.0041344, 3.2337,  0.63788, 1116.4 , 0.81959, 4766.0, 0.1179,
   2.5265,    0.0042282, 4.532,   0.53562, 1030.8 , 0.81652, 16252., 0.19722,
   7.3683,    0.0041007, 4.6791,  0.51428, 1160.0 , 0.82454, 17956., 0.13316,

   7.7197,    0.004388,  3.242,   0.68434, 1428.1 , 0.83389, 1786.7, 0.066512,
   16.78,     0.0041918, 9.3198,  0.29569, 3370.9 , 0.90298, 7431.7, 0.02616,
   4.2132,    0.0042098, 4.6753,  0.57945, 3503.9 , 0.89261, 1468.9, 0.014359,
   4.0818,    0.004214,  4.4425,  0.58393, 3945.3 , 0.90281, 1340.5, 0.013414,
   0.18517,   0.0036215,0.00058788,3.5315, 2931.3 , 0.88936,  26.18, 0.026393,
   4.8248,    0.0041458, 6.0934,  0.57026, 2300.1 , 0.86359, 2980.7, 0.038679,
   0.49857,   0.0041054, 1.9775,  0.95877, 786.55 , 0.78509,  806.6, 0.40882,
   3.2754,    0.0042177, 5.768,   0.54054, 6631.3 , 0.94282, 744.07, 0.0083026,
   2.9978,    0.0040901, 4.5299,  0.62025, 2161.2 , 0.85669, 1268.6, 0.043031,
   2.8701,    0.004096,  4.2568,  0.6138,  2130.4 , 0.85235, 1704.1, 0.039385,

   10.853,    0.0041149, 5.8906,  0.46834, 2857.2 , 0.87550, 3654.2, 0.029955,
   3.6407,    0.0041782, 4.8742,  0.57861, 1267.7 , 0.82211, 3508.2, 0.24174,
   17.645,    0.0040992, 6.5855,  0.32734, 3931.3 , 0.90754, 5156.7, 0.036278,
   7.5309,    0.0040814, 4.9389,  0.50679, 2519.7 , 0.85819, 3314.6, 0.030514,
   5.4742,    0.0040829, 4.897,   0.51113, 2340.1 , 0.85296, 2342.7, 0.035662,
   4.2661,    0.0040667, 4.5032,  0.55257, 2076.4 , 0.84151, 1666.6, 0.040801,
   6.8313,    0.0040486, 4.3987,  0.51675, 2003.0 , 0.83437, 1410.4, 0.03478,
   1.2707,    0.0040553, 4.6295,  0.57428, 1626.3 , 0.81858, 995.68, 0.055319,
   5.7561,    0.0040491, 4.357,   0.52496, 2207.3 , 0.83796, 1579.5, 0.027165,
   14.127,    0.0040596, 5.8304,  0.37755, 3645.9 , 0.87823, 3411.8, 0.016392,

   6.6948,    0.0040603, 4.9361,  0.47961, 2719.0 , 0.85249, 1885.8, 0.019713,
   3.0619,    0.0040511, 3.5803,  0.59082, 2346.1 , 0.83713, 1222.0, 0.020072,
   10.811,    0.0033008, 1.3767,  0.76512, 2003.7 , 0.82269, 1110.6, 0.024958,
   2.7101,    0.0040961, 1.2289,  0.98598, 1232.4 , 0.79066, 155.42, 0.047294,
   0.52345,   0.0040244, 1.4038,  0.8551,  1461.4 , 0.79677, 503.34, 0.036789,
   0.4616,    0.0040203, 1.3014,  0.87043, 1473.5 , 0.79687, 443.09, 0.036301,
   0.97814,   0.0040374, 2.0127,  0.7225,  1890.8 , 0.81747, 930.7,  0.02769,
   3.2086,    0.0040510, 3.6658,  0.53618, 3091.2 , 0.85602, 1508.1, 0.015401,
   2.0035,    0.0040431, 7.4882,  0.3561,  4464.3 , 0.88836, 3966.5, 0.012839,
   15.43,     0.0039432, 1.1237,  0.70703, 4595.7 , 0.88437, 1576.5, 0.0088534,

   3.1512,    0.0040524, 4.0996,  0.5425,  3246.3 , 0.85772, 1691.8, 0.015058,
   7.1896,    0.0040588, 8.6927,  0.35842, 4760.6 , 0.88833, 2888.3, 0.011029,
   9.3209,    0.0040540, 11.543,  0.32027, 4866.2 , 0.89124, 3213.4, 0.011935,
   29.242,    0.0036195, 0.16864, 1.1226,  5688.0 , 0.89812, 1033.3, 0.0071303,
   1.8522,    0.0039973, 3.1556,  0.65096, 3755.0 , 0.86383, 1602.0, 0.012042,
   3.222,     0.0040041, 5.9024,  0.52678, 4040.2 , 0.86804, 1658.4, 0.011747,
   9.3412,    0.0039661, 7.921,   0.42977, 5180.9 , 0.88773, 2173.2, 0.0092007,
   36.183,    0.0036003, 0.58341, 0.86747, 6990.2 , 0.91082, 1417.1, 0.0062187,
   5.9284,    0.0039695, 6.4082,  0.52122, 4619.5 , 0.88083, 2323.5, 0.011627,
   5.2454,    0.0039744, 6.7969,  0.48542, 4586.3 , 0.87794, 2481.5, 0.011282,

   33.702,    0.0036901, 0.47257, 0.89235, 5295.7 , 0.8893,  2053.3, 0.0091908,
   2.7589,    0.0039806, 3.2092,  0.66122, 2505.4 , 0.82863, 2065.1, 0.022816
  };

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4hZiegler1985p::G4hZiegler1985p():G4VhElectronicStoppingPower(), 
  protonMassAMU(1.007276)
{;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4hZiegler1985p::~G4hZiegler1985p() 
{;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4bool G4hZiegler1985p::HasMaterial(const G4Material* material) 
{
  if(1 == (material->GetNumberOfElements())) return true;
  return false ;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double G4hZiegler1985p::StoppingPower(const G4Material* material,
                                              G4double kineticEnergy) 
{
  G4double ionloss = 0.0 ;

  // pure material (normally not the case for this function)
  if(1 == (material->GetNumberOfElements())) {
    G4double z = material->GetZ() ;
    G4double ionloss = ElectronicStoppingPower( z, kineticEnergy ) ;  
  }
  
  return ionloss;
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double G4hZiegler1985p::ElectronicStoppingPower(G4double z,
                                                  G4double kineticEnergy) const
{
  G4double ionloss ;
  G4int i = G4int(z) - 1 ;  // index of atom
  if(i < 0)  i = 0 ;
  if(i > 91) i = 91 ;
  
  // The data and the fit from: 
  // J.F.Ziegler, J.P.Biersack, U.Littmark The Stoping and
  // Range of Ions in Solids, Vol.1, Pergamon Press, 1985
  // Proton kinetic energy for parametrisation in Ziegler's units (keV/amu)  
  
  G4double T = kineticEnergy/(keV*protonMassAMU) ; 
  
  
  
  G4double e = T ;
  if ( T < 25.0 ) e = 25.0 ;
  
  // universal approximation  
  G4double slow  = a[i][0] * pow(e, a[i][1]) + a[i][2] * pow(e, a[i][3])  ;
  G4double shigh = log( a[i][6]/e + a[i][7]*e ) * a[i][4] / pow(e, a[i][5]) ;
  ionloss = slow*shigh / (slow + shigh) ; 
    
  // low energy region
  if ( T < 25.0 ) {
    
    G4double  s = 0.45 ;
    // light elements
    if(6.5 > z) s = 0.25 ;
    // semiconductors
    if(5 == i || 13 == i || 31 == i) s = 0.375 ;
    
    ionloss *= pow(T/25.0, s) ;
  }
  
  if ( ionloss < 0.0) ionloss = 0.0 ;
  
  return ionloss;
}

