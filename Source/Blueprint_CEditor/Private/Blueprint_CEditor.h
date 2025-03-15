#pragma once
#include "WorkflowOrientedApp/WorkflowTabManager.h"
#include "BlueprintEditor.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/STreeView.h"
#include "TickableEditorObject.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "Templates/SharedPointer.h"
#include "Containers/Map.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Blueprint_CTextLayoutMarshaller.h"

struct FBlueprint_CEditorSummoner : public FWorkflowTabFactory
{
	FBlueprint_CEditorSummoner(TSharedPtr<FBlueprintEditor> BlueprintEditor);
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
protected:
	TWeakPtr<FBlueprintEditor> WeakBlueprintEditor;
};

class SBlueprint_CCodeEditor : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SBlueprint_CCodeEditor){}
	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs, TWeakPtr<FBlueprintEditor> InWeakBlueprintEditor);
	virtual ~SBlueprint_CCodeEditor();
	void OnBlueprintCompiled(UBlueprint* InBlueprint);
	void OnSettingsChanged(const FPropertyChangedEvent& ChangedEvent);
	void RefreshCode();
protected:
	struct TabInfo
	{
		TSharedPtr<SMultiLineEditableTextBox> Text;
		TSharedPtr<SScrollBar> HorizontalScrollBar;
		TSharedPtr<SScrollBar> VerticalScrollBar;
		TSharedPtr<SVerticalBox> Container;
	};
	TWeakPtr<FBlueprintEditor> WeakBlueprintEditor;
	TabInfo GeneratedHeader;
	TabInfo GeneratedSource;
	TSharedPtr<SComboButton> ScriptNameCombo;
	TSharedPtr<SHorizontalBox> ScriptNameContainer;
	TSharedPtr<SVerticalBox> TextBodyContainer;
	TSharedPtr<SSearchBox> SearchBox;
	TSharedPtr<STextBlock> SearchFoundMOfNText;
	TArray<FTextLocation> ActiveFoundTextEntries;
	TSharedPtr<FBlueprint_CTextLayoutMarshaller> SyntaxHighlighter;
	int32 CurrentFoundTextEntry;
};

