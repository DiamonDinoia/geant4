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
// neutron_hp -- source file
// J.P. Wellisch, Nov-1996
// A prototype of the low energy neutron transport model.
//
#include "G4NeutronHPEnAngCorrelation.hh"
#include "G4LorentzRotation.hh"
#include "G4LorentzVector.hh"

G4ReactionProduct * G4NeutronHPEnAngCorrelation::SampleOne(G4double anEnergy)
{  
  G4ReactionProduct * result = new G4ReactionProduct;
  
  // do we have an appropriate distribution
  if(nProducts!=1) throw G4HadronicException(__FILE__, __LINE__, "More than one product in SampleOne");
  
  // get the result
  G4ReactionProductVector * temp=0;
  G4int i=0;
  while(temp == 0) temp = theProducts[i++].Sample(anEnergy);
  
  // is the multiplicity correct
  if(temp->size()!=1) throw G4HadronicException(__FILE__, __LINE__, "SampleOne: Yield not correct");
  
  // fill result
  result = temp->operator[](0);
  
  // some garbage collection
  delete temp;
  
  // return result
  return result;
}

G4ReactionProductVector * G4NeutronHPEnAngCorrelation::Sample(G4double anEnergy)
{
  G4ReactionProductVector * result = new G4ReactionProductVector;
  G4int i;
  G4ReactionProductVector * it;
  G4ReactionProduct theCMS;
  G4LorentzRotation toZ;
  if(frameFlag==2)
  {
    // simplify and double check @
    G4ThreeVector the3Neutron = theNeutron.GetMomentum();
    G4double nEnergy = theNeutron.GetTotalEnergy();
    G4ThreeVector the3Target = theTarget.GetMomentum();
    G4double tEnergy = theTarget.GetTotalEnergy();
    G4double totE = nEnergy+tEnergy;
    G4ThreeVector the3CMS = the3Target+the3Neutron;
    theCMS.SetMomentum(the3CMS);
    G4double cmsMom = std::sqrt(the3CMS*the3CMS);
    G4double sqrts = std::sqrt((totE-cmsMom)*(totE+cmsMom));
    theCMS.SetMass(sqrts);
    theCMS.SetTotalEnergy(totE);
    G4ReactionProduct aNeutron;
    aNeutron.Lorentz(theNeutron, theCMS);
    anEnergy = aNeutron.GetKineticEnergy();

    G4LorentzVector Ptmp (aNeutron.GetMomentum(), aNeutron.GetTotalEnergy());
    toZ.rotateZ(-1*Ptmp.phi());
    toZ.rotateY(-1*Ptmp.theta());

  }
  theTotalMeanEnergy=0;
  G4LorentzRotation toLab(toZ.inverse());
  for(i=0; i<nProducts; i++)
  {
    it = theProducts[i].Sample(anEnergy);
    G4double aMeanEnergy = theProducts[i].MeanEnergyOfThisInteraction();
    if(aMeanEnergy>0)
    {
      theTotalMeanEnergy += aMeanEnergy;
    }
    else
    {
      theTotalMeanEnergy = anEnergy/nProducts+theProducts[i].GetQValue();
    }
    if(it!=0)
    {
      for(unsigned int ii=0; ii<it->size(); ii++)
      {
        G4LorentzVector pTmp1 (it->operator[](ii)->GetMomentum(),
	                       it->operator[](ii)->GetTotalEnergy());
	pTmp1 = toLab*pTmp1;
	it->operator[](ii)->SetMomentum(pTmp1.vect());
	it->operator[](ii)->SetTotalEnergy(pTmp1.e());
	if(frameFlag==1) // target rest
	{
          it->operator[](ii)->Lorentz(*(it->operator[](ii)), -1.*theTarget);
	}
	else if(frameFlag==2) // CMS
	{
#ifdef G4_NHP_DEBUG
          cout <<"G4NeutronHPEnAngCorrelation: "<<
        	 it->at(ii)->GetTotalEnergy()<<" "<<
        	 it->at(ii)->GetMomentum()<<G4endl;
#endif
          it->operator[](ii)->Lorentz(*(it->operator[](ii)), -1.*theCMS);
	}
	else
	{
          throw G4HadronicException(__FILE__, __LINE__, "G4NeutronHPEnAngCorrelation::Sample: The frame of the finalstate is not specified");
	}
	result->push_back(it->operator[](ii));
      }
    delete it;
    }
  }   
  return result;
}
