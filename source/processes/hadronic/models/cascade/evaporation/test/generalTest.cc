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
// $Id: generalTest.cc,v 1.4 2007-06-21 15:04:32 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// Test program for G4 Bertini Evaporation.

#include "G4LayeredNucleus.hh"
#include "G4BertiniEvaporation.hh"

int main(int argc, char *argv[])
{
  G4LayeredNucleus nucl;
  G4BertiniEvaporation bert;
  G4VParticleChange * pc;
  G4int A, Z;
  G4double E;


  G4cout << "Parameters " << argc-1 << endl;

  if ( argc == 4 )
    {
      sscanf( argv[1], "%d", &A);
      sscanf( argv[2], "%d", &Z);
      sscanf( argv[3], "%lf", &E);
      G4cout << A << " " << Z << " " << E << endl;
      nucl.SetParameters( A, Z);
      nucl.AddExcitationEnergy( E );
    }

  bert.setVerboseLevel(0);
  
  for ( G4double energy = 1 ; energy < 150 ; energy += 0.5 )
    {
      G4int n=0, p=0, d=0, t=0, h3=0, h4=0, g=0;

      nucl.SetParameters( A, Z);
      nucl.AddExcitationEnergy( E - nucl.GetEnergyDeposit() );

      pc = bert.BreakItUp( nucl );

      for ( G4int i = 0 ; i < pc->GetNumberOfSecondaries() ;  i++ )
	{
	  char * name = pc->GetSecondary( i )->GetDefinition()->GetParticleName() ;
	  if ( strcmp ( name , "proton" ) == 0 ) p++;
	  if ( strcmp ( name , "neutron" ) == 0 ) n++;
	  if ( strcmp ( name , "deuteron" ) == 0 ) d++;
	  if ( strcmp ( name , "triton" ) == 0 ) t++;
	  if ( strcmp ( name , "He3" ) == 0 ) h3++;
	  if ( strcmp ( name , "alpha" ) == 0 ) h4++;
	  if ( strcmp ( name , "gamma" ) == 0 ) g++;
	} // loop over particle change vector

      G4cout << energy << "\t"
	     << n << " "
	     << p << " "
	     << d << " "
	     << t << " "
	     << h3 << " "
	     << h4 << " "
	     << g << endl;
      
    } // energy loop
  
  return 0;
}



//G4cout << energy << " " << pc->GetSecondary( i )->GetDefinition()->GetParticleName() << endl;
