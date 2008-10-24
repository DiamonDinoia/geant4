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
//
// The lust update: M.V. Kossov, CERN/ITEP(Moscow) 17-June-02
// GEANT4 tag $Name: not supported by cvs2svn $
//
//
// G4 Physics class: G4PhotoNuclearCrossSection for gamma+A cross sections
// Created: M.V. Kossov, CERN/ITEP(Moscow), 10-OCT-01
// The last update: M.V. Kossov, CERN/ITEP (Moscow) 17-May-02
// 
//#define debug
//#define pdebug
//#define debug3
//#define debugn
//#define debugs

#include "G4PhotoNuclearCrossSection.hh"

// Initialization of the statics
G4int G4PhotoNuclearCrossSection::lastN=0;  
                            // The last N of calculated nucleus
G4int G4PhotoNuclearCrossSection::lastZ=0;  
                            // The last Z of calculated nucleus
G4double G4PhotoNuclearCrossSection::lastSig=0.; 
                            // Last value of the Cross Section
G4double* G4PhotoNuclearCrossSection::lastGDR=0; 
                            // Pointer to the last array of GDR cross sections
G4double* G4PhotoNuclearCrossSection::lastHEN=0; 
                            // Pointer to the last array of HEn cross sections
G4double G4PhotoNuclearCrossSection::lastE=0.;  
                            // Last used in the cross section Energy
G4double G4PhotoNuclearCrossSection::lastTH=0.; 
                            // Last value of the Energy Threshold (A-dependent)
G4double G4PhotoNuclearCrossSection::lastSP=0.; 
                            // Last value of the ShadowingPomeron (A-dependent)

std::vector<G4double*> G4PhotoNuclearCrossSection::GDR;   // Vector of pointers to the GDRPhotonuclearCrossSection
std::vector<G4double*> G4PhotoNuclearCrossSection::HEN;   // Vector of pointers to the HighEnPhotonuclearCrossSect

G4PhotoNuclearCrossSection::G4PhotoNuclearCrossSection()
{
}

G4PhotoNuclearCrossSection::~G4PhotoNuclearCrossSection()
{
  std::vector<G4double*>::iterator pos;
  for(pos=GDR.begin(); pos<GDR.end(); pos++)
  { delete [] *pos; }
  GDR.clear();
  for(pos=HEN.begin(); pos<HEN.end(); pos++)
  { delete [] *pos; }
  HEN.clear();
}

// The main member function giving the gamma-A cross section 
// (E in GeV, CS in mb)

G4double 
G4PhotoNuclearCrossSection::GetCrossSection(const G4DynamicParticle* aPart,
                                            const G4Element* anEle, 
                                            G4double temperature)
{
  G4int nIso = anEle->GetNumberOfIsotopes();
  G4double cross_section = 0;
  
  if (nIso) {
    G4double psig;
    G4IsotopeVector* isoVector = anEle->GetIsotopeVector();
    G4double* abundVector = anEle->GetRelativeAbundanceVector();
    G4double ZZ;
    G4double AA;
  
    for (G4int i = 0; i < nIso; i++) {
      ZZ = G4double( (*isoVector)[i]->GetZ() );
      AA = G4double( (*isoVector)[i]->GetN() );
      psig = GetIsoZACrossSection(aPart, ZZ, AA, temperature);
      cross_section += psig*abundVector[i];
    }

  } else {
    cross_section = 
      GetIsoZACrossSection(aPart, anEle->GetZ(), anEle->GetN(), temperature);
  }
 
  return cross_section;
}


G4double 
G4PhotoNuclearCrossSection::GetIsoZACrossSection(const G4DynamicParticle* aPart,
                                                 G4double ZZ, G4double AA, 
					         G4double /*temperature*/)
{
  static const G4double THmin=2.;          // minimum Energy Threshold
  static const G4double dE=1.;             // step for the GDR table
  static const G4int    nL=105;            // A#of GDResonance points in E 
                                           // (each MeV from 2 to 106)
  static const G4double Emin=THmin+(nL-1)*dE; // minE for the HighE part
  static const G4double Emax=50000.;       // maxE for the HighE part
  static const G4int    nH=224;            // A#of HResonance points in lnE
  static const G4double milE=std::log(Emin);  // Low logarithm energy for 
                                           // the HighE part
  static const G4double malE=std::log(Emax);  // High logarithm energy 
                                           // (each 2.75 percent)
  static const G4double dlE=(malE-milE)/(nH-1); // Step in logarithm energy 
                                           // in the HighE part
  //
  //static const G4double shd=1.075-.0023*std::log(2.);  // HE PomShadowing(D)
  static const G4double shd=1.0734;                 // HE PomShadowing(D)
  static const G4double shc=0.072;                  // HE Shadowing constant
  static const G4double poc=0.0375;                 // HE Pomeron coefficient
  static const G4double pos=16.5;                   // HE Pomeron shift
  static const G4double reg=.11;                    // HE Reggeon slope
  //static const G4double shp=1.075;                  // HE PomShadowing(P)

  // Associative memory for acceleration

  static std::vector <G4int> colN;      // N of calculated nuclei
  static std::vector <G4int> colZ;      // Z of calculated nuclei
  static std::vector <G4double> spA;    // shadowing coefficients (A-dependent)
  static std::vector <G4double> eTH;    // energy threshold (A-dependent)
  //
  const G4double Energy = aPart->GetKineticEnergy()/MeV;
  const G4int targetAtomicNumber = static_cast<int>(AA+.499); //@@ Nat mixture (?!)
  const G4int targZ = static_cast<int>(ZZ);
  const G4int targN = targetAtomicNumber-targZ;
#ifdef debug
  G4cout << "G4PhotoNuclearCrossSection::GetCS:N=" << targN << ",Z=" 
         << targZ << ",E=" << Energy << G4endl;
#endif
  if (Energy<THmin) return 0.;
  G4double sigma=0.;
  if( aPart->GetDefinition()->GetPDGEncoding() == 22 )
  {
    G4double A=targN+targZ;
    if(targN!=lastN || targZ!=lastZ)   // Otherwise the set of parameters is ready
    {
      lastN = targN;             // The last N of calculated nucleus
      lastZ = targZ;             // The last Z of calculated nucleus
      G4int n=colN.size();       // Size of Associated Memory
      G4bool in=false;           // The nucleus is in the AssocMem DB
      if(n) {
        for(G4int i=0; i<n; i++) {
          if(colN[i]==targN && colZ[i]==targZ) // Calculated nuclei in DB
          {
            in=true;          // The nucleus is found in the AssocMem DB
            lastGDR=GDR[i];   // Pointer to prepared GDR cross sections
            lastHEN=HEN[i];   // Pointer to prepared High Energy cross sections
            lastTH =eTH[i];   // Energy Threshold
            lastSP =spA[i];   // Shadowing coefficient for UHE
          }
	}
      }
#ifdef debug
      G4cout<<"G4PhotoNucCrossSect::GetCS:A="<<A<<",n="<<n<<",in="<<in<<G4endl;
#endif
      if(!in)          // Fill the new set of parameters for the new nucleus
      {
        G4double lnA=std::log(A); // The nucleus is not found in DB. It is new.
        if(A==1.) lastSP=1.;      // The Reggeon shadowing (A=1)
        else lastSP=A*(1.-shc*lnA);      // The Reggeon shadowing
#ifdef debug
        G4cout << "G4PhotoNucCrossSect::GetCS: lnA="
               << lnA << ",lastSP=" << lastSP << G4endl;
#endif
        lastTH=ThresholdEnergy(targZ, targN); // Energy Threshold
#ifdef debug
        G4cout << "G4PhotoNucCrossSect::GetCS: lastTH=" << lastTH << G4endl;
#endif
#ifdef debug3
        if(A==3) G4cout << "G4PhotoNucCrossSect::GetCS: lastTH="
                        << lastTH << ",lastSP=" << lastSP << G4endl;
#endif
        lastGDR = new G4double[nL];  // Allocate memory for the new 
                                     // GDR cross sections
        lastHEN = new G4double[nH];  // Allocate memory for the new 
                                     // HEN cross sections
        G4int er=GetFunctions(A,lastGDR,lastHEN); // new ZeroPosition and 
                                                  // filling of the functions
        if(er<1) G4cerr << "***G4PhotoNucCrossSection::GetCrossSection: A="
                        << A << " failed" << G4endl;
#ifdef debug
        G4cout<<"G4PhotoNucCrossSect::GetCS: GetFunctions er="<<er<<G4endl;
#endif
        colN.push_back(targN);
        colZ.push_back(targZ);
        GDR.push_back(lastGDR);     // added GDR, found by AH 10/7/02
        HEN.push_back(lastHEN);     // added HEN, found by AH 10/7/02
        eTH.push_back(lastTH);      // Threshold Energy
        spA.push_back(lastSP);      // Pomeron Shadowing
      } // End of creation of the new set of parameters
    } // End of parameters udate

    //
    // =================== NOW the Magic Formula ===================
    //
    if (Energy<lastTH)
    {
      lastE=Energy;
      lastSig=0.;
      return 0.;
    }
    else if (Energy<Emin)   // GDR region (approximated in E, not in lnE)
    {
#ifdef debug
      G4cout << "G4PNCS::GetCS: before GDR A=" << A 
              << ", nL=" << nL << ",TH=" << THmin << ",dE=" <<dE << G4endl;
#endif
      if(A<=1.) sigma=0.;
      else      sigma=EquLinearFit(Energy,nL,THmin,dE,lastGDR);
#ifdef debugn
      if(sigma<0.) G4cout << "G4PNCS::GetCS:A=" << A << ",E=" << Energy 
                          << ",TH=" << THmin << ",dE=" << dE << G4endl;
#endif
    }
    else if (Energy<Emax)                     // High Energy region
    {
      G4double lE=std::log(Energy);
#ifdef debug
      G4cout << "G4PNCS::GetCS: before HEN nH=" << nH << ",iE=" 
             << milE << ",dlE=" << dlE << G4endl;
#endif
      sigma=EquLinearFit(lE,nH,milE,dlE,lastHEN);
    }
    else               // UHE region (calculation, but not so frequent)
    {
      G4double lE=std::log(Energy);
      //G4double sh=shd;
      //if(A==1.)sh=shp;
      sigma=lastSP*(poc*(lE-pos)+shd*std::exp(-reg*lE));
    }
#ifdef debug
    G4cout<<"G4PNCS::GetCS: sigma="<<sigma<<G4endl;
#endif
#ifdef pdebug
    if(Energy>45000.&&Energy<60000.)G4cout<<"PN::CS:A="<<A<<",E="<<Energy<<",sigma="<<sigma<<G4endl;
#endif
  } // End of "sigma" calculation
  else return 0.;

  if(sigma<0.) return 0.;
  return sigma*millibarn;
}

// Gives the threshold energy for different nuclei (min of p- and n-threshold)
G4double G4PhotoNuclearCrossSection::ThresholdEnergy(G4int Z, G4int N)
{
  // CHIPS - Direct GEANT
  //static const G4double mNeut = G4QPDGCode(2112).GetMass();
  //static const G4double mProt = G4QPDGCode(2212).GetMass();
  static const G4double mNeut = G4NucleiProperties::GetNuclearMass(1,0);
  static const G4double mProt = G4NucleiProperties::GetNuclearMass(1,1);
  // ---------
  static const G4double infEn = 9.e27;

  G4int A=Z+N;
  if(A<1) return infEn;
  else if(A==1) return 134.9766; // Pi0 threshold for the nucleon
  // CHIPS - Direct GEANT
  //G4double mT= G4QPDGCode(111).GetNuclMass(Z,N,0);
  G4double mT= 0.;
  if(G4NucleiProperties::IsInStableTable(A,Z)) 
    mT=G4NucleiProperties::GetNuclearMass(A,Z);
  else
  {
    //G4cerr<<"G4PhotoNucCrossSect.hh::ThreshEn:Z="<<Z<<",A="<<A
    //    <<" element isn't in G4NucPr"<<G4endl;
    // If it is not in the Table of Stable Nuclei, then the Threshold=inf
    return infEn;               
  }
  // ---------
  G4double mP= infEn;
  //if(Z) mP= G4QPDGCode(111).GetNuclMass(Z-1,N,0);

  if(Z && G4NucleiProperties::IsInStableTable(A-1,Z-1))
  {
    mP = G4NucleiProperties::GetNuclearMass(A-1,Z-1);
  }
  /*
  else
  {
    G4cerr << "G4PhotoNuclearCrossSection::ThrEn:Z=" << Z-1 << ",A=" 
           << A-1 << " element isn't in G4NucP" << G4endl;
  }
  */
  G4double mN= infEn;
  //if(N) mN= G4QPDGCode(111).GetNuclMass(Z,N-1,0);
  if(N&&G4NucleiProperties::IsInStableTable(A-1,Z)) 
    mN=G4NucleiProperties::GetNuclearMass(A-1,Z);
  /*
  else
  {
    G4cerr<<"G4PhotoNuclearCrossSection::ThreshEn:Z="<<Z<<",A="<<A-1
    <<" element isn't in G4NuP"<<G4endl;
  }
  */
  G4double dP= mP+mProt-mT;
  G4double dN= mN+mNeut-mT;
  if(dP<dN)dN=dP;
  return dN;
}

//
//  Linear fit for XN[N],YN[N] tabulated function 
//  to X point // @@ !!! Not used !!!
//  G4double G4PhotoNuclearCrossSection::
//  LinearFit(G4double X, G4int N, const G4double* XN, const G4double* YN)
//{
//  G4double Xj=XN[0];
//  G4double Xh=XN[N-1];
//  if(X<=Xj) return YN[0]; //-------+
//  else if(X>=Xh) return YN[N-1];//-|
//  G4double Xp=0.; //               |
//  G4int j=0;   //                  |
//  while (X>Xj && j<N)//<-----------+
//  {
//    j++;
//    Xp=Xj;
//    Xj=XN[j];
//  }
//  return YN[j]-(Xj-X)*(YN[j]-YN[j-1])/(Xj-Xp);
//}

//
//  Linear fit for YN[N] tabulated (from X0 with step DX) function to X point
//
G4double 
G4PhotoNuclearCrossSection::EquLinearFit(G4double X, G4int N, 
                                         const G4double X0, const G4double DX, 
                                         const G4double* Y)
{
  if(DX<=0. || N<2)
  {
    G4cout<<"***G4PhotoNuclearCrossSection::EquLinearFit: DX="<<DX<<", N="<<N<<G4endl;
    return Y[0];
  }
  G4int    N2=N-2;
  G4double d=(X-X0)/DX;
  G4int         j=static_cast<int>(d);
  if     (j<0)  j=0;
  else if(j>N2) j=N2;
  d-=j; // excess
  G4double yi=Y[j];
  G4double sigma=yi+(Y[j+1]-yi)*d;
#ifdef debugn
  if(sigma<0.)G4cout<<"G4PhotNucCS::EquLinFit: j="<<j<<",yi="<<yi<<",ya="<<Y[j+1]<<",d="<<d<<G4endl;
#endif
  return sigma;
}

// Calculate the functions for the std::log(A)

G4int 
G4PhotoNuclearCrossSection::GetFunctions(G4double a, G4double* y, G4double* z)
{
  static const G4int nLA=49;               // A#of GDResonance basic nuclei
  static const G4double LA[nLA]={
    2.,    4.,    6.,    7.,    9.,   12.,   14.,   15.,   16.,  19.,   23.,
   24.,   27.,   28.,   32.,   34.,   40.,   54.,   55.,   56.,  58.7,  58.9, 
   63.5,  65.4,  76.,   82.,  107.9, 112.4, 118.7, 126.9, 154., 156.,  159., 
  165.,  168.,  174.,  178.,  180.,  181.,  184.,  186.,  197., 204.4, 207.2,
  209.,  232.,  235.,  238.,  239.};

  static const G4int nL=105;   // A#of GDResonance points in E 
                               // (each MeV from 2 to 106)
  static const G4int nHA=14;   // A#of HResonance basic nuclei
  static const G4double HA[nHA]={
   1.,2.,3.,4.,6.,7.,9.,12.,16.,27.,63.5,118.7,207.2,238.};

  static const G4int nH=224;   // A#of HResonance points in lnE 
                               // (each 2.75 percents)

  // If the cross section approximation formula is changed - replace from file

  static const G4double SL0[nL]={
    7.094260e-01,1.532987e+00,2.449381e+00,2.785790e+00,2.525673e+00,
    2.128172e+00,1.780549e+00,1.506934e+00,1.294560e+00,1.128048e+00,
    9.953850e-01,8.879274e-01,7.995356e-01,7.258111e-01,6.635555e-01,
    6.104038e-01,5.645786e-01,5.247229e-01,4.897864e-01,4.589445e-01,
    4.315429e-01,4.070560e-01,3.850576e-01,3.651990e-01,3.471920e-01,
    3.307971e-01,3.158133e-01,3.020711e-01,2.894266e-01,2.777569e-01,
    2.669563e-01,2.569336e-01,2.476099e-01,2.389161e-01,2.307920e-01,
    2.231848e-01,2.160475e-01,2.093390e-01,2.030225e-01,1.970653e-01,
    1.914383e-01,1.861152e-01,1.810725e-01,1.762891e-01,1.717459e-01,
    1.674254e-01,1.633120e-01,1.593914e-01,1.556505e-01,1.520775e-01,
    1.486616e-01,1.453926e-01,1.422615e-01,1.392599e-01,1.363800e-01,
    1.336147e-01,1.309573e-01,1.284017e-01,1.259423e-01,1.235738e-01,
    1.212914e-01,1.190904e-01,1.169666e-01,1.149161e-01,1.129353e-01,
    1.110206e-01,1.091688e-01,1.073770e-01,1.056423e-01,1.039619e-01,
    1.023336e-01,1.007548e-01,9.922335e-02,9.773724e-02,9.629446e-02,
    9.489316e-02,9.353161e-02,9.220814e-02,9.092120e-02,8.966931e-02,
    8.845106e-02,8.726514e-02,8.611027e-02,8.498527e-02,8.388900e-02,
    8.282039e-02,8.177841e-02,8.076208e-02,7.977047e-02,7.880271e-02,
    7.785794e-02,7.693536e-02,7.603421e-02,7.515376e-02,7.429330e-02,
    7.345216e-02,7.262971e-02,7.182534e-02,7.103847e-02,7.026852e-02,
    6.951498e-02,6.877732e-02,6.805505e-02,6.734772e-02,6.665486e-02};

  static const G4double SL1[nL]={
    2.017310e-04,9.866847e-04,3.081371e-03,7.486476e-03,1.550083e-02,
    2.873865e-02,4.915763e-02,7.909925e-02,1.213409e-01,1.791563e-01,
    2.563778e-01,3.574504e-01,4.874600e-01,6.521048e-01,8.575237e-01,
    1.109763e+00,1.413389e+00,1.768398e+00,2.164804e+00,2.576439e+00,
    2.960166e+00,3.267863e+00,3.467401e+00,3.555207e+00,3.550465e+00,
    3.480379e+00,3.369351e+00,3.235278e+00,3.090040e+00,2.941162e+00,
    2.793315e+00,2.649362e+00,2.511013e+00,2.379237e+00,2.254508e+00,
    2.136980e+00,2.026587e+00,1.923127e+00,1.826306e+00,1.735781e+00,
    1.651181e+00,1.572129e+00,1.498250e+00,1.429182e+00,1.364578e+00,
    1.304111e+00,1.247474e+00,1.194383e+00,1.144574e+00,1.097802e+00,
    1.053842e+00,1.012488e+00,9.735481e-01,9.368486e-01,9.022288e-01,
    8.695413e-01,8.386509e-01,8.094332e-01,7.817741e-01,7.555686e-01,
    7.307199e-01,7.071392e-01,6.847445e-01,6.634600e-01,6.432161e-01,
    6.239481e-01,6.055964e-01,5.881058e-01,5.714251e-01,5.555068e-01,
    5.403069e-01,5.257844e-01,5.119013e-01,4.986222e-01,4.859140e-01,
    4.737459e-01,4.620893e-01,4.509173e-01,4.402048e-01,4.299283e-01,
    4.200659e-01,4.105971e-01,4.015025e-01,3.927640e-01,3.843647e-01,
    3.762885e-01,3.685206e-01,3.610467e-01,3.538537e-01,3.469290e-01,
    3.402609e-01,3.338384e-01,3.276510e-01,3.216889e-01,3.159429e-01,
    3.104042e-01,3.050648e-01,2.999168e-01,2.949531e-01,2.901668e-01,
    2.855516e-01,2.811014e-01,2.768107e-01,2.726743e-01,2.686872e-01};

  static const G4double SL2[nL]={
    4.776434e-03,2.412116e-02,7.595870e-02,1.835144e-01,3.703569e-01,
    6.466818e-01,9.877908e-01,1.324697e+00,1.575559e+00,1.699764e+00,
    1.715038e+00,1.669943e+00,1.614318e+00,1.588675e+00,1.625472e+00,
    1.751419e+00,1.984310e+00,2.321229e+00,2.721619e+00,3.102285e+00,
    3.366107e+00,3.454042e+00,3.371971e+00,3.170282e+00,2.907055e+00,
    2.626333e+00,2.354348e+00,2.103834e+00,1.879323e+00,1.681008e+00,
    1.507070e+00,1.354935e+00,1.221899e+00,1.105415e+00,1.003205e+00,
    9.132844e-01,8.339468e-01,7.637380e-01,7.014215e-01,6.459465e-01,
    5.964196e-01,5.520797e-01,5.122769e-01,4.764549e-01,4.441358e-01,
    4.149083e-01,3.884171e-01,3.643543e-01,3.424525e-01,3.224787e-01,
    3.042291e-01,2.875255e-01,2.722111e-01,2.581479e-01,2.452141e-01,
    2.333017e-01,2.223151e-01,2.121693e-01,2.027886e-01,1.941052e-01,
    1.860586e-01,1.785946e-01,1.716644e-01,1.652242e-01,1.592345e-01,
    1.536595e-01,1.484670e-01,1.436278e-01,1.391153e-01,1.349053e-01,
    1.309760e-01,1.273073e-01,1.238809e-01,1.206803e-01,1.176900e-01,
    1.148962e-01,1.122861e-01,1.098477e-01,1.075703e-01,1.054441e-01,
    1.034596e-01,1.016087e-01,9.988340e-02,9.827659e-02,9.678165e-02,
    9.539245e-02,9.410337e-02,9.290919e-02,9.180512e-02,9.078673e-02,
    8.984997e-02,8.899108e-02,8.820664e-02,8.749353e-02,8.684888e-02,
    8.627010e-02,8.575488e-02,8.530112e-02,8.490697e-02,8.457084e-02,
    8.429132e-02,8.406729e-02,8.389779e-02,8.378214e-02,8.371985e-02};

  static const G4double SL3[nL]={
    1.375991e-04,6.420490e-04,2.009594e-03,5.073626e-03,1.137383e-02,
    2.408187e-02,5.091978e-02,1.151175e-01,2.955817e-01,8.132651e-01,
    1.635125e+00,1.931572e+00,2.185333e+00,2.701264e+00,3.269689e+00,
    3.632210e+00,3.708366e+00,3.594398e+00,3.418556e+00,3.260141e+00,
    3.149899e+00,3.091216e+00,3.075568e+00,3.090472e+00,3.123146e+00,
    3.162196e+00,3.198373e+00,3.224873e+00,3.237305e+00,3.233448e+00,
    3.212852e+00,3.176382e+00,3.125768e+00,3.063213e+00,2.991084e+00,
    2.911700e+00,2.827186e+00,2.739409e+00,2.649943e+00,2.560078e+00,
    2.470840e+00,2.383021e+00,2.297216e+00,2.213858e+00,2.133241e+00,
    2.055557e+00,1.980911e+00,1.909346e+00,1.840852e+00,1.775386e+00,
    1.712877e+00,1.653235e+00,1.596357e+00,1.542133e+00,1.490448e+00,
    1.441186e+00,1.394230e+00,1.349469e+00,1.306789e+00,1.266085e+00,
    1.227254e+00,1.190196e+00,1.154820e+00,1.121035e+00,1.088758e+00,
    1.057908e+00,1.028411e+00,1.000196e+00,9.731948e-01,9.473457e-01,
    9.225887e-01,8.988681e-01,8.761312e-01,8.543286e-01,8.334136e-01,
    8.133424e-01,7.940736e-01,7.755685e-01,7.577903e-01,7.407046e-01,
    7.242788e-01,7.084823e-01,6.932862e-01,6.786633e-01,6.645878e-01,
    6.510355e-01,6.379834e-01,6.254100e-01,6.132949e-01,6.016187e-01,
    5.903633e-01,5.795116e-01,5.690472e-01,5.589548e-01,5.492201e-01,
    5.398295e-01,5.307700e-01,5.220296e-01,5.135969e-01,5.054613e-01,
    4.976128e-01,4.900419e-01,4.827400e-01,4.756989e-01,4.689110e-01};

  static const G4double SL4[nL]={
    1.531367e-04,6.750684e-04,2.023434e-03,4.818832e-03,9.866691e-03,
    1.816857e-02,3.094217e-02,4.965477e-02,7.607934e-02,1.123974e-01,
    1.614108e-01,2.270208e-01,3.153403e-01,4.372460e-01,6.139880e-01,
    8.886525e-01,1.345605e+00,2.121366e+00,3.298049e+00,4.533310e+00,
    5.172459e+00,5.243522e+00,5.175754e+00,5.149633e+00,5.156364e+00,
    5.151144e+00,5.108382e+00,5.025027e+00,4.909480e+00,4.772279e+00,
    4.621981e+00,4.464473e+00,4.303590e+00,4.141874e+00,3.981115e+00,
    3.822656e+00,3.667551e+00,3.516631e+00,3.370536e+00,3.229738e+00,
    3.094556e+00,2.965180e+00,2.841688e+00,2.724066e+00,2.612228e+00,
    2.506035e+00,2.405305e+00,2.309830e+00,2.219381e+00,2.133721e+00,
    2.052608e+00,1.975802e+00,1.903066e+00,1.834170e+00,1.768894e+00,
    1.707024e+00,1.648361e+00,1.592714e+00,1.539903e+00,1.489759e+00,
    1.442123e+00,1.396846e+00,1.353788e+00,1.312819e+00,1.273817e+00,
    1.236668e+00,1.201266e+00,1.167510e+00,1.135308e+00,1.104573e+00,
    1.075223e+00,1.047183e+00,1.020382e+00,9.947538e-01,9.702356e-01,
    9.467696e-01,9.243013e-01,9.027797e-01,8.821569e-01,8.623879e-01,
    8.434307e-01,8.252457e-01,8.077958e-01,7.910459e-01,7.749634e-01,
    7.595173e-01,7.446784e-01,7.304196e-01,7.167151e-01,7.035406e-01,
    6.908733e-01,6.786917e-01,6.669756e-01,6.557060e-01,6.448649e-01,
    6.344356e-01,6.244022e-01,6.147497e-01,6.054644e-01,5.965332e-01,
    5.879438e-01,5.796850e-01,5.717461e-01,5.641173e-01,5.567897e-01};

  static const G4double SL5[nL]={
    1.905569e-04,7.771730e-04,2.250919e-03,5.273053e-03,1.071640e-02,
    1.969996e-02,3.365091e-02,5.440813e-02,8.439169e-02,1.268914e-01,
    1.866020e-01,2.707115e-01,3.912405e-01,5.701376e-01,8.501724e-01,
    1.317340e+00,2.143911e+00,3.657987e+00,6.387255e+00,1.074352e+01,
    1.571664e+01,1.840405e+01,1.776700e+01,1.557514e+01,1.329204e+01,
    1.138076e+01,9.874227e+00,8.700723e+00,7.781216e+00,7.050490e+00,
    6.458855e+00,5.969695e+00,5.556515e+00,5.200371e+00,4.887807e+00,
    4.609287e+00,4.358030e+00,4.129172e+00,3.919172e+00,3.725403e+00,3.545861e+00,3.378977e+00,
    3.223486e+00,3.078336e+00,2.942636e+00,2.815610e+00,2.696573e+00,2.584914e+00,2.480080e+00,
    2.381572e+00,2.288930e+00,2.201736e+00,2.119606e+00,2.042187e+00,1.969152e+00,1.900204e+00,
    1.835064e+00,1.773480e+00,1.715215e+00,1.660054e+00,1.607794e+00,1.558252e+00,1.511256e+00,
    1.466647e+00,1.424278e+00,1.384013e+00,1.345726e+00,1.309299e+00,1.274624e+00,1.241600e+00,
    1.210133e+00,1.180134e+00,1.151523e+00,1.124223e+00,1.098163e+00,1.073277e+00,1.049504e+00,
    1.026785e+00,1.005065e+00,9.842955e-01,9.644275e-01,9.454166e-01,9.272213e-01,9.098022e-01,
    8.931224e-01,8.771474e-01,8.618447e-01,8.471837e-01,8.331356e-01,8.196734e-01,8.067717e-01,
    7.944065e-01,7.825554e-01,7.711972e-01,7.603122e-01,7.498817e-01,7.398883e-01,7.303156e-01,
    7.211483e-01,7.123722e-01,7.039741e-01,6.959417e-01,6.882635e-01,6.809293e-01,6.739294e-01};

  static const G4double SL6[nL]={
    2.222448e-04,8.620556e-04,2.444896e-03,5.705453e-03,1.171159e-02,2.205349e-02,3.918281e-02,
    6.696997e-02,1.115720e-01,1.827533e-01,2.959155e-01,4.753435e-01,7.596938e-01,1.211738e+00,
    1.936099e+00,3.111254e+00,5.043478e+00,8.232698e+00,1.330416e+01,2.019140e+01,2.638709e+01,
    2.859878e+01,2.728600e+01,2.464338e+01,2.186072e+01,1.931943e+01,1.710886e+01,1.522482e+01,
    1.363142e+01,1.228407e+01,1.113950e+01,1.015995e+01,9.314220e+00,8.577403e+00,7.929898e+00,
    7.356396e+00,6.844921e+00,6.386042e+00,5.972257e+00,5.597518e+00,5.256885e+00,4.946267e+00,
    4.662228e+00,4.401855e+00,4.162646e+00,3.942438e+00,3.739345e+00,3.551711e+00,3.378077e+00,
    3.217151e+00,3.067782e+00,2.928947e+00,2.799729e+00,2.679306e+00,2.566940e+00,2.461965e+00,
    2.363783e+00,2.271852e+00,2.185682e+00,2.104827e+00,2.028885e+00,1.957488e+00,1.890304e+00,
    1.827026e+00,1.767378e+00,1.711104e+00,1.657972e+00,1.607769e+00,1.560299e+00,1.515382e+00,
    1.472853e+00,1.432558e+00,1.394357e+00,1.358121e+00,1.323729e+00,1.291070e+00,1.260042e+00,
    1.230549e+00,1.202502e+00,1.175819e+00,1.150425e+00,1.126246e+00,1.103218e+00,1.081279e+00,
    1.060370e+00,1.040438e+00,1.021433e+00,1.003308e+00,9.860183e-01,9.695234e-01,9.537847e-01,
    9.387662e-01,9.244342e-01,9.107573e-01,8.977058e-01,8.852523e-01,8.733710e-01,8.620378e-01,
    8.512302e-01,8.409275e-01,8.311102e-01,8.217603e-01,8.128613e-01,8.043977e-01,7.963557e-01};

  static const G4double SL7[nL]={
    2.400132e-04,9.082999e-04,2.545511e-03,5.912609e-03,1.214175e-02,2.297237e-02,4.117454e-02,
    7.124517e-02,1.204927e-01,2.006898e-01,3.306145e-01,5.401144e-01,8.769596e-01,1.418938e+00,
    2.295653e+00,3.727247e+00,6.087430e+00,9.967584e+00,1.601149e+01,2.371712e+01,2.968929e+01,
    3.091057e+01,2.878613e+01,2.564897e+01,2.255784e+01,1.981778e+01,1.747914e+01,1.551277e+01,
    1.386591e+01,1.248298e+01,1.131384e+01,1.031654e+01,9.457362e+00,8.709876e+00,8.053561e+00,
    7.472543e+00,6.954497e+00,6.489768e+00,6.070707e+00,5.691171e+00,5.346149e+00,5.031499e+00,
    4.743750e+00,4.479955e+00,4.237588e+00,4.014464e+00,3.808676e+00,3.618553e+00,3.442619e+00,
    3.279569e+00,3.128239e+00,2.987593e+00,2.856703e+00,2.734737e+00,2.620949e+00,2.514665e+00,
    2.415276e+00,2.322236e+00,2.235046e+00,2.153255e+00,2.076456e+00,2.004275e+00,1.936374e+00,
    1.872444e+00,1.812202e+00,1.755391e+00,1.701773e+00,1.651134e+00,1.603273e+00,1.558007e+00,
    1.515169e+00,1.474604e+00,1.436168e+00,1.399730e+00,1.365168e+00,1.332368e+00,1.301226e+00,
    1.271645e+00,1.243535e+00,1.216812e+00,1.191399e+00,1.167222e+00,1.144214e+00,1.122313e+00,
    1.101460e+00,1.081599e+00,1.062680e+00,1.044655e+00,1.027479e+00,1.011109e+00,9.955079e-01,
    9.806377e-01,9.664643e-01,9.529557e-01,9.400818e-01,9.278146e-01,9.161277e-01,9.049967e-01,
    8.943989e-01,8.843129e-01,8.747192e-01,8.655994e-01,8.569368e-01,8.487159e-01,8.409226e-01};

  static const G4double SL8[nL]={
    2.590923e-04,9.573672e-04,2.651275e-03,6.130118e-03,1.259782e-02,2.396311e-02,4.335926e-02,
    7.599430e-02,1.304582e-01,2.206539e-01,3.685911e-01,6.084138e-01,9.922345e-01,1.598590e+00,
    2.544422e+00,4.001018e+00,6.212589e+00,9.507804e+00,1.423676e+01,2.030771e+01,2.598385e+01,
    2.841920e+01,2.739643e+01,2.481830e+01,2.199791e+01,1.941582e+01,1.718791e+01,1.530626e+01,
    1.372572e+01,1.239454e+01,1.126539e+01,1.029864e+01,9.462554e+00,8.732318e+00,8.088729e+00,
    7.516956e+00,7.005488e+00,6.545306e+00,6.129254e+00,5.751565e+00,5.407521e+00,5.093205e+00,
    4.805315e+00,4.541037e+00,4.297946e+00,4.073932e+00,3.867148e+00,3.675966e+00,3.498945e+00,
    3.334802e+00,3.182394e+00,3.040697e+00,2.908794e+00,2.785859e+00,2.671150e+00,2.563996e+00,
    2.463791e+00,2.369986e+00,2.282085e+00,2.199636e+00,2.122228e+00,2.049489e+00,1.981076e+00,
    1.916681e+00,1.856017e+00,1.798827e+00,1.744870e+00,1.693929e+00,1.645803e+00,1.600307e+00,
    1.557271e+00,1.516539e+00,1.477966e+00,1.441418e+00,1.406772e+00,1.373914e+00,1.342737e+00,
    1.313142e+00,1.285040e+00,1.258344e+00,1.232976e+00,1.208862e+00,1.185934e+00,1.164128e+00,
    1.143384e+00,1.123646e+00,1.104863e+00,1.086985e+00,1.069967e+00,1.053766e+00,1.038344e+00,
    1.023662e+00,1.009685e+00,9.963805e-01,9.837187e-01,9.716705e-01,9.602093e-01,9.493103e-01,
    9.389503e-01,9.291078e-01,9.197629e-01,9.108970e-01,9.024933e-01,8.945360e-01,8.870112e-01};

  static const G4double SL9[nL]={
    3.243985e-04,1.122034e-03,3.000932e-03,6.850212e-03,1.414720e-02,2.751937e-02,5.204925e-02,
    9.887958e-02,1.966468e-01,4.282973e-01,1.041076e+00,2.706630e+00,6.509565e+00,1.085114e+01,
    1.162472e+01,1.124054e+01,1.202416e+01,1.402207e+01,1.659634e+01,1.891975e+01,2.032292e+01,
    2.059083e+01,1.993672e+01,1.873926e+01,1.732572e+01,1.590211e+01,1.457097e+01,1.336993e+01,
    1.230272e+01,1.135820e+01,1.052046e+01,9.773672e+00,9.103884e+00,8.499562e+00,7.951408e+00,
    7.451996e+00,6.995366e+00,6.576679e+00,6.191930e+00,5.837748e+00,5.511235e+00,5.209864e+00,
    4.931401e+00,4.673850e+00,4.435420e+00,4.214488e+00,4.009588e+00,3.819384e+00,3.642664e+00,
    3.478323e+00,3.325356e+00,3.182848e+00,3.049964e+00,2.925943e+00,2.810093e+00,2.701782e+00,
    2.600432e+00,2.505518e+00,2.416558e+00,2.333114e+00,2.254783e+00,2.181197e+00,2.112021e+00,
    2.046943e+00,1.985682e+00,1.927976e+00,1.873586e+00,1.822292e+00,1.773891e+00,1.728195e+00,
    1.685032e+00,1.644242e+00,1.605677e+00,1.569201e+00,1.534686e+00,1.502017e+00,1.471082e+00,
    1.441781e+00,1.414020e+00,1.387711e+00,1.362772e+00,1.339127e+00,1.316705e+00,1.295438e+00,
    1.275266e+00,1.256130e+00,1.237976e+00,1.220753e+00,1.204413e+00,1.188912e+00,1.174209e+00,
    1.160265e+00,1.147042e+00,1.134507e+00,1.122628e+00,1.111376e+00,1.100721e+00,1.090639e+00,
    1.081106e+00,1.072098e+00,1.063597e+00,1.055582e+00,1.048036e+00,1.040943e+00,1.034290e+00};

  static const G4double SL10[nL]={
    4.311217e-04,1.384716e-03,3.549518e-03,7.988549e-03,1.667330e-02,3.341344e-02,6.552895e-02,
    1.266167e-01,2.409191e-01,4.501490e-01,8.243911e-01,1.480280e+00,2.612343e+00,4.545249e+00,
    7.790746e+00,1.287033e+01,1.909053e+01,2.392952e+01,2.652790e+01,2.742592e+01,2.690891e+01,
    2.536786e+01,2.330746e+01,2.113183e+01,1.907371e+01,1.723144e+01,1.562538e+01,1.423904e+01,
    1.304262e+01,1.200459e+01,1.109667e+01,1.029534e+01,9.581841e+00,8.941546e+00,8.363124e+00,
    7.837784e+00,7.358628e+00,6.920153e+00,6.517878e+00,6.148072e+00,5.807568e+00,5.493625e+00,
    5.203837e+00,4.936070e+00,4.688412e+00,4.459144e+00,4.246710e+00,4.049702e+00,3.866841e+00,
    3.696964e+00,3.539013e+00,3.392026e+00,3.255128e+00,3.127519e+00,3.008473e+00,2.897326e+00,
    2.793474e+00,2.696364e+00,2.605493e+00,2.520398e+00,2.440659e+00,2.365890e+00,2.295737e+00,
    2.229875e+00,2.168007e+00,2.109859e+00,2.055179e+00,2.003736e+00,1.955316e+00,1.909723e+00,
    1.866773e+00,1.826299e+00,1.788145e+00,1.752167e+00,1.718232e+00,1.686214e+00,1.655999e+00,
    1.627479e+00,1.600555e+00,1.575133e+00,1.551128e+00,1.528457e+00,1.507047e+00,1.486825e+00,
    1.467726e+00,1.449689e+00,1.432656e+00,1.416572e+00,1.401389e+00,1.387057e+00,1.373533e+00,
    1.360776e+00,1.348747e+00,1.337409e+00,1.326730e+00,1.316677e+00,1.307222e+00,1.298337e+00,
    1.289997e+00,1.282179e+00,1.274863e+00,1.268027e+00,1.261656e+00,1.255732e+00,1.250242e+00};

  static const G4double SL11[nL]={
    4.614524e-04,1.458509e-03,3.702639e-03,8.309380e-03,1.740590e-02,3.519535e-02,6.986551e-02,
    1.367187e-01,2.630019e-01,4.950763e-01,9.087988e-01,1.624204e+00,2.825210e+00,4.782440e+00,
    7.867272e+00,1.250247e+01,1.878669e+01,2.530271e+01,2.928727e+01,3.015114e+01,2.903038e+01,
    2.689359e+01,2.438858e+01,2.190927e+01,1.964845e+01,1.767000e+01,1.597064e+01,1.451878e+01,
    1.327514e+01,1.220222e+01,1.126790e+01,1.044614e+01,9.716524e+00,9.063269e+00,8.474255e+00,
    7.940129e+00,7.453592e+00,7.008848e+00,6.601198e+00,6.226749e+00,5.882205e+00,5.564731e+00,
    5.271842e+00,5.001344e+00,4.751274e+00,4.519872e+00,4.305549e+00,4.106867e+00,3.922524e+00,
    3.751336e+00,3.592227e+00,3.444221e+00,3.306426e+00,3.178034e+00,3.058307e+00,2.946572e+00,
    2.842215e+00,2.744679e+00,2.653450e+00,2.568064e+00,2.488092e+00,2.413143e+00,2.342861e+00,
    2.276915e+00,2.215004e+00,2.156852e+00,2.102204e+00,2.050824e+00,2.002497e+00,1.957023e+00,
    1.914217e+00,1.873910e+00,1.835942e+00,1.800170e+00,1.766455e+00,1.734674e+00,1.704709e+00,
    1.676451e+00,1.649799e+00,1.624659e+00,1.600943e+00,1.578569e+00,1.557460e+00,1.537545e+00,
    1.518758e+00,1.501034e+00,1.484317e+00,1.468550e+00,1.453684e+00,1.439670e+00,1.426463e+00,
    1.414022e+00,1.402307e+00,1.391282e+00,1.380912e+00,1.371166e+00,1.362014e+00,1.353430e+00,
    1.345386e+00,1.337862e+00,1.330834e+00,1.324283e+00,1.318193e+00,1.312546e+00,1.307330e+00};

  static const G4double SL12[nL]={
    5.615148e-04,1.700309e-03,4.203181e-03,9.368359e-03,1.987519e-02,4.133574e-02,8.507565e-02,
    1.726852e-01,3.430025e-01,6.623201e-01,1.238631e+00,2.240098e+00,3.915001e+00,6.601693e+00,
    1.070034e+01,1.656745e+01,2.430795e+01,3.323297e+01,4.042222e+01,4.203499e+01,3.892326e+01,
    3.426056e+01,2.971854e+01,2.578645e+01,2.251925e+01,1.984150e+01,1.764928e+01,1.584399e+01,
    1.434242e+01,1.307807e+01,1.199938e+01,1.106710e+01,1.025167e+01,9.530877e+00,8.888043e+00,
    8.310522e+00,7.788614e+00,7.314751e+00,6.882912e+00,6.488204e+00,6.126573e+00,5.794594e+00,
    5.489325e+00,5.208212e+00,4.949006e+00,4.709718e+00,4.488572e+00,4.283980e+00,4.094514e+00,
    3.918886e+00,3.755935e+00,3.604610e+00,3.463959e+00,3.333120e+00,3.211310e+00,3.097817e+00,
    2.991993e+00,2.893250e+00,2.801049e+00,2.714902e+00,2.634361e+00,2.559016e+00,2.488493e+00,
    2.422448e+00,2.360566e+00,2.302559e+00,2.248160e+00,2.197125e+00,2.149229e+00,2.104262e+00,
    2.062033e+00,2.022365e+00,1.985093e+00,1.950064e+00,1.917138e+00,1.886183e+00,1.857078e+00,
    1.829709e+00,1.803970e+00,1.779763e+00,1.756997e+00,1.735586e+00,1.715451e+00,1.696516e+00,
    1.678712e+00,1.661974e+00,1.646241e+00,1.631456e+00,1.617565e+00,1.604520e+00,1.592272e+00,
    1.580780e+00,1.570002e+00,1.559901e+00,1.550441e+00,1.541590e+00,1.533317e+00,1.525595e+00,
    1.518397e+00,1.511701e+00,1.505485e+00,1.499729e+00,1.494416e+00,1.489531e+00,1.485061e+00};

  static const G4double SL13[nL]={
    5.979521e-04,1.787895e-03,4.384312e-03,9.755476e-03,2.079561e-02,4.366898e-02,9.094059e-02,
    1.867226e-01,3.746609e-01,7.299098e-01,1.376720e+00,2.513601e+00,4.446871e+00,7.627694e+00,
    1.267423e+01,2.032656e+01,3.102537e+01,4.279863e+01,4.924268e+01,4.764583e+01,4.223031e+01,
    3.635559e+01,3.114336e+01,2.680306e+01,2.327037e+01,2.041256e+01,1.809450e+01,1.619914e+01,
    1.463175e+01,1.331842e+01,1.220266e+01,1.124188e+01,1.040420e+01,9.665816e+00,9.008883e+00,
    8.419927e+00,7.888645e+00,7.407021e+00,6.968700e+00,6.568538e+00,6.202287e+00,5.866370e+00,
    5.557733e+00,5.273725e+00,5.012027e+00,4.770588e+00,4.547585e+00,4.341389e+00,4.150539e+00,
    3.973720e+00,3.809747e+00,3.657550e+00,3.516159e+00,3.384698e+00,3.262371e+00,3.148455e+00,
    3.042294e+00,2.943289e+00,2.850895e+00,2.764617e+00,2.683999e+00,2.608629e+00,2.538126e+00,
    2.472142e+00,2.410358e+00,2.352483e+00,2.298245e+00,2.247399e+00,2.199714e+00,2.154982e+00,
    2.113006e+00,2.073607e+00,2.036619e+00,2.001888e+00,1.969270e+00,1.938632e+00,1.909852e+00,
    1.882814e+00,1.857411e+00,1.833544e+00,1.811120e+00,1.790052e+00,1.770261e+00,1.751669e+00,
    1.734206e+00,1.717807e+00,1.702410e+00,1.687957e+00,1.674395e+00,1.661672e+00,1.649742e+00,
    1.638561e+00,1.628089e+00,1.618286e+00,1.609117e+00,1.600551e+00,1.592555e+00,1.585102e+00,
    1.578167e+00,1.571726e+00,1.565757e+00,1.560241e+00,1.555161e+00,1.550502e+00,1.546251e+00};

  static const G4double SL14[nL]={
    7.595609e-04,2.174487e-03,5.184472e-03,1.148979e-02,2.501660e-02,5.458957e-02,1.187206e-01,
    2.534357e-01,5.242273e-01,1.043266e+00,1.992371e+00,3.648981e+00,6.401444e+00,1.071384e+01,
    1.696937e+01,2.517085e+01,3.466133e+01,4.362657e+01,4.818786e+01,4.632665e+01,4.110517e+01,
    3.547368e+01,3.049881e+01,2.637019e+01,2.301312e+01,2.029373e+01,1.808104e+01,1.626396e+01,
    1.475365e+01,1.348118e+01,1.239409e+01,1.145289e+01,1.062807e+01,9.897589e+00,9.244965e+00,
    8.657732e+00,8.126345e+00,7.643355e+00,7.202834e+00,6.799954e+00,6.430699e+00,6.091668e+00,
    5.779927e+00,5.492917e+00,5.228379e+00,4.984303e+00,4.758895e+00,4.550538e+00,4.357779e+00,
    4.179304e+00,4.013924e+00,3.860561e+00,3.718237e+00,3.586064e+00,3.463234e+00,3.349014e+00,
    3.242733e+00,3.143782e+00,3.051603e+00,2.965688e+00,2.885571e+00,2.810827e+00,2.741066e+00,
    2.675929e+00,2.615087e+00,2.558239e+00,2.505107e+00,2.455433e+00,2.408981e+00,2.365534e+00,
    2.324889e+00,2.286859e+00,2.251272e+00,2.217967e+00,2.186795e+00,2.157618e+00,2.130308e+00,
    2.104744e+00,2.080815e+00,2.058418e+00,2.037456e+00,2.017838e+00,1.999480e+00,1.982303e+00,
    1.966234e+00,1.951205e+00,1.937150e+00,1.924011e+00,1.911731e+00,1.900259e+00,1.889545e+00,
    1.879545e+00,1.870216e+00,1.861520e+00,1.853420e+00,1.845884e+00,1.838880e+00,1.832382e+00,
    1.826362e+00,1.820798e+00,1.815670e+00,1.810960e+00,1.806650e+00,1.802729e+00,1.799183e+00};

  static const G4double SL15[nL]={
    8.500963e-04,2.390172e-03,5.632030e-03,1.247632e-02,2.747950e-02,6.109914e-02,1.355108e-01,
    2.941224e-01,6.161245e-01,1.237476e+00,2.378852e+00,4.376594e+00,7.697785e+00,1.288755e+01,
    2.037233e+01,3.017649e+01,4.195065e+01,5.485775e+01,6.453653e+01,6.432845e+01,5.643212e+01,
    4.707792e+01,3.899572e+01,3.258608e+01,2.760535e+01,2.373102e+01,2.068965e+01,1.827124e+01,
    1.631941e+01,1.471906e+01,1.338580e+01,1.225785e+01,1.128994e+01,1.044881e+01,9.709837e+00,
    9.054659e+00,8.469380e+00,7.943327e+00,7.468155e+00,7.037214e+00,6.645102e+00,6.287353e+00,
    5.960212e+00,5.660481e+00,5.385398e+00,5.132559e+00,4.899852e+00,4.685413e+00,4.487583e+00,
    4.304884e+00,4.135994e+00,3.979725e+00,3.835009e+00,3.700883e+00,3.576477e+00,3.461003e+00,
    3.353748e+00,3.254064e+00,3.161362e+00,3.075105e+00,2.994806e+00,2.920017e+00,2.850330e+00,
    2.785372e+00,2.724800e+00,2.668301e+00,2.615584e+00,2.566383e+00,2.520455e+00,2.477573e+00,
    2.437529e+00,2.400129e+00,2.365194e+00,2.332560e+00,2.302073e+00,2.273590e+00,2.246978e+00,
    2.222116e+00,2.198888e+00,2.177188e+00,2.156916e+00,2.137979e+00,2.120292e+00,2.103773e+00,
    2.088348e+00,2.073945e+00,2.060501e+00,2.047953e+00,2.036245e+00,2.025323e+00,2.015140e+00,
    2.005648e+00,1.996806e+00,1.988574e+00,1.980915e+00,1.973798e+00,1.967190e+00,1.961065e+00,
    1.955397e+00,1.950164e+00,1.945345e+00,1.940923e+00,1.936883e+00,1.933213e+00,1.929901e+00};

  static const G4double SL16[nL]={
    1.161977e-03,3.130797e-03,7.178175e-03,1.596595e-02,3.647036e-02,8.543942e-02,1.991615e-01,
    4.493705e-01,9.672255e-01,1.976461e+00,3.832134e+00,7.044334e+00,1.221939e+01,1.977226e+01,
    2.929890e+01,3.906811e+01,4.690664e+01,5.234861e+01,5.669474e+01,5.908286e+01,5.608983e+01,
    4.880825e+01,4.096475e+01,3.429691e+01,2.901943e+01,2.490513e+01,2.168321e+01,1.913006e+01,
    1.707654e+01,1.539807e+01,1.400363e+01,1.282689e+01,1.181939e+01,1.094565e+01,1.017948e+01,
    9.501359e+00,8.896578e+00,8.353847e+00,7.864355e+00,7.421093e+00,7.018379e+00,6.651519e+00,
    6.316575e+00,6.010193e+00,5.729483e+00,5.471930e+00,5.235327e+00,5.017723e+00,4.817386e+00,
    4.632774e+00,4.462505e+00,4.305337e+00,4.160155e+00,4.025952e+00,3.901818e+00,3.786931e+00,
    3.680544e+00,3.581977e+00,3.490615e+00,3.405895e+00,3.327304e+00,3.254375e+00,3.186678e+00,
    3.123820e+00,3.065443e+00,3.011214e+00,2.960830e+00,2.914011e+00,2.870498e+00,2.830053e+00,
    2.792457e+00,2.757506e+00,2.725010e+00,2.694797e+00,2.666703e+00,2.640580e+00,2.616286e+00,
    2.593694e+00,2.572682e+00,2.553138e+00,2.534958e+00,2.518045e+00,2.502310e+00,2.487667e+00,
    2.474040e+00,2.461354e+00,2.449544e+00,2.438546e+00,2.428302e+00,2.418759e+00,2.409866e+00,
    2.401579e+00,2.393854e+00,2.386654e+00,2.379943e+00,2.373689e+00,2.367864e+00,2.362442e+00,
    2.357400e+00,2.352718e+00,2.348380e+00,2.344371e+00,2.340680e+00,2.337299e+00,2.334221e+00};

  static const G4double SL17[nL]={
    2.137065e-03,5.442007e-03,1.210645e-02,2.774945e-02,6.888202e-02,1.771769e-01,4.450546e-01,
    1.057471e+00,2.354951e+00,4.918482e+00,9.652965e+00,1.776486e+01,3.037627e+01,4.763569e+01,
    6.860085e+01,9.419933e+01,1.267075e+02,1.511993e+02,1.442063e+02,1.180149e+02,9.193020e+01,
    7.155618e+01,5.654375e+01,4.555533e+01,3.744235e+01,3.137117e+01,2.675825e+01,2.319641e+01,
    2.039992e+01,1.816677e+01,1.635312e+01,1.485589e+01,1.360078e+01,1.253381e+01,1.161550e+01,
    1.081664e+01,1.011536e+01,9.495083e+00,8.943002e+00,8.449085e+00,8.005329e+00,7.605245e+00,
    7.243487e+00,6.915590e+00,6.617778e+00,6.346818e+00,6.099922e+00,5.874662e+00,5.668909e+00,
    5.480788e+00,5.308639e+00,5.150985e+00,5.006505e+00,4.874020e+00,4.752467e+00,4.640891e+00,
    4.538429e+00,4.444301e+00,4.357799e+00,4.278281e+00,4.205160e+00,4.137906e+00,4.076030e+00,
    4.019090e+00,3.966677e+00,3.918419e+00,3.873973e+00,3.833027e+00,3.795290e+00,3.760498e+00,
    3.728406e+00,3.698788e+00,3.671436e+00,3.646160e+00,3.622782e+00,3.601139e+00,3.581082e+00,
    3.562470e+00,3.545177e+00,3.529084e+00,3.514082e+00,3.500071e+00,3.486960e+00,3.474663e+00,
    3.463103e+00,3.452209e+00,3.441916e+00,3.432164e+00,3.422901e+00,3.414078e+00,3.405652e+00,
    3.397582e+00,3.389836e+00,3.382382e+00,3.375196e+00,3.368254e+00,3.361538e+00,3.355034e+00,
    3.348730e+00,3.342620e+00,3.336699e+00,3.330967e+00,3.325427e+00,3.320085e+00,3.314951e+00};

  static const G4double SL18[nL]={
    2.220534e-03,5.640053e-03,1.253572e-02,2.881392e-02,7.191580e-02,1.859408e-01,4.687157e-01,
    1.115760e+00,2.485562e+00,5.183559e+00,1.013008e+01,1.847496e+01,3.103145e+01,4.701870e+01,
    6.345164e+01,7.777111e+01,8.950804e+01,9.321427e+01,8.410731e+01,6.975786e+01,5.670984e+01,
    4.641759e+01,3.856198e+01,3.257293e+01,2.796698e+01,2.438084e+01,2.154901e+01,1.927832e+01,
    1.742802e+01,1.589540e+01,1.460538e+01,1.350313e+01,1.254846e+01,1.171188e+01,1.097157e+01,
    1.031123e+01,9.718498e+00,9.183826e+00,8.699693e+00,8.260038e+00,7.859873e+00,7.495011e+00,
    7.161876e+00,6.857372e+00,6.578785e+00,6.323715e+00,6.090025e+00,5.875801e+00,5.679326e+00,
    5.499048e+00,5.333567e+00,5.181614e+00,5.042039e+00,4.913795e+00,4.795932e+00,4.687583e+00,
    4.587960e+00,4.496341e+00,4.412068e+00,4.334539e+00,4.263201e+00,4.197551e+00,4.137125e+00,
    4.081496e+00,4.030275e+00,3.983101e+00,3.939643e+00,3.899598e+00,3.862684e+00,3.828641e+00,
    3.797233e+00,3.768237e+00,3.741451e+00,3.716686e+00,3.693770e+00,3.672542e+00,3.652854e+00,
    3.634571e+00,3.617565e+00,3.601721e+00,3.586931e+00,3.573099e+00,3.560132e+00,3.547947e+00,
    3.536470e+00,3.525629e+00,3.515361e+00,3.505610e+00,3.496321e+00,3.487449e+00,3.478950e+00,
    3.470787e+00,3.462928e+00,3.455342e+00,3.448006e+00,3.440898e+00,3.434002e+00,3.427303e+00,
    3.420792e+00,3.414463e+00,3.408314e+00,3.402345e+00,3.396560e+00,3.390968e+00,3.385579e+00};

  static const G4double SL19[nL]={
    2.305897e-03,5.842654e-03,1.297593e-02,2.991119e-02,7.506153e-02,1.950960e-01,4.938019e-01,
    1.179632e+00,2.638978e+00,5.539887e+00,1.095013e+01,2.037657e+01,3.550284e+01,5.759776e+01,
    8.715375e+01,1.188643e+02,1.303680e+02,1.150932e+02,9.265204e+01,7.339629e+01,5.867930e+01,
    4.768315e+01,3.944920e+01,3.322398e+01,2.845918e+01,2.476205e+01,2.185087e+01,1.952250e+01,
    1.762968e+01,1.606532e+01,1.475137e+01,1.363085e+01,1.266212e+01,1.181463e+01,1.106578e+01,
    1.039872e+01,9.800670e+00,9.261781e+00,8.774299e+00,8.331989e+00,7.929726e+00,7.563221e+00,
    7.228815e+00,6.923346e+00,6.644047e+00,6.388477e+00,6.154464e+00,5.940066e+00,5.743540e+00,
    5.563316e+00,5.397976e+00,5.246237e+00,5.106936e+00,4.979015e+00,4.861515e+00,4.753562e+00,
    4.654358e+00,4.563177e+00,4.479354e+00,4.402282e+00,4.331405e+00,4.266213e+00,4.206241e+00,
    4.151059e+00,4.100274e+00,4.053523e+00,4.010474e+00,3.970819e+00,3.934277e+00,3.900587e+00,
    3.869510e+00,3.840823e+00,3.814323e+00,3.789821e+00,3.767142e+00,3.746128e+00,3.726629e+00,
    3.708509e+00,3.691642e+00,3.675912e+00,3.661213e+00,3.647445e+00,3.634520e+00,3.622354e+00,
    3.610872e+00,3.600004e+00,3.589688e+00,3.579867e+00,3.570489e+00,3.561507e+00,3.552881e+00,
    3.544574e+00,3.536552e+00,3.528789e+00,3.521261e+00,3.513947e+00,3.506832e+00,3.499903e+00,
    3.493151e+00,3.486572e+00,3.480165e+00,3.473930e+00,3.467875e+00,3.462007e+00,3.456340e+00};

  static const G4double SL20[nL]={
    2.545914e-03,6.412659e-03,1.422001e-02,3.303967e-02,8.409149e-02,2.213646e-01,5.649122e-01,
    1.354715e+00,3.029540e+00,6.323258e+00,1.232016e+01,2.225805e+01,3.662567e+01,5.344971e+01,
    6.796031e+01,7.669870e+01,8.176394e+01,8.725461e+01,8.966246e+01,8.202204e+01,6.857177e+01,
    5.574957e+01,4.551980e+01,3.772808e+01,3.182031e+01,2.730095e+01,2.379636e+01,2.103601e+01,
    1.882555e+01,1.702523e+01,1.553424e+01,1.427965e+01,1.320850e+01,1.228212e+01,1.147206e+01,
    1.075719e+01,1.012157e+01,9.553019e+00,9.042024e+00,8.581021e+00,8.163877e+00,7.785521e+00,
    7.441691e+00,7.128756e+00,6.843578e+00,6.583419e+00,6.345874e+00,6.128812e+00,5.930337e+00,
    5.748752e+00,5.582537e+00,5.430325e+00,5.290879e+00,5.163086e+00,5.045934e+00,4.938508e+00,
    4.839973e+00,4.749573e+00,4.666616e+00,4.590473e+00,4.520568e+00,4.456376e+00,4.397414e+00,
    4.343241e+00,4.293452e+00,4.247676e+00,4.205572e+00,4.166825e+00,4.131148e+00,4.098275e+00,
    4.067961e+00,4.039982e+00,4.014130e+00,3.990215e+00,3.968060e+00,3.947505e+00,3.928399e+00,
    3.910607e+00,3.894002e+00,3.878468e+00,3.863899e+00,3.850198e+00,3.837275e+00,3.825050e+00,
    3.813448e+00,3.802402e+00,3.791850e+00,3.781739e+00,3.772017e+00,3.762641e+00,3.753573e+00,
    3.744778e+00,3.736225e+00,3.727891e+00,3.719753e+00,3.711795e+00,3.704003e+00,3.696369e+00,
    3.688887e+00,3.681555e+00,3.674374e+00,3.667351e+00,3.660494e+00,3.653816e+00,3.647333e+00};

  static const G4double SL21[nL]={
    2.564250e-03,6.456227e-03,1.431544e-02,3.328130e-02,8.479343e-02,2.234161e-01,5.704940e-01,
    1.368585e+00,3.061075e+00,6.389076e+00,1.244551e+01,2.247024e+01,3.692501e+01,5.375094e+01,
    6.804530e+01,7.622389e+01,8.041323e+01,8.592812e+01,9.222352e+01,8.976000e+01,7.746020e+01,
    6.319441e+01,5.115788e+01,4.188960e+01,3.488950e+01,2.958086e+01,2.550619e+01,2.233120e+01,
    1.981637e+01,1.779045e+01,1.613068e+01,1.474867e+01,1.358052e+01,1.257968e+01,1.171205e+01,
    1.095233e+01,1.028156e+01,9.685270e+00,9.152260e+00,8.673689e+00,8.242451e+00,7.852734e+00,
    7.499706e+00,7.179292e+00,6.888008e+00,6.622849e+00,6.381195e+00,6.160747e+00,5.959474e+00,
    5.775573e+00,5.607438e+00,5.453629e+00,5.312856e+00,5.183957e+00,5.065885e+00,4.957691e+00,
    4.858517e+00,4.767585e+00,4.684186e+00,4.607676e+00,4.537466e+00,4.473021e+00,4.413851e+00,
    4.359506e+00,4.309575e+00,4.263683e+00,4.221483e+00,4.182658e+00,4.146917e+00,4.113990e+00,
    4.083632e+00,4.055616e+00,4.029732e+00,4.005789e+00,3.983609e+00,3.963030e+00,3.943902e+00,
    3.926087e+00,3.909459e+00,3.893901e+00,3.879307e+00,3.865579e+00,3.852627e+00,3.840370e+00,
    3.828735e+00,3.817652e+00,3.807062e+00,3.796909e+00,3.787144e+00,3.777723e+00,3.768606e+00,
    3.759759e+00,3.751154e+00,3.742764e+00,3.734569e+00,3.726552e+00,3.718700e+00,3.711004e+00,
    3.703458e+00,3.696062e+00,3.688816e+00,3.681726e+00,3.674803e+00,3.668058e+00,3.661508e+00};

  static const G4double SL22[nL]={
    3.007427e-03,7.510236e-03,1.663782e-02,3.922952e-02,1.022486e-01,2.747505e-01,7.108351e-01,
    1.719543e+00,3.868385e+00,8.112125e+00,1.586986e+01,2.877567e+01,4.762099e+01,7.091149e+01,
    9.555037e+01,1.153230e+02,1.159518e+02,9.952423e+01,8.059227e+01,6.490265e+01,5.285116e+01,
    4.371769e+01,3.676918e+01,3.143378e+01,2.728996e+01,2.403042e+01,2.143067e+01,1.932609e+01,
    1.759555e+01,1.614984e+01,1.492332e+01,1.386771e+01,1.294753e+01,1.213663e+01,1.141562e+01,
    1.076997e+01,1.018862e+01,9.662970e+00,9.186212e+00,8.752798e+00,8.358120e+00,7.998266e+00,
    7.669857e+00,7.369937e+00,7.095886e+00,6.845367e+00,6.616282e+00,6.406739e+00,6.215024e+00,
    6.039585e+00,5.879011e+00,5.732019e+00,5.597439e+00,5.474206e+00,5.361347e+00,5.257976e+00,
    5.163281e+00,5.076520e+00,4.997014e+00,4.924143e+00,4.857338e+00,4.796075e+00,4.739877e+00,
    4.688303e+00,4.640952e+00,4.597451e+00,4.557461e+00,4.520669e+00,4.486786e+00,4.455548e+00,
    4.426712e+00,4.400054e+00,4.375369e+00,4.352466e+00,4.331174e+00,4.311331e+00,4.292793e+00,
    4.275425e+00,4.259104e+00,4.243719e+00,4.229166e+00,4.215352e+00,4.202192e+00,4.189610e+00,
    4.177536e+00,4.165907e+00,4.154667e+00,4.143766e+00,4.133160e+00,4.122809e+00,4.112681e+00,
    4.102746e+00,4.092980e+00,4.083363e+00,4.073880e+00,4.064520e+00,4.055276e+00,4.046143e+00,
    4.037123e+00,4.028220e+00,4.019441e+00,4.010799e+00,4.002309e+00,3.993991e+00,3.985867e+00};

  static const G4double SL23[nL]={
    3.202591e-03,7.975022e-03,1.767003e-02,4.191214e-02,1.102179e-01,2.983388e-01,7.754839e-01,
    1.881188e+00,4.239060e+00,8.896849e+00,1.740204e+01,3.149771e+01,5.193761e+01,7.708634e+01,
    1.037149e+02,1.232794e+02,1.201643e+02,1.011825e+02,8.136267e+01,6.536884e+01,5.318799e+01,
    4.398670e+01,3.699713e+01,3.163451e+01,2.747158e+01,2.419807e+01,2.158786e+01,1.947527e+01,
    1.773851e+01,1.628791e+01,1.505750e+01,1.399877e+01,1.307609e+01,1.226319e+01,1.154059e+01,
    1.089372e+01,1.031145e+01,9.785155e+00,9.307994e+00,8.874394e+00,8.479723e+00,8.120049e+00,
    7.791975e+00,7.492525e+00,7.219066e+00,6.969243e+00,6.740944e+00,6.532261e+00,6.341471e+00,
    6.167007e+00,6.007447e+00,5.861497e+00,5.727978e+00,5.605816e+00,5.494030e+00,5.391724e+00,
    5.298080e+00,5.212351e+00,5.133851e+00,5.061955e+00,4.996087e+00,4.935723e+00,4.880379e+00,
    4.829614e+00,4.783020e+00,4.740226e+00,4.700887e+00,4.664691e+00,4.631348e+00,4.600593e+00,
    4.572181e+00,4.545889e+00,4.521511e+00,4.498859e+00,4.477759e+00,4.458053e+00,4.439594e+00,
    4.422252e+00,4.405903e+00,4.390437e+00,4.375754e+00,4.361761e+00,4.348375e+00,4.335521e+00,
    4.323132e+00,4.311147e+00,4.299511e+00,4.288177e+00,4.277103e+00,4.266251e+00,4.255591e+00,
    4.245096e+00,4.234743e+00,4.224516e+00,4.214402e+00,4.204390e+00,4.194477e+00,4.184662e+00,
    4.174947e+00,4.165339e+00,4.155849e+00,4.146491e+00,4.137283e+00,4.128247e+00,4.119408e+00};

  static const G4double SL24[nL]={
    4.424391e-03,1.089365e-02,2.425454e-02,5.950323e-02,1.636625e-01,4.585667e-01,1.218434e+00,
    3.000059e+00,6.849073e+00,1.459387e+01,2.913751e+01,5.434530e+01,9.302757e+01,1.344961e+02,
    1.455690e+02,1.285470e+02,1.059779e+02,8.603243e+01,7.002571e+01,5.756765e+01,4.795028e+01,
    4.051569e+01,3.473537e+01,3.020519e+01,2.662094e+01,2.375420e+01,2.143316e+01,1.952845e+01,
    1.794276e+01,1.660309e+01,1.545498e+01,1.445799e+01,1.358216e+01,1.280530e+01,1.211087e+01,
    1.148642e+01,1.092239e+01,1.041132e+01,9.947182e+00,9.525039e+00,9.140708e+00,8.790584e+00,
    8.471504e+00,8.180659e+00,7.915525e+00,7.673825e+00,7.453491e+00,7.252639e+00,7.069553e+00,
    6.902666e+00,6.750545e+00,6.611879e+00,6.485472e+00,6.370227e+00,6.265143e+00,6.169303e+00,
    6.081868e+00,6.002071e+00,5.929208e+00,5.862637e+00,5.801770e+00,5.746067e+00,5.695037e+00,
    5.648227e+00,5.605225e+00,5.565653e+00,5.529167e+00,5.495449e+00,5.464212e+00,5.435193e+00,
    5.408150e+00,5.382864e+00,5.359136e+00,5.336785e+00,5.315644e+00,5.295565e+00,5.276412e+00,
    5.258062e+00,5.240405e+00,5.223341e+00,5.206783e+00,5.190650e+00,5.174872e+00,5.159387e+00,
    5.144141e+00,5.129087e+00,5.114183e+00,5.099396e+00,5.084697e+00,5.070063e+00,5.055476e+00,
    5.040923e+00,5.026397e+00,5.011893e+00,4.997412e+00,4.982959e+00,4.968544e+00,4.954178e+00,
    4.939879e+00,4.925668e+00,4.911569e+00,4.897612e+00,4.883829e+00,4.870257e+00,4.856937e+00};

  static const G4double SL25[nL]={
    5.218262e-03,1.279812e-02,2.863691e-02,7.159626e-02,2.012920e-01,5.725263e-01,1.533263e+00,
    3.785207e+00,8.620686e+00,1.819011e+01,3.550779e+01,6.346712e+01,1.028732e+02,1.498888e+02,
    1.795448e+02,1.653851e+02,1.332773e+02,1.044804e+02,8.246369e+01,6.611941e+01,5.397243e+01,
    4.486079e+01,3.794689e+01,3.263665e+01,2.850697e+01,2.525335e+01,2.265438e+01,2.054771e+01,
    1.881372e+01,1.736415e+01,1.613394e+01,1.507520e+01,1.415268e+01,1.334041e+01,1.261911e+01,
    1.197431e+01,1.139498e+01,1.087253e+01,1.040012e+01,9.972149e+00,9.583943e+00,9.231507e+00,
    8.911366e+00,8.620462e+00,8.356070e+00,8.115746e+00,7.897283e+00,7.698685e+00,7.518138e+00,
    7.353992e+00,7.204745e+00,7.069027e+00,6.945589e+00,6.833293e+00,6.731099e+00,6.638057e+00,
    6.553303e+00,6.476044e+00,6.405559e+00,6.341189e+00,6.282333e+00,6.228441e+00,6.179012e+00,
    6.133589e+00,6.091756e+00,6.053132e+00,6.017372e+00,5.984159e+00,5.953209e+00,5.924259e+00,
    5.897073e+00,5.871438e+00,5.847158e+00,5.824059e+00,5.801982e+00,5.780784e+00,5.760337e+00,
    5.740528e+00,5.721253e+00,5.702421e+00,5.683953e+00,5.665777e+00,5.647831e+00,5.630063e+00,
    5.612426e+00,5.594882e+00,5.577398e+00,5.559949e+00,5.542515e+00,5.525080e+00,5.507636e+00,
    5.490177e+00,5.472703e+00,5.455220e+00,5.437735e+00,5.420261e+00,5.402815e+00,5.385418e+00,
    5.368096e+00,5.350876e+00,5.333791e+00,5.316880e+00,5.300182e+00,5.283744e+00,5.267615e+00};

  static const G4double SL26[nL]={
    9.533418e-03,2.324917e-02,5.364098e-02,1.447139e-01,4.381268e-01,1.303754e+00,3.571583e+00,
    8.890991e+00,2.014541e+01,4.138069e+01,7.546159e+01,1.179996e+02,1.568622e+02,1.907924e+02,
    2.305942e+02,2.457159e+02,2.095925e+02,1.607399e+02,1.215717e+02,9.340695e+01,7.340074e+01,
    5.903474e+01,4.855591e+01,4.078885e+01,3.494131e+01,3.047098e+01,2.700048e+01,2.426339e+01,
    2.206960e+01,2.028240e+01,1.880303e+01,1.755986e+01,1.650082e+01,1.558780e+01,1.479275e+01,
    1.409475e+01,1.347801e+01,1.293034e+01,1.244217e+01,1.200582e+01,1.161497e+01,1.126435e+01,
    1.094945e+01,1.066638e+01,1.041174e+01,1.018252e+01,9.976076e+00,9.790007e+00,9.622186e+00,
    9.470694e+00,9.333803e+00,9.209956e+00,9.097750e+00,8.995915e+00,8.903308e+00,8.818895e+00,
    8.741745e+00,8.671018e+00,8.605956e+00,8.545878e+00,8.490170e+00,8.438281e+00,8.389716e+00,
    8.344033e+00,8.300834e+00,8.259766e+00,8.220512e+00,8.182794e+00,8.146361e+00,8.110993e+00,
    8.076498e+00,8.042705e+00,8.009465e+00,7.976650e+00,7.944147e+00,7.911862e+00,7.879713e+00,
    7.847632e+00,7.815563e+00,7.783460e+00,7.751287e+00,7.719017e+00,7.686631e+00,7.654117e+00,
    7.621469e+00,7.588688e+00,7.555781e+00,7.522758e+00,7.489637e+00,7.456437e+00,7.423184e+00,
    7.389908e+00,7.356640e+00,7.323419e+00,7.290286e+00,7.257286e+00,7.224468e+00,7.191884e+00,
    7.159594e+00,7.127657e+00,7.096141e+00,7.065118e+00,7.034663e+00,7.004858e+00,6.975791e+00};

  static const G4double SL27[nL]={
    1.043535e-02,2.545247e-02,5.908485e-02,1.613411e-01,4.935021e-01,1.477078e+00,4.058976e+00,
    1.012392e+01,2.297013e+01,4.720890e+01,8.609450e+01,1.355874e+02,1.876865e+02,2.480184e+02,
    2.968858e+02,2.729073e+02,2.110699e+02,1.571879e+02,1.183563e+02,9.114148e+01,7.189981e+01,
    5.807038e+01,4.795957e+01,4.044610e+01,3.477555e+01,3.043070e+01,2.705064e+01,2.437987e+01,
    2.223556e+01,2.048595e+01,1.903566e+01,1.781544e+01,1.677484e+01,1.587694e+01,1.509453e+01,
    1.440731e+01,1.379991e+01,1.326049e+01,1.277969e+01,1.235001e+01,1.196525e+01,1.162022e+01,
    1.131048e+01,1.103217e+01,1.078194e+01,1.055679e+01,1.035408e+01,1.017143e+01,1.000671e+01,
    9.858006e+00,9.723600e+00,9.601935e+00,9.491608e+00,9.391355e+00,9.300037e+00,9.216625e+00,
    9.140195e+00,9.069912e+00,9.005027e+00,8.944866e+00,8.888826e+00,8.836365e+00,8.786998e+00,
    8.740293e+00,8.695865e+00,8.653370e+00,8.612505e+00,8.573001e+00,8.534620e+00,8.497153e+00,
    8.460418e+00,8.424256e+00,8.388530e+00,8.353120e+00,8.317926e+00,8.282862e+00,8.247857e+00,
    8.212851e+00,8.177798e+00,8.142660e+00,8.107410e+00,8.072027e+00,8.036502e+00,8.000828e+00,
    7.965007e+00,7.929047e+00,7.892960e+00,7.856763e+00,7.820479e+00,7.784135e+00,7.747759e+00,
    7.711389e+00,7.675061e+00,7.638818e+00,7.602707e+00,7.566777e+00,7.531083e+00,7.495683e+00,
    7.460641e+00,7.426023e+00,7.391901e+00,7.358353e+00,7.325461e+00,7.293313e+00,7.262004e+00};

  static const G4double SL28[nL]={
    1.177612e-02,2.873855e-02,6.729897e-02,1.868027e-01,5.790453e-01,1.745770e+00,4.814971e+00,
    1.203191e+01,2.730811e+01,5.598968e+01,1.014774e+02,1.592489e+02,2.225285e+02,2.776163e+02,
    2.688536e+02,2.132293e+02,1.608280e+02,1.222362e+02,9.485335e+01,7.529219e+01,6.110006e+01,
    5.063436e+01,4.279795e+01,3.684661e+01,3.226548e+01,2.869156e+01,2.586467e+01,2.359614e+01,
    2.174808e+01,2.021937e+01,1.893580e+01,1.784291e+01,1.690071e+01,1.607977e+01,1.535827e+01,
    1.471984e+01,1.415200e+01,1.364501e+01,1.319113e+01,1.278403e+01,1.241840e+01,1.208972e+01,
    1.179407e+01,1.152799e+01,1.128840e+01,1.107255e+01,1.087797e+01,1.070243e+01,1.054390e+01,
    1.040057e+01,1.027077e+01,1.015302e+01,1.004596e+01,9.948371e+00,9.859147e+00,9.777296e+00,
    9.701922e+00,9.632220e+00,9.567466e+00,9.507013e+00,9.450281e+00,9.396751e+00,9.345963e+00,
    9.297506e+00,9.251017e+00,9.206173e+00,9.162691e+00,9.120322e+00,9.078848e+00,9.038079e+00,
    8.997851e+00,8.958023e+00,8.918474e+00,8.879101e+00,8.839819e+00,8.800557e+00,8.761258e+00,
    8.721877e+00,8.682378e+00,8.642738e+00,8.602939e+00,8.562975e+00,8.522843e+00,8.482548e+00,
    8.442103e+00,8.401522e+00,8.360828e+00,8.320046e+00,8.279205e+00,8.238341e+00,8.197491e+00,
    8.156696e+00,8.116003e+00,8.075461e+00,8.035123e+00,7.995046e+00,7.955292e+00,7.915926e+00,
    7.877018e+00,7.838644e+00,7.800883e+00,7.763820e+00,7.727547e+00,7.692162e+00,7.657767e+00};

  static const G4double SL29[nL]={
    1.365967e-02,3.337537e-02,7.906748e-02,2.239693e-01,7.052155e-01,2.143189e+00,5.929377e+00,
    1.480860e+01,3.341772e+01,6.741315e+01,1.178596e+02,1.735378e+02,2.243020e+02,2.786430e+02,
    3.008511e+02,2.541744e+02,1.921484e+02,1.437224e+02,1.094876e+02,8.542953e+01,6.828273e+01,
    5.584793e+01,4.667796e+01,3.980970e+01,3.458954e+01,3.056493e+01,2.741696e+01,2.491771e+01,
    2.290273e+01,2.125263e+01,1.988051e+01,1.872296e+01,1.773367e+01,1.687863e+01,1.613270e+01,
    1.547709e+01,1.489750e+01,1.438287e+01,1.392442e+01,1.351505e+01,1.314884e+01,1.282080e+01,
    1.252665e+01,1.226263e+01,1.202543e+01,1.181213e+01,1.162010e+01,1.144700e+01,1.129071e+01,
    1.114933e+01,1.102117e+01,1.090467e+01,1.079846e+01,1.070131e+01,1.061208e+01,1.052979e+01,
    1.045353e+01,1.038251e+01,1.031602e+01,1.025342e+01,1.019413e+01,1.013767e+01,1.008359e+01,
    1.003150e+01,9.981059e+00,9.931961e+00,9.883946e+00,9.836788e+00,9.790289e+00,9.744282e+00,
    9.698622e+00,9.653189e+00,9.607882e+00,9.562616e+00,9.517324e+00,9.471954e+00,9.426464e+00,
    9.380825e+00,9.335018e+00,9.289031e+00,9.242863e+00,9.196518e+00,9.150008e+00,9.103348e+00,
    9.056561e+00,9.009673e+00,8.962717e+00,8.915727e+00,8.868742e+00,8.821806e+00,8.774965e+00,
    8.728269e+00,8.681773e+00,8.635535e+00,8.589616e+00,8.544082e+00,8.499003e+00,8.454454e+00,
    8.410514e+00,8.367269e+00,8.324807e+00,8.283225e+00,8.242627e+00,8.203121e+00,8.164824e+00};

  static const G4double SL30[nL]={
    2.103117e-02,5.172350e-02,1.273601e-01,3.830439e-01,1.257967e+00,3.901184e+00,1.087470e+01,
    2.709930e+01,6.005843e+01,1.151063e+02,1.807622e+02,2.248182e+02,2.337093e+02,2.305565e+02,
    2.282780e+02,2.025860e+02,1.607669e+02,1.243617e+02,9.741054e+01,7.794111e+01,6.376177e+01,
    5.328718e+01,4.543843e+01,3.947900e+01,3.489731e+01,3.133117e+01,2.851997e+01,2.627388e+01,
    2.445368e+01,2.295694e+01,2.170826e+01,2.065218e+01,1.974793e+01,1.896539e+01,1.828223e+01,
    1.768163e+01,1.715075e+01,1.667954e+01,1.625998e+01,1.588548e+01,1.555055e+01,1.525048e+01,
    1.498116e+01,1.473901e+01,1.452086e+01,1.432386e+01,1.414548e+01,1.398346e+01,1.383576e+01,
    1.370056e+01,1.357624e+01,1.346135e+01,1.335459e+01,1.325483e+01,1.316103e+01,1.307231e+01,
    1.298788e+01,1.290703e+01,1.282916e+01,1.275374e+01,1.268031e+01,1.260848e+01,1.253790e+01,
    1.246828e+01,1.239939e+01,1.233100e+01,1.226295e+01,1.219511e+01,1.212735e+01,1.205959e+01,
    1.199175e+01,1.192379e+01,1.185567e+01,1.178738e+01,1.171889e+01,1.165022e+01,1.158138e+01,
    1.151237e+01,1.144324e+01,1.137400e+01,1.130470e+01,1.123538e+01,1.116608e+01,1.109684e+01,
    1.102773e+01,1.095880e+01,1.089011e+01,1.082171e+01,1.075367e+01,1.068607e+01,1.061897e+01,
    1.055244e+01,1.048657e+01,1.042144e+01,1.035714e+01,1.029376e+01,1.023139e+01,1.017015e+01,
    1.011014e+01,1.005149e+01,9.994313e+00,9.938752e+00,9.884949e+00,9.833059e+00,9.783245e+00};

  static const G4double SL31[nL]={
    2.164664e-02,5.326850e-02,1.315360e-01,3.972007e-01,1.307919e+00,4.061184e+00,1.132688e+01,
    2.822812e+01,6.251535e+01,1.195506e+02,1.871899e+02,2.337421e+02,2.500245e+02,2.569217e+02,
    2.417968e+02,1.964670e+02,1.512314e+02,1.169992e+02,9.234431e+01,7.451650e+01,6.143973e+01,
    5.170376e+01,4.435567e+01,3.874082e+01,3.440004e+01,3.100496e+01,2.831703e+01,2.616109e+01,
    2.440774e+01,2.296126e+01,2.175087e+01,2.072434e+01,1.984318e+01,1.907891e+01,1.841036e+01,
    1.782157e+01,1.730033e+01,1.683705e+01,1.642406e+01,1.605504e+01,1.572468e+01,1.542842e+01,
    1.516228e+01,1.492277e+01,1.470678e+01,1.451152e+01,1.433452e+01,1.417353e+01,1.402657e+01,
    1.389184e+01,1.376774e+01,1.365284e+01,1.354587e+01,1.344569e+01,1.335132e+01,1.326186e+01,
    1.317653e+01,1.309467e+01,1.301566e+01,1.293899e+01,1.286422e+01,1.279096e+01,1.271887e+01,
    1.264768e+01,1.257715e+01,1.250708e+01,1.243731e+01,1.236770e+01,1.229815e+01,1.222857e+01,
    1.215890e+01,1.208909e+01,1.201911e+01,1.194895e+01,1.187861e+01,1.180808e+01,1.173739e+01,
    1.166655e+01,1.159559e+01,1.152454e+01,1.145345e+01,1.138236e+01,1.131131e+01,1.124036e+01,
    1.116955e+01,1.109896e+01,1.102863e+01,1.095863e+01,1.088903e+01,1.081989e+01,1.075129e+01,
    1.068331e+01,1.061602e+01,1.054952e+01,1.048389e+01,1.041922e+01,1.035562e+01,1.029319e+01,
    1.023205e+01,1.017232e+01,1.011413e+01,1.005761e+01,1.000292e+01,9.950209e+00,9.899647e+00};

  static const G4double SL32[nL]={
    2.258863e-02,5.563670e-02,1.379665e-01,4.191065e-01,1.385411e+00,4.309706e+00,1.202998e+01,
    2.998579e+01,6.634936e+01,1.265293e+02,1.975930e+02,2.495457e+02,2.807339e+02,3.044005e+02,
    2.792187e+02,2.183968e+02,1.646189e+02,1.256518e+02,9.814320e+01,7.851348e+01,6.426612e+01,
    5.375330e+01,4.588048e+01,3.990576e+01,3.531490e+01,3.174407e+01,2.893143e+01,2.668630e+01,
    2.486877e+01,2.337590e+01,2.213188e+01,2.108096e+01,2.018214e+01,1.940513e+01,1.872746e+01,
    1.813222e+01,1.760647e+01,1.714012e+01,1.672508e+01,1.635475e+01,1.602359e+01,1.572684e+01,
    1.546043e+01,1.522073e+01,1.500457e+01,1.480910e+01,1.463180e+01,1.447040e+01,1.432289e+01,
    1.418745e+01,1.406248e+01,1.394655e+01,1.383837e+01,1.373683e+01,1.364092e+01,1.354977e+01,
    1.346262e+01,1.337878e+01,1.329768e+01,1.321880e+01,1.314171e+01,1.306603e+01,1.299144e+01,
    1.291767e+01,1.284450e+01,1.277173e+01,1.269921e+01,1.262681e+01,1.255444e+01,1.248202e+01,
    1.240949e+01,1.233681e+01,1.226397e+01,1.219095e+01,1.211774e+01,1.204437e+01,1.197085e+01,
    1.189720e+01,1.182346e+01,1.174967e+01,1.167586e+01,1.160209e+01,1.152840e+01,1.145484e+01,
    1.138148e+01,1.130838e+01,1.123558e+01,1.116317e+01,1.109121e+01,1.101977e+01,1.094893e+01,
    1.087876e+01,1.080935e+01,1.074079e+01,1.067317e+01,1.060658e+01,1.054114e+01,1.047695e+01,
    1.041413e+01,1.035280e+01,1.029310e+01,1.023517e+01,1.017916e+01,1.012525e+01,1.007359e+01};

  static const G4double SL33[nL]={
    2.454062e-02,6.055745e-02,1.514382e-01,4.653812e-01,1.549692e+00,4.836226e+00,1.351062e+01,
    3.362441e+01,7.394409e+01,1.387856e+02,2.098544e+02,2.505891e+02,2.554933e+02,2.445847e+02,
    2.124190e+02,1.687167e+02,1.317569e+02,1.042289e+02,8.404509e+01,6.911577e+01,5.793269e+01,
    4.945432e+01,4.295680e+01,3.792803e+01,3.399868e+01,3.089784e+01,2.842428e+01,2.642742e+01,
    2.479430e+01,2.344031e+01,2.230232e+01,2.133341e+01,2.049880e+01,1.977269e+01,1.913579e+01,
    1.857355e+01,1.807474e+01,1.763054e+01,1.723383e+01,1.687870e+01,1.656015e+01,1.627387e+01,
    1.601607e+01,1.578341e+01,1.557289e+01,1.538185e+01,1.520789e+01,1.504885e+01,1.490281e+01,
    1.476805e+01,1.464303e+01,1.452637e+01,1.441687e+01,1.431346e+01,1.421517e+01,1.412119e+01,
    1.403079e+01,1.394334e+01,1.385828e+01,1.377515e+01,1.369354e+01,1.361311e+01,1.353357e+01,
    1.345466e+01,1.337620e+01,1.329802e+01,1.321997e+01,1.314197e+01,1.306393e+01,1.298578e+01,
    1.290750e+01,1.282905e+01,1.275042e+01,1.267163e+01,1.259268e+01,1.251358e+01,1.243438e+01,
    1.235509e+01,1.227578e+01,1.219646e+01,1.211721e+01,1.203806e+01,1.195908e+01,1.188033e+01,
    1.180186e+01,1.172374e+01,1.164603e+01,1.156881e+01,1.149216e+01,1.141614e+01,1.134083e+01,
    1.126633e+01,1.119272e+01,1.112009e+01,1.104854e+01,1.097817e+01,1.090910e+01,1.084144e+01,
    1.077532e+01,1.071087e+01,1.064824e+01,1.058758e+01,1.052905e+01,1.047282e+01,1.041909e+01};

  static const G4double SL34[nL]={
    2.555084e-02,6.311099e-02,1.584856e-01,4.897911e-01,1.636776e+00,5.116560e+00,1.430535e+01,
    3.561279e+01,7.827264e+01,1.466546e+02,2.221104e+02,2.722029e+02,3.010875e+02,3.106231e+02,
    2.658424e+02,2.035783e+02,1.541383e+02,1.188706e+02,9.384637e+01,7.582817e+01,6.263287e+01,
    5.281838e+01,4.541822e+01,3.977013e+01,3.540993e+01,3.200563e+01,2.931600e+01,2.716381e+01,
    2.541809e+01,2.398190e+01,2.278352e+01,2.177003e+01,2.090240e+01,2.015174e+01,1.949657e+01,
    1.892070e+01,1.841172e+01,1.795992e+01,1.755750e+01,1.719807e+01,1.687624e+01,1.658741e+01,
    1.632757e+01,1.609322e+01,1.588122e+01,1.568882e+01,1.551353e+01,1.535316e+01,1.520573e+01,
    1.506950e+01,1.494290e+01,1.482456e+01,1.471325e+01,1.460789e+01,1.450755e+01,1.441139e+01,
    1.431870e+01,1.422884e+01,1.414128e+01,1.405556e+01,1.397128e+01,1.388810e+01,1.380574e+01,
    1.372397e+01,1.364260e+01,1.356147e+01,1.348045e+01,1.339945e+01,1.331839e+01,1.323723e+01,
    1.315592e+01,1.307446e+01,1.299284e+01,1.291106e+01,1.282914e+01,1.274710e+01,1.266499e+01,
    1.258282e+01,1.250066e+01,1.241854e+01,1.233652e+01,1.225466e+01,1.217300e+01,1.209162e+01,
    1.201058e+01,1.192993e+01,1.184976e+01,1.177014e+01,1.169113e+01,1.161282e+01,1.153529e+01,
    1.145863e+01,1.138292e+01,1.130827e+01,1.123477e+01,1.116254e+01,1.109168e+01,1.102231e+01,
    1.095457e+01,1.088860e+01,1.082454e+01,1.076255e+01,1.070280e+01,1.064547e+01,1.059075e+01};

  static const G4double SL35[nL]={
    2.764032e-02,6.840683e-02,1.732180e-01,5.412140e-01,1.820749e+00,5.707551e+00,1.596471e+01,
    3.965730e+01,8.650314e+01,1.589997e+02,2.314210e+02,2.624498e+02,2.501423e+02,2.222984e+02,
    1.892896e+02,1.529510e+02,1.218148e+02,9.800381e+01,8.015642e+01,6.671593e+01,5.649992e+01,
    4.866270e+01,4.259935e+01,3.787106e+01,3.415439e+01,3.120761e+01,2.884831e+01,2.693819e+01,
    2.537244e+01,2.407192e+01,2.297720e+01,2.204394e+01,2.123915e+01,2.053827e+01,1.992295e+01,
    1.937927e+01,1.889649e+01,1.846614e+01,1.808135e+01,1.773642e+01,1.742651e+01,1.714742e+01,
    1.689546e+01,1.666738e+01,1.646026e+01,1.627150e+01,1.609875e+01,1.593994e+01,1.579319e+01,
    1.565683e+01,1.552939e+01,1.540955e+01,1.529615e+01,1.518817e+01,1.508473e+01,1.498504e+01,
    1.488843e+01,1.479431e+01,1.470218e+01,1.461162e+01,1.452226e+01,1.443381e+01,1.434600e+01,
    1.425863e+01,1.417154e+01,1.408459e+01,1.399767e+01,1.391072e+01,1.382367e+01,1.373650e+01,
    1.364917e+01,1.356169e+01,1.347407e+01,1.338633e+01,1.329848e+01,1.321057e+01,1.312263e+01,
    1.303472e+01,1.294688e+01,1.285916e+01,1.277162e+01,1.268433e+01,1.259735e+01,1.251074e+01,
    1.242457e+01,1.233891e+01,1.225383e+01,1.216942e+01,1.208575e+01,1.200290e+01,1.192096e+01,
    1.184003e+01,1.176019e+01,1.168155e+01,1.160422e+01,1.152831e+01,1.145394e+01,1.138123e+01,
    1.131034e+01,1.124140e+01,1.117458e+01,1.111003e+01,1.104795e+01,1.098853e+01,1.093197e+01};

  static const G4double SL36[nL]={
    2.908478e-02,7.207879e-02,1.835222e-01,5.774992e-01,1.951299e+00,6.129692e+00,1.716666e+01,
    4.268370e+01,9.316524e+01,1.715413e+02,2.537232e+02,3.118305e+02,3.698348e+02,4.142092e+02,
    3.599490e+02,2.689394e+02,1.969985e+02,1.471686e+02,1.129308e+02,8.901259e+01,7.196919e+01,
    5.959877e+01,5.047252e+01,4.364148e+01,3.846005e+01,3.447932e+01,3.138128e+01,2.893751e+01,
    2.698235e+01,2.539504e+01,2.408733e+01,2.299466e+01,2.206971e+01,2.127766e+01,2.059274e+01,
    1.999559e+01,1.947149e+01,1.900901e+01,1.859907e+01,1.823428e+01,1.790856e+01,1.761673e+01,
    1.735437e+01,1.711766e+01,1.690324e+01,1.670817e+01,1.652986e+01,1.636602e+01,1.621462e+01,
    1.607388e+01,1.594222e+01,1.581828e+01,1.570083e+01,1.558882e+01,1.548134e+01,1.537759e+01,
    1.527688e+01,1.517863e+01,1.508233e+01,1.498755e+01,1.489394e+01,1.480120e+01,1.470908e+01,
    1.461738e+01,1.452595e+01,1.443465e+01,1.434339e+01,1.425209e+01,1.416072e+01,1.406924e+01,
    1.397765e+01,1.388593e+01,1.379410e+01,1.370220e+01,1.361024e+01,1.351827e+01,1.342633e+01,
    1.333447e+01,1.324275e+01,1.315122e+01,1.305994e+01,1.296897e+01,1.287838e+01,1.278824e+01,
    1.269862e+01,1.260959e+01,1.252123e+01,1.243361e+01,1.234682e+01,1.226095e+01,1.217608e+01,
    1.209230e+01,1.200973e+01,1.192845e+01,1.184859e+01,1.177025e+01,1.169358e+01,1.161869e+01,
    1.154574e+01,1.147488e+01,1.140627e+01,1.134009e+01,1.127653e+01,1.121578e+01,1.115808e+01};

  static const G4double SL37[nL]={
    2.982256e-02,7.395762e-02,1.888215e-01,5.962481e-01,2.018837e+00,6.347438e+00,1.778020e+01,
    4.418698e+01,9.625560e+01,1.763759e+02,2.587902e+02,3.153492e+02,3.717654e+02,4.152507e+02,
    3.605764e+02,2.693717e+02,1.973299e+02,1.474418e+02,1.131676e+02,8.922544e+01,7.216593e+01,
    5.978476e+01,5.065157e+01,4.381637e+01,3.863284e+01,3.465151e+01,3.155398e+01,2.911148e+01,
    2.715812e+01,2.557296e+01,2.426760e+01,2.317735e+01,2.225483e+01,2.146515e+01,2.078248e+01,
    2.018745e+01,1.966529e+01,1.920456e+01,1.879617e+01,1.843272e+01,1.810811e+01,1.781717e+01,
    1.755549e+01,1.731924e+01,1.710506e+01,1.691002e+01,1.673154e+01,1.656733e+01,1.641538e+01,
    1.627392e+01,1.614137e+01,1.601639e+01,1.589776e+01,1.578445e+01,1.567554e+01,1.557025e+01,
    1.546791e+01,1.536794e+01,1.526984e+01,1.517321e+01,1.507768e+01,1.498297e+01,1.488885e+01,
    1.479512e+01,1.470162e+01,1.460824e+01,1.451488e+01,1.442149e+01,1.432802e+01,1.423444e+01,
    1.414075e+01,1.404695e+01,1.395306e+01,1.385910e+01,1.376512e+01,1.367114e+01,1.357723e+01,
    1.348342e+01,1.338977e+01,1.329635e+01,1.320321e+01,1.311042e+01,1.301805e+01,1.292616e+01,
    1.283483e+01,1.274413e+01,1.265413e+01,1.256493e+01,1.247660e+01,1.238923e+01,1.230290e+01,
    1.221773e+01,1.213380e+01,1.205122e+01,1.197011e+01,1.189058e+01,1.181277e+01,1.173682e+01,
    1.166286e+01,1.159105e+01,1.152158e+01,1.145460e+01,1.139033e+01,1.132895e+01,1.127071e+01};

  static const G4double SL38[nL]={
    3.019534e-02,7.490782e-02,1.915089e-01,6.057947e-01,2.053522e+00,6.462026e+00,1.812420e+01,
    4.516020e+01,9.894848e+01,1.839628e+02,2.814112e+02,3.765563e+02,4.501148e+02,4.042364e+02,
    3.029151e+02,2.212396e+02,1.647273e+02,1.259548e+02,9.887598e+01,7.956617e+01,6.553083e+01,
    5.515881e+01,4.738384e+01,4.148198e+01,3.694970e+01,3.342931e+01,3.066239e+01,2.845999e+01,
    2.668305e+01,2.522895e+01,2.402196e+01,2.300629e+01,2.214081e+01,2.139515e+01,2.074672e+01,
    2.017852e+01,1.967755e+01,1.923363e+01,1.883866e+01,1.848598e+01,1.817002e+01,1.788607e+01,
    1.763002e+01,1.739830e+01,1.718776e+01,1.699564e+01,1.681946e+01,1.665704e+01,1.650645e+01,
    1.636599e+01,1.623415e+01,1.610960e+01,1.599119e+01,1.587790e+01,1.576885e+01,1.566328e+01,
    1.556054e+01,1.546006e+01,1.536137e+01,1.526406e+01,1.516780e+01,1.507230e+01,1.497734e+01,
    1.488273e+01,1.478833e+01,1.469401e+01,1.459971e+01,1.450534e+01,1.441089e+01,1.431633e+01,
    1.422164e+01,1.412685e+01,1.403197e+01,1.393703e+01,1.384207e+01,1.374712e+01,1.365224e+01,
    1.355748e+01,1.346290e+01,1.336855e+01,1.327450e+01,1.318081e+01,1.308756e+01,1.299481e+01,
    1.290264e+01,1.281111e+01,1.272032e+01,1.263033e+01,1.254124e+01,1.245313e+01,1.236609e+01,
    1.228022e+01,1.219562e+01,1.211239e+01,1.203067e+01,1.195055e+01,1.187219e+01,1.179570e+01,
    1.172125e+01,1.164898e+01,1.157908e+01,1.151171e+01,1.144709e+01,1.138541e+01,1.132690e+01};

  static const G4double SL39[nL]={
    3.132934e-02,7.780169e-02,1.997199e-01,6.350103e-01,2.159047e+00,6.802554e+00,1.908355e+01,
    4.750616e+01,1.037394e+02,1.913355e+02,2.887491e+02,3.778240e+02,4.291402e+02,3.695233e+02,
    2.761193e+02,2.038392e+02,1.536530e+02,1.188331e+02,9.423086e+01,7.650062e+01,6.349328e+01,
    5.380369e+01,4.649042e+01,4.090667e+01,3.659746e+01,3.323617e+01,3.058461e+01,2.846722e+01,
    2.675392e+01,2.534816e+01,2.417843e+01,2.319185e+01,2.234937e+01,2.162211e+01,2.098852e+01,
    2.043239e+01,1.994127e+01,1.950544e+01,1.911710e+01,1.876982e+01,1.845825e+01,1.817778e+01,
    1.792446e+01,1.769479e+01,1.748570e+01,1.729449e+01,1.711874e+01,1.695632e+01,1.680535e+01,
    1.666414e+01,1.653123e+01,1.640532e+01,1.628529e+01,1.617015e+01,1.605904e+01,1.595123e+01,
    1.584607e+01,1.574304e+01,1.564166e+01,1.554157e+01,1.544242e+01,1.534396e+01,1.524598e+01,
    1.514829e+01,1.505077e+01,1.495332e+01,1.485585e+01,1.475831e+01,1.466069e+01,1.456296e+01,
    1.446512e+01,1.436719e+01,1.426920e+01,1.417118e+01,1.407316e+01,1.397520e+01,1.387735e+01,
    1.377966e+01,1.368219e+01,1.358501e+01,1.348818e+01,1.339176e+01,1.329584e+01,1.320047e+01,
    1.310574e+01,1.301172e+01,1.291849e+01,1.282613e+01,1.273474e+01,1.264439e+01,1.255519e+01,
    1.246724e+01,1.238063e+01,1.229548e+01,1.221190e+01,1.213003e+01,1.204999e+01,1.197192e+01,
    1.189599e+01,1.182235e+01,1.175118e+01,1.168267e+01,1.161701e+01,1.155444e+01,1.149517e+01};

  static const G4double SL40[nL]={
    3.209841e-02,7.976722e-02,2.053207e-01,6.550332e-01,2.231758e+00,7.040054e+00,1.977368e+01,
    4.932232e+01,1.081304e+02,2.014268e+02,3.129604e+02,4.273335e+02,4.672288e+02,3.790370e+02,
    2.780145e+02,2.044508e+02,1.540079e+02,1.191075e+02,9.446637e+01,7.671257e+01,6.368984e+01,
    5.399013e+01,4.667044e+01,4.108296e+01,3.677199e+01,3.341039e+01,3.075957e+01,2.864366e+01,
    2.693231e+01,2.552882e+01,2.436152e+01,2.337743e+01,2.253741e+01,2.181251e+01,2.118116e+01,
    2.062710e+01,2.013787e+01,1.970371e+01,1.931682e+01,1.897076e+01,1.866018e+01,1.838048e+01,
    1.812768e+01,1.789832e+01,1.768932e+01,1.749797e+01,1.732189e+01,1.715894e+01,1.700725e+01,
    1.686515e+01,1.673120e+01,1.660410e+01,1.648274e+01,1.636614e+01,1.625346e+01,1.614398e+01,
    1.603707e+01,1.593219e+01,1.582891e+01,1.572684e+01,1.562568e+01,1.552516e+01,1.542508e+01,
    1.532528e+01,1.522563e+01,1.512602e+01,1.502639e+01,1.492671e+01,1.482693e+01,1.472705e+01,
    1.462708e+01,1.452704e+01,1.442695e+01,1.432685e+01,1.422678e+01,1.412679e+01,1.402694e+01,
    1.392729e+01,1.382789e+01,1.372881e+01,1.363012e+01,1.353188e+01,1.343417e+01,1.333705e+01,
    1.324062e+01,1.314493e+01,1.305008e+01,1.295615e+01,1.286323e+01,1.277140e+01,1.268077e+01,
    1.259142e+01,1.250348e+01,1.241705e+01,1.233226e+01,1.224922e+01,1.216807e+01,1.208897e+01,
    1.201207e+01,1.193753e+01,1.186553e+01,1.179627e+01,1.172995e+01,1.166680e+01,1.160705e+01};

  static const G4double SL41[nL]={
    3.651675e-02,9.110302e-02,2.379739e-01,7.729064e-01,2.661008e+00,8.435802e+00,2.376201e+01,
    5.938433e+01,1.302605e+02,2.436285e+02,3.917132e+02,5.585883e+02,5.737641e+02,4.370072e+02,
    3.113977e+02,2.250866e+02,1.673725e+02,1.281023e+02,1.007441e+02,8.125622e+01,6.710300e+01,
    5.665354e+01,4.882987e+01,4.290028e+01,3.835570e+01,3.483427e+01,3.207439e+01,2.988467e+01,
    2.812414e+01,2.668871e+01,2.550153e+01,2.450594e+01,2.366020e+01,2.293344e+01,2.230273e+01,
    2.175080e+01,2.126446e+01,2.083343e+01,2.044949e+01,2.010594e+01,1.979722e+01,1.951859e+01,
    1.926597e+01,1.903585e+01,1.882512e+01,1.863109e+01,1.845136e+01,1.828384e+01,1.812670e+01,
    1.797832e+01,1.783729e+01,1.770239e+01,1.757256e+01,1.744688e+01,1.732458e+01,1.720499e+01,
    1.708753e+01,1.697174e+01,1.685722e+01,1.674365e+01,1.663076e+01,1.651834e+01,1.640623e+01,
    1.629429e+01,1.618244e+01,1.607062e+01,1.595877e+01,1.584690e+01,1.573499e+01,1.562305e+01,
    1.551112e+01,1.539924e+01,1.528743e+01,1.517577e+01,1.506429e+01,1.495307e+01,1.484216e+01,
    1.473164e+01,1.462157e+01,1.451203e+01,1.440308e+01,1.429481e+01,1.418729e+01,1.408060e+01,
    1.397483e+01,1.387005e+01,1.376636e+01,1.366384e+01,1.356258e+01,1.346270e+01,1.336429e+01,
    1.326746e+01,1.317233e+01,1.307902e+01,1.298767e+01,1.289841e+01,1.281140e+01,1.272680e+01,
    1.264478e+01,1.256554e+01,1.248927e+01,1.241619e+01,1.234653e+01,1.228054e+01,1.221850e+01};

  static const G4double SL42[nL]={
    3.967023e-02,9.923729e-02,2.617528e-01,8.598507e-01,2.978671e+00,9.461208e+00,2.661567e+01,
    6.608688e+01,1.423929e+02,2.556972e+02,3.845421e+02,5.509335e+02,6.958427e+02,6.038492e+02,
    4.319374e+02,3.026257e+02,2.171501e+02,1.607002e+02,1.225958e+02,9.625007e+01,7.763059e+01,
    6.421712e+01,5.439270e+01,4.709187e+01,4.159468e+01,3.740346e+01,3.416762e+01,3.163650e+01,
    2.962908e+01,2.801379e+01,2.669475e+01,2.560194e+01,2.468411e+01,2.390360e+01,2.323257e+01,
    2.265018e+01,2.214060e+01,2.169159e+01,2.129351e+01,2.093858e+01,2.062040e+01,2.033365e+01,
    2.007380e+01,1.983699e+01,1.961986e+01,1.941954e+01,1.923351e+01,1.905958e+01,1.889585e+01,
    1.874067e+01,1.859262e+01,1.845046e+01,1.831315e+01,1.817977e+01,1.804956e+01,1.792188e+01,
    1.779617e+01,1.767200e+01,1.754898e+01,1.742682e+01,1.730528e+01,1.718417e+01,1.706334e+01,
    1.694269e+01,1.682214e+01,1.670165e+01,1.658119e+01,1.646076e+01,1.634038e+01,1.622006e+01,
    1.609985e+01,1.597979e+01,1.585993e+01,1.574033e+01,1.562106e+01,1.550217e+01,1.538375e+01,
    1.526585e+01,1.514856e+01,1.503195e+01,1.491609e+01,1.480108e+01,1.468698e+01,1.457387e+01,
    1.446186e+01,1.435101e+01,1.424143e+01,1.413321e+01,1.402645e+01,1.392125e+01,1.381771e+01,
    1.371597e+01,1.361614e+01,1.351835e+01,1.342275e+01,1.332949e+01,1.323872e+01,1.315063e+01,
    1.306540e+01,1.298324e+01,1.290436e+01,1.282900e+01,1.275740e+01,1.268985e+01,1.262663e+01};

  static const G4double SL43[nL]={
    4.090181e-02,1.024236e-01,2.711450e-01,8.945313e-01,3.107339e+00,9.893526e+00,2.794798e+01,
    7.002434e+01,1.540039e+02,2.909962e+02,4.857228e+02,6.871085e+02,6.405347e+02,4.648476e+02,
    3.262939e+02,2.342891e+02,1.734786e+02,1.323680e+02,1.038662e+02,8.364716e+01,6.901712e+01,
    5.825243e+01,5.021882e+01,4.414930e+01,3.951210e+01,3.593043e+01,3.313262e+01,3.092037e+01,
    2.914791e+01,2.770777e+01,2.652069e+01,2.552830e+01,2.468759e+01,2.396683e+01,2.334242e+01,
    2.279664e+01,2.231596e+01,2.188986e+01,2.150996e+01,2.116947e+01,2.086272e+01,2.058497e+01,
    2.033214e+01,2.010070e+01,1.988760e+01,1.969016e+01,1.950605e+01,1.933324e+01,1.916994e+01,
    1.901461e+01,1.886590e+01,1.872267e+01,1.858390e+01,1.844877e+01,1.831654e+01,1.818660e+01,
    1.805846e+01,1.793169e+01,1.780594e+01,1.768095e+01,1.755648e+01,1.743238e+01,1.730850e+01,
    1.718477e+01,1.706112e+01,1.693751e+01,1.681394e+01,1.669040e+01,1.656692e+01,1.644353e+01,
    1.632027e+01,1.619719e+01,1.607435e+01,1.595181e+01,1.582965e+01,1.570791e+01,1.558669e+01,
    1.546605e+01,1.534607e+01,1.522683e+01,1.510840e+01,1.499087e+01,1.487432e+01,1.475882e+01,
    1.464448e+01,1.453138e+01,1.441961e+01,1.430927e+01,1.420046e+01,1.409328e+01,1.398785e+01,
    1.388429e+01,1.378272e+01,1.368328e+01,1.358612e+01,1.349139e+01,1.339925e+01,1.330989e+01,
    1.322351e+01,1.314030e+01,1.306050e+01,1.298434e+01,1.291209e+01,1.284402e+01,1.278043e+01};

  static const G4double SL44[nL]={
    4.170472e-02,1.045035e-01,2.772928e-01,9.171279e-01,3.188391e+00,1.013541e+01,2.846282e+01,
    7.022927e+01,1.487823e+02,2.562178e+02,3.490445e+02,4.157065e+02,4.491619e+02,3.805077e+02,
    2.830064e+02,2.087710e+02,1.575446e+02,1.221104e+02,9.711794e+01,7.913014e+01,6.595149e+01,
    5.615029e+01,4.876829e+01,4.314714e+01,3.882350e+01,3.546449e+01,3.282708e+01,3.073199e+01,
    2.904621e+01,2.767098e+01,2.653307e+01,2.557832e+01,2.476670e+01,2.406860e+01,2.346198e+01,
    2.293024e+01,2.246069e+01,2.204342e+01,2.167052e+01,2.133555e+01,2.103313e+01,2.075871e+01,
    2.050838e+01,2.027875e+01,2.006688e+01,1.987017e+01,1.968637e+01,1.951349e+01,1.934981e+01,
    1.919382e+01,1.904422e+01,1.889988e+01,1.875984e+01,1.862328e+01,1.848949e+01,1.835789e+01,
    1.822798e+01,1.809938e+01,1.797173e+01,1.784478e+01,1.771833e+01,1.759220e+01,1.746629e+01,
    1.734050e+01,1.721479e+01,1.708912e+01,1.696349e+01,1.683791e+01,1.671240e+01,1.658700e+01,
    1.646175e+01,1.633671e+01,1.621193e+01,1.608749e+01,1.596344e+01,1.583986e+01,1.571683e+01,
    1.559442e+01,1.547270e+01,1.535176e+01,1.523167e+01,1.511252e+01,1.499439e+01,1.487736e+01,
    1.476153e+01,1.464698e+01,1.453380e+01,1.442210e+01,1.431197e+01,1.420353e+01,1.409689e+01,
    1.399216e+01,1.388949e+01,1.378900e+01,1.369084e+01,1.359517e+01,1.350217e+01,1.341201e+01,
    1.332489e+01,1.324103e+01,1.316065e+01,1.308400e+01,1.301134e+01,1.294296e+01,1.287917e+01};

  static const G4double SL45[nL]={
    5.274378e-02,1.333110e-01,3.641511e-01,1.243435e+00,4.396136e+00,1.406245e+01,3.947630e+01,
    9.639532e+01,1.976332e+02,3.177821e+02,3.939795e+02,4.252091e+02,4.177958e+02,3.393139e+02,
    2.539062e+02,1.908284e+02,1.468253e+02,1.158554e+02,9.364418e+01,7.742461e+01,6.539740e+01,
    5.636581e+01,4.951281e+01,4.426627e+01,4.021621e+01,3.706322e+01,3.458554e+01,3.261752e+01,
    3.103504e+01,2.974522e+01,2.867873e+01,2.778411e+01,2.702322e+01,2.636780e+01,2.579681e+01,
    2.529440e+01,2.484848e+01,2.444965e+01,2.409041e+01,2.376468e+01,2.346741e+01,2.319435e+01,
    2.294188e+01,2.270688e+01,2.248667e+01,2.227890e+01,2.208156e+01,2.189291e+01,2.171143e+01,
    2.153584e+01,2.136504e+01,2.119811e+01,2.103426e+01,2.087283e+01,2.071330e+01,2.055522e+01,
    2.039825e+01,2.024210e+01,2.008657e+01,1.993150e+01,1.977677e+01,1.962233e+01,1.946811e+01,
    1.931412e+01,1.916036e+01,1.900686e+01,1.885366e+01,1.870082e+01,1.854839e+01,1.839645e+01,
    1.824507e+01,1.809433e+01,1.794431e+01,1.779509e+01,1.764676e+01,1.749940e+01,1.735310e+01,
    1.720795e+01,1.706403e+01,1.692142e+01,1.678022e+01,1.664052e+01,1.650241e+01,1.636599e+01,
    1.623135e+01,1.609859e+01,1.596783e+01,1.583917e+01,1.571273e+01,1.558865e+01,1.546706e+01,
    1.534810e+01,1.523193e+01,1.511872e+01,1.500865e+01,1.490192e+01,1.479874e+01,1.469935e+01,
    1.460399e+01,1.451294e+01,1.442648e+01,1.434493e+01,1.426862e+01,1.419792e+01,1.413321e+01};

  static const G4double SL46[nL]={
    5.429151e-02,1.373796e-01,3.766506e-01,1.291127e+00,4.573808e+00,1.464102e+01,4.109390e+01,
    1.001858e+02,2.044221e+02,3.254972e+02,3.979352e+02,4.222069e+02,4.093963e+02,3.318361e+02,
    2.490584e+02,1.878782e+02,1.450486e+02,1.147964e+02,9.303167e+01,7.709613e+01,6.525408e+01,
    5.634628e+01,4.957827e+01,4.439170e+01,4.038515e+01,3.726461e+01,3.481177e+01,3.286321e+01,
    3.129630e+01,3.001910e+01,2.896297e+01,2.807689e+01,2.732305e+01,2.667342e+01,2.610713e+01,
    2.560848e+01,2.516547e+01,2.476879e+01,2.441101e+01,2.408612e+01,2.378911e+01,2.351580e+01,
    2.326261e+01,2.302646e+01,2.280470e+01,2.259503e+01,2.239548e+01,2.220433e+01,2.202011e+01,
    2.184156e+01,2.166762e+01,2.149737e+01,2.133007e+01,2.116508e+01,2.100189e+01,2.084008e+01,
    2.067932e+01,2.051935e+01,2.035997e+01,2.020104e+01,2.004246e+01,1.988416e+01,1.972612e+01,
    1.956833e+01,1.941080e+01,1.925358e+01,1.909671e+01,1.894024e+01,1.878425e+01,1.862880e+01,
    1.847398e+01,1.831986e+01,1.816653e+01,1.801408e+01,1.786258e+01,1.771212e+01,1.756280e+01,
    1.741470e+01,1.726790e+01,1.712250e+01,1.697859e+01,1.683625e+01,1.669557e+01,1.655667e+01,
    1.641963e+01,1.628456e+01,1.615157e+01,1.602077e+01,1.589228e+01,1.576624e+01,1.564279e+01,
    1.552206e+01,1.540424e+01,1.528948e+01,1.517797e+01,1.506993e+01,1.496557e+01,1.486512e+01,
    1.476885e+01,1.467703e+01,1.458997e+01,1.450798e+01,1.443142e+01,1.436066e+01,1.429608e+01};

  static const G4double SL47[nL]={
    5.586443e-02,1.415214e-01,3.894297e-01,1.340049e+00,4.756205e+00,1.523387e+01,4.273995e+01,
    1.039652e+02,2.107611e+02,3.306660e+02,3.922061e+02,3.966237e+02,4.015659e+02,3.735019e+02,
    2.931095e+02,2.186308e+02,1.651709e+02,1.280396e+02,1.019243e+02,8.320469e+01,6.954857e+01,
    5.943717e+01,5.185713e+01,4.611438e+01,4.172171e+01,3.832984e+01,3.568426e+01,3.359747e+01,
    3.193051e+01,3.058030e+01,2.947046e+01,2.854456e+01,2.776093e+01,2.708881e+01,2.650532e+01,
    2.599335e+01,2.553984e+01,2.513469e+01,2.476993e+01,2.443910e+01,2.413690e+01,2.385890e+01,
    2.360137e+01,2.336109e+01,2.313534e+01,2.292175e+01,2.271830e+01,2.252324e+01,2.233509e+01,
    2.215258e+01,2.197463e+01,2.180035e+01,2.162898e+01,2.145990e+01,2.129261e+01,2.112668e+01,
    2.096180e+01,2.079771e+01,2.063423e+01,2.047122e+01,2.030859e+01,2.014627e+01,1.998426e+01,
    1.982254e+01,1.966115e+01,1.950011e+01,1.933948e+01,1.917932e+01,1.901970e+01,1.886070e+01,
    1.870239e+01,1.854486e+01,1.838819e+01,1.823247e+01,1.807778e+01,1.792421e+01,1.777185e+01,
    1.762079e+01,1.747111e+01,1.732291e+01,1.717627e+01,1.703129e+01,1.688806e+01,1.674668e+01,
    1.660725e+01,1.646987e+01,1.633466e+01,1.620173e+01,1.607120e+01,1.594322e+01,1.581792e+01,
    1.569546e+01,1.557599e+01,1.545971e+01,1.534680e+01,1.523747e+01,1.513195e+01,1.503048e+01,
    1.493334e+01,1.484080e+01,1.475318e+01,1.467081e+01,1.459405e+01,1.452327e+01,1.445890e+01};

  static const G4double SL48[nL]={
    5.639434e-02,1.429184e-01,3.937524e-01,1.356644e+00,4.818271e+00,
    1.543710e+01,4.331539e+01,1.053565e+02,2.134958e+02,3.351211e+02,
    4.008312e+02,4.175491e+02,4.294249e+02,3.789784e+02,2.880226e+02,
    2.137731e+02,1.618452e+02,1.258669e+02,1.005048e+02,8.227129e+01,
    6.893285e+01,5.903288e+01,5.159623e+01,4.595252e+01,4.162949e+01,
    3.828743e+01,3.567806e+01,3.361799e+01,3.197104e+01,3.063601e+01,
    2.953782e+01,2.862094e+01,2.784434e+01,2.717773e+01,2.659856e+01,
    2.608997e+01,2.563908e+01,2.523594e+01,2.487267e+01,2.454291e+01,
    2.424142e+01,2.396383e+01,2.370644e+01,2.346608e+01,2.324005e+01,
    2.302601e+01,2.282197e+01,2.262619e+01,2.243721e+01,2.225378e+01,
    2.207484e+01,2.189950e+01,2.172701e+01,2.155676e+01,2.138826e+01,
    2.122110e+01,2.105496e+01,2.088961e+01,2.072484e+01,2.056055e+01,
    2.039663e+01,2.023303e+01,2.006974e+01,1.990676e+01,1.974411e+01,
    1.958183e+01,1.941998e+01,1.925861e+01,1.909781e+01,1.893764e+01,
    1.877819e+01,1.861953e+01,1.846176e+01,1.830496e+01,1.814922e+01,
    1.799463e+01,1.784127e+01,1.768923e+01,1.753860e+01,1.738947e+01,
    1.724194e+01,1.709608e+01,1.695201e+01,1.680981e+01,1.666958e+01,
    1.653144e+01,1.639550e+01,1.626187e+01,1.613067e+01,1.600205e+01,
    1.587614e+01,1.575310e+01,1.563310e+01,1.551631e+01,1.540294e+01,
    1.529319e+01,1.518730e+01,1.508550e+01,1.498808e+01,1.489531e+01,
    1.480751e+01,1.472502e+01,1.464821e+01,1.457745e+01,1.451316e+01};

  static const G4double SH0[nH]={
    1.718841e-05,1.912141e-05,2.128656e-05,2.372770e-05,2.651339e-05,2.976162e-05,3.369201e-05,
    3.873597e-05,4.577051e-05,5.661516e-05,7.508997e-05,1.092699e-04,1.762839e-04,3.124886e-04,
    5.948094e-04,1.184449e-03,2.411855e-03,4.923726e-03,9.871386e-03,1.894320e-02,3.373152e-02,
    5.419455e-02,7.777948e-02,1.011811e-01,1.227807e-01,1.428966e-01,1.626818e-01,1.833195e-01,
    2.057743e-01,2.307930e-01,2.589428e-01,2.906090e-01,3.259289e-01,3.646554e-01,4.059556e-01,
    4.481828e-01,4.887166e-01,5.240358e-01,5.501959e-01,5.637401e-01,5.627614e-01,5.475832e-01,
    5.206446e-01,4.856647e-01,4.465759e-01,4.067172e-01,3.684796e-01,3.333189e-01,3.019524e-01,
    2.745971e-01,2.511726e-01,2.314485e-01,2.151395e-01,2.019637e-01,1.916740e-01,1.840748e-01,
    1.790291e-01,1.764601e-01,1.763488e-01,1.787259e-01,1.836564e-01,1.912090e-01,2.014025e-01,
    2.141163e-01,2.289594e-01,2.451064e-01,2.611598e-01,2.751583e-01,2.848795e-01,2.884723e-01,
    2.851743e-01,2.756664e-01,2.618121e-01,2.459864e-01,2.304469e-01,2.170242e-01,2.071089e-01,
    2.017331e-01,2.014838e-01,2.059886e-01,2.130499e-01,2.185478e-01,2.186039e-01,2.124513e-01,
    2.023557e-01,1.911989e-01,1.808918e-01,1.722630e-01,1.654744e-01,1.603770e-01,1.567046e-01,
    1.541608e-01,1.524546e-01,1.513189e-01,1.505256e-01,1.498980e-01,1.493175e-01,1.487199e-01,
    1.480828e-01,1.474096e-01,1.467148e-01,1.460147e-01,1.453221e-01,1.446452e-01,1.439881e-01,
    1.433514e-01,1.427339e-01,1.421336e-01,1.415477e-01,1.409739e-01,1.404099e-01,1.398539e-01,
    1.393046e-01,1.387609e-01,1.382221e-01,1.376879e-01,1.371581e-01,1.366326e-01,1.361116e-01,
    1.355952e-01,1.350837e-01,1.345775e-01,1.340767e-01,1.335816e-01,1.330926e-01,1.326099e-01,
    1.321338e-01,1.316644e-01,1.312019e-01,1.307465e-01,1.302983e-01,1.298574e-01,1.294239e-01,
    1.289978e-01,1.285792e-01,1.281681e-01,1.277645e-01,1.273684e-01,1.269797e-01,1.265984e-01,
    1.262246e-01,1.258580e-01,1.254987e-01,1.251465e-01,1.248015e-01,1.244635e-01,1.241324e-01,
    1.238082e-01,1.234908e-01,1.231801e-01,1.228760e-01,1.225784e-01,1.222872e-01,1.220024e-01,
    1.217239e-01,1.214515e-01,1.211852e-01,1.209249e-01,1.206706e-01,1.204221e-01,1.201793e-01,
    1.199423e-01,1.197109e-01,1.194850e-01,1.192646e-01,1.190497e-01,1.188400e-01,1.186357e-01,
    1.184365e-01,1.182425e-01,1.180536e-01,1.178697e-01,1.176908e-01,1.175169e-01,1.173477e-01,
    1.171834e-01,1.170239e-01,1.168690e-01,1.167189e-01,1.165733e-01,1.164323e-01,1.162959e-01,
    1.161639e-01,1.160364e-01,1.159132e-01,1.157944e-01,1.156800e-01,1.155698e-01,1.154639e-01,
    1.153622e-01,1.152646e-01,1.151712e-01,1.150819e-01,1.149967e-01,1.149155e-01,1.148384e-01,
    1.147652e-01,1.146960e-01,1.146307e-01,1.145693e-01,1.145118e-01,1.144581e-01,1.144082e-01,
    1.143621e-01,1.143198e-01,1.142812e-01,1.142464e-01,1.142152e-01,1.141877e-01,1.141639e-01,
    1.141437e-01,1.141271e-01,1.141140e-01,1.141046e-01,1.140986e-01,1.140962e-01,1.140973e-01,
    1.141019e-01,1.141099e-01,1.141214e-01,1.141363e-01,1.141546e-01,1.141763e-01,1.142013e-01};

  static const G4double SH1[nH]={
    6.668702e-02,6.471599e-02,6.280838e-02,6.096276e-02,5.917858e-02,5.745696e-02,5.580253e-02,
    5.422715e-02,5.275790e-02,5.145368e-02,5.043968e-02,4.997674e-02,5.059345e-02,5.330701e-02,
    5.988460e-02,7.281273e-02,9.420106e-02,1.234283e-01,1.561378e-01,1.872520e-01,2.145181e-01,
    2.385444e-01,2.610207e-01,2.835546e-01,3.073666e-01,3.333473e-01,3.621748e-01,3.943976e-01,
    4.304707e-01,4.707493e-01,5.154414e-01,5.645200e-01,6.175936e-01,6.737415e-01,7.313391e-01,
    7.879273e-01,8.402160e-01,8.843244e-01,9.163187e-01,9.329844e-01,9.326046e-01,9.154272e-01,
    8.836029e-01,8.406235e-01,7.905189e-01,7.371246e-01,6.836113e-01,6.323070e-01,5.847347e-01,
    5.417618e-01,5.037798e-01,4.708677e-01,4.429213e-01,4.197458e-01,4.011180e-01,3.868245e-01,
    3.766827e-01,3.705478e-01,3.683091e-01,3.698743e-01,3.751405e-01,3.839482e-01,3.960151e-01,
    4.108509e-01,4.276609e-01,4.452665e-01,4.620878e-01,4.762505e-01,4.858563e-01,4.893850e-01,
    4.860977e-01,4.762589e-01,4.610644e-01,4.423195e-01,4.220327e-01,4.020934e-01,3.841080e-01,
    3.693549e-01,3.587356e-01,3.525376e-01,3.499160e-01,3.485284e-01,3.454232e-01,3.391105e-01,
    3.303977e-01,3.210857e-01,3.125479e-01,3.053688e-01,2.995858e-01,2.949875e-01,2.913061e-01,
    2.883070e-01,2.858170e-01,2.837185e-01,2.819325e-01,2.804025e-01,2.790830e-01,2.779344e-01,
    2.769203e-01,2.760078e-01,2.751677e-01,2.743752e-01,2.736094e-01,2.728539e-01,2.720958e-01,
    2.713259e-01,2.705376e-01,2.697270e-01,2.688919e-01,2.680317e-01,2.671470e-01,2.662393e-01,
    2.653104e-01,2.643629e-01,2.633994e-01,2.624226e-01,2.614353e-01,2.604402e-01,2.594400e-01,
    2.584370e-01,2.574337e-01,2.564319e-01,2.554338e-01,2.544410e-01,2.534551e-01,2.524775e-01,
    2.515094e-01,2.505518e-01,2.496056e-01,2.486718e-01,2.477508e-01,2.468434e-01,2.459499e-01,
    2.450708e-01,2.442063e-01,2.433567e-01,2.425221e-01,2.417026e-01,2.408984e-01,2.401094e-01,
    2.393357e-01,2.385771e-01,2.378336e-01,2.371052e-01,2.363916e-01,2.356928e-01,2.350087e-01,
    2.343390e-01,2.336837e-01,2.330425e-01,2.324153e-01,2.318018e-01,2.312020e-01,2.306156e-01,
    2.300425e-01,2.294824e-01,2.289352e-01,2.284007e-01,2.278786e-01,2.273690e-01,2.268715e-01,
    2.263859e-01,2.259122e-01,2.254502e-01,2.249996e-01,2.245604e-01,2.241323e-01,2.237153e-01,
    2.233091e-01,2.229137e-01,2.225289e-01,2.221545e-01,2.217905e-01,2.214366e-01,2.210928e-01,
    2.207590e-01,2.204350e-01,2.201206e-01,2.198159e-01,2.195207e-01,2.192348e-01,2.189582e-01,
    2.186908e-01,2.184324e-01,2.181831e-01,2.179426e-01,2.177109e-01,2.174878e-01,2.172735e-01,
    2.170676e-01,2.168702e-01,2.166812e-01,2.165004e-01,2.163279e-01,2.161635e-01,2.160072e-01,
    2.158589e-01,2.157185e-01,2.155860e-01,2.154613e-01,2.153443e-01,2.152351e-01,2.151334e-01,
    2.150393e-01,2.149526e-01,2.148734e-01,2.148017e-01,2.147372e-01,2.146800e-01,2.146301e-01,
    2.145873e-01,2.145516e-01,2.145230e-01,2.145015e-01,2.144869e-01,2.144793e-01,2.144786e-01,
    2.144847e-01,2.144976e-01,2.145173e-01,2.145437e-01,2.145768e-01,2.146166e-01,2.146629e-01};

  static const G4double SH2[nH]={
    1.542383e-01,1.519749e-01,1.500571e-01,1.485008e-01,1.473380e-01,1.466231e-01,1.464406e-01,
    1.469146e-01,1.482202e-01,1.505946e-01,1.543461e-01,1.598551e-01,1.675614e-01,1.779302e-01,
    1.913930e-01,2.082703e-01,2.286959e-01,2.525746e-01,2.795999e-01,3.093368e-01,3.413423e-01,
    3.752810e-01,4.110019e-01,4.485617e-01,4.882066e-01,5.303277e-01,5.754066e-01,6.239567e-01,
    6.764587e-01,7.332866e-01,7.946123e-01,8.602851e-01,9.296820e-01,1.001541e+00,1.073807e+00,
    1.143552e+00,1.207044e+00,1.260057e+00,1.298438e+00,1.318861e+00,1.319550e+00,1.300718e+00,
    1.264525e+00,1.214592e+00,1.155254e+00,1.090830e+00,1.025091e+00,9.610003e-01,9.006681e-01,
    8.454562e-01,7.961371e-01,7.530613e-01,7.163006e-01,6.857589e-01,6.612494e-01,6.425427e-01,
    6.293909e-01,6.215310e-01,6.186705e-01,6.204555e-01,6.264246e-01,6.359503e-01,6.481772e-01,
    6.619737e-01,6.759222e-01,6.883771e-01,6.976142e-01,7.020655e-01,7.005923e-01,6.927105e-01,
    6.786831e-01,6.594410e-01,6.363628e-01,6.109982e-01,5.848221e-01,5.590708e-01,5.346711e-01,
    5.122400e-01,4.921277e-01,4.744756e-01,4.592744e-01,4.464140e-01,4.357218e-01,4.269910e-01,
    4.199996e-01,4.145245e-01,4.103489e-01,4.072685e-01,4.050942e-01,4.036540e-01,4.027937e-01,
    4.023771e-01,4.022854e-01,4.024166e-01,4.026846e-01,4.030181e-01,4.033588e-01,4.036607e-01,
    4.038884e-01,4.040156e-01,4.040242e-01,4.039024e-01,4.036443e-01,4.032483e-01,4.027164e-01,
    4.020532e-01,4.012654e-01,4.003614e-01,3.993501e-01,3.982412e-01,3.970447e-01,3.957703e-01,
    3.944278e-01,3.930263e-01,3.915747e-01,3.900812e-01,3.885536e-01,3.869989e-01,3.854235e-01,
    3.838335e-01,3.822342e-01,3.806303e-01,3.790262e-01,3.774257e-01,3.758321e-01,3.742484e-01,
    3.726773e-01,3.711210e-01,3.695814e-01,3.680602e-01,3.665588e-01,3.650785e-01,3.636202e-01,
    3.621847e-01,3.607727e-01,3.593848e-01,3.580212e-01,3.566824e-01,3.553685e-01,3.540796e-01,
    3.528157e-01,3.515768e-01,3.503629e-01,3.491738e-01,3.480093e-01,3.468693e-01,3.457535e-01,
    3.446616e-01,3.435935e-01,3.425488e-01,3.415272e-01,3.405285e-01,3.395522e-01,3.385982e-01,
    3.376660e-01,3.367553e-01,3.358660e-01,3.349975e-01,3.341496e-01,3.333220e-01,3.325144e-01,
    3.317265e-01,3.309579e-01,3.302084e-01,3.294778e-01,3.287656e-01,3.280716e-01,3.273956e-01,
    3.267373e-01,3.260964e-01,3.254728e-01,3.248660e-01,3.242759e-01,3.237024e-01,3.231450e-01,
    3.226036e-01,3.220781e-01,3.215681e-01,3.210735e-01,3.205941e-01,3.201297e-01,3.196801e-01,
    3.192451e-01,3.188245e-01,3.184182e-01,3.180260e-01,3.176477e-01,3.172832e-01,3.169323e-01,
    3.165949e-01,3.162708e-01,3.159598e-01,3.156619e-01,3.153769e-01,3.151046e-01,3.148449e-01,
    3.145978e-01,3.143631e-01,3.141406e-01,3.139302e-01,3.137319e-01,3.135455e-01,3.133709e-01,
    3.132080e-01,3.130567e-01,3.129170e-01,3.127886e-01,3.126715e-01,3.125656e-01,3.124708e-01,
    3.123871e-01,3.123143e-01,3.122524e-01,3.122012e-01,3.121607e-01,3.121308e-01,3.121115e-01,
    3.121025e-01,3.121040e-01,3.121157e-01,3.121377e-01,3.121699e-01,3.122121e-01,3.122643e-01};

  static const G4double SH3[nH]={
    2.629521e-01,2.526203e-01,2.431577e-01,2.345868e-01,2.269548e-01,2.203418e-01,2.148720e-01,
    2.107267e-01,2.081600e-01,2.075143e-01,2.092319e-01,2.138553e-01,2.220073e-01,2.343402e-01,
    2.514498e-01,2.737640e-01,3.014354e-01,3.342839e-01,3.718253e-01,4.133919e-01,4.583047e-01,
    5.060360e-01,5.563139e-01,6.091512e-01,6.648144e-01,7.237579e-01,7.865441e-01,8.537601e-01,
    9.259290e-01,1.003412e+00,1.086287e+00,1.174207e+00,1.266226e+00,1.360618e+00,1.454730e+00,
    1.544923e+00,1.626692e+00,1.695047e+00,1.745152e+00,1.773146e+00,1.776925e+00,1.756615e+00,
    1.714557e+00,1.654820e+00,1.582408e+00,1.502458e+00,1.419603e+00,1.337617e+00,1.259313e+00,
    1.186603e+00,1.120659e+00,1.062094e+00,1.011127e+00,9.677216e-01,9.316827e-01,9.027247e-01,
    8.805089e-01,8.646577e-01,8.547494e-01,8.502958e-01,8.507072e-01,8.552495e-01,8.630006e-01,
    8.728198e-01,8.833474e-01,8.930526e-01,9.003428e-01,9.037330e-01,9.020462e-01,8.945963e-01,
    8.812964e-01,8.626545e-01,8.396586e-01,8.135868e-01,7.858018e-01,7.575779e-01,7.299877e-01,
    7.038504e-01,6.797291e-01,6.579596e-01,6.386918e-01,6.219342e-01,6.075947e-01,5.955137e-01,
    5.854904e-01,5.773026e-01,5.707205e-01,5.655170e-01,5.614749e-01,5.583909e-01,5.560792e-01,
    5.543723e-01,5.531226e-01,5.522016e-01,5.514995e-01,5.509243e-01,5.504007e-01,5.498680e-01,
    5.492791e-01,5.485986e-01,5.478012e-01,5.468703e-01,5.457963e-01,5.445754e-01,5.432087e-01,
    5.417006e-01,5.400585e-01,5.382913e-01,5.364098e-01,5.344250e-01,5.323486e-01,5.301922e-01,
    5.279674e-01,5.256850e-01,5.233556e-01,5.209890e-01,5.185943e-01,5.161800e-01,5.137537e-01,
    5.113224e-01,5.088925e-01,5.064694e-01,5.040582e-01,5.016634e-01,4.992887e-01,4.969376e-01,
    4.946129e-01,4.923172e-01,4.900525e-01,4.878208e-01,4.856233e-01,4.834613e-01,4.813359e-01,
    4.792477e-01,4.771973e-01,4.751850e-01,4.732112e-01,4.712759e-01,4.693792e-01,4.675209e-01,
    4.657009e-01,4.639189e-01,4.621746e-01,4.604677e-01,4.587977e-01,4.571643e-01,4.555669e-01,
    4.540052e-01,4.524784e-01,4.509863e-01,4.495282e-01,4.481036e-01,4.467119e-01,4.453527e-01,
    4.440254e-01,4.427295e-01,4.414644e-01,4.402296e-01,4.390246e-01,4.378489e-01,4.367021e-01,
    4.355835e-01,4.344928e-01,4.334295e-01,4.323931e-01,4.313831e-01,4.303992e-01,4.294410e-01,
    4.285079e-01,4.275996e-01,4.267157e-01,4.258558e-01,4.250196e-01,4.242067e-01,4.234167e-01,
    4.226493e-01,4.219041e-01,4.211809e-01,4.204793e-01,4.197990e-01,4.191397e-01,4.185012e-01,
    4.178831e-01,4.172851e-01,4.167071e-01,4.161487e-01,4.156098e-01,4.150899e-01,4.145890e-01,
    4.141068e-01,4.136430e-01,4.131974e-01,4.127699e-01,4.123602e-01,4.119681e-01,4.115935e-01,
    4.112360e-01,4.108956e-01,4.105721e-01,4.102652e-01,4.099749e-01,4.097009e-01,4.094431e-01,
    4.092013e-01,4.089753e-01,4.087651e-01,4.085704e-01,4.083912e-01,4.082272e-01,4.080784e-01,
    4.079445e-01,4.078256e-01,4.077213e-01,4.076317e-01,4.075566e-01,4.074959e-01,4.074494e-01,
    4.074170e-01,4.073987e-01,4.073943e-01,4.074037e-01,4.074268e-01,4.074635e-01,4.075137e-01};

  static const G4double SH4[nH]={
    7.405778e-02,7.529642e-02,7.695159e-02,7.911585e-02,8.191973e-02,8.554566e-02,9.024590e-02,
    9.636447e-02,1.043620e-01,1.148402e-01,1.285589e-01,1.464340e-01,1.695011e-01,1.988282e-01,
    2.353712e-01,2.797917e-01,3.322864e-01,3.925018e-01,4.595934e-01,5.324297e-01,6.098729e-01,
    6.910366e-01,7.754430e-01,8.630582e-01,9.542326e-01,1.049589e+00,1.149890e+00,1.255911e+00,
    1.368302e+00,1.487447e+00,1.613306e+00,1.745230e+00,1.881750e+00,2.020382e+00,2.157473e+00,
    2.288176e+00,2.406638e+00,2.506475e+00,2.581539e+00,2.626868e+00,2.639586e+00,2.619491e+00,
    2.569112e+00,2.493232e+00,2.398030e+00,2.290122e+00,2.175753e+00,2.060259e+00,1.947831e+00,
    1.841504e+00,1.743297e+00,1.654408e+00,1.575420e+00,1.506477e+00,1.447431e+00,1.397940e+00,
    1.357542e+00,1.325684e+00,1.301743e+00,1.285014e+00,1.274698e+00,1.269869e+00,1.269456e+00,
    1.272221e+00,1.276770e+00,1.281590e+00,1.285130e+00,1.285911e+00,1.282675e+00,1.274519e+00,
    1.260999e+00,1.242178e+00,1.218585e+00,1.191121e+00,1.160911e+00,1.129160e+00,1.097017e+00,
    1.065486e+00,1.035376e+00,1.007278e+00,9.815797e-01,9.584915e-01,9.380753e-01,9.202798e-01,
    9.049702e-01,8.919552e-01,8.810082e-01,8.718847e-01,8.643355e-01,8.581166e-01,8.529964e-01,
    8.487605e-01,8.452151e-01,8.421882e-01,8.395306e-01,8.371154e-01,8.348367e-01,8.326085e-01,
    8.303629e-01,8.280477e-01,8.256249e-01,8.230684e-01,8.203621e-01,8.174981e-01,8.144752e-01,
    8.112975e-01,8.079728e-01,8.045118e-01,8.009274e-01,7.972336e-01,7.934448e-01,7.895759e-01,
    7.856415e-01,7.816556e-01,7.776315e-01,7.735820e-01,7.695185e-01,7.654519e-01,7.613920e-01,
    7.573474e-01,7.533262e-01,7.493353e-01,7.453809e-01,7.414684e-01,7.376024e-01,7.337870e-01,
    7.300255e-01,7.263208e-01,7.226752e-01,7.190907e-01,7.155687e-01,7.121104e-01,7.087166e-01,
    7.053878e-01,7.021244e-01,6.989265e-01,6.957938e-01,6.927263e-01,6.897235e-01,6.867848e-01,
    6.839097e-01,6.810975e-01,6.783474e-01,6.756586e-01,6.730302e-01,6.704613e-01,6.679511e-01,
    6.654984e-01,6.631024e-01,6.607622e-01,6.584766e-01,6.562449e-01,6.540659e-01,6.519387e-01,
    6.498624e-01,6.478361e-01,6.458588e-01,6.439296e-01,6.420476e-01,6.402120e-01,6.384218e-01,
    6.366764e-01,6.349747e-01,6.333162e-01,6.316999e-01,6.301252e-01,6.285912e-01,6.270973e-01,
    6.256428e-01,6.242270e-01,6.228493e-01,6.215089e-01,6.202054e-01,6.189380e-01,6.177062e-01,
    6.165095e-01,6.153472e-01,6.142189e-01,6.131240e-01,6.120620e-01,6.110324e-01,6.100348e-01,
    6.090686e-01,6.081335e-01,6.072289e-01,6.063545e-01,6.055099e-01,6.046946e-01,6.039083e-01,
    6.031506e-01,6.024211e-01,6.017194e-01,6.010453e-01,6.003983e-01,5.997781e-01,5.991845e-01,
    5.986171e-01,5.980756e-01,5.975596e-01,5.970690e-01,5.966035e-01,5.961627e-01,5.957464e-01,
    5.953544e-01,5.949863e-01,5.946420e-01,5.943211e-01,5.940236e-01,5.937490e-01,5.934973e-01,
    5.932682e-01,5.930615e-01,5.928769e-01,5.927143e-01,5.925735e-01,5.924543e-01,5.923565e-01,
    5.922798e-01,5.922242e-01,5.921894e-01,5.921754e-01,5.921818e-01,5.922085e-01,5.922555e-01};

  static const G4double SH5[nH]={
    4.659776e-01,4.476902e-01,4.309775e-01,4.158946e-01,4.025449e-01,3.910970e-01,3.818062e-01,
    3.750402e-01,3.713091e-01,3.712933e-01,3.758637e-01,3.860770e-01,4.031303e-01,4.282529e-01,
    4.625322e-01,5.066943e-01,5.609019e-01,6.246586e-01,6.968852e-01,7.761692e-01,8.611016e-01,
    9.505815e-01,1.043997e+00,1.141265e+00,1.242749e+00,1.349132e+00,1.461255e+00,1.579962e+00,
    1.705950e+00,1.839605e+00,1.980828e+00,2.128829e+00,2.281907e+00,2.437240e+00,2.590731e+00,
    2.736990e+00,2.869544e+00,2.981348e+00,3.065603e+00,3.116761e+00,3.131496e+00,3.109354e+00,
    3.052863e+00,2.967066e+00,2.858644e+00,2.734889e+00,2.602808e+00,2.468500e+00,2.336846e+00,
    2.211463e+00,2.094818e+00,1.988432e+00,1.893096e+00,1.809073e+00,1.736261e+00,1.674311e+00,
    1.622712e+00,1.580840e+00,1.547984e+00,1.523343e+00,1.506019e+00,1.494995e+00,1.489116e+00,
    1.487081e+00,1.487447e+00,1.488669e+00,1.489169e+00,1.487439e+00,1.482168e+00,1.472372e+00,
    1.457487e+00,1.437429e+00,1.412571e+00,1.383670e+00,1.351749e+00,1.317954e+00,1.283431e+00,
    1.249221e+00,1.216200e+00,1.185043e+00,1.156226e+00,1.130034e+00,1.106598e+00,1.085916e+00,
    1.067888e+00,1.052343e+00,1.039061e+00,1.027798e+00,1.018293e+00,1.010290e+00,1.003540e+00,
    9.978077e-01,9.928816e-01,9.885710e-01,9.847092e-01,9.811537e-01,9.777849e-01,9.745051e-01,
    9.712366e-01,9.679192e-01,9.645088e-01,9.609745e-01,9.572969e-01,9.534658e-01,9.494785e-01,
    9.453386e-01,9.410538e-01,9.366353e-01,9.320965e-01,9.274525e-01,9.227188e-01,9.179114e-01,
    9.130459e-01,9.081377e-01,9.032012e-01,8.982499e-01,8.932966e-01,8.883526e-01,8.834286e-01,
    8.785340e-01,8.736773e-01,8.688658e-01,8.641061e-01,8.594039e-01,8.547640e-01,8.501906e-01,
    8.456872e-01,8.412565e-01,8.369010e-01,8.326224e-01,8.284221e-01,8.243011e-01,8.202600e-01,
    8.162992e-01,8.124188e-01,8.086186e-01,8.048983e-01,8.012573e-01,7.976949e-01,7.942105e-01,
    7.908030e-01,7.874715e-01,7.842150e-01,7.810324e-01,7.779224e-01,7.748840e-01,7.719159e-01,
    7.690169e-01,7.661858e-01,7.634213e-01,7.607221e-01,7.580872e-01,7.555153e-01,7.530051e-01,
    7.505555e-01,7.481653e-01,7.458334e-01,7.435587e-01,7.413400e-01,7.391763e-01,7.370666e-01,
    7.350099e-01,7.330050e-01,7.310511e-01,7.291473e-01,7.272925e-01,7.254859e-01,7.237267e-01,
    7.220140e-01,7.203469e-01,7.187247e-01,7.171466e-01,7.156118e-01,7.141196e-01,7.126694e-01,
    7.112604e-01,7.098919e-01,7.085633e-01,7.072740e-01,7.060234e-01,7.048108e-01,7.036357e-01,
    7.024976e-01,7.013959e-01,7.003300e-01,6.992995e-01,6.983039e-01,6.973426e-01,6.964153e-01,
    6.955214e-01,6.946606e-01,6.938324e-01,6.930363e-01,6.922720e-01,6.915391e-01,6.908371e-01,
    6.901658e-01,6.895248e-01,6.889136e-01,6.883320e-01,6.877797e-01,6.872562e-01,6.867613e-01,
    6.862947e-01,6.858561e-01,6.854452e-01,6.850616e-01,6.847052e-01,6.843755e-01,6.840725e-01,
    6.837958e-01,6.835452e-01,6.833203e-01,6.831211e-01,6.829472e-01,6.827983e-01,6.826744e-01,
    6.825752e-01,6.825004e-01,6.824498e-01,6.824233e-01,6.824206e-01,6.824416e-01,6.824860e-01};

  static const G4double SH6[nH]={
    5.445765e-01,5.259720e-01,5.092147e-01,4.943922e-01,4.816568e-01,4.712475e-01,4.635187e-01,
    4.589754e-01,4.583113e-01,4.624465e-01,4.725502e-01,4.900327e-01,5.164768e-01,5.534896e-01,
    6.024632e-01,6.642824e-01,7.390624e-01,8.260352e-01,9.236699e-01,1.030017e+00,1.143159e+00,
    1.261606e+00,1.384524e+00,1.511770e+00,1.643781e+00,1.781393e+00,1.925637e+00,2.077538e+00,
    2.237925e+00,2.407239e+00,2.585320e+00,2.771179e+00,2.962748e+00,3.156646e+00,3.348019e+00,
    3.530524e+00,3.696566e+00,3.837865e+00,3.946347e+00,4.015266e+00,4.040307e+00,4.020377e+00,
    3.957835e+00,3.858084e+00,3.728662e+00,3.578110e+00,3.414904e+00,3.246656e+00,3.079662e+00,
    2.918748e+00,2.767352e+00,2.627710e+00,2.501104e+00,2.388097e+00,2.288730e+00,2.202685e+00,
    2.129397e+00,2.068133e+00,2.018030e+00,1.978119e+00,1.947324e+00,1.924455e+00,1.908200e+00,
    1.897116e+00,1.889649e+00,1.884158e+00,1.878985e+00,1.872537e+00,1.863397e+00,1.850433e+00,
    1.832895e+00,1.810468e+00,1.783286e+00,1.751879e+00,1.717086e+00,1.679941e+00,1.641548e+00,
    1.602978e+00,1.565190e+00,1.528974e+00,1.494936e+00,1.463490e+00,1.434878e+00,1.409189e+00,
    1.386391e+00,1.366358e+00,1.348893e+00,1.333755e+00,1.320678e+00,1.309384e+00,1.299598e+00,
    1.291059e+00,1.283522e+00,1.276765e+00,1.270593e+00,1.264837e+00,1.259353e+00,1.254023e+00,
    1.248754e+00,1.243470e+00,1.238117e+00,1.232657e+00,1.227063e+00,1.221321e+00,1.215428e+00,
    1.209386e+00,1.203202e+00,1.196890e+00,1.190463e+00,1.183940e+00,1.177337e+00,1.170673e+00,
    1.163967e+00,1.157235e+00,1.150494e+00,1.143761e+00,1.137048e+00,1.130370e+00,1.123738e+00,
    1.117164e+00,1.110656e+00,1.104223e+00,1.097872e+00,1.091609e+00,1.085440e+00,1.079369e+00,
    1.073400e+00,1.067535e+00,1.061776e+00,1.056126e+00,1.050585e+00,1.045154e+00,1.039834e+00,
    1.034624e+00,1.029523e+00,1.024532e+00,1.019650e+00,1.014875e+00,1.010206e+00,1.005642e+00,
    1.001182e+00,9.968228e-01,9.925643e-01,9.884044e-01,9.843414e-01,9.803736e-01,9.764992e-01,
    9.727164e-01,9.690235e-01,9.654188e-01,9.619005e-01,9.584670e-01,9.551164e-01,9.518472e-01,
    9.486577e-01,9.455463e-01,9.425115e-01,9.395517e-01,9.366654e-01,9.338511e-01,9.311074e-01,
    9.284330e-01,9.258264e-01,9.232864e-01,9.208117e-01,9.184010e-01,9.160531e-01,9.137668e-01,
    9.115411e-01,9.093748e-01,9.072668e-01,9.052161e-01,9.032217e-01,9.012826e-01,8.993979e-01,
    8.975665e-01,8.957878e-01,8.940607e-01,8.923845e-01,8.907582e-01,8.891813e-01,8.876527e-01,
    8.861720e-01,8.847382e-01,8.833507e-01,8.820089e-01,8.807120e-01,8.794595e-01,8.782507e-01,
    8.770849e-01,8.759618e-01,8.748805e-01,8.738407e-01,8.728417e-01,8.718831e-01,8.709643e-01,
    8.700849e-01,8.692443e-01,8.684421e-01,8.676779e-01,8.669512e-01,8.662615e-01,8.656085e-01,
    8.649917e-01,8.644108e-01,8.638654e-01,8.633550e-01,8.628794e-01,8.624381e-01,8.620308e-01,
    8.616572e-01,8.613169e-01,8.610097e-01,8.607351e-01,8.604929e-01,8.602828e-01,8.601045e-01,
    8.599577e-01,8.598422e-01,8.597575e-01,8.597036e-01,8.596801e-01,8.596867e-01,8.597232e-01};

  static const G4double SH7[nH]={
    6.601789e-01,6.429790e-01,6.279582e-01,6.152537e-01,6.050918e-01,5.978188e-01,5.939410e-01,
    5.941728e-01,5.994893e-01,6.111752e-01,6.308524e-01,6.604603e-01,7.021523e-01,7.580774e-01,
    8.300428e-01,9.191103e-01,1.025252e+00,1.147222e+00,1.282761e+00,1.429089e+00,1.583531e+00,
    1.744040e+00,1.909484e+00,2.079667e+00,2.255167e+00,2.437080e+00,2.626743e+00,2.825481e+00,
    3.034365e+00,3.253973e+00,3.484139e+00,3.723691e+00,3.970159e+00,4.219512e+00,4.465971e+00,
    4.701980e+00,4.918458e+00,5.105403e+00,5.252875e+00,5.352255e+00,5.397546e+00,5.386392e+00,
    5.320522e+00,5.205479e+00,5.049727e+00,4.863392e+00,4.656975e+00,4.440289e+00,4.221774e+00,
    4.008169e+00,3.804492e+00,3.614203e+00,3.439460e+00,3.281400e+00,3.140395e+00,3.016263e+00,
    2.908433e+00,2.816062e+00,2.738112e+00,2.673395e+00,2.620597e+00,2.578285e+00,2.544911e+00,
    2.518816e+00,2.498248e+00,2.481393e+00,2.466431e+00,2.451611e+00,2.435343e+00,2.416298e+00,
    2.393491e+00,2.366348e+00,2.334725e+00,2.298889e+00,2.259451e+00,2.217280e+00,2.173389e+00,
    2.128833e+00,2.084617e+00,2.041630e+00,2.000598e+00,1.962072e+00,1.926419e+00,1.893839e+00,
    1.864390e+00,1.838007e+00,1.814532e+00,1.793744e+00,1.775373e+00,1.759127e+00,1.744707e+00,
    1.731819e+00,1.720184e+00,1.709544e+00,1.699669e+00,1.690356e+00,1.681435e+00,1.672762e+00,
    1.664220e+00,1.655719e+00,1.647190e+00,1.638584e+00,1.629868e+00,1.621024e+00,1.612045e+00,
    1.602933e+00,1.593696e+00,1.584347e+00,1.574905e+00,1.565388e+00,1.555818e+00,1.546216e+00,
    1.536603e+00,1.526999e+00,1.517424e+00,1.507896e+00,1.498432e+00,1.489047e+00,1.479755e+00,
    1.470568e+00,1.461496e+00,1.452549e+00,1.443734e+00,1.435059e+00,1.426529e+00,1.418148e+00,
    1.409919e+00,1.401847e+00,1.393931e+00,1.386174e+00,1.378576e+00,1.371137e+00,1.363856e+00,
    1.356733e+00,1.349767e+00,1.342956e+00,1.336298e+00,1.329791e+00,1.323434e+00,1.317223e+00,
    1.311158e+00,1.305234e+00,1.299449e+00,1.293802e+00,1.288289e+00,1.282908e+00,1.277655e+00,
    1.272529e+00,1.267527e+00,1.262647e+00,1.257885e+00,1.253239e+00,1.248707e+00,1.244286e+00,
    1.239975e+00,1.235770e+00,1.231670e+00,1.227671e+00,1.223773e+00,1.219973e+00,1.216269e+00,
    1.212660e+00,1.209142e+00,1.205714e+00,1.202375e+00,1.199123e+00,1.195955e+00,1.192871e+00,
    1.189869e+00,1.186947e+00,1.184104e+00,1.181338e+00,1.178648e+00,1.176032e+00,1.173489e+00,
    1.171019e+00,1.168619e+00,1.166288e+00,1.164026e+00,1.161831e+00,1.159702e+00,1.157638e+00,
    1.155638e+00,1.153701e+00,1.151825e+00,1.150011e+00,1.148257e+00,1.146563e+00,1.144926e+00,
    1.143348e+00,1.141826e+00,1.140360e+00,1.138949e+00,1.137593e+00,1.136290e+00,1.135041e+00,
    1.133844e+00,1.132699e+00,1.131605e+00,1.130561e+00,1.129567e+00,1.128623e+00,1.127727e+00,
    1.126879e+00,1.126079e+00,1.125326e+00,1.124620e+00,1.123959e+00,1.123344e+00,1.122775e+00,
    1.122249e+00,1.121768e+00,1.121331e+00,1.120937e+00,1.120586e+00,1.120278e+00,1.120011e+00,
    1.119786e+00,1.119603e+00,1.119460e+00,1.119358e+00,1.119296e+00,1.119274e+00,1.119291e+00};

  static const G4double SH8[nH]={
    8.326620e-01,8.187860e-01,8.074156e-01,7.987501e-01,7.931123e-01,7.909917e-01,7.931005e-01,
    8.004391e-01,8.143676e-01,8.366683e-01,8.695766e-01,9.157404e-01,9.780603e-01,1.059371e+00,
    1.161963e+00,1.287041e+00,1.434275e+00,1.601688e+00,1.785989e+00,1.983289e+00,2.189948e+00,
    2.403252e+00,2.621760e+00,2.845287e+00,3.074662e+00,3.311376e+00,3.557223e+00,3.813967e+00,
    4.083048e+00,4.365291e+00,4.660621e+00,4.967743e+00,5.283822e+00,5.604165e+00,5.921995e+00,
    6.228396e+00,6.512543e+00,6.762334e+00,6.965444e+00,7.110740e+00,7.189809e+00,7.198283e+00,
    7.136586e+00,7.009905e+00,6.827389e+00,6.600804e+00,6.343002e+00,6.066546e+00,5.782692e+00,
    5.500809e+00,5.228183e+00,4.970100e+00,4.730096e+00,4.510269e+00,4.311603e+00,4.134245e+00,
    3.977736e+00,3.841183e+00,3.723389e+00,3.622934e+00,3.538226e+00,3.467539e+00,3.409022e+00,
    3.360729e+00,3.320633e+00,3.286663e+00,3.256758e+00,3.228933e+00,3.201359e+00,3.172457e+00,
    3.140973e+00,3.106044e+00,3.067232e+00,3.024518e+00,2.978262e+00,2.929127e+00,2.877988e+00,
    2.825830e+00,2.773655e+00,2.722401e+00,2.672884e+00,2.625763e+00,2.581520e+00,2.540465e+00,
    2.502745e+00,2.468369e+00,2.437227e+00,2.409125e+00,2.383802e+00,2.360958e+00,2.340275e+00,
    2.321429e+00,2.304110e+00,2.288023e+00,2.272905e+00,2.258519e+00,2.244664e+00,2.231170e+00,
    2.217898e+00,2.204739e+00,2.191611e+00,2.178454e+00,2.165229e+00,2.151912e+00,2.138493e+00,
    2.124976e+00,2.111368e+00,2.097687e+00,2.083952e+00,2.070187e+00,2.056416e+00,2.042664e+00,
    2.028956e+00,2.015316e+00,2.001766e+00,1.988328e+00,1.975021e+00,1.961862e+00,1.948866e+00,
    1.936048e+00,1.923419e+00,1.910988e+00,1.898765e+00,1.886757e+00,1.874968e+00,1.863402e+00,
    1.852064e+00,1.840954e+00,1.830074e+00,1.819424e+00,1.809004e+00,1.798812e+00,1.788847e+00,
    1.779107e+00,1.769589e+00,1.760290e+00,1.751207e+00,1.742337e+00,1.733676e+00,1.725221e+00,
    1.716968e+00,1.708913e+00,1.701051e+00,1.693380e+00,1.685895e+00,1.678592e+00,1.671467e+00,
    1.664517e+00,1.657738e+00,1.651125e+00,1.644675e+00,1.638385e+00,1.632251e+00,1.626270e+00,
    1.620437e+00,1.614751e+00,1.609207e+00,1.603802e+00,1.598534e+00,1.593400e+00,1.588396e+00,
    1.583519e+00,1.578768e+00,1.574140e+00,1.569631e+00,1.565240e+00,1.560964e+00,1.556801e+00,
    1.552749e+00,1.548805e+00,1.544967e+00,1.541233e+00,1.537602e+00,1.534071e+00,1.530639e+00,
    1.527303e+00,1.524063e+00,1.520915e+00,1.517860e+00,1.514895e+00,1.512018e+00,1.509229e+00,
    1.506525e+00,1.503906e+00,1.501370e+00,1.498916e+00,1.496542e+00,1.494247e+00,1.492031e+00,
    1.489891e+00,1.487827e+00,1.485838e+00,1.483923e+00,1.482080e+00,1.480310e+00,1.478609e+00,
    1.476979e+00,1.475418e+00,1.473924e+00,1.472498e+00,1.471138e+00,1.469844e+00,1.468614e+00,
    1.467449e+00,1.466346e+00,1.465306e+00,1.464328e+00,1.463411e+00,1.462555e+00,1.461758e+00,
    1.461021e+00,1.460342e+00,1.459721e+00,1.459157e+00,1.458650e+00,1.458199e+00,1.457804e+00,
    1.457464e+00,1.457179e+00,1.456948e+00,1.456770e+00,1.456645e+00,1.456574e+00,1.456554e+00};

  static const G4double SH9[nH]={
    1.425410e+00,1.421381e+00,1.420064e+00,1.421846e+00,1.427350e+00,1.437522e+00,1.453729e+00,
    1.477877e+00,1.512528e+00,1.560992e+00,1.627344e+00,1.716285e+00,1.832783e+00,1.981419e+00,
    2.165508e+00,2.386174e+00,2.641752e+00,2.927832e+00,3.238097e+00,3.565696e+00,3.904657e+00,
    4.250887e+00,4.602541e+00,4.959837e+00,5.324557e+00,5.699439e+00,6.087597e+00,6.492035e+00,
    6.915215e+00,7.358674e+00,7.822640e+00,8.305618e+00,8.803962e+00,9.311461e+00,9.819000e+00,
    1.031441e+01,1.078267e+01,1.120652e+01,1.156771e+01,1.184869e+01,1.203464e+01,1.211552e+01,
    1.208752e+01,1.195370e+01,1.172359e+01,1.141180e+01,1.103616e+01,1.061560e+01,1.016840e+01,
    9.710864e+00,9.256615e+00,8.816313e+00,8.397788e+00,8.006357e+00,7.645246e+00,7.316023e+00,
    7.018988e+00,6.753498e+00,6.518232e+00,6.311380e+00,6.130788e+00,5.974058e+00,5.838619e+00,
    5.721783e+00,5.620790e+00,5.532858e+00,5.455242e+00,5.385289e+00,5.320521e+00,5.258707e+00,
    5.197938e+00,5.136693e+00,5.073880e+00,5.008852e+00,4.941393e+00,4.871670e+00,4.800166e+00,
    4.727591e+00,4.654788e+00,4.582647e+00,4.512022e+00,4.443675e+00,4.378228e+00,4.316142e+00,
    4.257712e+00,4.203068e+00,4.152199e+00,4.104970e+00,4.061152e+00,4.020445e+00,3.982508e+00,
    3.946978e+00,3.913492e+00,3.881699e+00,3.851274e+00,3.821926e+00,3.793398e+00,3.765476e+00,
    3.737984e+00,3.710781e+00,3.683763e+00,3.656854e+00,3.630006e+00,3.603191e+00,3.576400e+00,
    3.549640e+00,3.522927e+00,3.496284e+00,3.469742e+00,3.443334e+00,3.417094e+00,3.391057e+00,
    3.365257e+00,3.339727e+00,3.314496e+00,3.289591e+00,3.265039e+00,3.240859e+00,3.217072e+00,
    3.193692e+00,3.170733e+00,3.148206e+00,3.126119e+00,3.104477e+00,3.083284e+00,3.062543e+00,
    3.042254e+00,3.022416e+00,3.003026e+00,2.984082e+00,2.965578e+00,2.947509e+00,2.929870e+00,
    2.912654e+00,2.895854e+00,2.879463e+00,2.863473e+00,2.847876e+00,2.832664e+00,2.817830e+00,
    2.803363e+00,2.789258e+00,2.775505e+00,2.762096e+00,2.749024e+00,2.736279e+00,2.723855e+00,
    2.711744e+00,2.699939e+00,2.688431e+00,2.677214e+00,2.666280e+00,2.655624e+00,2.645238e+00,
    2.635115e+00,2.625250e+00,2.615637e+00,2.606269e+00,2.597141e+00,2.588247e+00,2.579582e+00,
    2.571141e+00,2.562918e+00,2.554909e+00,2.547110e+00,2.539514e+00,2.532119e+00,2.524920e+00,
    2.517912e+00,2.511092e+00,2.504456e+00,2.498001e+00,2.491721e+00,2.485615e+00,2.479679e+00,
    2.473909e+00,2.468302e+00,2.462856e+00,2.457567e+00,2.452433e+00,2.447450e+00,2.442617e+00,
    2.437930e+00,2.433387e+00,2.428985e+00,2.424723e+00,2.420598e+00,2.416608e+00,2.412750e+00,
    2.409023e+00,2.405425e+00,2.401953e+00,2.398607e+00,2.395383e+00,2.392280e+00,2.389297e+00,
    2.386432e+00,2.383683e+00,2.381049e+00,2.378528e+00,2.376118e+00,2.373819e+00,2.371629e+00,
    2.369546e+00,2.367569e+00,2.365697e+00,2.363929e+00,2.362263e+00,2.360698e+00,2.359233e+00,
    2.357868e+00,2.356600e+00,2.355429e+00,2.354353e+00,2.353373e+00,2.352486e+00,2.351691e+00,
    2.350989e+00,2.350378e+00,2.349856e+00,2.349424e+00,2.349080e+00,2.348823e+00,2.348653e+00};

  static const G4double SH10[nH]={
    3.918292e+00,3.904931e+00,3.893792e+00,3.886847e+00,3.886858e+00,3.897612e+00,3.924175e+00,
    3.973155e+00,4.052892e+00,4.173448e+00,4.346251e+00,4.583168e+00,4.894929e+00,5.289011e+00,
    5.767472e+00,6.325587e+00,6.952077e+00,7.631192e+00,8.346046e+00,9.081993e+00,9.828955e+00,
    1.058224e+01,1.134205e+01,1.211228e+01,1.289914e+01,1.370979e+01,1.455140e+01,1.543028e+01,
    1.635126e+01,1.731704e+01,1.832759e+01,1.937949e+01,2.046524e+01,2.157253e+01,2.268371e+01,
    2.377554e+01,2.481942e+01,2.578236e+01,2.662884e+01,2.732355e+01,2.783477e+01,2.813791e+01,
    2.821866e+01,2.807489e+01,2.771701e+01,2.716665e+01,2.645395e+01,2.561414e+01,2.468397e+01,
    2.369877e+01,2.269021e+01,2.168502e+01,2.070456e+01,1.976487e+01,1.887726e+01,1.804896e+01,
    1.728387e+01,1.658334e+01,1.594670e+01,1.537185e+01,1.485564e+01,1.439420e+01,1.398317e+01,
    1.361791e+01,1.329360e+01,1.300539e+01,1.274849e+01,1.251824e+01,1.231020e+01,1.212025e+01,
    1.194464e+01,1.178007e+01,1.162372e+01,1.147334e+01,1.132717e+01,1.118402e+01,1.104314e+01,
    1.090421e+01,1.076726e+01,1.063254e+01,1.050046e+01,1.037150e+01,1.024612e+01,1.012471e+01,
    1.000757e+01,9.894853e+00,9.786587e+00,9.682662e+00,9.582857e+00,9.486860e+00,9.394299e+00,
    9.304761e+00,9.217824e+00,9.133076e+00,9.050131e+00,8.968642e+00,8.888313e+00,8.808898e+00,
    8.730205e+00,8.652092e+00,8.574465e+00,8.497273e+00,8.420498e+00,8.344153e+00,8.268275e+00,
    8.192917e+00,8.118146e+00,8.044035e+00,7.970660e+00,7.898102e+00,7.826434e+00,7.755730e+00,
    7.686057e+00,7.617475e+00,7.550037e+00,7.483791e+00,7.418775e+00,7.355021e+00,7.292555e+00,
    7.231395e+00,7.171554e+00,7.113040e+00,7.055853e+00,6.999992e+00,6.945450e+00,6.892218e+00,
    6.840281e+00,6.789624e+00,6.740229e+00,6.692076e+00,6.645144e+00,6.599408e+00,6.554846e+00,
    6.511433e+00,6.469143e+00,6.427951e+00,6.387831e+00,6.348757e+00,6.310703e+00,6.273644e+00,
    6.237554e+00,6.202409e+00,6.168183e+00,6.134852e+00,6.102393e+00,6.070783e+00,6.039998e+00,
    6.010017e+00,5.980819e+00,5.952382e+00,5.924686e+00,5.897711e+00,5.871440e+00,5.845852e+00,
    5.820930e+00,5.796658e+00,5.773017e+00,5.749993e+00,5.727569e+00,5.705730e+00,5.684462e+00,
    5.663751e+00,5.643583e+00,5.623944e+00,5.604824e+00,5.586208e+00,5.568086e+00,5.550445e+00,
    5.533276e+00,5.516567e+00,5.500309e+00,5.484490e+00,5.469103e+00,5.454137e+00,5.439585e+00,
    5.425436e+00,5.411684e+00,5.398319e+00,5.385335e+00,5.372724e+00,5.360478e+00,5.348591e+00,
    5.337056e+00,5.325867e+00,5.315016e+00,5.304499e+00,5.294309e+00,5.284441e+00,5.274889e+00,
    5.265647e+00,5.256711e+00,5.248076e+00,5.239736e+00,5.231688e+00,5.223926e+00,5.216445e+00,
    5.209243e+00,5.202315e+00,5.195656e+00,5.189262e+00,5.183131e+00,5.177258e+00,5.171640e+00,
    5.166273e+00,5.161154e+00,5.156279e+00,5.151646e+00,5.147251e+00,5.143092e+00,5.139165e+00,
    5.135467e+00,5.131996e+00,5.128750e+00,5.125724e+00,5.122918e+00,5.120328e+00,5.117952e+00,
    5.115788e+00,5.113833e+00,5.112084e+00,5.110541e+00,5.109201e+00,5.108061e+00,5.107120e+00};

  static const G4double SH11[nH]={
    7.590321e+00,7.509120e+00,7.439927e+00,7.389122e+00,7.365094e+00,7.378718e+00,7.443759e+00,
    7.577042e+00,7.798111e+00,8.128047e+00,8.587142e+00,9.191438e+00,9.948696e+00,1.085507e+01,
    1.189410e+01,1.303902e+01,1.425810e+01,1.552108e+01,1.680454e+01,1.809464e+01,1.938727e+01,
    2.068629e+01,2.200102e+01,2.334386e+01,2.472823e+01,2.616717e+01,2.767219e+01,2.925247e+01,
    3.091405e+01,3.265909e+01,3.448491e+01,3.638304e+01,3.833817e+01,4.032706e+01,4.231784e+01,
    4.426974e+01,4.613359e+01,4.785354e+01,4.937004e+01,5.062412e+01,5.156263e+01,5.214377e+01,
    5.234188e+01,5.215066e+01,5.158403e+01,5.067443e+01,4.946906e+01,4.802467e+01,4.640214e+01,
    4.466144e+01,4.285783e+01,4.103934e+01,3.924553e+01,3.750731e+01,3.584751e+01,3.428176e+01,
    3.281973e+01,3.146624e+01,3.022230e+01,2.908612e+01,2.805376e+01,2.711986e+01,2.627800e+01,
    2.552116e+01,2.484197e+01,2.423289e+01,2.368645e+01,2.319532e+01,2.275246e+01,2.235122e+01,
    2.198543e+01,2.164944e+01,2.133824e+01,2.104744e+01,2.077331e+01,2.051280e+01,2.026347e+01,
    2.002347e+01,1.979144e+01,1.956643e+01,1.934784e+01,1.913527e+01,1.892847e+01,1.872725e+01,
    1.853145e+01,1.834084e+01,1.815514e+01,1.797402e+01,1.779705e+01,1.762379e+01,1.745373e+01,
    1.728639e+01,1.712128e+01,1.695797e+01,1.679605e+01,1.663522e+01,1.647522e+01,1.631586e+01,
    1.615706e+01,1.599876e+01,1.584098e+01,1.568380e+01,1.552732e+01,1.537170e+01,1.521709e+01,
    1.506367e+01,1.491163e+01,1.476115e+01,1.461241e+01,1.446558e+01,1.432082e+01,1.417827e+01,
    1.403806e+01,1.390031e+01,1.376511e+01,1.363253e+01,1.350265e+01,1.337550e+01,1.325113e+01,
    1.312955e+01,1.301078e+01,1.289481e+01,1.278164e+01,1.267124e+01,1.256360e+01,1.245866e+01,
    1.235641e+01,1.225680e+01,1.215977e+01,1.206529e+01,1.197330e+01,1.188374e+01,1.179657e+01,
    1.171172e+01,1.162914e+01,1.154878e+01,1.147057e+01,1.139446e+01,1.132039e+01,1.124831e+01,
    1.117817e+01,1.110991e+01,1.104348e+01,1.097882e+01,1.091590e+01,1.085465e+01,1.079504e+01,
    1.073701e+01,1.068053e+01,1.062555e+01,1.057202e+01,1.051991e+01,1.046918e+01,1.041979e+01,
    1.037170e+01,1.032488e+01,1.027930e+01,1.023491e+01,1.019170e+01,1.014962e+01,1.010865e+01,
    1.006876e+01,1.002992e+01,9.992112e+00,9.955302e+00,9.919468e+00,9.884586e+00,9.850632e+00,
    9.817586e+00,9.785425e+00,9.754129e+00,9.723679e+00,9.694054e+00,9.665237e+00,9.637210e+00,
    9.609955e+00,9.583456e+00,9.557697e+00,9.532663e+00,9.508339e+00,9.484709e+00,9.461761e+00,
    9.439481e+00,9.417856e+00,9.396874e+00,9.376521e+00,9.356788e+00,9.337661e+00,9.319131e+00,
    9.301186e+00,9.283817e+00,9.267013e+00,9.250765e+00,9.235064e+00,9.219900e+00,9.205264e+00,
    9.191149e+00,9.177546e+00,9.164447e+00,9.151845e+00,9.139731e+00,9.128099e+00,9.116942e+00,
    9.106252e+00,9.096024e+00,9.086249e+00,9.076924e+00,9.068040e+00,9.059593e+00,9.051576e+00,
    9.043985e+00,9.036813e+00,9.030055e+00,9.023706e+00,9.017761e+00,9.012215e+00,9.007064e+00,
    9.002302e+00,8.997926e+00,8.993930e+00,8.990311e+00,8.987065e+00,8.984186e+00,8.981672e+00};

  static const G4double SH12[nH]={
    1.274173e+01,1.261154e+01,1.253680e+01,1.253678e+01,1.263549e+01,1.286155e+01,1.324712e+01,
    1.382513e+01,1.462465e+01,1.566474e+01,1.694815e+01,1.845745e+01,2.015574e+01,2.199299e+01,
    2.391598e+01,2.587848e+01,2.784823e+01,2.980938e+01,3.176107e+01,3.371382e+01,3.568546e+01,
    3.769757e+01,3.977278e+01,4.193303e+01,4.419831e+01,4.658588e+01,4.910951e+01,5.177874e+01,
    5.459799e+01,5.756537e+01,6.067137e+01,6.389725e+01,6.721344e+01,7.057806e+01,7.393584e+01,
    7.721799e+01,8.034325e+01,8.322076e+01,8.575479e+01,8.785141e+01,8.942639e+01,9.041346e+01,
    9.077151e+01,9.048934e+01,8.958707e+01,8.811385e+01,8.614235e+01,8.376122e+01,8.106674e+01,
    7.815525e+01,7.511689e+01,7.203142e+01,6.896584e+01,6.597376e+01,6.309588e+01,6.036126e+01,
    5.778900e+01,5.538996e+01,5.316850e+01,5.112392e+01,4.925181e+01,4.754501e+01,4.599451e+01,
    4.459006e+01,4.332071e+01,4.217515e+01,4.114200e+01,4.021009e+01,3.936858e+01,3.860711e+01,
    3.791594e+01,3.728600e+01,3.670900e+01,3.617743e+01,3.568464e+01,3.522478e+01,3.479282e+01,
    3.438448e+01,3.399619e+01,3.362496e+01,3.326834e+01,3.292429e+01,3.259109e+01,3.226728e+01,
    3.195155e+01,3.164275e+01,3.133978e+01,3.104162e+01,3.074731e+01,3.045594e+01,3.016670e+01,
    2.987885e+01,2.959177e+01,2.930495e+01,2.901801e+01,2.873069e+01,2.844289e+01,2.815459e+01,
    2.786589e+01,2.757701e+01,2.728821e+01,2.699984e+01,2.671229e+01,2.642597e+01,2.614131e+01,
    2.585875e+01,2.557869e+01,2.530156e+01,2.502772e+01,2.475752e+01,2.449130e+01,2.422931e+01,
    2.397182e+01,2.371903e+01,2.347111e+01,2.322821e+01,2.299044e+01,2.275787e+01,2.253056e+01,
    2.230854e+01,2.209182e+01,2.188038e+01,2.167419e+01,2.147321e+01,2.127737e+01,2.108662e+01,
    2.090087e+01,2.072003e+01,2.054401e+01,2.037271e+01,2.020604e+01,2.004387e+01,1.988611e+01,
    1.973265e+01,1.958337e+01,1.943817e+01,1.929694e+01,1.915958e+01,1.902597e+01,1.889600e+01,
    1.876959e+01,1.864662e+01,1.852699e+01,1.841062e+01,1.829740e+01,1.818725e+01,1.808008e+01,
    1.797579e+01,1.787432e+01,1.777556e+01,1.767946e+01,1.758593e+01,1.749489e+01,1.740629e+01,
    1.732004e+01,1.723608e+01,1.715436e+01,1.707480e+01,1.699735e+01,1.692196e+01,1.684856e+01,
    1.677711e+01,1.670754e+01,1.663983e+01,1.657390e+01,1.650973e+01,1.644726e+01,1.638646e+01,
    1.632728e+01,1.626968e+01,1.621363e+01,1.615908e+01,1.610601e+01,1.605438e+01,1.600415e+01,
    1.595530e+01,1.590779e+01,1.586160e+01,1.581669e+01,1.577303e+01,1.573061e+01,1.568940e+01,
    1.564937e+01,1.561049e+01,1.557275e+01,1.553612e+01,1.550058e+01,1.546611e+01,1.543269e+01,
    1.540030e+01,1.536892e+01,1.533854e+01,1.530912e+01,1.528067e+01,1.525316e+01,1.522657e+01,
    1.520089e+01,1.517611e+01,1.515221e+01,1.512917e+01,1.510699e+01,1.508564e+01,1.506512e+01,
    1.504542e+01,1.502651e+01,1.500840e+01,1.499106e+01,1.497449e+01,1.495868e+01,1.494362e+01,
    1.492928e+01,1.491568e+01,1.490279e+01,1.489061e+01,1.487912e+01,1.486832e+01,1.485821e+01,
    1.484876e+01,1.483998e+01,1.483185e+01,1.482437e+01,1.481753e+01,1.481133e+01,1.480575e+01};

  static const G4double SH13[nH]={
    1.444282e+01,1.433200e+01,1.430197e+01,1.437852e+01,1.459264e+01,
    1.497946e+01,1.557548e+01,1.641373e+01,1.751708e+01,1.889114e+01,
    2.051932e+01,2.236282e+01,2.436683e+01,2.647125e+01,2.862233e+01,
    3.078116e+01,3.292713e+01,3.505686e+01,3.718040e+01,3.931668e+01,
    4.148940e+01,4.372398e+01,4.604543e+01,4.847705e+01,5.103960e+01,
    5.375059e+01,5.662368e+01,5.966783e+01,6.288632e+01,6.627543e+01,
    6.982292e+01,7.350621e+01,7.729059e+01,8.112759e+01,8.495381e+01,
    8.869083e+01,9.224647e+01,9.551803e+01,9.839764e+01,1.007797e+02,
    1.025696e+02,1.036932e+02,1.041041e+02,1.037897e+02,1.027723e+02,
    1.011066e+02,9.887372e+01,9.617299e+01,9.311251e+01,8.980074e+01,
    8.633954e+01,8.281934e+01,7.931645e+01,7.589222e+01,7.259351e+01,
    6.945404e+01,6.649625e+01,6.373321e+01,6.117052e+01,5.880801e+01,
    5.664119e+01,5.466241e+01,5.286187e+01,5.122830e+01,4.974958e+01,
    4.841316e+01,4.720643e+01,4.611690e+01,4.513247e+01,4.424158e+01,
    4.343327e+01,4.269734e+01,4.202440e+01,4.140591e+01,4.083420e+01,
    4.030251e+01,3.980491e+01,3.933629e+01,3.889231e+01,3.846927e+01,
    3.806404e+01,3.767398e+01,3.729683e+01,3.693063e+01,3.657365e+01,
    3.622434e+01,3.588127e+01,3.554314e+01,3.520875e+01,3.487703e+01,
    3.454699e+01,3.421782e+01,3.388880e+01,3.355941e+01,3.322925e+01,
    3.289811e+01,3.256591e+01,3.223270e+01,3.189868e+01,3.156414e+01,
    3.122945e+01,3.089506e+01,3.056146e+01,3.022918e+01,2.989873e+01,
    2.957065e+01,2.924544e+01,2.892361e+01,2.860559e+01,2.829182e+01,
    2.798266e+01,2.767845e+01,2.737947e+01,2.708598e+01,2.679819e+01,
    2.651624e+01,2.624029e+01,2.597040e+01,2.570666e+01,2.544908e+01,
    2.519768e+01,2.495244e+01,2.471333e+01,2.448028e+01,2.425323e+01,
    2.403210e+01,2.381680e+01,2.360721e+01,2.340323e+01,2.320475e+01,
    2.301165e+01,2.282379e+01,2.264106e+01,2.246332e+01,2.229045e+01,
    2.212233e+01,2.195881e+01,2.179978e+01,2.164512e+01,2.149469e+01,
    2.134838e+01,2.120607e+01,2.106765e+01,2.093300e+01,2.080201e+01,
    2.067458e+01,2.055060e+01,2.042998e+01,2.031261e+01,2.019839e+01,
    2.008725e+01,1.997909e+01,1.987382e+01,1.977137e+01,1.967165e+01,
    1.957458e+01,1.948010e+01,1.938812e+01,1.929859e+01,1.921144e+01,
    1.912659e+01,1.904399e+01,1.896358e+01,1.888531e+01,1.880911e+01,
    1.873493e+01,1.866273e+01,1.859245e+01,1.852404e+01,1.845746e+01,
    1.839266e+01,1.832961e+01,1.826826e+01,1.820857e+01,1.815050e+01,
    1.809402e+01,1.803908e+01,1.798567e+01,1.793373e+01,1.788325e+01,
    1.783418e+01,1.778651e+01,1.774019e+01,1.769521e+01,1.765154e+01,
    1.760914e+01,1.756801e+01,1.752810e+01,1.748940e+01,1.745188e+01,
    1.741553e+01,1.738032e+01,1.734624e+01,1.731325e+01,1.728134e+01,
    1.725050e+01,1.722071e+01,1.719194e+01,1.716418e+01,1.713742e+01,
    1.711164e+01,1.708682e+01,1.706295e+01,1.704001e+01,1.701800e+01,
    1.699688e+01,1.697667e+01,1.695733e+01,1.693885e+01,1.692124e+01,
    1.690446e+01,1.688852e+01,1.687340e+01,1.685909e+01,1.684558e+01,
    1.683285e+01,1.682090e+01,1.680973e+01,1.679931e+01,1.678964e+01,
    1.678072e+01,1.677252e+01,1.676505e+01,1.675830e+01};

  static const G4double* SL[nLA]={
    SL0, SL1, SL2, SL3, SL4, SL5, SL6, SL7, SL8, SL9,SL10,SL11,SL12,SL13,
   SL14,SL15,SL16,SL17,SL18,SL19,SL20,SL21,SL22,SL23,SL24,SL25,SL26,SL27,
   SL28,SL29,SL30,SL31,SL32,SL33,SL34,SL35,SL36,SL37,SL38,SL39,SL40,SL41,
   SL42,SL43,SL44,SL45,SL46,SL47,SL48};

  static const G4double* SH[nHA]={
    SH0,SH1,SH2,SH3,SH4,SH5,SH6,SH7,SH8,SH9,SH10,SH11,SH12,SH13};

  if(a<=.9)
  {
    G4cout << "***G4PhotoNuclearCS::GetFunctions: A=" << a 
           << "(?). No CS returned!" << G4endl;
    return -1;
  }
  G4int r=0;                    // Low channel for GDR (filling-flag for GDR)
  for(G4int i=0; i<nLA; i++) if(std::abs(a-LA[i])<.0005)
  {
    for(G4int k=0; k<nL; k++) y[k]=SL[i][k];
    r=1;                          // Flag of filled GDR part 
  }
  G4int h=0;
  for(G4int j=0; j<nHA; j++) if(std::abs(a-HA[j])<.0005)
  {
    for(G4int k=0; k<nH; k++) z[k]=SH[j][k];
    h=1;                          // Flag of filled GDR part 
  }
  if(!r)                          // GDR part is not filled
  {
    G4int k=0;                    // !! To be good for different compilers !!
    for(k=1; k<nLA; k++) if(a<LA[k]) break;
    if(k<1) k=1;                  // Extrapolation from the first bin (D/He)
    if(k>=nLA) k=nLA-1;           // Extrapolation from the last bin (U)
    G4int     k1=k-1;
    G4double  xi=LA[k1];
    G4double   b=(a-xi)/(LA[k]-xi);
    for(G4int m=0; m<nL; m++)
    {
      if(a>1.5)
      {
        G4double yi=SL[k1][m];
        y[m]=yi+(SL[k][m]-yi)*b;
#ifdef debugs
        if(y[m]<0.) G4cout << "G4PhotNucCS::GetF:y=" << y[m] << ",k="
                           << k << ",yi=" << yi << ",ya=" << SL[k][m] 
                           << ",b=" << b << ",xi=" << xi << ",xa=" 
                           << LA[k] << ",a=" << a << G4endl;
#endif
	  }
      else y[m]=0.;
    }
    r=1;
  }
  if(!h)                            // High Energy part is not filled
  {
    G4int k=0;
    for(k=1; k<nHA; k++) if(a<HA[k]) break;
    if(k<1) k=1;                    // Extrapolation from the first bin (D/He)
    if(k>=nHA) k=nHA-1;             // Extrapolation from the last bin (Pu)
    G4int     k1=k-1;
    G4double  xi=HA[k1];
    G4double   b=(a-xi)/(HA[k]-xi);
    for(G4int m=0; m<nH; m++)
    {
      G4double zi=SH[k1][m];
      z[m]=zi+(SH[k][m]-zi)*b;
    }
    h=1;
  }
  return r*h;
}
