#include "Blueprint_CEditorSettings.h"
#include "Blueprint_CUtils.h"

TArray<FString> UBlueprint_CEditorSettings::GetSuperClassList()
{
	TArray<FString> ClassList;
	TObjectPtr<UClass> Class = CurrentClass;
	while(Class){
		if(Class->GetSuperClass() == nullptr)
			break;
		Class = Class->GetSuperClass();
		ClassList.Add(FBlueprint_C::GetStructFullName(Class));
	}
	return ClassList;
}
