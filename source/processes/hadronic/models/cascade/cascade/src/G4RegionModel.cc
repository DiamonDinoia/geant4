#include "../include/G4RegionModel.hh"

G4RegionModel::G4RegionModel(const G4int numberOfLayers, const G4int A, const G4int Z){
  //count the radiuses, densities and fermi momenta with A and Z
  G4double r = radius0*pow(A, 1/3);

  if(numberOfLayers==1){ 
    radius.push_back(r);

    G4double rho = A / (4/3*pi*pow(r,3));
    density.push_back(rho);

    G4double protonMass = G4Proton::Proton()->GetPDGMass();
    G4double neutronMass = G4Neutron::Neutron()->GetPDGMass();
    G4double protonDensity = Z / (4/3*pi*pow(r,3));
    G4double neutronDensity = (A-Z) / (4/3*pi*pow(r,3));

    protonFermiEnergy.push_back(GetFermiEnergy(protonDensity, protonMass));
    neutronFermiEnergy.push_back(GetFermiEnergy(neutronDensity, neutronMass));
    
    protonFermiMomentum.push_back(GetFermiMomentum(protonDensity, protonMass));
    neutronFermiMomentum.push_back(GetFermiMomentum(neutronDensity, neutronMass));

    G4double fermiEP = *protonFermiEnergy.begin();
    G4double fermiEN = *neutronFermiEnergy.begin();
    protonPotentialEnergy.push_back(-(fermiEP + BE));
    neutronPotentialEnergy.push_back(-(fermiEN + BE));
  }
  else{
  if(numberOfLayers==3){
    radius.push_back(0.1*r);
    radius.push_back(0.2*r);
    radius.push_back(0.9*r);
    
  }
  }
}
G4RegionModel::~G4RegionModel(){}

/*
void G4RegionModel::Init(const G4int numberOfLayers, const G4int A, const G4int Z){
  //count the radiuses, densities and fermi momenta with A and Z
  G4double r = radius0*pow(A, 1/3);

  if(numberOfLayers==1){ 
    radius.push_back(r);

    G4double rho = A / (4/3*pi*pow(r,3)); 
    density.push_back(rho);

    G4double protonMass = G4Proton::Proton()->GetPDGMass()/MeV;
    G4double neutronMass = G4Neutron::Neutron()->GetPDGMass()/MeV ;
    G4double protonDensity = Z / (4/3*pi*pow(r,3));
    G4double neutronDensity = (A-Z) / (4/3*pi*pow(r,3));

    protonFermiEnergy.push_back(GetFermiEnergy(protonDensity, protonMass));
    neutronFermiEnergy.push_back(GetFermiEnergy(neutronDensity, neutronMass));
    
    protonFermiMomentum.push_back(GetFermiMomentum(protonDensity, protonMass));
    neutronFermiMomentum.push_back(GetFermiMomentum(neutronDensity, neutronMass));

    G4double fermiEP = *protonFermiEnergy.begin();
    G4double fermiEN = *neutronFermiEnergy.begin();
    protonPotentialEnergy.push_back(-(fermiEP + BE));
    neutronPotentialEnergy.push_back(-(fermiEN + BE));
  }
  else{
  if(numberOfLayers==3){
    radius.push_back(0.1*r);
    radius.push_back(0.2*r);
    radius.push_back(0.9*r);
    
  }
  }
}
*/

//--------------------------------------------------------------

G4double G4RegionModel::GetDensity(G4double r){
  iterator j=density.begin();
     for(iterator i=radius.begin(); i<radius.end(); i++){
     if(r <= *i) return *j;
     j++;
   }
   return 0;
}


G4double G4RegionModel::GetPotentialEnergy(G4double r, G4int particle){
  if(particle == 0){ //proton
    iterator j=protonPotentialEnergy.begin();
     for(iterator i=radius.begin(); i<radius.end(); i++){
     if(r <= *i) return *j;
     j++;
   }
     return 0;
  }
  
  if(particle == 1){ //neutron
    iterator j=neutronPotentialEnergy.begin();
     for(iterator i=radius.begin(); i<radius.end(); i++){
     if(r <= *i) return *j;
     j++;
   }
     return 0;
  }
  return 0;
}

G4double G4RegionModel::GetMaximumNucleonMomentum(G4double r,
						  G4int nucleon){
 
  if(nucleon == 0){
     iterator j=protonFermiMomentum.begin();
     for(iterator i=radius.begin(); i<radius.end(); i++){
     if(r <= *i)  return *j;
     j++;
     }
  }
  if(nucleon==1){
     iterator j=neutronFermiMomentum.begin();
     for(iterator i=radius.begin(); i<radius.end(); i++){
     if(r <= *i)  return *j;
     j++;
     }
  }


}


G4double G4RegionModel::GetFermiMomentum(G4double density,
					 G4double mass){
  
  return sqrt(2*mass*GetFermiEnergy(density, mass));
  
 
}

G4double G4RegionModel::GetFermiEnergy(G4double density,
					 G4double mass){
  
  //G4double hbar = 1.0E-6;
    return (pow(hbar_Planck,2)/(2*mass)*pow((3*pi2*density),2/3)); 
 
 
}














