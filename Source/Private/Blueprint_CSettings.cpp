﻿#include "Blueprint_CSettings.h"
#include "Blueprint_CUtils.h"

TArray<FString> UBlueprint_CSettings::GetSuperClassList()
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

void UBlueprint_CSettings::SetCurrentClass(UClass* InCurrentClass, FString InSuperClass)
{
	CurrentClass = InCurrentClass;
	SuperClass = InSuperClass;
	if (UBlueprint* Blueprint = Cast<UBlueprint>(InCurrentClass->ClassGeneratedBy)) {
		bAutoGenerate = FBlueprint_C::IsAutoGenerateEnabled(Blueprint); 
	}
}

void UBlueprint_CSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	const FName PropertyName = PropertyChangedEvent.GetMemberPropertyName();

	if (CurrentClass && PropertyName == GET_MEMBER_NAME_CHECKED(UBlueprint_CSettings, bAutoGenerate)) {
		if (UBlueprint* Blueprint = Cast<UBlueprint>(CurrentClass->ClassGeneratedBy)) {
			FBlueprint_C::SetAutoGenerateEnabled(Blueprint, bAutoGenerate);
		}
	}
}
