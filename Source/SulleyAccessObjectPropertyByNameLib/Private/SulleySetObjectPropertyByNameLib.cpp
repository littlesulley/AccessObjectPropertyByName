// Sulley. All rights reserved.

#include "SulleySetObjectPropertyByNameLib.h"
#include "SulleyGetObjectPropertyByNameLib.h"

void USulleySetObjectPropertyByNameLib::Generic_SulleySetObjectPropertyByName(UObject* OwnerObject, FName PropertyName, void* NewValuePtr, FProperty* NewValueProp, bool& Success, void* ValuePtr, FProperty* ValueProp)
{
	if (OwnerObject != nullptr)
	{
		FProperty* Property = USulleyGetObjectPropertyByNameLib::GetPropertyFromObject(OwnerObject, PropertyName);

		if (Property != nullptr)
		{
			if (!Property->SameType(ValueProp))
			{
				Success = false;
				return;
			}

			void* SrcPtr = Property->ContainerPtrToValuePtr<void>(OwnerObject);

			if (NewValueProp != nullptr)
			{
				Property->CopyCompleteValue(SrcPtr, NewValuePtr);
				Property->CopyCompleteValue(ValuePtr, SrcPtr);
				Success = true;
			}
			else
			{
				Property->CopyCompleteValue(ValuePtr, SrcPtr);
				Success = false;
			}
		}
	}
}