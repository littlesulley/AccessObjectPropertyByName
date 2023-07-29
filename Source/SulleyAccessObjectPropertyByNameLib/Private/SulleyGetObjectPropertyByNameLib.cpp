// Sulley. All rights reserved.

#include "SulleyGetObjectPropertyByNameLib.h"

void USulleyGetObjectPropertyByNameLib::Generic_SulleyGetObjectPropertyByName(UObject* OwnerObject, FName PropertyName, bool& Success, void* ValuePtr, FProperty* ValueProp)
{
	if (OwnerObject != nullptr)
	{
		FProperty* Property = GetPropertyFromObject(OwnerObject, PropertyName);

		if (Property != nullptr)
		{
			if (!Property->SameType(ValueProp))
			{
				Success = false;
				return;
			}

			void* SrcPtr = Property->ContainerPtrToValuePtr<void>(OwnerObject);
			Property->CopyCompleteValue(ValuePtr, SrcPtr);
			Success = true;
		}
	}
}

FProperty* USulleyGetObjectPropertyByNameLib::GetPropertyFromObject(UObject* Object, FName PropertyName)
{
	UClass* ObjectClass = Object->GetClass();

	FProperty* Property = FindFProperty<FProperty>(ObjectClass, PropertyName);
	if (Property != nullptr)
	{
		return Property;
	}

#if WITH_EDITORONLY_DATA
	UBlueprint* Blueprint = Cast<UBlueprint>(ObjectClass->ClassGeneratedBy);

	if (Blueprint == nullptr)
	{
		return nullptr;
	}

	Property = FindFProperty<FProperty>(Blueprint->GetClass(), PropertyName);

	return Property;
#endif
}