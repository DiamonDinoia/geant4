#include "G4ITSteppingMessenger.hh"
#include "G4ITStepManager.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UImanager.hh"
#include "G4ios.hh"

G4ITSteppingMessenger::G4ITSteppingMessenger(G4ITStepManager * stepMgr)
:fITStepManager(stepMgr)
{
  fITDirectory = new G4UIdirectory("/IT/");
  fITDirectory->SetGuidance("IT control commands.");

  // Set end Time
  fEndTime = new G4UIcmdWithADoubleAndUnit("/IT/endTime", this);
  fEndTime->SetGuidance("Set end Time");
  fEndTime->AvailableForStates(G4State_PreInit,G4State_Idle);
  fEndTime->SetUnitCategory("Time");
  fEndTime->SetDefaultUnit("picosecond");
  fEndTime->SetDefaultValue(1);

  // Initialize
  fInitCmd = new G4UIcmdWithoutParameter("/IT/initialize",this);
  fInitCmd->SetGuidance("Initialize G4ITStepManager.");
  fInitCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  // Set Max Null time Step
  fMaxNULLTimeSteps = new G4UIcmdWithAnInteger("/IT/maxNullTimeSteps",this);
  fMaxNULLTimeSteps->SetGuidance("Set Max Null time Step");
  fMaxNULLTimeSteps->SetParameterName("numberOfNullTimeSteps",true);
  fMaxNULLTimeSteps->SetDefaultValue(10);
  fMaxNULLTimeSteps->SetRange("numberOfNullTimeSteps >=0 ");

  fMaxStepNumber = new G4UIcmdWithAnInteger("/IT/maxStepsNumber",this);
  fMaxStepNumber->SetParameterName("maximumNumberOfSteps",true);
  fMaxStepNumber->SetDefaultValue(-1);

  // Beam On
  fProcessCmd = new G4UIcmdWithoutParameter("/IT/process",this);
  fProcessCmd->SetGuidance("Process track stacked in G4ITStepManager");
  fProcessCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  // Verbose
  fVerboseCmd = new G4UIcmdWithAnInteger("/IT/verbose",this);
  fVerboseCmd->SetGuidance("Set the Verbose level of G4ITStepManager.");
  fVerboseCmd->SetGuidance(" 0 : Silent (default)");
  fVerboseCmd->SetGuidance(" 1 : Display reactions");
  fVerboseCmd->SetGuidance(" 2 ");
  fVerboseCmd->SetParameterName("level",true);
  fVerboseCmd->SetDefaultValue(1);
//  fVerboseCmd->SetRange("level >=0 && level <=4");
}

G4ITSteppingMessenger::~G4ITSteppingMessenger()
{
  delete fITDirectory;
  delete fProcessCmd;
  delete fVerboseCmd;
  delete fInitCmd;
}

void G4ITSteppingMessenger::SetNewValue(G4UIcommand * command,G4String newValue)
{
  if( command==fProcessCmd )
  { fITStepManager->Process(); }
  else if(command==fEndTime)
  { fITStepManager->SetEndTime(fEndTime->GetNewDoubleValue(newValue)); }
  else if( command==fVerboseCmd )
  { fITStepManager->SetVerbose(fVerboseCmd->GetNewIntValue(newValue)); }
  else if( command==fInitCmd )
  { fITStepManager->Initialize(); }
  else if(command == fMaxNULLTimeSteps)
  { fITStepManager->SetMaxZeroTimeAllowed(fMaxNULLTimeSteps->GetNewIntValue(newValue)); }
  else if(command == fMaxStepNumber)
  { fITStepManager->SetMaxNbSteps(fMaxStepNumber->GetNewIntValue(newValue)); }
}

G4String G4ITSteppingMessenger::GetCurrentValue(G4UIcommand * command)
{
  G4String cv;

  if( command==fVerboseCmd )
  { cv = fVerboseCmd->ConvertToString(fITStepManager->GetVerbose()); }
  else if(command==fEndTime)
  { cv = fEndTime->ConvertToString(fITStepManager->GetEndTime()); }
  else if( command==fInitCmd )
  { cv = fInitCmd->ConvertToString(fITStepManager->IsInitialized()); }
  else if(command == fMaxNULLTimeSteps)
  { cv = fMaxNULLTimeSteps->ConvertToString(fITStepManager->GetMaxZeroTimeAllowed()); }
  else if(command == fMaxStepNumber)
  { cv = fMaxStepNumber->ConvertToString(fITStepManager->GetMaxNbSteps()); }

  return cv;
}

