// $Id: G4VPHitIO.hh,v 1.3 2002-12-04 13:57:29 morita Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// File: G4VPHitIO.hh
//
// History:
//   '01.08.10  Youhei Morita  Initial creation (with "fadsclass3")

#ifndef V_P_HIT_I_O_HH
#define V_P_HIT_I_O_HH 1

#include "G4HCofThisEvent.hh"
#include "G4HCIOcatalog.hh"
#include "G4VPHitsCollectionIO.hh"

// Class Description:
//   Abstract base class for storing and retrieving hit collections

class G4VPHitIO
{
    public: // With description
      G4VPHitIO();
      // Constructor

      virtual ~G4VPHitIO() {};
      // Destructor

    public: // With description
      G4VPHitIO* GetVPHitIO() { return f_G4VPHitIO; };
      // Returns the pointer of the hit collection I/O manager.

      virtual G4bool Store(const G4HCofThisEvent*) =0;
      // Pure virtual method for storing hit collections of this event.
      // Each persistency package should implement a concrete method
      // of storing the hit collection of this event with this signature.

      virtual G4bool Retrieve(G4HCofThisEvent*&) =0;
      // Pure virtual method for retrieving hit collections of this event.
      // Each persistency package should implement a concrete method
      // of storing the hit collection of this event with this signature.

      void SetVerboseLevel(int v);
      // Set verbose level.

    protected:
      void SetG4VPHitIO(G4VPHitIO* hitMan) { f_G4VPHitIO = hitMan; };
      // Registers the hit collection I/O manager.

    protected:
      G4int m_verbose;
      static G4VPHitIO* f_G4VPHitIO;
      G4HCIOcatalog*    f_catalog;

}; // End of class G4VPHitIO

#endif

