//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: G4CrossSectionHandler.cc,v 1.5 2001-09-05 12:29:51 vnivanch Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// Author: Maria Grazia Pia (Maria.Grazia.Pia@cern.ch)
//
// History:
// -----------
// 1 Aug 2001   MGP        Created
//
//  Modified: 30.08.01 V.Ivanchenko add G4VEMSecondaryGenerator
//
// -------------------------------------------------------------------

#include "G4CrossSectionHandler.hh"
#include "G4VDataSetAlgorithm.hh"
#include "G4VEMSecondaryGenerator.hh"
#include "G4VEMDataSet.hh"
#include "G4EMDataSet.hh"
#include "G4CompositeEMDataSet.hh"
#include "G4ShellEMDataSet.hh"
#include "G4MaterialTable.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "Randomize.hh" 
#include "g4std/map"
#include "g4std/vector"
#include "g4std/fstream"
#include "g4std/strstream"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4CrossSectionHandler::G4CrossSectionHandler(
                       const G4VDataSetAlgorithm* algorithm,
			     G4double minE, G4double maxE, G4int bins,
			     G4double unitE, G4double unitData,
			     G4int minZ, G4int maxZ)
  : interpolation(algorithm), 
    eMin(minE), 
    eMax(maxE), 
    nBins(bins),
    unit1(unitE), 
    unit2(unitData), 
    zMin(minZ), 
    zMax(maxZ),
    theGenerator(0),
    verbose(0)
{
  ActiveElements();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4CrossSectionHandler::~G4CrossSectionHandler()
{
  Clear();
  activeZ.clear();
  delete interpolation;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4CrossSectionHandler::PrintData() const
{
  G4std::map<G4int,G4VEMDataSet*,G4std::less<G4int> >::const_iterator pos;

  for (pos = dataMap.begin(); pos != dataMap.end(); pos++)
    {
      G4int z = pos->first;
      G4VEMDataSet* dataSet = pos->second;
      
      G4cout << "---- Data set for Z = "
	     << z
	     << G4endl;
      dataSet->PrintData();
      G4cout << "--------------------------------------------------" << G4endl;
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4CrossSectionHandler::LoadData(const G4String& fileName)
{
  if(verbose > 0) {
    G4cout << "G4CrossSectionHandler::Load cross section data from file <" 
           << fileName
           << ">" << G4endl;
  }
 
  size_t nZ = activeZ.size();
  for (size_t i=0; i<nZ; i++)
    {
      G4int Z = (G4int) activeZ[i];

      // Build the complete string identifying the file with the data set
      
      char nameChar[100] = {""};
      G4std::ostrstream ost(nameChar, 100, G4std::ios::out);
      
      ost << fileName << Z << ".dat";
      
      G4String name(nameChar);
      
      char* path = getenv("G4LEDATA");
      if (!path)
	{ 
	  G4String excep = "G4CrossSectionHandler - G4LEDATA environment variable not set";
	  G4Exception(excep);
	}
      
      G4String pathString(path);
      G4String dirFile = pathString + "/" + name;
      G4std::ifstream file(dirFile);
      G4std::filebuf* lsdp = file.rdbuf();
      
      if (! (lsdp->is_open()) )
	{
	  G4String excep = "G4CrossSectionHandler - data file: " + 
                           dirFile + " not found";
	  G4Exception(excep);
	}
      G4double a = 0;
      G4int k = 1;
      G4DataVector* energies = new G4DataVector;
      G4DataVector* data = new G4DataVector;
      do
	{
	  file >> a;
	  G4int nColumns = 2;
	  // The file is organized into two columns:
	  // 1st column is the energy
	  // 2nd column is the corresponding value
	  // The file terminates with the pattern: -1   -1
	  //                                       -2   -2
	  if (a == -1 || a == -2)
	    {
	    }
	  else
	    {
	      if (k%nColumns != 0)
		{	
		  G4double e = a * unit1;
		  energies->push_back(e);
		  k++;
		}
	      else if (k%nColumns == 0)
		{
		  G4double value = a * unit2;
		  data->push_back(value);
		  k = 1;
		}
	    }
	} while (a != -2); // end of file
      
      file.close();
      
      G4VEMDataSet* dataSet = new G4EMDataSet(Z,energies,data,interpolation);
      dataMap[Z] = dataSet;
    }

  if(verbose > 0) {
    G4cout << "G4CrossSectionHandler: end of loading" 
           << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4CrossSectionHandler::LoadShellData(const G4String& fileName)
{
  if(verbose > 0) {
    G4cout << "G4CrossSectionHandler::Load cross shell data from file <" 
           << fileName
           << ">" << G4endl;
  }
  size_t nZ = activeZ.size();
  for (size_t i=0; i<nZ; i++)
    {
      G4int Z = (G4int) activeZ[i];      
      G4VEMDataSet* dataSet = new G4ShellEMDataSet(Z,fileName,interpolation);
      dataMap[Z] = dataSet;
    }

  if(verbose > 0) {
    G4cout << "G4CrossSectionHandler: end of loading" 
           << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4CrossSectionHandler::Clear()
{
  // Reset the map of data sets: remove the data sets from the map 
  G4std::map<G4int,G4VEMDataSet*,G4std::less<G4int> >::iterator pos;

  for (pos = dataMap.begin(); pos != dataMap.end(); pos++)
    {
      G4VEMDataSet* dataSet = pos->second;
      dataMap.erase(pos);
      delete dataSet;
    }

  for (pos = dataWithCutMap.begin(); pos != dataWithCutMap.end(); pos++)
    {
      G4VEMDataSet* dataSet = pos->second;
      dataMap.erase(pos);
      delete dataSet;
    }

  // Reset the list of cross sections
  G4std::vector<G4VEMDataSet*>::iterator mat;
  for (mat = crossSections.begin(); mat!= crossSections.end(); ++mat)
    {
      G4VEMDataSet* set = *mat;
      crossSections.erase(mat);
      delete set;
    }

  activeZ.clear();
  ActiveElements();
  if(verbose > 0) {
    G4cout << "G4CrossSectionHandler is cleared" 
           << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double G4CrossSectionHandler::FindValue(G4int Z, G4double e) const
{
  G4double value = 0.;
  
  G4std::map<G4int,G4VEMDataSet*,G4std::less<G4int> >::const_iterator pos;

  // First search in the data with cut
  pos = dataWithCutMap.find(Z);
  if (pos!= dataWithCutMap.end())
    {
      G4VEMDataSet* dataSet = pos->second;
      value = dataSet->FindValue(e);
    }
  else
    {

    // second search in the data without cut
    pos = dataMap.find(Z);
    if (pos!= dataMap.end())
      {
        G4VEMDataSet* dataSet = pos->second;
        value = dataSet->FindValue(e);
      }
    else
      {
        G4cout << "WARNING: G4CrossSectionHandler::FindValue did not find Z = "
	       << Z << G4endl;
      }
    }
  return value;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double G4CrossSectionHandler::ValueForMaterial(const G4Material* material, 
			  			       G4double e) const
{
  G4double value = 0.;

  const G4ElementVector* elementVector = material->GetElementVector();
  const G4double* nAtomsPerVolume = material->GetVecNbOfAtomsPerVolume();   
  G4double totNAtomsPerVolume = material->GetTotNbOfAtomsPerVolume();   
  G4int nElements = material->GetNumberOfElements();

  for (G4int i=0 ; i<nElements ; i++)
    { 
      G4int Z = (G4int) (*elementVector)[i]->GetZ();
      G4double elementValue = FindValue(Z,e);
      G4double nAtomsVol = nAtomsPerVolume[i];
      value += nAtomsVol * elementValue;
    }

  value /= totNAtomsPerVolume;

  return value;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4CrossSectionHandler::BuildCrossSectionsForMaterials(
                            const G4DataVector& energyVector)
{
  G4DataVector* energies;
  G4DataVector* data;

  //  const G4MaterialTable* materialTable = GetMaterialTable();
  const G4MaterialTable* materialTable = G4Material::GetMaterialTable();
  if (materialTable == 0)
     G4Exception("G4CrossSectionHandler: no MaterialTable found)");

  G4int nMaterials = materialTable->length();

  for (G4int m=0; m<nMaterials; m++)
    {
      const G4Material* material= (*materialTable)[m];
      energies = new G4DataVector;
      data = new G4DataVector;
      for (G4int bin=0; bin<nBins; bin++)
	{
	  G4double e = energyVector[bin];
	  energies->push_back(e);
	  G4double materialCrossSection = ValueForMaterial(material,e);
	  if (materialCrossSection > 0.)
	    {
	      data->push_back(materialCrossSection);
	    }
	  else
	    {
	      data->push_back(0.0);
	    }
	}
      G4VEMDataSet* dataSet = 
                    new G4EMDataSet(m,energies,data,interpolation,1.,1.); 
      crossSections.push_back(dataSet);
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4CrossSectionHandler::BuildCrossSectionsWithCut(
                                     const G4DataVector& energyVector,
                                     const G4DataVector& minEnergy,
                                     const G4DataVector& maxEnergy)
{
  if(verbose > 0) {
    G4cout << "G4CrossSectionHandler::BuildCrossSectionsWithCut start" 
           << G4endl;
  }

  G4DataVector* energies;
  G4DataVector* data;
  G4std::map<G4int,G4VEMDataSet*,G4std::less<G4int> >::const_iterator posDB;
  G4std::map<G4int,G4VEMDataSet*,G4std::less<G4int> >::const_iterator pos;
  G4VEMDataSet* dataSet;
  G4VEMDataSet* dataSetWithCut;

  //  const G4MaterialTable* materialTable = GetMaterialTable();
  const G4MaterialTable* materialTable = G4Material::GetMaterialTable();
  if (materialTable == 0)
     G4Exception("G4CrossSectionHandler: no MaterialTable found)");

  G4int nMaterials = materialTable->length();

  for (G4int m=0; m<nMaterials; m++) {

      const G4Material* material= (*materialTable)[m];
      G4double tcut = minEnergy[m];
      G4double tmax = DBL_MAX;
      if( maxEnergy.size() ) tmax = maxEnergy[m];
      if(verbose > 1) {
        G4cout << "Material# " << m
               << "  tcut(MeV)= " << tcut/MeV 
               << "  tmax= " << tmax 
               << G4endl;
      }

      const G4ElementVector* elementVector = material->GetElementVector();
      const G4double* nAtomsPerVolume = material->GetVecNbOfAtomsPerVolume(); 
      G4double totNAtomsPerVolume = material->GetTotNbOfAtomsPerVolume(); 
      G4int nElements = material->GetNumberOfElements();

      // Build cross section per element
      for (size_t i=0; i<nElements; i++) {
 
        G4int Z = (G4int) (*elementVector)[i]->GetZ();
        pos = dataWithCutMap.find(Z);

	// If the element data were not exist then build

        if(pos == dataWithCutMap.end()) {

          energies = new G4DataVector;
          data = new G4DataVector;

	  // If dataSet exist this mean that total cross section 
          // for the element is availbale, in that case cross
	  // section with cut is built on the base of probabilities
          // else it is built on using cross sections with cut

          posDB = dataMap.find(Z);

          for (G4int bin=0; bin<nBins; bin++) {

	    G4double e = energyVector[bin];
	    energies->push_back(e);
	    G4double elemCrossSection = 0.0;
            if(tmax > tcut) {

              if(posDB!= dataMap.end()) {

                elemCrossSection  = theGenerator->Probability(Z,e,tcut,tmax);
                elemCrossSection *= (posDB->second)->FindValue(e);
  	          
              } else {

                elemCrossSection = 
                          theGenerator->CrossSectionWithCut(Z, e, tcut, tmax);
	      }
	    }
            if(elemCrossSection < 0.0) elemCrossSection = 0.0; 
            if(verbose > 1) {
              G4cout << "Energy(MeV) " << e/MeV 
                     << "; Cross section(barn) = " << elemCrossSection/barn
                     << "; Element # " << i
                     << "; Z= " << Z
                     << G4endl;
            }
            data->push_back(elemCrossSection);
	  }

          dataSetWithCut = new G4EMDataSet(Z,energies,data,interpolation);
          dataWithCutMap[Z] = dataSetWithCut;
	}
      }

      // Fill the cross section for the material
      if(verbose > 0) {
        G4cout << "For the material # " << m
               << " dataWithCutMap is filled"
               << G4endl;
      }
      energies = new G4DataVector;
      data = new G4DataVector;

      for (G4int bin=0; bin<nBins; bin++) {

	G4double e = energyVector[bin];
	energies->push_back(e);
        G4double materialCrossSection = 0.0;

        for (size_t j=0; j<nElements; j++) {

          G4double nAtomsVol = nAtomsPerVolume[j];
          G4int Z = (G4int) (*elementVector)[j]->GetZ();
          pos = dataWithCutMap.find(Z);
          if(pos == dataWithCutMap.end()) {
            G4cout << "Element with Z= " << Z 
                   << " is not found out in the dataWithCutMap"
                   << " nAtomsVol= " << nAtomsVol
                   << G4endl;
          }
          G4double x = (pos->second)->FindValue(e);
          materialCrossSection += x * nAtomsVol; 

	}

	if (materialCrossSection < 0.0) materialCrossSection = 0.0; 
        data->push_back(materialCrossSection);
        if(verbose > 1) {
          G4cout << "Energy(MeV) " << e/MeV 
                 << "; Material cross section(barn) = " 
                 << materialCrossSection/barn
                 << G4endl;
          }
      
      }
      G4VEMDataSet* dataSet = 
                    new G4EMDataSet(m,energies,data,interpolation,1.,1.); 
      crossSections.push_back(dataSet);
  }
  if(verbose > 0) {
    G4cout << "G4CrossSectionHandler::BuildCrossSectionsWithCut end" 
           << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4VEMDataSet* G4CrossSectionHandler::BuildMeanFreePathForMaterials()
{
  // Builds a CompositeDataSet containing the mean free path for each material
  // in the material table

  G4DataVector energyVector;
  G4double dBin = 0.0;
  if(nBins > 1) dBin = log10(eMax/eMin) / (nBins-1);

  for (G4int i=0; i<nBins+1; i++) 
    {
      energyVector.push_back(pow(10., log10(eMin)+i*dBin));
    }

  BuildCrossSectionsForMaterials(energyVector);
  G4VEMDataSet* set = BuildMeanFreePathForMaterials(energyVector);
  return set;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4VEMDataSet* G4CrossSectionHandler::BuildMeanFreePathForMaterials(
                                     const G4DataVector& minEnergy,
                                     const G4DataVector& maxEnergy)
{
  // Builds a CompositeDataSet containing the mean free path for each material
  // in the material table

  G4DataVector energyVector;
  G4double dBin = 0.0;
  if(nBins > 1) dBin = log10(eMax/eMin) / (nBins-1);

  for (G4int i=0; i<nBins+1; i++) 
    {
      energyVector.push_back(pow(10., log10(eMin)+i*dBin));
    }

  // For ContinuousDiscrete processes
  BuildCrossSectionsWithCut(energyVector, minEnergy, maxEnergy);
  G4VEMDataSet* set = BuildMeanFreePathForMaterials(energyVector);
  return set;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4VEMDataSet* G4CrossSectionHandler::BuildMeanFreePathForMaterials(
                                     const G4DataVector& energyVector)
{
  G4VEMDataSet* materialSet = new G4CompositeEMDataSet(interpolation);

  G4DataVector* energies;
  G4DataVector* data;

  const G4MaterialTable* materialTable = G4Material::GetMaterialTable();
  if (materialTable == 0)
     G4Exception("G4CrossSectionHandler: no MaterialTable found)");

  size_t nMaterials = materialTable->length();

  for (size_t m=0; m<nMaterials; m++)
    {
      energies = new G4DataVector;
      data = new G4DataVector;
      for (G4int bin=0; bin<nBins; bin++)
	{
	  G4double e = energyVector[bin];
	  energies->push_back(e);
	  G4VEMDataSet* materialSet = crossSections[m];
	  G4double materialCrossSection = materialSet->FindValue(e);
  
	  if (materialCrossSection > 0.)
	    {
	      data->push_back(1./materialCrossSection);
	    }
	  else
	    {
	      data->push_back(DBL_MAX);
	    }
	}
      G4VEMDataSet* dataSet = 
                    new G4EMDataSet(m,energies,data,interpolation,1.,1.);
      materialSet->AddComponent(dataSet);
     }

  return materialSet;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4int G4CrossSectionHandler::SelectRandomAtom(const G4Material* material, 
                                                    G4double e) const
{
  // Select randomly an element within the material, according to the 
  // weight determined by the cross sections in the data set

  G4int nElements = material->GetNumberOfElements();
  const G4ElementVector* elementVector = material->GetElementVector();

  // Special case: the material consists of one element
  if (nElements == 1) 
    {
      G4int Z = (G4int) (*elementVector)[0]->GetZ();
      return Z;
    }

  // Composite material

  G4double materialCrossSection0 = ValueForMaterial(material,e);

  G4double random = G4UniformRand() * materialCrossSection0;
  const G4double* nAtomsPerVolume = material->GetVecNbOfAtomsPerVolume();   
  G4double partialSumSigma = 0.;

  for ( G4int i=0 ; i < nElements ; i++ )
    { 
      G4int Z = (G4int) (*elementVector)[i]->GetZ();
      G4double crossSection = FindValue(Z,e);
      partialSumSigma += nAtomsPerVolume[i] * crossSection;
      if (random <= partialSumSigma) return Z;
    }
  // It should never get here
  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

const G4Element* G4CrossSectionHandler::SelectRandomElement(
                                  const G4Material* material, 
					G4double e) const
{
  // Select randomly an element within the material, according 
  // to the weight determined by the cross sections in the data set

  G4Element* nullElement = 0;
  G4int nElements = material->GetNumberOfElements();
  const G4ElementVector* elementVector = material->GetElementVector();

  // Special case: the material consists of one element
  if (nElements == 1) 
    {
      G4Element* element = (*elementVector)[0];
      return element;
    }
  else
    {
      // Composite material   
      G4double materialCrossSection0 = ValueForMaterial(material,e);
      
      G4double random = G4UniformRand() * materialCrossSection0;
      const G4double* nAtomsPerVolume = material->GetVecNbOfAtomsPerVolume();   
      G4double partialSumSigma = 0.;
      
      for ( G4int i=0 ; i < nElements ; i++ )
	{ 
	  G4Element* element = (*elementVector)[i];
	  G4int Z = (G4int) element->GetZ();
	  G4double crossSection = FindValue(Z,e);
	  partialSumSigma += nAtomsPerVolume[i] * crossSection;
	  if (random <= partialSumSigma) return element;
	}
    }  
    // It should never end up here
    G4cout << "G4CrossSectionHandler::SelectRandomElement - no element found"
           << G4endl;
    return nullElement;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4int G4CrossSectionHandler::SelectRandomShell(G4int Z, G4double e) const
{
  // Select randomly a shell, according to the weight determined by 
  // the cross sections in the data set

  // Note for later improvement: it would be useful to add 
  // a cache mechanism for already
  // used shells to improve performance

  G4int shell = 0;

  G4double totCrossSection = FindValue(Z,e);
  G4double random = G4UniformRand() * totCrossSection;
  G4double partialSum = 0.;

  G4VEMDataSet* dataSet = 0;
  G4std::map<G4int,G4VEMDataSet*,G4std::less<G4int> >::const_iterator pos;
  pos = dataMap.find(Z);
  if (pos != dataMap.end()) dataSet = pos->second;

  size_t nShells = dataSet->NumberOfComponents();
  for (size_t i=0; i<nShells; i++)
    {
      const G4VEMDataSet* shellDataSet = dataSet->GetComponent(i);
      if (shellDataSet != 0)
	{
	  G4double value = shellDataSet->FindValue(e);
	  partialSum += value;
	  if (random <= partialSum) return i;
	}
    }
  // It should never get here
  return shell;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4CrossSectionHandler::ActiveElements()
{
  const G4MaterialTable* materialTable = G4Material::GetMaterialTable();
  if (materialTable == 0)
     G4Exception("G4CrossSectionHandler: no MaterialTable found)");

  G4int nMaterials = materialTable->length();
  
  for (G4int m=0; m<nMaterials; m++)
    {
      const G4Material* material= (*materialTable)[m];        
      const G4ElementVector* elementVector = material->GetElementVector();
      const G4int nElements = material->GetNumberOfElements();
      
      for (G4int iEl=0; iEl<nElements; iEl++)
	{
	  G4Element* element = (*elementVector)[iEl];
	  G4double Z = element->GetZ();
	  if (!(activeZ.contains(Z)) && Z >= zMin && Z <= zMax) 
	    {
	      activeZ.push_back(Z);
	    }
	}
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
