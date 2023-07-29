// Sulley. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SulleyGetObjectPropertyByNameLib.generated.h"

UCLASS()
class SULLEYACCESSOBJECTPROPERTYBYNAMELIB_API USulleyGetObjectPropertyByNameLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Custom thunk function used to get a property's value. 
	 * @param Object - The object that owns this property.
	 * @param PropertyName - Name of property you want to get value from or set to. Note this should be the full name rather than the display name. For most boolean values, a prefix `b` should be appended.
	 * @param Success - Whether successfully extract the property of interest.
	 * @param Value - The value you want to get from the object's property.
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, CustomThunk, meta = (CustomStructureParam = "Value"))
	static void SulleyGetObjectPropertyByName(UObject* Object, FName PropertyName, bool& Success, int32& Value);
	
	DECLARE_FUNCTION(execSulleyGetObjectPropertyByName)
	{
		P_GET_OBJECT(UObject, Object);
		P_GET_PROPERTY(FNameProperty, PropertyName);
		P_GET_PROPERTY_REF(FBoolProperty, Success);

		Stack.StepCompiledIn<FMapProperty>(NULL);
		void* ValuePtr = Stack.MostRecentPropertyAddress;
		FProperty* ValueProp = Stack.MostRecentProperty;
		P_FINISH;

		P_NATIVE_BEGIN;
		Generic_SulleyGetObjectPropertyByName(Object, PropertyName, Success, ValuePtr, ValueProp);
		P_NATIVE_END;
	}
	/** Actual implementation of SulleyGetObjectPropertyByName. */
	static void Generic_SulleyGetObjectPropertyByName(UObject* OwnerObject, FName PropertyName, bool& Success, void* ValuePtr, FProperty* ValueProp);

	static FProperty* GetPropertyFromObject(UObject* Object, FName PropertyName);
};
