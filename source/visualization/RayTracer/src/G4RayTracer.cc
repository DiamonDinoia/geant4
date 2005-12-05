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
// $Id: G4RayTracer.cc,v 1.19 2005-12-05 03:51:22 asaim Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//
//


#include "G4RayTracer.hh"
#include "G4RayTracerFeatures.hh"
#include "G4RayTracerSceneHandler.hh"
#include "G4RayTracerViewer.hh"
#include "G4EventManager.hh"
#include "G4RTMessenger.hh"
#include "G4RayShooter.hh"
#include "G4VFigureFileMaker.hh"
#include "G4RTTrackingAction.hh"
#include "G4RTSteppingAction.hh"
#include "G4RayTrajectory.hh"
#include "G4RayTrajectoryPoint.hh"
#include "G4RTJpegMaker.hh"
#include "G4RTSimpleScanner.hh"
#include "G4GeometryManager.hh"
#include "G4SDManager.hh"
#include "G4StateManager.hh"
#include "G4Event.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4UImanager.hh"
#include "G4TransportationManager.hh"
#include "G4RegionStore.hh"
#include "G4ProductionCutsTable.hh"

G4RayTracer::G4RayTracer(G4VFigureFileMaker* figMaker,
			 G4VRTScanner* scanner)
:G4VGraphicsSystem
(scanner ? scanner->GetGSName() : "RayTracer",
 scanner ? scanner->GetGSNickname() : "RayTracer",
 RAYTRACER_FEATURES,
 G4VGraphicsSystem::threeD)
{
  theFigMaker = figMaker;
  if(!theFigMaker) theFigMaker = new G4RTJpegMaker();
  theScanner = scanner;
  if(!theScanner) theScanner = new G4RTSimpleScanner;
  theRayShooter = new G4RayShooter();
  theRayTracerEventAction = 0;
  theRayTracerStackingAction = 0;
  theRayTracerTrackingAction = 0;
  theRayTracerSteppingAction = 0;
  theMessenger = new G4RTMessenger(this,theRayTracerSteppingAction);
  theEventManager = G4EventManager::GetEventManager();

  nColumn = 640;
  nRow = 640;

  eyePosition = G4ThreeVector(1.*m,1.*m,1.*m);
  targetPosition = G4ThreeVector(0.,0.,0.);
  lightDirection = G4ThreeVector(-0.1,-0.2,-0.3).unit();
  viewSpan = 5.0*deg;
  headAngle = 270.*deg; 
  attenuationLength = 1.0*m;

  distortionOn = false;

  backgroundColour = G4Colour(1.,1.,1.);
}

G4RayTracer::~G4RayTracer()
{
  delete theRayShooter;
  if(theRayTracerTrackingAction) delete theRayTracerTrackingAction;
  if(theRayTracerSteppingAction) delete theRayTracerSteppingAction;
  delete theMessenger;
  delete theScanner;
  delete theFigMaker;
}

void G4RayTracer::Trace(G4String fileName)
{
  G4StateManager* theStateMan = G4StateManager::GetStateManager();
  G4ApplicationState currentState = theStateMan->GetCurrentState();
  if(currentState!=G4State_Idle)
  {
    G4cerr << "Illegal application state - Trace() ignored." << G4endl;
    return;
  }

  if(!theFigMaker)
  {
    G4cerr << "Figure file maker class is not specified - Trace() ignored." << G4endl;
    return;
  }

  G4UImanager* UI = G4UImanager::GetUIpointer();
  G4int storeTrajectory = UI->GetCurrentIntValue("/tracking/storeTrajectory");
  if(storeTrajectory==0) UI->ApplyCommand("/tracking/storeTrajectory 1");


  G4ThreeVector tmpVec = targetPosition - eyePosition;
  eyeDirection = tmpVec.unit();
  colorR = new unsigned char[nColumn*nRow];
  colorG = new unsigned char[nColumn*nRow];
  colorB = new unsigned char[nColumn*nRow];

  StoreUserActions();
  G4bool succeeded = CreateBitMap();
  if(succeeded)
  { CreateFigureFile(fileName); }
  else
  { G4cerr << "Could not create figure file" << G4endl;
    G4cerr << "You might set the eye position outside of the world volume" << G4endl; }
  RestoreUserActions();

  if(storeTrajectory==0) UI->ApplyCommand("/tracking/storeTrajectory 0");

  delete [] colorR;
  delete [] colorG;
  delete [] colorB;
}

G4VSceneHandler* G4RayTracer::CreateSceneHandler (const G4String& name) {
  G4VSceneHandler* pScene = new G4RayTracerSceneHandler (*this, name);
  return pScene;
}

G4VViewer* G4RayTracer::CreateViewer (G4VSceneHandler& sceneHandler,
				      const G4String& name) {
  G4VViewer* pView = new G4RayTracerViewer (sceneHandler, name);
  return pView;
}

void G4RayTracer::StoreUserActions()
{ 
  theUserEventAction = theEventManager->GetUserEventAction();
  theUserStackingAction = theEventManager->GetUserStackingAction();
  theUserTrackingAction = theEventManager->GetUserTrackingAction();
  theUserSteppingAction = theEventManager->GetUserSteppingAction();

  if(!theRayTracerTrackingAction) theRayTracerTrackingAction = new G4RTTrackingAction();
  if(!theRayTracerSteppingAction) theRayTracerSteppingAction = new G4RTSteppingAction();

  theEventManager->SetUserAction(theRayTracerEventAction);
  theEventManager->SetUserAction(theRayTracerStackingAction);
  theEventManager->SetUserAction(theRayTracerTrackingAction);
  theEventManager->SetUserAction(theRayTracerSteppingAction);

  G4SDManager* theSDMan = G4SDManager::GetSDMpointerIfExist();
  if(theSDMan)
  { theSDMan->Activate("/",false); }

  G4GeometryManager* theGeomMan = G4GeometryManager::GetInstance();
  theGeomMan->OpenGeometry();
  theGeomMan->CloseGeometry(true);
}

void G4RayTracer::RestoreUserActions()
{
  theEventManager->SetUserAction(theUserEventAction);
  theEventManager->SetUserAction(theUserStackingAction);
  theEventManager->SetUserAction(theUserTrackingAction);
  theEventManager->SetUserAction(theUserSteppingAction);

  G4SDManager* theSDMan = G4SDManager::GetSDMpointerIfExist();
  if(theSDMan)
  { theSDMan->Activate("/",true); }
}

#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4Geantino.hh"

G4bool G4RayTracer::CreateBitMap()
{
  G4int iEvent = 0;
  G4double stepAngle = viewSpan/100.;
  G4double viewSpanX = stepAngle*nColumn;
  G4double viewSpanY = stepAngle*nRow;
  G4bool succeeded;

// Confirm process(es) of Geantino is initialized
  G4VPhysicalVolume* pWorld =
	G4TransportationManager::GetTransportationManager()->
	GetNavigatorForTracking()->GetWorldVolume();
  G4RegionStore::GetInstance()->UpdateMaterialList(pWorld);
  G4ProductionCutsTable::GetProductionCutsTable()->UpdateCoupleTable(pWorld);
  G4ProcessVector* pVector
    = G4Geantino::GeantinoDefinition()->GetProcessManager()->GetProcessList();
  for (G4int j=0; j < pVector->size(); ++j) {
      (*pVector)[j]->BuildPhysicsTable(*(G4Geantino::GeantinoDefinition()));
  }

// Close geometry and set the application state
  G4GeometryManager* geomManager = G4GeometryManager::GetInstance();
  geomManager->OpenGeometry();
  geomManager->CloseGeometry(1,0);
  
  G4ThreeVector center(0,0,0);
  G4Navigator* navigator =
      G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
  navigator->LocateGlobalPointAndSetup(center,0,false);

  G4StateManager* theStateMan = G4StateManager::GetStateManager();
  theStateMan->SetNewState(G4State_GeomClosed); 

// Event loop
  theScanner->Initialize(nRow,nColumn);
  G4int iRow, iColumn;
  while (theScanner->Coords(iRow,iColumn)) {
      G4int iCoord = iRow * nColumn + iColumn;
      G4Event* anEvent = new G4Event(iEvent++);
      G4double angleX = -(viewSpanX/2. - iColumn*stepAngle);
      G4double angleY = viewSpanY/2. - iRow*stepAngle;
      G4ThreeVector rayDirection;
      if(distortionOn)
      {
	rayDirection = G4ThreeVector(std::tan(angleX)/std::cos(angleY),-std::tan(angleY)/std::cos(angleX),1.0);
      }
      else
      {
	rayDirection = G4ThreeVector(std::tan(angleX),-std::tan(angleY),1.0);
      }
      rayDirection.rotateZ(headAngle);
      rayDirection.rotateUz(eyeDirection);
      G4ThreeVector rayPosition(eyePosition);
      G4bool interceptable = true;
      // Check if rayPosition is in the world.
      EInside whereisit =
	pWorld->GetLogicalVolume()->GetSolid()->Inside(rayPosition);
      if (whereisit != kInside) {
	// It's outside the world, so move it inside.
	G4double outsideDistance =
	  pWorld->GetLogicalVolume()->GetSolid()->
	  DistanceToIn(rayPosition,rayDirection);  
	if (outsideDistance != kInfinity) {
	  // Borrowing form geometry, where 1e-8 < epsilon < 1e-3, in
	  // absolute/internal length units, is used for ensuring good
	  // nehaviour, choose to add 0.001 to ensure rayPosition is
	  // definitely inside the world volume (JA 16/9/2005)...
	  rayPosition = rayPosition+(outsideDistance+0.001)*rayDirection;
	}
	else {
	  interceptable = false;
	}
      }
      if (interceptable) {
	theRayShooter->Shoot(anEvent,rayPosition,rayDirection);
	theEventManager->ProcessOneEvent(anEvent);
	succeeded = GenerateColour(anEvent,iCoord);
      }
      else {  // Ray does not intercept world at all.
	// Store background colour...
	colorR[iCoord] = (unsigned char)(int(255*backgroundColour.GetRed()));
	colorG[iCoord] = (unsigned char)(int(255*backgroundColour.GetGreen()));
	colorB[iCoord] = (unsigned char)(int(255*backgroundColour.GetBlue()));
	theScanner->Draw(colorR[iCoord],colorG[iCoord],colorB[iCoord],this);
	succeeded = true;
      }

      delete anEvent;
      if(!succeeded) return false;
  }

  theStateMan->SetNewState(G4State_Idle); 
  return true;
}

void G4RayTracer::CreateFigureFile(G4String fileName)
{
  //G4cout << nColumn << " " << nRow << G4endl;
  theFigMaker->CreateFigureFile(fileName,nColumn,nRow,colorR,colorG,colorB);
}

G4bool G4RayTracer::GenerateColour(G4Event* anEvent, G4int iCoord)
{
  G4TrajectoryContainer * trajectoryContainer = anEvent->GetTrajectoryContainer();
  
  G4RayTrajectory* trajectory = (G4RayTrajectory*)( (*trajectoryContainer)[0] );
  if(!trajectory) return false;

  G4int nPoint = trajectory->GetPointEntries();
  if(nPoint==0) return false;

  G4Colour rayColour;
  G4Colour initialColour(backgroundColour);
  if( trajectory->GetPointC(nPoint-1)->GetPostStepAtt() )
  { initialColour = GetSurfaceColour(trajectory->GetPointC(nPoint-1)); }
  rayColour = Attenuate(trajectory->GetPointC(nPoint-1),initialColour);

  for(int i=nPoint-2;i>=0;i--)
  {
    G4Colour surfaceColour = GetSurfaceColour(trajectory->GetPointC(i));
    G4double weight = 1.0 - surfaceColour.GetAlpha();
    G4Colour mixedColour = GetMixedColour(rayColour,surfaceColour,weight);
    rayColour = Attenuate(trajectory->GetPointC(i),mixedColour);
  }
    
  colorR[iCoord] = (unsigned char)(int(255*rayColour.GetRed()));
  colorG[iCoord] = (unsigned char)(int(255*rayColour.GetGreen()));
  colorB[iCoord] = (unsigned char)(int(255*rayColour.GetBlue()));
  theScanner->Draw(colorR[iCoord],colorG[iCoord],colorB[iCoord],this);
  return true;
}

G4Colour G4RayTracer::GetMixedColour(G4Colour surfCol,G4Colour transCol,G4double weight)
{
  G4double r = weight*surfCol.GetRed() + (1.-weight)*transCol.GetRed();
  G4double g = weight*surfCol.GetGreen() + (1.-weight)*transCol.GetGreen();
  G4double b = weight*surfCol.GetBlue() + (1.-weight)*transCol.GetBlue();
  G4double a = weight*surfCol.GetAlpha() + (1.-weight)*transCol.GetAlpha();
  return G4Colour(r,g,b,a);
}

G4Colour G4RayTracer::GetSurfaceColour(G4RayTrajectoryPoint* point)
{
  const G4VisAttributes* preAtt = point->GetPreStepAtt();
  const G4VisAttributes* postAtt = point->GetPostStepAtt();

  G4bool preVis = ValidColour(preAtt);
  G4bool postVis = ValidColour(postAtt);

  G4Colour transparent(1.,1.,1.,0.);

  if(!preVis&&!postVis) return transparent;

  G4ThreeVector normal = point->GetSurfaceNormal();

  G4Colour preCol(1.,1.,1.);
  G4Colour postCol(1.,1.,1.);

  if(preVis)
  {
    G4double brill = (1.0-(-lightDirection).dot(normal))/2.0;
    G4double r = preAtt->GetColour().GetRed();
    G4double g = preAtt->GetColour().GetGreen();
    G4double b = preAtt->GetColour().GetBlue();
    preCol = G4Colour(r*brill,g*brill,b*brill,preAtt->GetColour().GetAlpha());
  }
  else
  { preCol = transparent; }

  if(postVis)
  {
    G4double brill = (1.0-(-lightDirection).dot(-normal))/2.0;
    G4double r = postAtt->GetColour().GetRed();
    G4double g = postAtt->GetColour().GetGreen();
    G4double b = postAtt->GetColour().GetBlue();
    postCol = G4Colour(r*brill,g*brill,b*brill,postAtt->GetColour().GetAlpha());
  }
  else
  { postCol = transparent; }
    
  if(!preVis) return postCol;
  if(!postVis) return preCol;

  G4double weight = 0.5;
  return GetMixedColour(preCol,postCol,weight);
}

G4Colour G4RayTracer::Attenuate(G4RayTrajectoryPoint* point, G4Colour sourceCol)
{
  const G4VisAttributes* preAtt = point->GetPreStepAtt();

  G4bool visible = ValidColour(preAtt);
  if(!visible) return sourceCol;

  G4Colour objCol = preAtt->GetColour();
  G4double stepRed = objCol.GetRed();
  G4double stepGreen = objCol.GetGreen();
  G4double stepBlue = objCol.GetBlue();
  G4double stepAlpha = objCol.GetAlpha();
  G4double stepLength = point->GetStepLength();

  G4double attenuationFuctor;
  if(stepAlpha > 0.9999999){ stepAlpha = 0.9999999; } // patch to the next line
    attenuationFuctor = -stepAlpha/(1.0-stepAlpha)*stepLength/attenuationLength;
 
  G4double KtRed = std::exp((1.0-stepRed)*attenuationFuctor);
  G4double KtGreen = std::exp((1.0-stepGreen)*attenuationFuctor);
  G4double KtBlue = std::exp((1.0-stepBlue)*attenuationFuctor);
  if(KtRed>1.0){KtRed=1.0;}
  if(KtGreen>1.0){KtGreen=1.0;}
  if(KtBlue>1.0){KtBlue=1.0;}
  return G4Colour(sourceCol.GetRed()*KtRed,
    sourceCol.GetGreen()*KtGreen,sourceCol.GetBlue()*KtBlue);
}

G4bool G4RayTracer::ValidColour(const G4VisAttributes* visAtt)
{
  G4bool val = true;
  if(!visAtt)
  { val = false; }
  else if(!(visAtt->IsVisible()))
  { val = false; }
  else if(visAtt->IsForceDrawingStyle()
    &&(visAtt->GetForcedDrawingStyle()==G4VisAttributes::wireframe))
  { val = false; }
  return val;
}

