// Sulley. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SulleySetObjectPropertyByNameLib.generated.h"

UCLASS()
class SULLEYACCESSOBJECTPROPERTYBYNAMELIB_API USulleySetObjectPropertyByNameLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Custom thunk function used to set a property's value.
	 * @param Object - The object that owns this property.
	 * @param PropertyName - Name of property you want to set to. Note this should be the full name rather than the display name. For most boolean values, a prefix `b` should be appended.
	 * @param Success - Whether successfully set the property of interest.
	 * @param OutValue - The value corresponding to the object's property of interest. Will return the new value if successfully set, otherwise return the old value.
	 * @param NewValue - The new value you want to set to the object's property.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, CustomThunk, meta = (CustomStructureParam = "Value, NewValue"))
	static void SulleySetObjectPropertyByName(UObject* Object, FName PropertyName, bool& Success, int32& Value, int32 NewValue);

	DECLARE_FUNCTION(execSulleySetObjectPropertyByName)
	{
		P_GET_OBJECT(UObject, Object);
		P_GET_PROPERTY(FNameProperty, PropertyName);
		P_GET_PROPERTY_REF(FBoolProperty, Success);

		Stack.StepCompiledIn<FProperty>(NULL);
		void* ValuePtr = Stack.MostRecentPropertyAddress;
		FProperty* ValueProp = Stack.MostRecentProperty;

		// From https://github.com/colory-games/UEPlugin-AccessVariableByName/blob/main/AccessVariableByName/Source/VariableAccessFunctionLibrary/Private/VariableAccessFunctionLibraryUtils.cpp
		int32 PropertySize = ValueProp->ElementSize * ValueProp->ArrayDim;
		void* NewValuePtr = FMemory_Alloca(PropertySize);
		ValueProp->InitializeValue(NewValuePtr);
		Stack.MostRecentPropertyAddress = NULL;
		Stack.StepCompiledIn<FProperty>(NewValuePtr);
		FProperty* NewValueProp = Stack.MostRecentProperty;
		P_FINISH;

		P_NATIVE_BEGIN;
		Generic_SulleySetObjectPropertyByName(Object, PropertyName,NewValuePtr, NewValueProp, Success, ValuePtr, ValueProp);
		P_NATIVE_END;
	}
	/** Actual implementation of SulleySetObjectPropertyByName. */
	static void Generic_SulleySetObjectPropertyByName(UObject* OwnerObject, FName PropertyName, void* NewValuePtr, FProperty* NewValueProp, bool& Success, void* ValuePtr, FProperty* ValueProp);	
};
