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
// $Id: G4WaterStopping.cc,v 1.5 2008-08-04 08:49:42 vnivanch Exp $
// GEANT4 tag $Name: not supported by cvs2svn $

//---------------------------------------------------------------------------
//
// GEANT4 Class file
//
// Description: Data on stopping power
//
// Author:      V.Ivanchenko 12.05.2006
//
// Modifications:
//
//----------------------------------------------------------------------------
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4WaterStopping.hh"
#include "G4EmCorrections.hh"
#include "G4LPhysicsFreeVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4WaterStopping::G4WaterStopping(G4EmCorrections* corr)
{
  Initialise(corr);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4WaterStopping::~G4WaterStopping()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double G4WaterStopping::GetElectronicDEDX(G4int iz, G4double energy)
{
  G4double res = 0.0;
  G4bool b;
  for(G4int idx = 0; idx<16; idx++) {
    if(iz == Z[idx]) {
      res = (dedx[idx])->GetValue(energy, b);
      break;
    }
  }
  return res;
 }

void G4WaterStopping::Initialise(G4EmCorrections* corr)
{
  G4int i;
  G4LPhysicsFreeVector* pv = 0;
  //..List of ions
  G4int zz[16] = {3, 4, 5, 6, 7, 8, 9, 10,11,12,13,14,15,16,17,18};
  G4int aa[16] = {7, 9, 11, 12, 14, 16, 19, 20, 23, 24, 27, 28,31,32, 35,40};

  for(i=0; i<16; i++) {
    Z[i] = zz[i];
    A[i] = aa[i];
  }
  //..Reduced energies
  G4double E[53] = {0.025,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.1,0.15,0.2,0.25,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1,1.5,2,2.5,3,4,5,6,7,8,9,10,15,20,25,30,40,50,60,70,80,90,100,150,200,250,300,400,500,600,700,800,900,1000};
  for(i=0; i<53; i++) {E[i] *= MeV;}

  G4double factor = 1000.*MeV/cm;

G4double G4_WATER_Li [53]={2.626, 2.84, 3.191, 3.461, 3.665, 3.817, 3.927, 4.004, 4.056, 4.102, 3.998, 3.853, 3.702, 3.413, 3.158, 2.934, 2.739, 2.567, 2.415, 2.28, 1.782, 1.465, 1.247, 1.087, 0.8706, 0.7299, 0.631, 0.5575, 0.5005, 0.455, 0.4178, 0.3004, 0.2376, 0.1981, 0.1708, 0.1354, 0.1132, 0.09803, 0.08692, 0.07842, 0.07171, 0.06627, 0.0495, 0.04085, 0.03557, 0.03203, 0.0276, 0.02498, 0.02327, 0.0221, 0.02126, 0.02064, 0.02016, };
pv = new G4LPhysicsFreeVector(53,E[0],E[52]);
pv->SetSpline(true);
dedx.push_back(pv);
for(i=0; i<53; i++) { pv->PutValues(i,E[i],G4_WATER_Li[i]*factor); }

G4double G4_WATER_Be [53]={3.272, 3.565, 4.061, 4.463, 4.79, 5.052, 5.258, 5.419, 5.542, 5.803, 5.787, 5.675, 5.529, 5.215, 4.912, 4.634, 4.381, 4.152, 3.944, 3.756, 3.026, 2.533, 2.179, 1.913, 1.542, 1.296, 1.122, 0.9911, 0.8898, 0.8087, 0.7423, 0.5335, 0.4219, 0.3518, 0.3034, 0.2406, 0.2013, 0.1743, 0.1545, 0.1394, 0.1275, 0.1178, 0.08805, 0.07266, 0.06328, 0.05698, 0.0491, 0.04444, 0.04141, 0.03933, 0.03783, 0.03672, 0.03588, };
pv = new G4LPhysicsFreeVector(53,E[0],E[52]);
pv->SetSpline(true);
dedx.push_back(pv);
for(i=0; i<53; i++) { pv->PutValues(i,E[i],G4_WATER_Be[i]*factor); }

G4double G4_WATER_B [53]={3.773, 4.142, 4.776, 5.304, 5.749, 6.122, 6.431, 6.684, 6.89, 7.432, 7.551, 7.505, 7.391, 7.091, 6.772, 6.463, 6.172, 5.901, 5.65, 5.418, 4.484, 3.817, 3.322, 2.94, 2.392, 2.02, 1.752, 1.549, 1.391, 1.265, 1.161, 0.8332, 0.6587, 0.5492, 0.4737, 0.3757, 0.3144, 0.2723, 0.2415, 0.2179, 0.1993, 0.1842, 0.1376, 0.1136, 0.09894, 0.08909, 0.07678, 0.0695, 0.06477, 0.06151, 0.05916, 0.05743, 0.05611, };
pv = new G4LPhysicsFreeVector(53,E[0],E[52]);
pv->SetSpline(true);
dedx.push_back(pv);
for(i=0; i<53; i++) { pv->PutValues(i,E[i],G4_WATER_B[i]*factor); }

G4double G4_WATER_C [53]={4.154, 4.593, 5.358, 6.009, 6.568, 7.049, 7.46, 7.809, 8.103, 8.968, 9.262, 9.311, 9.25, 8.994, 8.68, 8.358, 8.045, 7.747, 7.465, 7.199, 6.093, 5.269, 4.636, 4.137, 3.403, 2.891, 2.516, 2.23, 2.004, 1.823, 1.673, 1.2, 0.9483, 0.7904, 0.6817, 0.5406, 0.4525, 0.392, 0.3477, 0.3138, 0.287, 0.2653, 0.1983, 0.1637, 0.1426, 0.1284, 0.1107, 0.1002, 0.09335, 0.08865, 0.08528, 0.08278, 0.08088, };
pv = new G4LPhysicsFreeVector(53,E[0],E[52]);
pv->SetSpline(true);
dedx.push_back(pv);
for(i=0; i<53; i++) { pv->PutValues(i,E[i],G4_WATER_C[i]*factor); }

G4double G4_WATER_N [53]={4.49, 4.984, 5.86, 6.616, 7.276, 7.854, 8.36, 8.799, 9.179, 10.39, 10.89, 11.07, 11.08, 10.9, 10.61, 10.3, 9.974, 9.66, 9.357, 9.068, 7.823, 6.859, 6.097, 5.484, 4.56, 3.9, 3.408, 3.029, 2.727, 2.482, 2.28, 1.636, 1.291, 1.076, 0.9274, 0.7354, 0.6156, 0.5333, 0.4731, 0.427, 0.3906, 0.3611, 0.27, 0.2229, 0.1942, 0.1749, 0.1507, 0.1365, 0.1272, 0.1208, 0.1162, 0.1128, 0.1102, };
pv = new G4LPhysicsFreeVector(53,E[0],E[52]);
pv->SetSpline(true);
dedx.push_back(pv);
for(i=0; i<53; i++) { pv->PutValues(i,E[i],G4_WATER_N[i]*factor); }

G4double G4_WATER_O [53]={4.778, 5.321, 6.298, 7.152, 7.907, 8.578, 9.173, 9.7, 10.16, 11.73, 12.46, 12.78, 12.89, 12.81, 12.57, 12.27, 11.95, 11.63, 11.32, 11.01, 9.659, 8.571, 7.691, 6.967, 5.854, 5.042, 4.427, 3.945, 3.56, 3.245, 2.983, 2.142, 1.689, 1.406, 1.212, 0.9602, 0.8037, 0.6963, 0.6178, 0.5577, 0.5102, 0.4716, 0.3527, 0.2913, 0.2538, 0.2285, 0.197, 0.1784, 0.1663, 0.1579, 0.1519, 0.1475, 0.1441, };
pv = new G4LPhysicsFreeVector(53,E[0],E[52]);
pv->SetSpline(true);
dedx.push_back(pv);
for(i=0; i<53; i++) { pv->PutValues(i,E[i],G4_WATER_O[i]*factor); }

G4double G4_WATER_F [53]={4.992, 5.575, 6.637, 7.578, 8.418, 9.171, 9.847, 10.45, 11, 12.9, 13.88, 14.35, 14.56, 14.59, 14.4, 14.13, 13.83, 13.51, 13.19, 12.87, 11.44, 10.26, 9.279, 8.463, 7.187, 6.237, 5.506, 4.928, 4.461, 4.076, 3.753, 2.707, 2.137, 1.779, 1.533, 1.215, 1.017, 0.8809, 0.7816, 0.7056, 0.6456, 0.5969, 0.4466, 0.3688, 0.3213, 0.2894, 0.2496, 0.2259, 0.2106, 0.2, 0.1924, 0.1868, 0.1825, };
pv = new G4LPhysicsFreeVector(53,E[0],E[52]);
pv->SetSpline(true);
dedx.push_back(pv);
for(i=0; i<53; i++) { pv->PutValues(i,E[i],G4_WATER_F[i]*factor); }

G4double G4_WATER_Ne [53]={5.182, 5.797, 6.931, 7.948, 8.865, 9.693, 10.44, 11.12, 11.74, 13.98, 15.21, 15.85, 16.17, 16.33, 16.21, 15.98, 15.69, 15.38, 15.06, 14.74, 13.24, 11.98, 10.91, 10.01, 8.584, 7.503, 6.66, 5.986, 5.436, 4.979, 4.595, 3.332, 2.635, 2.195, 1.892, 1.499, 1.255, 1.087, 0.9646, 0.8709, 0.7969, 0.7368, 0.5514, 0.4555, 0.3969, 0.3576, 0.3083, 0.2792, 0.2602, 0.2472, 0.2378, 0.2308, 0.2255, };
pv = new G4LPhysicsFreeVector(53,E[0],E[52]);
pv->SetSpline(true);
dedx.push_back(pv);
for(i=0; i<53; i++) { pv->PutValues(i,E[i],G4_WATER_Ne[i]*factor); }

G4double G4_WATER_Na [53]={5.352, 5.998, 7.203, 8.3, 9.298, 10.21, 11.04, 11.81, 12.5, 15.13, 16.68, 17.56, 18.04, 18.43, 18.44, 18.29, 18.05, 17.78, 17.48, 17.18, 15.67, 14.32, 13.15, 12.14, 10.5, 9.226, 8.218, 7.401, 6.728, 6.166, 5.69, 4.112, 3.237, 2.686, 2.307, 1.821, 1.521, 1.317, 1.168, 1.054, 0.9644, 0.8917, 0.6674, 0.5514, 0.4806, 0.4329, 0.3734, 0.3381, 0.3152, 0.2993, 0.288, 0.2796, 0.2732, };
pv = new G4LPhysicsFreeVector(53,E[0],E[52]);
pv->SetSpline(true);
dedx.push_back(pv);
for(i=0; i<53; i++) { pv->PutValues(i,E[i],G4_WATER_Na[i]*factor); }

G4double G4_WATER_Mg [53]={5.542, 6.193, 7.42, 8.551, 9.59, 10.54, 11.42, 12.23, 12.98, 15.85, 17.62, 18.66, 19.26, 19.76, 19.83, 19.7, 19.47, 19.2, 18.89, 18.58, 17.02, 15.62, 14.41, 13.36, 11.64, 10.3, 9.233, 8.362, 7.64, 7.033, 6.516, 4.777, 3.792, 3.162, 2.725, 2.159, 1.806, 1.565, 1.388, 1.254, 1.147, 1.061, 0.7944, 0.6565, 0.5722, 0.5156, 0.4447, 0.4027, 0.3754, 0.3566, 0.343, 0.333, 0.3254, };
pv = new G4LPhysicsFreeVector(53,E[0],E[52]);
pv->SetSpline(true);
dedx.push_back(pv);
for(i=0; i<53; i++) { pv->PutValues(i,E[i],G4_WATER_Mg[i]*factor); }

G4double G4_WATER_Al [53]={5.724, 6.39, 7.649, 8.82, 9.905, 10.91, 11.84, 12.71, 13.51, 16.66, 18.69, 19.93, 20.68, 21.38, 21.56, 21.5, 21.32, 21.07, 20.79, 20.48, 18.91, 17.47, 16.19, 15.08, 13.24, 11.78, 10.61, 9.641, 8.835, 8.153, 7.569, 5.583, 4.444, 3.71, 3.199, 2.534, 2.12, 1.836, 1.629, 1.471, 1.346, 1.245, 0.9325, 0.7707, 0.6719, 0.6054, 0.5223, 0.473, 0.441, 0.4189, 0.403, 0.3912, 0.3823, };
pv = new G4LPhysicsFreeVector(53,E[0],E[52]);
pv->SetSpline(true);
dedx.push_back(pv);
for(i=0; i<53; i++) { pv->PutValues(i,E[i],G4_WATER_Al[i]*factor); }

G4double G4_WATER_Si [53]={5.905, 6.583, 7.868, 9.073, 10.2, 11.25, 12.23, 13.14, 13.99, 17.4, 19.66, 21.1, 22.01, 22.91, 23.21, 23.22, 23.09, 22.87, 22.61, 22.32, 20.76, 19.28, 17.95, 16.78, 14.83, 13.26, 11.99, 10.94, 10.06, 9.304, 8.656, 6.43, 5.135, 4.294, 3.705, 2.938, 2.458, 2.129, 1.889, 1.706, 1.561, 1.444, 1.082, 0.8942, 0.7796, 0.7026, 0.6061, 0.549, 0.5119, 0.4862, 0.4678, 0.4542, 0.4438, };
pv = new G4LPhysicsFreeVector(53,E[0],E[52]);
pv->SetSpline(true);
dedx.push_back(pv);
for(i=0; i<53; i++) { pv->PutValues(i,E[i],G4_WATER_Si[i]*factor); }

G4double G4_WATER_P [53]={6.12, 6.81, 8.118, 9.352, 10.51, 11.61, 12.63, 13.58, 14.48, 18.13, 20.63, 22.28, 23.34, 24.47, 24.91, 25.02, 24.95, 24.78, 24.55, 24.28, 22.76, 21.26, 19.89, 18.67, 16.59, 14.92, 13.54, 12.39, 11.42, 10.59, 9.867, 7.367, 5.896, 4.935, 4.259, 3.376, 2.824, 2.445, 2.169, 1.959, 1.792, 1.657, 1.242, 1.027, 0.8954, 0.807, 0.6963, 0.6308, 0.5881, 0.5587, 0.5375, 0.5219, 0.51, };
pv = new G4LPhysicsFreeVector(53,E[0],E[52]);
pv->SetSpline(true);
dedx.push_back(pv);
for(i=0; i<53; i++) { pv->PutValues(i,E[i],G4_WATER_P[i]*factor); }

G4double G4_WATER_S [53]={6.294, 7, 8.338, 9.604, 10.8, 11.94, 13, 14, 14.94, 18.82, 21.55, 23.41, 24.65, 26.01, 26.6, 26.81, 26.81, 26.69, 26.5, 26.26, 24.79, 23.28, 21.88, 20.61, 18.43, 16.64, 15.16, 13.92, 12.86, 11.95, 11.15, 8.371, 6.715, 5.624, 4.856, 3.847, 3.217, 2.785, 2.47, 2.229, 2.04, 1.886, 1.413, 1.169, 1.019, 0.9187, 0.7929, 0.7183, 0.6697, 0.6362, 0.6122, 0.5944, 0.5808, };
pv = new G4LPhysicsFreeVector(53,E[0],E[52]);
pv->SetSpline(true);
dedx.push_back(pv);
for(i=0; i<53; i++) { pv->PutValues(i,E[i],G4_WATER_S[i]*factor); }

G4double G4_WATER_Cl [53]={6.522, 7.237, 8.59, 9.875, 11.1, 12.26, 13.36, 14.39, 15.37, 19.45, 22.4, 24.45, 25.86, 27.46, 28.19, 28.5, 28.57, 28.5, 28.34, 28.13, 26.72, 25.21, 23.78, 22.47, 20.2, 18.32, 16.75, 15.42, 14.28, 13.3, 12.44, 9.392, 7.557, 6.34, 5.479, 4.344, 3.633, 3.145, 2.789, 2.517, 2.303, 2.13, 1.596, 1.32, 1.151, 1.038, 0.8957, 0.8115, 0.7567, 0.7189, 0.6917, 0.6717, 0.6563, };
pv = new G4LPhysicsFreeVector(53,E[0],E[52]);
pv->SetSpline(true);
dedx.push_back(pv);
for(i=0; i<53; i++) { pv->PutValues(i,E[i],G4_WATER_Cl[i]*factor); }

G4double G4_WATER_Ar [53]={6.642, 7.369, 8.739, 10.04, 11.28, 12.47, 13.59, 14.66, 15.66, 19.93, 23.08, 25.32, 26.89, 28.72, 29.6, 30.01, 30.15, 30.13, 30, 29.82, 28.46, 26.94, 25.49, 24.15, 21.81, 19.86, 18.22, 16.82, 15.61, 14.57, 13.65, 10.39, 8.394, 7.063, 6.114, 4.859, 4.067, 3.522, 3.125, 2.821, 2.581, 2.387, 1.789, 1.48, 1.291, 1.164, 1.005, 0.9105, 0.8491, 0.8067, 0.7763, 0.7537, 0.7365, };
pv = new G4LPhysicsFreeVector(53,E[0],E[52]);
pv->SetSpline(true);
dedx.push_back(pv);
for(i=0; i<53; i++) { pv->PutValues(i,E[i],G4_WATER_Ar[i]*factor); }

  if(corr) {
    for(i=0; i<16; i++) {corr->AddStoppingData(Z[i], A[i], "G4_WATER", dedx[i]);}
  }
}
