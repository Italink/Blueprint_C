#pragma once

#include "Blueprint_CEditorSettings.generated.h"

UCLASS(EditInlineNew, CollapseCategories, config = Blueprint_CEditorSettings)
class UBlueprint_CEditorSettings : public UObject {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Config, meta = (GetOptions = "GetSuperClassList"))
	FString SuperClass;

	UPROPERTY(EditAnywhere, Config)
	bool bProperty = true;

	UPROPERTY(EditAnywhere, Config)
	bool bFunction = true;

	UPROPERTY(EditAnywhere, Config)
	bool bMacro = true;

	UPROPERTY(EditAnywhere, Config, meta = (EditCondition = "bMacro", EditConditionHides))
	bool bMetaData = true;

	UPROPERTY(EditAnywhere, Config)
	bool bSplitHeader = false;

	UPROPERTY(Transient)
	TObjectPtr<UClass> CurrentClass;

	UFUNCTION()
	TArray<FString> GetSuperClassList();
}; 
