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
//              sensitive detectors and visibilities
///////////////////////////////////////////////////////////////////////////////
#include "CCalSensitiveConfiguration.hh"

#include "g4std/fstream"
#include <stdlib.h>


//Comment/Uncomment next line to hide/show debug information
//#define debug


CCalSensitiveConfiguration * CCalSensitiveConfiguration::instance = 0;

CCalSensitiveConfiguration* CCalSensitiveConfiguration::getInstance(){
  if (!instance) 
    instance = new CCalSensitiveConfiguration;
  return instance;
}


int CCalSensitiveConfiguration::getSensitiveFlag(const G4String& n) /*const*/ {
  int flag = -1;
  CCalSensitiveConfIterator it = theConfiguration.find(n);

  if (it != theConfiguration.end())
    flag = (*it).second.SensitiveFlag;
  else {
    G4cerr << "ERROR: In CCalSensitiveConfiguration::getConstructFlag(const "
	 << "G4String& n)" << G4endl 
	 << "       " << n << " not found in configuration file" << G4endl;
  }

  return flag;
}

G4String CCalSensitiveConfiguration::getFileName(const G4String& n) /*const*/ {
  G4String fn;
  CCalSensitiveConfIterator it = theConfiguration.find(n);

  if (it != theConfiguration.end())
    fn = (*it).second.FileName;
  else {
    G4cerr << "ERROR: In CCalSensitiveConfiguration::getConstructFlag(const "
	 << "G4String& n)" << G4endl 
	 << "       " << n << " not found in configuration file" << G4endl;
  }

  return fn;
}

CCalSensitiveConfiguration::CCalSensitiveConfiguration():
  theConfiguration() {
  
  ///////////////////////////////////////////////////////////////
  // Open the file
  G4String pathName = getenv("CCAL_CONFPATH");
  G4String fileenv  = getenv("CCAL_SENSITIVECONF");
  if (!pathName || !fileenv) {
    G4cerr << "ERROR: CCAL_SENSITIVECONF and/or CCAL_CONFPATH not set" << G4endl
	 << "       Set them to the sensitive configuration file/path" << G4endl;
    exit(-2);
  }

  G4cout << " ==> Opening file " << fileenv << "..." << G4endl;
  G4std::ifstream is;
  bool ok = openGeomFile(is, pathName, fileenv);
  if (!ok)
    exit(-1);

  G4String name;
  GCInfo gcinfo;
  
  while (is) {
    readName(is, name);
    readName(is, gcinfo.FileName);
    is >> gcinfo.SensitiveFlag >> jump;
#ifdef debug
    G4cout << "CCalSensitiveConfiguration constructor: Read \"" << name 
	 << "\" \"" << gcinfo.FileName << "\"" << tab << gcinfo.SensitiveFlag 
	 << G4endl;
#endif
    theConfiguration[name] = gcinfo;
  }
  

  ///////////////////////////////////////////////////////////////
  // Close the file  
  is.close();
  G4cout << " <== Closed file " << fileenv << G4endl;
}
