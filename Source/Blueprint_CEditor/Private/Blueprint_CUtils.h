#pragma once

namespace FBlueprint_C{
	struct FPropertyDefinition
	{
		FString TypeName = "void";
		FString Name;
		uint16 Offset = 0;
		uint16 Size = 0;
		EPropertyFlags Flags;
		TMap<FName, FString> MetaData;
		FPropertyDefinition() = default;

		FPropertyDefinition(FProperty* InProperty);
		bool HasAllPropertyFlags(uint64 FlagsToCheck) const;
	};
	struct FFunctionDefinition
	{
		FString Name;
		uint16 ParmsSize;
		uint16 ReturnValueOffset;
		FPropertyDefinition ReturnParam;
		TArray<FPropertyDefinition> Params;
		TArray<FPropertyDefinition> OutParams;
		EFunctionFlags Flags;
		TMap<FName, FString> MetaData;

		FFunctionDefinition(UFunction* InFunction);
		bool HasAllFunctionFlags(uint64 FlagsToCheck) const;
	};

	FString GetStructFullName(UStruct* InStruct);
	FString GetDelegateFullName(UFunction* InSignatureFunction);
	FString GetPropertyTypeString(FProperty* InProperty);
	FString GetPropertyRawTypeString(FProperty * InProperty);
	TPair<FString, FString> GetAutoGenerateFilePath(UClass* InClass);

	TPair<FString, FString> GenerateCode(UClass* InClass, UClass* InSuperClass = UObject::StaticClass(), bool bSplitHeader = false, bool bWithProperty = true, bool bWithFunction = true, bool bWithMarco = true, bool bWithMetaData = true);
}