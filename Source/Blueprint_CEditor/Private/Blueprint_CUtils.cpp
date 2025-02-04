#include "Blueprint_CUtils.h"
#include "Misc/EngineVersionComparison.h"
#include "UObject/PropertyOptional.h"
#include "Interfaces/IProjectManager.h"
#include "Interfaces/IPluginManager.h"

FBlueprint_C::FPropertyDefinition::FPropertyDefinition(FProperty* InProperty)
{
	TypeName = FBlueprint_C::GetPropertyTypeString(InProperty);
	Name = InProperty->GetName();
	Offset = InProperty->GetOffset_ReplaceWith_ContainerPtrToValuePtr();
	Size = InProperty->GetSize();
	Flags = InProperty->GetPropertyFlags();
	if (InProperty->GetMetaDataMap() != nullptr) {
		MetaData = *InProperty->GetMetaDataMap();
	}
}

bool FBlueprint_C::FPropertyDefinition::HasAllPropertyFlags(uint64 FlagsToCheck) const
{
	return ((Flags & FlagsToCheck) == FlagsToCheck);
}

FBlueprint_C::FFunctionDefinition::FFunctionDefinition(UFunction* InFunction)
{
	Name = InFunction->GetName();
	ParmsSize = InFunction->ParmsSize;
	ReturnValueOffset = InFunction->ReturnValueOffset;
	Flags = InFunction->FunctionFlags;
	UPackage* Package = InFunction->GetOutermost();
	UMetaData* MetaDataSource = Package->GetMetaData();
	if (auto MetaDataPtr = MetaDataSource->ObjectMetaDataMap.Find(InFunction)) {
		MetaData = *MetaDataPtr;
	}
	for (TFieldIterator<FProperty> ParamIterator(InFunction); ParamIterator && (ParamIterator->PropertyFlags & CPF_Parm); ++ParamIterator) {
		FPropertyDefinition ParamDef(*ParamIterator);
		if (ParamIterator->HasAnyPropertyFlags(CPF_ReturnParm)) {
			ReturnParam = ParamDef;
		}		
		else{
			if (ParamIterator->HasAnyPropertyFlags(CPF_OutParm) && !ParamIterator->HasAnyPropertyFlags(CPF_ConstParm)) {
				OutParams.Emplace(ParamDef);
			}
			Params.Emplace(ParamDef);
		}
	}
}

bool FBlueprint_C::FFunctionDefinition::HasAllFunctionFlags(uint64 FlagsToCheck) const
{
	return ((Flags & FlagsToCheck) == FlagsToCheck);
}

FString FBlueprint_C::GetStructFullName(UStruct* InStruct)
{
	if (InStruct) {
		return FString::Printf(TEXT("%s%s")
			, InStruct->GetPrefixCPP()
			, *InStruct->GetName()
		);
	}
	return FString();
}

FString FBlueprint_C::GetDelegateFullName(UFunction* InSignatureFunction)
{
	if (InSignatureFunction != nullptr)
	{
		FString DelegateName = InSignatureFunction->GetName();

		DelegateName.LeftChopInline(FString(HEADER_GENERATED_DELEGATE_SIGNATURE_SUFFIX).Len(),
#if UE_VERSION_NEWER_THAN(5, 5, 0)
			EAllowShrinking::No
#else
			false
#endif
		);
		return FString::Printf(TEXT("F%s"),*DelegateName);
	}
	return FString();
}

FString FBlueprint_C::GetPropertyTypeString(FProperty* InProperty)
{
	if (InProperty == nullptr)
		return TEXT("");
	FString TypeString = GetPropertyRawTypeString(InProperty);
	if (InProperty->HasAnyPropertyFlags(CPF_ReturnParm)) {
		return TypeString;
	}
	if (InProperty->HasAnyPropertyFlags(CPF_OutParm)|| InProperty->HasAnyPropertyFlags(CPF_ReferenceParm)) 
		TypeString += "&";
	if (InProperty->HasAnyPropertyFlags(CPF_ConstParm)) 
		TypeString = "const " + TypeString;
	return TypeString;
}

FString FBlueprint_C::GetPropertyRawTypeString(FProperty* InProperty)
{
	if (InProperty == nullptr) 
		return TEXT("");
	if (const auto ByteProperty = CastField<FByteProperty>(InProperty)){
		if (ByteProperty->Enum != nullptr){
			return ByteProperty->Enum->GetName();
		}
		return "uint8";
	}
	if (const auto EnumProperty = CastField<FEnumProperty>(InProperty)){
		return EnumProperty->GetEnum()->GetName();
	}
	if (CastField<FUInt16Property>(InProperty)) 
		return "uint16";
	if (CastField<FUInt32Property>(InProperty))
		return "uint32";
	if (CastField<FUInt64Property>(InProperty))
		return "uint64";
	if (CastField<FInt8Property>(InProperty))
		return "int8";
	if (CastField<FInt16Property>(InProperty))
		return "int16";	
	if (CastField<FIntProperty>(InProperty))
		return "int32";
	if (CastField<FInt64Property>(InProperty))
		return "int64";
	if (CastField<FBoolProperty>(InProperty))
		return "bool";
	if (CastField<FFloatProperty>(InProperty))
		return "float";
	if (CastField<FDoubleProperty>(InProperty))
		return "double";
	if (CastField<FNameProperty>(InProperty))
		return "FName";
	if (CastField<FStrProperty>(InProperty))
		return "FString";
	if (CastField<FTextProperty>(InProperty)) 
		return "FText";
	if (const auto ArrayProperty = CastField<FArrayProperty>(InProperty)) {
		return FString::Printf(TEXT("TArray<%s>")
			, *GetPropertyTypeString(ArrayProperty->Inner)
		);
	}
	if (const auto MapProperty = CastField<FMapProperty>(InProperty)){
		return FString::Printf(TEXT("TMap<%s, %s>")
			, *GetPropertyTypeString(MapProperty->KeyProp)
			, *GetPropertyTypeString(MapProperty->ValueProp)
		);
	}

	if (const auto SetProperty = CastField<FSetProperty>(InProperty)){
		return FString::Printf(TEXT("TSet<%s>")
			, *GetPropertyTypeString(SetProperty->ElementProp)
		);
	}
	if (const auto ClassProperty = CastField<FClassProperty>(InProperty)){
		if (ClassProperty->HasAnyPropertyFlags(CPF_UObjectWrapper)){
			return FString::Printf(TEXT("TSubclassOf<%s>"),
				*GetStructFullName(ClassProperty->MetaClass)
			);
		}
		else{
			return "TObjectPtr<UClass>";
		}
	}
	if (const auto SoftClassProperty = CastField<FSoftClassProperty>(InProperty)){
		return FString::Printf(TEXT("TSoftClassPtr<%s>")
			, *GetStructFullName(SoftClassProperty->MetaClass)
		);
	}
	if (const auto ObjectProperty = CastField<FObjectProperty>(InProperty)){
		return FString::Printf(TEXT("TObjectPtr<%s>")
			, *GetStructFullName(ObjectProperty->PropertyClass)
		);
	}
	if (const auto WeakObjectProperty = CastField<FWeakObjectProperty>(InProperty)){
		return FString::Printf(TEXT("TWeakObjectPtr<%s>")
			, *GetStructFullName(WeakObjectProperty->PropertyClass)
		);
	}
	if (const auto LazyObjectProperty = CastField<FLazyObjectProperty>(InProperty)){
		return FString::Printf(TEXT("TLazyObjectPtr<%s>")
			, *GetStructFullName(LazyObjectProperty->PropertyClass)
		);
	}
	if (const auto SoftObjectProperty = CastField<FSoftObjectProperty>(InProperty)){
		return FString::Printf(TEXT("TSoftObjectPtr<%s>")
			, *GetStructFullName(SoftObjectProperty->PropertyClass)
		);
	}
	if (const auto StructProperty = CastField<FStructProperty>(InProperty)){
		return GetStructFullName(StructProperty->Struct);
	}
	if (const auto FieldPathProperty = CastField<FFieldPathProperty>(InProperty)){
		return FString::Printf(TEXT("TFieldPath<F%s>")
			, *FieldPathProperty->PropertyClass->GetName()
		);
	}
	if (const auto DelegateProperty = CastField<FDelegateProperty>(InProperty)){
		return GetDelegateFullName(DelegateProperty->SignatureFunction);
	}
	if (const auto MulticastDelegateProperty = CastField<FMulticastDelegateProperty>(InProperty)){
		return GetDelegateFullName(MulticastDelegateProperty->SignatureFunction);
	}
	if (const auto InterfaceProperty = CastField<FInterfaceProperty>(InProperty)){
		return FString::Printf(TEXT("TScriptInterface<I%s>")
			, *GetStructFullName(InterfaceProperty->InterfaceClass)
		);
	}
#if UE_VERSION_NEWER_THAN(5, 3, 0)
	if (const auto OptionalProperty = CastField<FOptionalProperty>(InProperty)){
		return FString::Printf(TEXT("TOptional<I%s>")
			, *GetPropertyTypeString(OptionalProperty->GetValueProperty())
		);
	}
#endif
	return TEXT("");
}

TPair<FString, FString> FBlueprint_C::GetAutoGenerateFilePath(UClass* InClass)
{
	FString PathName = InClass->GetPackage()->GetPathName();
	TArray<FString> SplitPath;
	PathName.ParseIntoArray(SplitPath, TEXT("/"));
	FString PluginName = SplitPath.IsEmpty() ? "Game" : SplitPath[0];
	TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin("Blueprint_C");
	FString HeaderFilePath = FString::Printf(TEXT("%s/%s/%s.h")
		, *FPaths::ConvertRelativePathToFull(Plugin->GetBaseDir())
		, TEXT("Source/Blueprint_C/Public")
		, *PathName.RightChop(PluginName.Len() + 2)
	);
	FString SourceFilePath = FString::Printf(TEXT("%s/%s/%s.cpp")
		, *FPaths::ConvertRelativePathToFull(Plugin->GetBaseDir())
		, TEXT("Source/Blueprint_C/Private")
		, *PathName.RightChop(PluginName.Len() + 2)
	);
	return { HeaderFilePath, SourceFilePath };
}

TPair<FString, FString> FBlueprint_C::GenerateCode(UClass* InClass, UClass* InSuperClass /*= UObject::StaticClass()*/, bool bSplitHeader, bool bWithProperty, bool bWithFunction, bool bWithMarco, bool bWithMetaData)
{
	TPair<FString, FString> Code;
	if (InClass == nullptr)
		return Code;
	TArray<FBlueprint_C::FPropertyDefinition> Properties;
	TArray<FBlueprint_C::FFunctionDefinition> Functions;
	if (bWithProperty) {
		UClass* CurrentClass = InClass;
		while (CurrentClass)
		{
			for (TFieldIterator<FProperty> PropertyIterator(CurrentClass, EFieldIteratorFlags::ExcludeSuper, EFieldIteratorFlags::ExcludeDeprecated); PropertyIterator; ++PropertyIterator) {
				Properties.Add(FBlueprint_C::FPropertyDefinition(*PropertyIterator));
			}
			CurrentClass = CurrentClass->GetSuperClass();
			if (CurrentClass == InSuperClass) {
				break;
			}
		}

		Properties.Sort([](const FBlueprint_C::FPropertyDefinition& A, const FBlueprint_C::FPropertyDefinition& B) {
			return A.Offset < B.Offset;
		});
	}
	if (bWithFunction) {
		UClass* CurrentClass = InClass;
		while (CurrentClass)
		{
			for (TFieldIterator<UFunction> FunctionIterator(CurrentClass, EFieldIteratorFlags::ExcludeSuper, EFieldIteratorFlags::ExcludeDeprecated); FunctionIterator; ++FunctionIterator) {
				Functions.Add(FBlueprint_C::FFunctionDefinition(*FunctionIterator));
			}
			CurrentClass = CurrentClass->GetSuperClass();
			if (CurrentClass == InSuperClass) {
				break;
			}
		}
	}
	FString& Header = Code.Key;
	FString& Source = Code.Value;

	FString ClassName = GetStructFullName(InClass);
	FString SuperClassName = GetStructFullName(InSuperClass);
	FString ClassPath = InClass->GetPathName();

	Header += FString::Printf(TEXT("class %s : public %s \n{\npublic:\n"), *ClassName, *SuperClassName);
	if (bSplitHeader) {
		Header += FString::Printf(TEXT("    static UClass* StaticClass();\n\n"), *InClass->GetPathName());
		Source += FString::Printf(TEXT("UClass* %s::StaticClass();\n{\n    return LoadObject<UClass>(nullptr, TEXT(\"%s\")); \n}\n\n"), *ClassName, *ClassPath);
	}
	else {
		Header += FString::Printf(TEXT("    static UClass* StaticClass()\n    {\n        return LoadObject<UClass>(nullptr, TEXT(\"%s\")); \n    }\n\n"), *ClassPath);
	}

	uint32 MemoryOffset = InSuperClass->GetPropertiesSize();
	for (int i = 0; i < Properties.Num(); i++) {
		const FBlueprint_C::FPropertyDefinition& PropertyDef = Properties[i];
		if (MemoryOffset < PropertyDef.Offset) {
			int PaddingSize = PropertyDef.Offset - MemoryOffset;
			Header += FString::Printf(TEXT("    uint8 __Padding%d[%d];\n"), i, PaddingSize);
			if (bWithMarco) {
				Header += FString::Printf(TEXT("\n"));
			}
			MemoryOffset += PaddingSize;
		}
		if (bWithMarco) {
			Header += FString::Printf(TEXT("    UPROPERTY("));
			FString MacroString;
			if (PropertyDef.HasAllPropertyFlags((CPF_BlueprintReadOnly)))
				MacroString += ", BlueprintReadOnly";
			else if (PropertyDef.HasAllPropertyFlags((CPF_BlueprintVisible))) 
				MacroString += ", BlueprintReadWrite";
			if (PropertyDef.HasAllPropertyFlags(CPF_Edit | CPF_EditConst))
				MacroString += ", VisibleAnywhere";	
			else if (PropertyDef.HasAllPropertyFlags(CPF_Edit)) 
				MacroString += ", EditAnywhere";
			if (PropertyDef.HasAllPropertyFlags(CPF_BlueprintAssignable))
				MacroString += ", BlueprintAssignable";
			if (PropertyDef.HasAllPropertyFlags(CPF_BlueprintAuthorityOnly))
				MacroString += ", BlueprintAuthorityOnly";
			if (PropertyDef.HasAllPropertyFlags(CPF_BlueprintAuthorityOnly))
				MacroString += ", BlueprintAuthorityOnly";
			if (PropertyDef.HasAllPropertyFlags(CPF_Config))
				MacroString += ", Config";
			if (PropertyDef.HasAllPropertyFlags(CPF_AdvancedDisplay))
				MacroString += ", AdvancedDisplay";
			if (PropertyDef.HasAllPropertyFlags(CPF_SaveGame))
				MacroString += ", SaveGame";
			if (PropertyDef.HasAllPropertyFlags((CPF_PersistentInstance | CPF_ExportObject | CPF_InstancedReference)))
				MacroString += ", Instanced";
			else if (PropertyDef.HasAllPropertyFlags(CPF_ExportObject))
				MacroString += ", Export";
			if (PropertyDef.HasAllPropertyFlags(CPF_Transient))
				MacroString += ", Transient";
			if (PropertyDef.HasAllPropertyFlags(CPF_DuplicateTransient))
				MacroString += ", DuplicateTransient";
			if (PropertyDef.HasAllPropertyFlags(CPF_NonPIEDuplicateTransient))
				MacroString += ", NonPIEDuplicateTransient";
			if (PropertyDef.HasAllPropertyFlags(CPF_SkipSerialization))
				MacroString += ", SkipSerialization";
			if (PropertyDef.HasAllPropertyFlags(CPF_Net))
				MacroString += ", Replicated";
			if (PropertyDef.HasAllPropertyFlags(CPF_RepSkip))
				MacroString += ", NotReplicated";
			
			if (bWithMetaData && !PropertyDef.MetaData.IsEmpty()) {
				FString MetaDataString;
				for (auto MetaData : PropertyDef.MetaData) {
					MetaDataString += FString::Printf(TEXT(", %s=\"%s\""),*MetaData.Key.ToString(), *MetaData.Value);
				}
				if (!MetaDataString.IsEmpty())
					MacroString += FString::Printf(TEXT(", Meta = (%s)"), *MetaDataString.RightChop(2)).Replace(TEXT("\n"), TEXT("\\n"));
			}
			if(!MacroString.IsEmpty())
				MacroString = MacroString.RightChop(2);
			Header += MacroString;
			Header += FString::Printf(TEXT(")\n"));
		}
		Header += FString::Printf(TEXT("    %s %s;\n"), *PropertyDef.TypeName, *PropertyDef.Name);
		if (bWithMarco) {
			Header += FString::Printf(TEXT("\n"));
		}
		MemoryOffset += PropertyDef.Size;
	}

	Header += "\n";

	for (int i = 0; i < Functions.Num(); i++) {
		const FBlueprint_C::FFunctionDefinition& FunctionDef = Functions[i];
		FString ParamsString;
		FString ParamsMemcpyString;
		for (int j = 0; j < FunctionDef.Params.Num(); j++) {
			if (j != 0)
				ParamsString += ", ";
			ParamsString += FString::Printf(TEXT("%s %s"), *FunctionDef.Params[j].TypeName, *FunctionDef.Params[j].Name);
			if (bSplitHeader) {
				ParamsMemcpyString += FString::Printf(TEXT("    FMemory::Memcpy(Params + %d, &%s, %d);\n"), FunctionDef.Params[j].Offset, *FunctionDef.Params[j].Name, FunctionDef.Params[j].Size);
			}
			else {
				ParamsMemcpyString += FString::Printf(TEXT("        FMemory::Memcpy(Params + %d, &%s, %d);\n"), FunctionDef.Params[j].Offset, *FunctionDef.Params[j].Name, FunctionDef.Params[j].Size);
			}
		}
		if (bWithMarco) {
			Header += FString::Printf(TEXT("    UFUNCTION("));
			FString MacroString;
			if (FunctionDef.HasAllFunctionFlags(FUNC_BlueprintCallable))
				MacroString += ", BlueprintCallable";
			if (FunctionDef.HasAllFunctionFlags((FUNC_Event | FUNC_Public | FUNC_BlueprintCallable | FUNC_BlueprintEvent)))
				MacroString += ", BlueprintImplementableEvent";
			if (FunctionDef.HasAllFunctionFlags((FUNC_Native | FUNC_Event | FUNC_Public | FUNC_BlueprintCallable | FUNC_BlueprintEvent)))
				MacroString += ", BlueprintNativeEvent";
			if (FunctionDef.HasAllFunctionFlags((FUNC_BlueprintPure)))
				MacroString += ", BlueprintPure";
			if (FunctionDef.HasAllFunctionFlags((FUNC_Exec)))
				MacroString += ", Exec";
			if (FunctionDef.HasAllFunctionFlags((FUNC_BlueprintAuthorityOnly)))
				MacroString += ", BlueprintAuthorityOnly";
			if (FunctionDef.HasAllFunctionFlags((FUNC_BlueprintCosmetic)))
				MacroString += ", BlueprintCosmetic";
			if (FunctionDef.HasAllFunctionFlags((FUNC_NetMulticast)))
				MacroString += ", NetMulticast";
			if (FunctionDef.HasAllFunctionFlags((FUNC_NetReliable)))
				MacroString += ", Reliable";
			if (FunctionDef.HasAllFunctionFlags((FUNC_NetClient)))
				MacroString += ", Client";
			if (bWithMetaData && !FunctionDef.MetaData.IsEmpty()) {
				FString MetaDataString;
				for (auto MetaData : FunctionDef.MetaData) {
					MetaDataString += FString::Printf(TEXT(", %s=\"%s\""), *MetaData.Key.ToString(), *MetaData.Value);
				}
				if (!MetaDataString.IsEmpty())
					MacroString += FString::Printf(TEXT(", Meta = (%s)"), *MetaDataString.RightChop(2)).Replace(TEXT("\n"), TEXT("\\n"));
			}
			if (!MacroString.IsEmpty())
				MacroString = MacroString.RightChop(2);
			Header += MacroString;
			Header += FString::Printf(TEXT(")\n"));
		}
		if (bSplitHeader) {
			Header += FString::Printf(TEXT("    %s %s(%s);\n\n"), *FunctionDef.ReturnParam.TypeName, *FunctionDef.Name, *ParamsString);
			Source += FString::Printf(TEXT("%s %s::%s(%s)\n{\n"), *FunctionDef.ReturnParam.TypeName, *ClassName, *FunctionDef.Name, *ParamsString);
			Source += FString::Printf(TEXT("    UFunction* Function = GetClass()->FindFunctionByName(\"%s\");\n"), *FunctionDef.Name);
			if (FunctionDef.ParmsSize == 0) {
				Source += FString::Printf(TEXT("    uint8* Params = nullptr;\n"));
			}
			else {
				Source += FString::Printf(TEXT("    uint8 Params[%d];\n"), FunctionDef.ParmsSize);
			}
			Source += ParamsMemcpyString;
			Source += FString::Printf(TEXT("    ProcessEvent(Function, Params);\n"));
			for (int j = 0; j < FunctionDef.OutParams.Num(); j++) {
				Source += FString::Printf(TEXT("    FMemory::Memcpy(&%s, Params + %d, %d);\n"), *FunctionDef.OutParams[j].Name, FunctionDef.OutParams[j].Offset, FunctionDef.OutParams[j].Size);
			}
			if (FunctionDef.ReturnParam.Size != 0) {
				Source += FString::Printf(TEXT("    %s ReturnParam;\n"), *FunctionDef.ReturnParam.TypeName);
				Source += FString::Printf(TEXT("    FMemory::Memcpy(&ReturnParam, Params + %d, %d);\n"), FunctionDef.ReturnValueOffset, FunctionDef.ReturnParam.Size);
				Source += FString::Printf(TEXT("    return ReturnParam;\n"));
			}
			Source += FString(TEXT("}\n\n"));
		}
		else {
			Header += FString::Printf(TEXT("    %s %s(%s)\n    {\n"), *FunctionDef.ReturnParam.TypeName, *FunctionDef.Name, *ParamsString);
			Header += FString::Printf(TEXT("        UFunction* Function = GetClass()->FindFunctionByName(\"%s\");\n"), *FunctionDef.Name);
			if (FunctionDef.ParmsSize == 0) {
				Header += FString::Printf(TEXT("        uint8* Params = nullptr;\n"));
			}
			else {
				Header += FString::Printf(TEXT("        uint8 Params[%d];\n"), FunctionDef.ParmsSize);
			}
			Header += ParamsMemcpyString;
			Header += FString::Printf(TEXT("        ProcessEvent(Function, Params);\n"));
			for (int j = 0; j < FunctionDef.OutParams.Num(); j++) {
				Header += FString::Printf(TEXT("        FMemory::Memcpy(&%s, Params + %d, %d);\n"), *FunctionDef.OutParams[j].Name, FunctionDef.OutParams[j].Offset, FunctionDef.OutParams[j].Size);
			}
			if (FunctionDef.ReturnParam.Size != 0) {
				Header += FString::Printf(TEXT("        %s ReturnParam;\n"), *FunctionDef.ReturnParam.TypeName);
				Header += FString::Printf(TEXT("        FMemory::Memcpy(&ReturnParam, Params + %d, %d);\n"), FunctionDef.ReturnValueOffset, FunctionDef.ReturnParam.Size);
				Header += FString::Printf(TEXT("        return ReturnParam;\n"));
			}
			Header += FString(TEXT("    }\n\n"));
		}
	}
	Header += "};";
	return Code;
}