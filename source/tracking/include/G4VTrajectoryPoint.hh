// This code implementation is the intellectual property of
// the RD44 GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: G4VTrajectoryPoint.hh,v 1.2 1999-10-14 05:39:48 tsasaki Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//

#ifndef G4VTrajectoryPoint_h
#define G4VTrajectoryPoint_h 1

class G4VTrajectoryPoint
{
   public:

   G4VTrajectoryPoint() {;}
   virtual ~G4VTrajectoryPoint() {;}

   inline int operator==(const G4VTrajectoryPoint& right) const
   { return (this==&right); }
};


#endif

