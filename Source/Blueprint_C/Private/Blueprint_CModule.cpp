#include "Blueprint_CModule.h"
#include "BlueprintEditorModule.h"

#define LOCTEXT_NAMESPACE "Blueprint_C"

void FBlueprint_CModule::StartupModule()
{
	
}

void FBlueprint_CModule::ShutdownModule()
{

}
#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBlueprint_CModule, Blueprint_C)
