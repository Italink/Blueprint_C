#include "Blueprint_CEditorModule.h"
#include "Blueprint_CUtils.h"
#include "BlueprintEditorModule.h"
#include "Blueprint_CEditor.h"
#include "Toolkits/AssetEditorToolkitMenuContext.h"
#include "UMGEditorModule.h"

#define LOCTEXT_NAMESPACE "Blueprint_C"

void FBlueprint_CEditorModule::StartupModule()
{
	FBlueprintEditorModule& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet");
	BlueprintEditorModule.OnRegisterTabsForEditor().AddRaw(this, &FBlueprint_CEditorModule::RegisterBlueprintEditorTab);

	IUMGEditorModule& UMGEditorModule = FModuleManager::LoadModuleChecked<IUMGEditorModule>("UMGEditor");
	UMGEditorModule.OnRegisterTabsForEditor().AddRaw(this, &FBlueprint_CEditorModule::RegisterUMGEditorTab);


	//UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("AssetEditor.BlueprintEditor.ToolBar");
	//FToolMenuSection& Section = ToolbarMenu->AddSection("Blueprint_C");
	//Section.AddDynamicEntry("Blueprint_C", FNewToolMenuSectionDelegate::CreateLambda([this](FToolMenuSection& InSection) {
	//	if (auto EditorContext = InSection.FindContext<UAssetEditorToolkitMenuContext>()) {
	//		TWeakPtr<FAssetEditorToolkit> ToolkitPtr = EditorContext->Toolkit;
	//		auto Objects = ToolkitPtr.Pin()->GetObjectsCurrentlyBeingEdited();
	//		UBlueprint* Blueprint = nullptr;
	//		if (Objects->Num() == 1)
	//			Blueprint = Cast<UBlueprint>((*Objects)[0]);
	//		if (Blueprint) {
	//			InSection.AddEntry(FToolMenuEntry::InitWidget(
	//				"Blueprint_C"
	//				, SNew(SBlueprint_CToolbarButton, Blueprint)
	//				, LOCTEXT("Blueprint_C", "Blueprint_C")
	//			));
	//		}
	//	}
	//}));
}

void FBlueprint_CEditorModule::ShutdownModule()
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

void FBlueprint_CEditorModule::RegisterBlueprintEditorTab(FWorkflowAllowedTabSet& TabFactories, FName InModeName, TSharedPtr<FBlueprintEditor> BlueprintEditor)
{

}

void FBlueprint_CEditorModule::RegisterUMGEditorTab(const FWidgetBlueprintApplicationMode& Mode, FWorkflowAllowedTabSet& TabFactories)
{
	TabFactories.RegisterFactory(MakeShared<FBlueprint_CEditorSummoner>(Mode.GetBlueprintEditor()));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBlueprint_CEditorModule, Blueprint_CEditor)
