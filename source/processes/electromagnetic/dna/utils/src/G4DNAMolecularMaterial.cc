#include "G4DNAMolecularMaterial.hh"
#include "G4Material.hh"
#include <utility>
#include "G4StateManager.hh"

using namespace std;


bool CompareMaterial::operator() (const G4Material* mat1, const G4Material* mat2) const
{
    if(mat1==0 && mat2==0) return false; //(mat1 == mat2)
    if(mat1==0) return true; // mat1 < mat2
    if(mat2==0) return false; //mat2 < mat1

    const G4Material* baseMat1 = mat1->GetBaseMaterial();
    const G4Material* baseMat2 = mat2->GetBaseMaterial();

    if((baseMat1 || baseMat2) == 0) // None of the materials derives from a base material
    {
        return mat1 < mat2;
    }
    else if(baseMat1 && baseMat2) // Both materials derive from a base material
    {
        return baseMat1 < baseMat2;
    }

    else if(baseMat1 && (baseMat2 == 0)) // Only the material 1 derives from a base material
    {
        return baseMat1 < mat2;
    }
    // only case baseMat1==0 && baseMat2 remains
    return mat1 < baseMat2;
}

G4DNAMolecularMaterial* G4DNAMolecularMaterial::fInstance(0);

G4DNAMolecularMaterial* G4DNAMolecularMaterial::Instance()
{
    if(! fInstance) new G4DNAMolecularMaterial();
    return fInstance;
}

void G4DNAMolecularMaterial::DeleteInstance()
{
    delete fInstance;
    fInstance = 0;
}

G4DNAMolecularMaterial::G4DNAMolecularMaterial() :G4VStateDependent()
{
    fpCompFractionTable = 0;
    fpCompDensityTable = 0;
    fpCompNumMolPerVolTable = 0;
    fIsInitialized = false;
    fInstance = this;
}

G4bool G4DNAMolecularMaterial::Notify(G4ApplicationState requestedState)
{
    if(requestedState == G4State_Idle) Initialize();
    return true;
}

G4DNAMolecularMaterial::G4DNAMolecularMaterial(const G4DNAMolecularMaterial& rhs) : G4VStateDependent()
{
    *fpCompFractionTable = *rhs.fpCompFractionTable;
    *fpCompDensityTable = *rhs.fpCompDensityTable;
    fIsInitialized = rhs.fIsInitialized;
}

G4DNAMolecularMaterial& G4DNAMolecularMaterial::operator=(const G4DNAMolecularMaterial& rhs)
{
    if(this == &rhs) return *this;
    *fpCompFractionTable = *rhs.fpCompFractionTable;
    *fpCompDensityTable = *rhs.fpCompDensityTable;
    fIsInitialized = rhs.fIsInitialized;
    return *this;
}

G4DNAMolecularMaterial::~G4DNAMolecularMaterial()
{
    if(fpCompFractionTable)
    {
        fpCompFractionTable->clear();
        delete fpCompFractionTable;
        fpCompFractionTable = 0;
    }
    if(fpCompDensityTable)
    {
        fpCompDensityTable->clear();
        delete fpCompDensityTable;
        fpCompDensityTable = 0;
    }
    if(fpCompNumMolPerVolTable)
    {
        fpCompNumMolPerVolTable->clear();
        delete fpCompNumMolPerVolTable;
        fpCompNumMolPerVolTable = 0;
    }
}

void G4DNAMolecularMaterial::RecordMolecularMaterial(G4Material* parentMaterial, G4Material* molecularMaterial, G4double fraction)
{
    ComponentMap& matComponent = (*fpCompFractionTable)[parentMaterial->GetIndex()];

    if(matComponent.empty())
    {
        matComponent[molecularMaterial] = fraction;
        return;
    }

    ComponentMap::iterator it = matComponent.find(molecularMaterial);

    if(it == matComponent.end())
    {
        matComponent[molecularMaterial] = fraction;
    }
    else
    {
        matComponent[molecularMaterial] = it->second + fraction;
    }
}

void G4DNAMolecularMaterial::SearchMolecularMaterial(G4Material* parentMaterial, G4Material* material, double currentFraction)
{
    if(material->GetMassOfMolecule() != 0.0)
    {
        RecordMolecularMaterial(parentMaterial,material,currentFraction);
        return;
    }

    G4Material* compMat(0);
    G4double fraction = -1;
    std::map<G4Material*,G4double> matComponent = material->GetMatComponents();
    std::map<G4Material*,G4double>::iterator it = matComponent.begin();

    for( ; it!=matComponent.end() ; it++)
    {
        compMat = it->first;
        fraction = it->second;
        if(compMat->GetMassOfMolecule() == 0.0)
        {
            SearchMolecularMaterial(parentMaterial,compMat,currentFraction*fraction);
        }
        else
        {
            RecordMolecularMaterial(parentMaterial,compMat,currentFraction*fraction);
        }

        compMat = 0;
        fraction = -1;
    }
}

void G4DNAMolecularMaterial::InitializeDensity()
{
    if(fpCompFractionTable)
    {
        const G4MaterialTable* materialTable = G4Material::GetMaterialTable();
        fpCompDensityTable = new vector<ComponentMap>(G4Material::GetMaterialTable()->size());

        G4Material* parentMat;
        const G4Material* compMat(0);
        double massFraction = -1;
        double parentDensity = -1;

        for(int i = 0 ; i < int(materialTable->size()) ; i++)
        {
            parentMat = materialTable->at(i);
            ComponentMap& massFractionComp = (*fpCompFractionTable)[i];
            ComponentMap& densityComp = (*fpCompDensityTable)[i];

            parentDensity = parentMat->GetDensity();

            for(ComponentMap::iterator it = massFractionComp.begin() ; it!=massFractionComp.end() ; it++)
            {
                compMat = it->first;
                massFraction = it->second;
                densityComp[compMat] = massFraction*parentDensity;
                compMat = 0;
                massFraction = -1;
            }
        }
    }
    else
    {
        G4ExceptionDescription exceptionDescription;
        exceptionDescription << "The pointer fpCompFractionTable is not initialized" << G4endl;
        G4Exception("G4DNAMolecularMaterial::InitializeDensity","G4DNAMolecularMaterial001",
                    FatalException,exceptionDescription);
    }
}

void G4DNAMolecularMaterial::InitializeNumMolPerVol()
{
    if(fpCompDensityTable)
    {
        const G4MaterialTable* materialTable = G4Material::GetMaterialTable();
        fpCompNumMolPerVolTable = new vector<ComponentMap>(G4Material::GetMaterialTable()->size());

        const G4Material* compMat(0);

        for(int i = 0 ; i < int(materialTable->size()) ; i++)
        {
            ComponentMap& massFractionComp = (*fpCompFractionTable)[i];
            ComponentMap& densityComp = (*fpCompDensityTable)[i];
            ComponentMap& numMolPerVol = (*fpCompNumMolPerVolTable)[i];

            for(ComponentMap::iterator it = massFractionComp.begin() ; it!=massFractionComp.end() ; it++)
            {
                compMat = it->first;
                numMolPerVol[compMat] = densityComp[compMat]/ compMat->GetMassOfMolecule();
                compMat = 0;
            }
        }
    }
    else
    {
        G4ExceptionDescription exceptionDescription;
        exceptionDescription << "The pointer fpCompDensityTable is not initialized" << G4endl;
        G4Exception("G4DNAMolecularMaterial::InitializeNumMolPerVol","G4DNAMolecularMaterial002",
                    FatalException,exceptionDescription);
    }
}

void G4DNAMolecularMaterial::Initialize()
{
    if(fIsInitialized) return;

    const G4MaterialTable* materialTable = G4Material::GetMaterialTable();

    if(fpCompFractionTable==0)
    {
        fpCompFractionTable = new vector<ComponentMap>(materialTable->size());
    }

    G4Material* mat(0);

    for(int i = 0 ; i < int(materialTable->size()) ; i++)
    {
        mat = materialTable->at(i);
        SearchMolecularMaterial(mat,mat,1);

        mat = 0;
    }

    InitializeDensity();
    InitializeNumMolPerVol();

    fIsInitialized = true;
}

const std::vector<double>* G4DNAMolecularMaterial::GetDensityTableFor(const G4Material* lookForMaterial) const
{
    if(!fpCompDensityTable)
    {
        if(fIsInitialized)
        {
            G4ExceptionDescription exceptionDescription;
            exceptionDescription << "The pointer fpCompDensityTable is not initialized will the singleton of G4DNAMolecularMaterial "
                                 << "has already been initialized."<< G4endl;
            G4Exception("G4DNAMolecularMaterial::GetDensityTableFor","G4DNAMolecularMaterial003",
                        FatalException,exceptionDescription);
        }

        if(G4StateManager::GetStateManager()->GetCurrentState() == G4State_Idle)
            const_cast<G4DNAMolecularMaterial*>(this)->Initialize();
        else
        {
            G4ExceptionDescription exceptionDescription;
            exceptionDescription << "The geant4 application is at the wrong state. State must be: G4State_Idle."<< G4endl;
            G4Exception("G4DNAMolecularMaterial::GetDensityTableFor",
                        "G4DNAMolecularMaterial_WRONG_STATE_APPLICATION",FatalException,exceptionDescription);
            abort();
        }
    }

    std::map<const G4Material*,std::vector<double>,CompareMaterial>::const_iterator it_askedDensityTable = fAskedDensityTable.find(lookForMaterial);
    if(it_askedDensityTable != fAskedDensityTable.end())
    {
        return &it_askedDensityTable->second;
    }

    const G4MaterialTable* materialTable = G4Material::GetMaterialTable();

    std::vector<double>* output = new std::vector<double>(materialTable->size());

    ComponentMap::const_iterator it;

    G4bool materialWasNotFound = true;

    for(int i = 0 ; i < int(materialTable->size()) ; i++)
    {
        ComponentMap& densityTable = (*fpCompDensityTable)[i];

        it = densityTable.find(lookForMaterial);

        if(it==densityTable.end())
        {
            (*output)[i] = 0.0;
        }
        else
        {
            materialWasNotFound = false;
            (*output)[i] = it->second;
        }
    }

    if(materialWasNotFound)
    {
        PrintNotAMolecularMaterial("G4DNAMolecularMaterial::GetDensityTableFor",lookForMaterial);
    }

    fAskedDensityTable.insert(make_pair(lookForMaterial, *output));

    return output;
}

const std::vector<double>* G4DNAMolecularMaterial::GetNumMolPerVolTableFor(const G4Material* lookForMaterial) const
{
    if(!fpCompNumMolPerVolTable)
    {
        if(fIsInitialized)
        {
            G4ExceptionDescription exceptionDescription;
            exceptionDescription << "The pointer fpCompNumMolPerVolTable is not initialized will the singleton of G4DNAMolecularMaterial "
                                 << "has already been initialized."<< G4endl;
            G4Exception("G4DNAMolecularMaterial::GetNumMolPerVolTableFor","G4DNAMolecularMaterial005",
                        FatalException,exceptionDescription);
        }

        if(G4StateManager::GetStateManager()->GetCurrentState() == G4State_Idle)
        {
            const_cast<G4DNAMolecularMaterial*>(this)->Initialize();
        }
        else
        {
            G4ExceptionDescription exceptionDescription;
            exceptionDescription << "The geant4 application is at the wrong state. State must be : G4State_Idle."<< G4endl;
            G4Exception("G4DNAMolecularMaterial::GetNumMolPerVolTableFor",
                        "G4DNAMolecularMaterial_WRONG_STATE_APPLICATION",FatalException,exceptionDescription);
            abort();
        }
    }

    std::map<const G4Material*,std::vector<double>,CompareMaterial>::const_iterator it_askedNumMolPerVolTable = fAskedNumPerVolTable.find(lookForMaterial);
    if(it_askedNumMolPerVolTable != fAskedNumPerVolTable.end())
    {
        return &it_askedNumMolPerVolTable->second;
    }

    const G4MaterialTable* materialTable = G4Material::GetMaterialTable();

    std::vector<double>* output = new std::vector<double>(materialTable->size());

    ComponentMap::const_iterator it;

    G4bool materialWasNotFound = true;

    for(int i = 0 ; i < int(materialTable->size()) ; i++)
    {
        ComponentMap& densityTable = (*fpCompNumMolPerVolTable)[i];

        it = densityTable.find(lookForMaterial);

        if(it==densityTable.end())
        {
            (*output)[i] = 0.0;
        }
        else
        {
            materialWasNotFound = false;
            (*output)[i] = it->second;
        }
    }

    if(materialWasNotFound)
    {
        PrintNotAMolecularMaterial("G4DNAMolecularMaterial::GetNumMolPerVolTableFor",lookForMaterial);
    }

    fAskedNumPerVolTable.insert(make_pair(lookForMaterial, *output));

    return output;
}

void G4DNAMolecularMaterial::PrintNotAMolecularMaterial(const char* methodName, const G4Material* lookForMaterial) const
{
    std::map<const G4Material*,bool,CompareMaterial>::iterator it = fWarningPrinted.find(lookForMaterial);

    if(it == fWarningPrinted.end())
    {
        G4ExceptionDescription exceptionDescription;
        exceptionDescription
                << "The material " << lookForMaterial->GetName()
                << " is not defined as a molecular material."<< G4endl
                << "Meaning: The elements should be added to the material using atom count rather than mass fraction (cf. G4Material)"
                << G4endl
                << "If you want to use DNA processes on liquid water, you should better use the NistManager to create the water material."
                << G4endl
                << "Since this message is displayed, it means that the DNA models will not be called."
                << "Please note that this message will only appear once even if you are using other methods of G4DNAMolecularMaterial."
                << G4endl;

        G4Exception(methodName,"MATERIAL_NOT_DEFINE_USING_ATOM_COUNT",JustWarning,exceptionDescription);
        fWarningPrinted[lookForMaterial] = true;
    }
}