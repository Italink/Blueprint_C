#pragma once

#include "Blueprint_CSettings.generated.h"

UCLASS(EditInlineNew, CollapseCategories, config = Blueprint_CSettings)
class UBlueprint_CSettings : public UObject {
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

	UPROPERTY(EditAnywhere)
	bool bAutoGenerate = false;

	UFUNCTION()
	TArray<FString> GetSuperClassList();

	void SetCurrentClass(UClass* InCurrentClass, FString InSuperClass);

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
}; 
