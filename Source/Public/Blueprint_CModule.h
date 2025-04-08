#pragma once

#include "Modules/ModuleManager.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
#include "BlueprintEditor.h"
#include "BlueprintModes/WidgetBlueprintApplicationMode.h"

class FBlueprint_CModule : public IModuleInterface
{
public:
protected:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void RegisterBlueprintEditorTab(FWorkflowAllowedTabSet& TabFactories, FName InModeName, TSharedPtr<FBlueprintEditor> BlueprintEditor);
	void RegisterUMGEditorTab(const FWidgetBlueprintApplicationMode& Mode, FWorkflowAllowedTabSet& TabFactories);
};
