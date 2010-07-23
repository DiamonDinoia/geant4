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

#ifndef Test2PhantomHit_h
#define Test2PhantomHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"

class G4AttDef;

class Test2PhantomHit : public G4VHit
{
  public:

      Test2PhantomHit();
      Test2PhantomHit(G4LogicalVolume* logVol,G4int z,G4int phi);
      ~Test2PhantomHit();
      Test2PhantomHit(const Test2PhantomHit &right);
      const Test2PhantomHit& operator=(const Test2PhantomHit &right);
      G4int operator==(const Test2PhantomHit &right) const;

      inline void *operator new(size_t);
      inline void operator delete(void *aHit);

      void Draw();
      const std::map<G4String,G4AttDef>* GetAttDefs() const;
      std::vector<G4AttValue>* CreateAttValues() const;
      void Print();

  private:
      G4int ZCellID;
      G4int PhiCellID;
      G4double edep;
      G4ThreeVector pos;
      G4RotationMatrix rot;
      const G4LogicalVolume* pLogV;
      static std::map<G4String,G4AttDef> fAttDefs;

  public:
      inline void SetCellID(G4int z,G4int phi)
      {
        ZCellID = z;
        PhiCellID = phi;
      }
      inline G4int GetZ() { return ZCellID; }
      inline G4int GetPhi() { return PhiCellID; }
      inline void SetEdep(G4double de)
      { edep = de; }
      inline void AddEdep(G4double de)
      { edep += de; }
      inline G4double GetEdep()
      { return edep; }
      inline void SetPos(G4ThreeVector xyz)
      { pos = xyz; }
      inline G4ThreeVector GetPos()
      { return pos; }
      inline void SetRot(G4RotationMatrix rmat)
      { rot = rmat; }
      inline G4RotationMatrix GetRot()
      { return rot; }
      inline const G4LogicalVolume * GetLogV()
      { return pLogV; }

};

typedef G4THitsCollection<Test2PhantomHit> Test2PhantomHitsCollection;

extern G4Allocator<Test2PhantomHit> Test2PhantomHitAllocator;

inline void* Test2PhantomHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) Test2PhantomHitAllocator.MallocSingle();
  return aHit;
}

inline void Test2PhantomHit::operator delete(void *aHit)
{
  Test2PhantomHitAllocator.FreeSingle((Test2PhantomHit*) aHit);
}

#endif


