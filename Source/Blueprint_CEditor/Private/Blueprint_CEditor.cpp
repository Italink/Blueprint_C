#include "Blueprint_CEditor.h"
#include "Blueprint_CUtils.h"
#include "Blueprint_CEditorSettings.h"

#define LOCTEXT_NAMESPACE "Blueprint_C"

FBlueprint_CEditorSummoner::FBlueprint_CEditorSummoner(TSharedPtr<FBlueprintEditor> BlueprintEditor)
	: FWorkflowTabFactory("Blueprint_C", BlueprintEditor)
	, WeakBlueprintEditor(BlueprintEditor)
{
	TabLabel = LOCTEXT("Blueprint_C", "Blueprint_C");
	TabIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "EditorPreferences.TabIcon");
	bIsSingleton = true;

	ViewMenuDescription = LOCTEXT("Blueprint_C", "Blueprint_C");
	ViewMenuTooltip = LOCTEXT("Blueprint_C", "Blueprint_C");
}

TSharedRef<SWidget> FBlueprint_CEditorSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	return SNew(SBlueprint_CCodeEditor, WeakBlueprintEditor);
}

void SBlueprint_CCodeEditor::Construct(const FArguments& InArgs, TWeakPtr<FBlueprintEditor> InWeakBlueprintEditor)
{
	WeakBlueprintEditor = InWeakBlueprintEditor;
	UBlueprint* Blueprint = WeakBlueprintEditor.Pin()->GetBlueprintObj();
	Blueprint->OnCompiled().AddSP(this, &SBlueprint_CCodeEditor::OnBlueprintCompiled);
	SyntaxHighlighter  = FBlueprint_CTextLayoutMarshaller::Create(
		FBlueprint_CTextLayoutMarshaller::FSyntaxTextStyle()
	);

	FPropertyEditorModule& EditModule = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bShowObjectLabel = false;
	DetailsViewArgs.bAllowSearch = false;
	DetailsViewArgs.bAllowFavoriteSystem = false;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::ENameAreaSettings::HideNameArea;
	DetailsViewArgs.ViewIdentifier = FName("BlueprintDefaults");
	auto DetailsView = EditModule.CreateDetailView(DetailsViewArgs);

	UBlueprint_CEditorSettings* Settings = GetMutableDefault<UBlueprint_CEditorSettings>();
	Settings->SuperClass = FBlueprint_C::GetStructFullName(Blueprint->ParentClass);
	Settings->CurrentClass = Blueprint->GeneratedClass;

	DetailsView->SetObject(Settings);
	DetailsView->OnFinishedChangingProperties().AddSP(this, &SBlueprint_CCodeEditor::OnSettingsChanged);

	GeneratedHeader.HorizontalScrollBar = SNew(SScrollBar)
		.Orientation(Orient_Horizontal)
		.Thickness(FVector2D(12.0f, 12.0f));
	GeneratedHeader.VerticalScrollBar = SNew(SScrollBar)
		.Orientation(Orient_Vertical)
		.Thickness(FVector2D(12.0f, 12.0f));

	SAssignNew(GeneratedHeader.Container, SVerticalBox)
		+ SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				[
					SAssignNew(GeneratedHeader.Text, SMultiLineEditableTextBox)
						.Marshaller(SyntaxHighlighter)
						.IsReadOnly(true)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					GeneratedHeader.VerticalScrollBar.ToSharedRef()
				]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			GeneratedHeader.HorizontalScrollBar.ToSharedRef()
		];

	GeneratedSource.HorizontalScrollBar = SNew(SScrollBar)
		.Orientation(Orient_Horizontal)
		.Thickness(FVector2D(12.0f, 12.0f));
	GeneratedSource.VerticalScrollBar = SNew(SScrollBar)
		.Orientation(Orient_Vertical)
		.Thickness(FVector2D(12.0f, 12.0f));

	SAssignNew(GeneratedSource.Container, SVerticalBox)
		+ SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				[
					SAssignNew(GeneratedSource.Text, SMultiLineEditableTextBox)
						.Marshaller(SyntaxHighlighter)
						.IsReadOnly(true)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					GeneratedSource.VerticalScrollBar.ToSharedRef()
				]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			GeneratedSource.HorizontalScrollBar.ToSharedRef()
		];

	ChildSlot
	[
		SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				DetailsView
			]
			+ SVerticalBox::Slot()
			[
				GeneratedHeader.Container.ToSharedRef()
			]
			+ SVerticalBox::Slot()
			[
				GeneratedSource.Container.ToSharedRef()
			]
	];
	OnBlueprintCompiled(Blueprint);
}

SBlueprint_CCodeEditor::~SBlueprint_CCodeEditor()
{
}

void SBlueprint_CCodeEditor::OnBlueprintCompiled(UBlueprint* InBlueprint)
{
	RefreshCode();
}

void SBlueprint_CCodeEditor::OnSettingsChanged(const FPropertyChangedEvent& ChangedEvent)
{
	RefreshCode();
	UBlueprint_CEditorSettings* Settings = GetMutableDefault<UBlueprint_CEditorSettings>();
	Settings->TryUpdateDefaultConfigFile();
}

void SBlueprint_CCodeEditor::RefreshCode()
{
	UBlueprint* Blueprint = WeakBlueprintEditor.Pin()->GetBlueprintObj();
	if (Blueprint) {
		UBlueprint_CEditorSettings* Settings = GetMutableDefault<UBlueprint_CEditorSettings>();
		TObjectPtr<UClass> SuperClass = UObject::StaticClass();
		TObjectPtr<UClass> Class = Settings->CurrentClass = Blueprint->GeneratedClass;
		while (Class) {
			if (Class->GetSuperClass() == nullptr)
				break;
			Class = Class->GetSuperClass();
			if (FBlueprint_C::GetStructFullName(Class) == Settings->SuperClass) {
				SuperClass = Class;
				break;
			}
		}
		GeneratedSource.Container->SetVisibility(Settings->bSplitHeader ? EVisibility::Visible : EVisibility::Collapsed);
		AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, Blueprint, SuperClass, Settings]() {
			TPair<FString, FString> Code = FBlueprint_C::GenerateCode(Blueprint->GeneratedClass, SuperClass, Settings->bSplitHeader, Settings->bProperty, Settings->bFunction, Settings->bMacro, Settings->bMetaData);
			AsyncTask(ENamedThreads::GameThread, [this, Code](){
				GeneratedHeader.Text->SetText(FText::FromString(Code.Key));
				GeneratedSource.Text->SetText(FText::FromString(Code.Value));
			});
		});
	}
}

void SBlueprint_CToolbarButton::Construct(const FArguments& InArgs, TObjectPtr<UBlueprint> InBlueprint)
{
	Blueprint = InBlueprint;
	Blueprint->OnCompiled().AddSP(this, &SBlueprint_CToolbarButton::OnBlueprintCompiled);
	ChildSlot
	.Padding(5)
	[
		SNew(SHorizontalBox)
			.ToolTipText(LOCTEXT("Blueprint_CAutoGen", "Auto Generate Blueprint_C"))
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.AutoWidth()
			[
				SNew(SImage)
					.DesiredSizeOverride(FVector2D(20,20))
					.Image(FAppStyle::GetBrush("MainFrame.AddCodeToProject"))
			]
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.Padding(5, 0)
			[
				SNew(SCheckBox)
					.IsChecked(this, &SBlueprint_CToolbarButton::IsAutoGenerateEnabled)
					.OnCheckStateChanged(this, &SBlueprint_CToolbarButton::OnAutoGenerateChanged)
			]
	];
}

SBlueprint_CToolbarButton::~SBlueprint_CToolbarButton()
{
}

void SBlueprint_CToolbarButton::OnBlueprintCompiled(UBlueprint* InBlueprint)
{
	if (IsAutoGenerateEnabled() == ECheckBoxState::Checked) {
		TPair<FString, FString> FilePath = FBlueprint_C::GetAutoGenerateFilePath(InBlueprint->GeneratedClass);
		TPair<FString, FString> Code = FBlueprint_C::GenerateCode(InBlueprint->GeneratedClass);
		FFileHelper::SaveStringToFile(Code.Key, *FilePath.Key);
		FFileHelper::SaveStringToFile(Code.Value, *FilePath.Value);
	}
}

ECheckBoxState SBlueprint_CToolbarButton::IsAutoGenerateEnabled() const
{
	if (Blueprint) {
		Blueprint->Modify();
		return Blueprint->HideCategories.Contains("Blueprint_C") ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}
	return ECheckBoxState::Unchecked;
}

void SBlueprint_CToolbarButton::OnAutoGenerateChanged(ECheckBoxState InState)
{
	if (InState == ECheckBoxState::Checked) {
		Blueprint->HideCategories.AddUnique("Blueprint_C");
	}
	else {
		Blueprint->HideCategories.Remove("Blueprint_C");
	}
}

#undef LOCTEXT_NAMESPACE