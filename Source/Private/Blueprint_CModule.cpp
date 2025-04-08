#include "Blueprint_CModule.h"
#include "Blueprint_CUtils.h"
#include "BlueprintEditorModule.h"
#include "Blueprint_CEditor.h"
#include "Toolkits/AssetEditorToolkitMenuContext.h"
#include "UMGEditorModule.h"

#define LOCTEXT_NAMESPACE "Blueprint_C"

void FBlueprint_CModule::StartupModule()
{
	FBlueprintEditorModule& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet");
	BlueprintEditorModule.OnRegisterTabsForEditor().AddRaw(this, &FBlueprint_CModule::RegisterBlueprintEditorTab);

	IUMGEditorModule& UMGEditorModule = FModuleManager::LoadModuleChecked<IUMGEditorModule>("UMGEditor");
	UMGEditorModule.OnRegisterTabsForEditor().AddRaw(this, &FBlueprint_CModule::RegisterUMGEditorTab);
}

void FBlueprint_CModule::ShutdownModule()
{
	FBlueprintEditorModule* BlueprintEditorModule = FModuleManager::GetModulePtr<FBlueprintEditorModule>("Kismet");
	if (BlueprintEditorModule)
	{
		BlueprintEditorModule->OnRegisterTabsForEditor().RemoveAll(this);
	}

	IUMGEditorModule* UMGEditorModule = FModuleManager::GetModulePtr<IUMGEditorModule>("UMGEditor");
	if (UMGEditorModule)
	{
		UMGEditorModule->OnRegisterTabsForEditor().RemoveAll(this);
	}
}

void FBlueprint_CModule::RegisterBlueprintEditorTab(FWorkflowAllowedTabSet& TabFactories, FName InModeName, TSharedPtr<FBlueprintEditor> BlueprintEditor)
{
	TabFactories.RegisterFactory(MakeShared<FBlueprint_CEditorSummoner>(BlueprintEditor));
}

void FBlueprint_CModule::RegisterUMGEditorTab(const FWidgetBlueprintApplicationMode& Mode, FWorkflowAllowedTabSet& TabFactories)
{
	TabFactories.RegisterFactory(MakeShared<FBlueprint_CEditorSummoner>(Mode.GetBlueprintEditor()));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBlueprint_CModule, Blueprint_CEditor)
