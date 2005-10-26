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
// $Id: G4strstreambuf.hh,v 1.13 2005-10-26 05:54:19 kmura Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
// ====================================================================
//   G4strstreambuf.hh
//
//   28/Nov/2005  K. Murakami
//   - revised version of stream buffer class
//   - disuse of "strsteam" because it has been obsolete in ANSI C++.
//
// ====================================================================
#ifndef G4_STR_STREAM_BUF_HH
#define G4_STR_STREAM_BUF_HH

#include "globals.hh"
#include "G4coutDestination.hh"
#include <streambuf>

class G4strstreambuf;

#if defined G4IOS_EXPORT
extern G4DLLEXPORT G4strstreambuf G4coutbuf;
extern G4DLLEXPORT G4strstreambuf G4cerrbuf;
#else
extern G4DLLIMPORT G4strstreambuf G4coutbuf;
extern G4DLLIMPORT G4strstreambuf G4cerrbuf;
#endif

class G4strstreambuf : public std::basic_streambuf<char> {
private:
  char* buffer;
  G4int count, size;
  G4coutDestination* destination;

  // hiden...
  G4strstreambuf(const G4strstreambuf&);
  G4strstreambuf& operator=(const G4strstreambuf&);
  
public:
  G4strstreambuf();
  ~G4strstreambuf();
    
  virtual G4int overflow(G4int c=EOF);
  virtual G4int sync();

#ifdef WIN32
  virtual G4int underflow();
#endif

  void SetDestination(G4coutDestination* dest);
  G4int ReceiveString ();
  
};

#include "G4strstreambuf.icc"

#endif

