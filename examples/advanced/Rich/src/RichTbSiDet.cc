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
// Rich advanced example for Geant4
// RichTbSiDet.cc for Rich of LHCb
// History:
// Created: Sajan Easo (Sajan.Easo@cern.ch)
// Revision and changes: Patricia Mendez (Patricia.Mendez@cern.ch)
/////////////////////////////////////////////////////////////////////////////
#include <iostream.h>
#include "globals.hh"
#include "G4Box.hh"
#include "G4Trd.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4UnionSolid.hh"
#include "RichTbSiDet.hh"
#include "G4SDManager.hh"
#include "RichTbSD.hh"
#include "G4LogicalBorderSurface.hh"

RichTbSiDet::RichTbSiDet() {;}
RichTbSiDet::~RichTbSiDet() {;}
RichTbSiDet::RichTbSiDet(RichTbMaterial* RMaterial,
                         G4VPhysicalVolume* MotherOfSiDet,
                         G4bool ConstructTrackingGeomSwitch,
                         G4int IHpdNum, G4int SectNum){
  //
  //
    ConstructTrackGeomSwitch=ConstructTrackingGeomSwitch;
    IHpdNumber=IHpdNum;
    CurSiSectNum=SectNum;   
    //This is done for each sector.

    G4double SiSectRotZ;
    G4double SiSectPosX,SiSectPosY;
    G4double SiSectCoatingRotZ;
    G4double SiSectCoatingPosX,SiSectCoatingPosY;
    G4int isec=SectNum;
    G4Trd* HpdSiSect=
      new G4Trd("HpdSiSect",SiSectTrapHalfX1, SiSectTrapHalfX2,
		SiSectTrapHalfY1,SiSectTrapHalfY2,SiSectTrapHalfZ);

    G4RotationMatrix SiSectRotationX,SiSectRotationZ; 

    SiSectRotationX.rotateX(SiSectRotX);
    SiSectRotZ = SiSectAngStart-isec*SiSectAngSize;    
    SiSectRotationZ.rotateZ(SiSectRotZ);
    SiSectPosX=SiSectTrapHalfZ*cos(SiSectRotZ-pi/2.0);
    //
    if(isec < NumberOfSiDetSectors/2 ) {
    SiSectPosY=SiSectTrapHalfZ*sin(SiSectRotZ-pi/2.0)-SiSectHalfMoonGap/2.0;
    }else{
    SiSectPosY=SiSectTrapHalfZ*sin(SiSectRotZ-pi/2.0)+SiSectHalfMoonGap/2.0;
    }

    G4ThreeVector SiSectTrapPos(SiSectPosX,SiSectPosY,SiSectPosZ);

    G4Transform3D SiSectTransform(SiSectRotationZ*SiSectRotationX,
				    SiSectTrapPos);


    G4LogicalVolume* HpdSiSectLog =
      new G4LogicalVolume(HpdSiSect,RMaterial->getHpdSiDetMaterial(),
			  "HpdSiSect",0,0,0);
 
    G4VPhysicalVolume* HpdSiSectPhys=
           new G4PVPlacement(SiSectTransform,"HpdSiSect", 
                HpdSiSectLog,MotherOfSiDet,false,SectNum);


     if(ConstructTrackingGeomSwitch) {


    G4Trd* HpdSectCoat=
      new G4Trd("HpdSectCoat",SiSectCoatingTrapHalfX1, 
                SiSectCoatingTrapHalfX2,
		SiSectCoatingTrapHalfY1,SiSectCoatingTrapHalfY2,
                SiSectCoatingTrapHalfZ);
     

    G4RotationMatrix SiSectCoatingRotationX,SiSectCoatingRotationZ; 
    SiSectCoatingRotationX.rotateX(SiSectCoatingRotX);
    SiSectCoatingRotZ =  SiSectRotZ;   
    SiSectCoatingRotationZ.rotateZ(SiSectCoatingRotZ);
    SiSectCoatingPosX= SiSectPosX;
    SiSectCoatingPosY=SiSectPosY;
    G4ThreeVector SiSectCoatingTrapPos(SiSectCoatingPosX,SiSectCoatingPosY,
                SiSectCoatingPosZ);

    G4Transform3D SiSectCoatingTransform(SiSectCoatingRotationZ*SiSectCoatingRotationX,	SiSectCoatingTrapPos);



     G4LogicalVolume* HpdSectCoatLog =
      new G4LogicalVolume(HpdSectCoat,RMaterial->getHpdSiCoatingMaterial(),
   			  "HpdSectCoat",0,0,0);
 
     G4VPhysicalVolume* HpdSectCoatPhys=
           new G4PVPlacement(SiSectCoatingTransform,"HpdSectCoat", 
                HpdSectCoatLog,MotherOfSiDet,false,SectNum);

//     G4LogicalBorderSurface* HpdSectCoatSurface =
       new G4LogicalBorderSurface("HpdSectCoatSurface",MotherOfSiDet ,
                               HpdSectCoatPhys ,
                 RMaterial-> getHpdSiCoatSurface());

     RichTbHpdSectCoatLVol=HpdSectCoatLog;
     RichTbHpdSectCoatPVol=HpdSectCoatPhys;


     }


    //Construct the Silicon Pixels
     NumHpdSiPix=NumberOfPadHpdSiPixels;
     if(!ConstructTrackingGeomSwitch) {
       for (G4int ipix=0; ipix<NumberOfPadHpdSiPixels; ipix++){

        hpdSiPixel[ipix] = new RichTbSiPixel(RMaterial,HpdSiSectPhys,
                                                 IHpdNum,isec,ipix);

       }
    }
    RichTbHpdSiSectLVol=HpdSiSectLog;
    RichTbHpdSiSectPVol=HpdSiSectPhys;

  


}






