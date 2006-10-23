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
// $Id: G4SliceTimer.cc,v 1.1 2006-10-23 07:43:31 gcosmo Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 
// ----------------------------------------------------------------------
// class G4SliceTimer
//
// Implementation
// ----------------------------------------------------------------------

#include "G4SliceTimer.hh"
#include "G4ios.hh"

#undef times

// Global error function
void G4Exception(const char* s=0);

#if defined(IRIX6_2)
#  if defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE_EXTENDED==1)
#    define __vfork vfork
#  endif
#endif

#ifdef WIN32
#  include <sys/types.h>
#  include <windows.h>

   // extract milliseconds time unit
   int sysconf(int a){
     if( a == _SC_CLK_TCK ) return 1000;
     else return 0;
   }

   static clock_t filetime2msec( FILETIME* t ){
   
      return (clock_t)((((float)t->dwHighDateTime)*429496.7296)+
              (((float)t->dwLowDateTime)*.0001) );
   }


   clock_t times(struct tms * t){
           FILETIME      ct = {0,0}, et = {0,0}, st = {0,0}, ut = {0,0}, rt = {0,0};
           SYSTEMTIME realtime;

           GetSystemTime( &realtime );
           SystemTimeToFileTime( &realtime, &rt ); // get real time in 10^-9 sec
           if( t != 0 ){
                   GetProcessTimes( GetCurrentProcess(), &ct, &et, &st, &ut);// get process time in 10^-9 sec
                   t->tms_utime = t->tms_cutime = filetime2msec(&ut);
                   t->tms_stime = t->tms_cstime = filetime2msec(&st);
           }
           return filetime2msec(&rt);
   }
#endif /* WIN32 */

// Print timer status n std::ostream
std::ostream& operator << (std::ostream& os, const G4SliceTimer& t)
{
    if (t.IsValid())
        {
            os << "User=" << t.GetUserElapsed()
               << "s Real=" << t.GetRealElapsed()
               << "s Sys=" << t.GetSystemElapsed() << "s";
        }
    else
        {
            os << "User=****s Real=****s Sys=****s";
        }
    return os;
}

G4SliceTimer::G4SliceTimer()
  : fValidTimes(true)
{
  Clear();
}

G4double G4SliceTimer::GetRealElapsed() const
{
    return fRealElapsed/sysconf(_SC_CLK_TCK);
}


G4double G4SliceTimer::GetSystemElapsed() const
{
    return fSystemElapsed/sysconf(_SC_CLK_TCK);
}

G4double G4SliceTimer::GetUserElapsed() const
{
    return fUserElapsed/sysconf(_SC_CLK_TCK);
}

