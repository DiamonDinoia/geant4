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
// File: G4HCIOcatalog.cc
//
// History:
//   '01.09.12  Youhei Morita  Initial creation

#include "G4HCIOcatalog.hh"

// Addtional Include:
#include "G4VHCIOentry.hh"

G4HCIOcatalog* G4HCIOcatalog::f_thePointer = 0;

// Implementation of Constructor #1
G4HCIOcatalog::G4HCIOcatalog()
 : m_verbose(0)
{}

// Implementation of GetHCIOcatalog
G4HCIOcatalog* G4HCIOcatalog::GetHCIOcatalog()
{
  if ( f_thePointer == 0 ) f_thePointer = new G4HCIOcatalog;
  return f_thePointer;
}

// Implementation of RegisterEntry
void G4HCIOcatalog::RegisterEntry(G4VHCIOentry* d)
{
  if ( m_verbose > 0 ) {
    G4cout << "registering I/O manager entry \"" << d->GetName()
           << "\" " << d << "." << G4endl;
  }
  if ( theCatalog.find(d->GetName()) != theCatalog.end() ) {
    G4cout << "Redefining I/O Managers list " << d->GetName() << G4endl;
  } else {
    theCatalog[d->GetName()] = d;
  }
}

// Implementation of RegisterHCIOmanager
void G4HCIOcatalog::RegisterHCIOmanager(G4VPHitsCollectionIO* d)
{
  if ( m_verbose > 0 ) {
    G4cout << "registering I/O manager \"" << d->SDname()
           << "\" " << d << "." << G4endl;
  }
  if ( theStore.find(d->SDname()) != theStore.end() ) {
    G4cout << "Redefining I/O Manager " << d->SDname() << G4endl;
  } else {
    theStore[d->SDname()] = d;
  }
}

// Implementation of GetEntry
G4VHCIOentry* G4HCIOcatalog::GetEntry(G4std::string name)
{
  if ( theCatalog.find(name) == theCatalog.end() ) {
    G4cout << "Hit Collection I/O manager entry \"" << name
           << "\" not found!" << G4std::endl;
    return 0;
  } else {
    G4VHCIOentry* ds = theCatalog[name];
    return ds;
  }
}

// Implementation of GetHCIOmanager
G4VPHitsCollectionIO* G4HCIOcatalog::GetHCIOmanager(G4std::string name)
{
  if ( theStore.find(name) == theStore.end() ) {
    G4cout << "Hit Collection I/O manager \"" << name
           << "\" not found!" << G4endl;
    return 0;
  } else {
    G4VPHitsCollectionIO* ds = theStore[name];
    return ds;
  }
}

// Implementation of PrintEntries
void G4HCIOcatalog::PrintEntries()
{
  G4cout << "I/O manager entries: ";
  G4cout << theCatalog.size() << G4endl;
  HCIOmap::const_iterator it;
  for ( it=theCatalog.begin(); it != theCatalog.end(); it++ ) {
    G4cout << "  --- " << (*it).first << G4endl;
  }
}

// Implementation of CurrentHCIOmanager
G4std::string G4HCIOcatalog::CurrentHCIOmanager()
{
  G4std::string list = "";
  HCIOstore::const_iterator it;
  for ( it=theStore.begin(); it != theStore.end(); it++ ) {
    list += (*it).first + " ";
  }
  return list;
}

// Implementation of PrintHCIOmanager
void G4HCIOcatalog::PrintHCIOmanager()
{
  G4cout << "I/O managers: ";
  G4cout << theStore.size() << G4endl;
  HCIOstore::const_iterator it;
  for ( it=theStore.begin(); it != theStore.end(); it++ ) {
    G4cout << "  --- " << (*it).first
           << ", " << (*it).second << "." << G4endl;
  }
}

// Implementation of GetHCIOmanager
G4VPHitsCollectionIO* G4HCIOcatalog::GetHCIOmanager(int n)
{
  int i = 0;
  HCIOstore::const_iterator it;
  for ( it=theStore.begin(); it != theStore.end(); it++ ) {
    if (i++ == n) return (*it).second;
  }
  return 0;
}

// End of G4HCIOcatalog.cc

