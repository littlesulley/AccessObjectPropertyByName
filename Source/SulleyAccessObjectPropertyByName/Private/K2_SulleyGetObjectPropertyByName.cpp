// Sulley. All rights reserved.

#include "K2_SulleyGetObjectPropertyByName.h"

#include "SulleyGetObjectPropertyByNameLib.h"
#include "EditorCategoryUtils.h"
#include "GraphEditorSettings.h"
#include "EdGraphUtilities.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "K2Node.h"
#include "K2Node_Self.h"
#include "K2Node_CallFunction.h"
#include "Kismet/GameplayStatics.h"
#include "KismetCompiler.h"
#include "Kismet2/BlueprintEditorUtils.h"

struct FK2_SulleyGetObjectPropertyByName
{
	static const FName InObjectPinName;
	static const FName InPropertyPinName;
	static const FName OutValidPinName;
};

const FName FK2_SulleyGetObjectPropertyByName::InObjectPinName(TEXT("InObject"));
const FName FK2_SulleyGetObjectPropertyByName::InPropertyPinName(TEXT("InProperty"));
const FName FK2_SulleyGetObjectPropertyByName::OutValidPinName(TEXT("PropertyValid"));

#define LOCTEXT_NAMESPACE "UK2_SulleyGetObjectPropertyByName"

void UK2_SulleyGetObjectPropertyByName::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	/** Do something. */
}

void UK2_SulleyGetObjectPropertyByName::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	/** Input */
	UEdGraphPin* InObjectPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UObject::StaticClass(), FK2_SulleyGetObjectPropertyByName::InObjectPinName);
	UEdGraphPin* InPropertyPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Name, FK2_SulleyGetObjectPropertyByName::InPropertyPinName);
	K2Schema->ConstructBasicPinTooltip(*InObjectPin, LOCTEXT("InObjectPinDescription", "The object we want to get property from."), InObjectPin->PinToolTip);
	K2Schema->ConstructBasicPinTooltip(*InPropertyPin, LOCTEXT("InPropertyPinDescription", "The name of the property we want to retrieve. Literal name only."), InPropertyPin->PinToolTip);

	/** Output */
	UEdGraphPin* OutValidPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Boolean, FK2_SulleyGetObjectPropertyByName::OutValidPinName);

	Super::AllocateDefaultPins();
}

void UK2_SulleyGetObjectPropertyByName::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	UEdGraphPin* InObjectPin = GetInObjectPin();

	/** Check the validity of in-object class. */
	UClass* InObjectClass = GetInObjectClass();

	if (InObjectClass == nullptr)
	{
		CompilerContext.MessageLog.Error(*(LOCTEXT("GetCameraPropertyByName_Error", "Node must have a valid input object.").ToString()));
		this->BreakAllNodeLinks();
		return;
	}

	/** Get in property and its name. */
	UEdGraphPin* InPropertyPin = GetInPropertyPin();
	FName InPropertyName = GetInPropertyName();

	if (InPropertyName.IsNone())
	{
		return;
	}

	/** Check if out value pin exists. */
	UEdGraphPin* OutValuePin = GetOutValuePin();

	if (OutValuePin == nullptr)
	{
		return;
	}

	/** Create intermediate nodes. */
	UClass* SulleyGetObjectPropertyByNameLib = USulleyGetObjectPropertyByNameLib::StaticClass();
	UFunction* GetterFunction = SulleyGetObjectPropertyByNameLib->FindFunctionByName(FName("SulleyGetObjectPropertyByName"));

	if (GetterFunction == nullptr)
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("SulleyGetObjectPropertyByName_FunctionNotFound", "Getter function not found.").ToString());
		return;
	}

	UK2Node_CallFunction* CallFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	CallFunction->SetFromFunction(GetterFunction);
	CallFunction->AllocateDefaultPins();

	UEdGraphPin* FunctionExecPin = CallFunction->GetExecPin();
	UEdGraphPin* FunctionThenPin = CallFunction->GetThenPin();
	UEdGraphPin* FunctionInObjectPin = CallFunction->FindPinChecked(TEXT("Object"));
	UEdGraphPin* FunctionInPropertyPin = CallFunction->FindPinChecked(TEXT("PropertyName"));
	UEdGraphPin* FunctionOutValidPin = CallFunction->FindPinChecked(TEXT("Success"));
	UEdGraphPin* FunctionOutValuePin = CallFunction->FindPinChecked(TEXT("Value"));

	FunctionOutValuePin->PinType = OutValuePin->PinType;

	UEdGraphPin* ExecPin = GetExecPin();
	UEdGraphPin* ThenPin = GetThenPin();
	UEdGraphPin* OutValidPin = GetOutValidPin();

	CompilerContext.MovePinLinksToIntermediate(*ExecPin, *FunctionExecPin);
	CompilerContext.MovePinLinksToIntermediate(*ThenPin, *FunctionThenPin);

	if (InObjectPin->LinkedTo.Num() == 1 || InObjectPin->DefaultObject)
	{
		CompilerContext.MovePinLinksToIntermediate(*InObjectPin, *FunctionInObjectPin);
	}
	else
	{
		UK2Node_Self* SelfNode = CompilerContext.SpawnIntermediateNode<UK2Node_Self>(this, SourceGraph);
		SelfNode->AllocateDefaultPins();
		SelfNode->Pins[0]->MakeLinkTo(FunctionInObjectPin);
	}

	CompilerContext.MovePinLinksToIntermediate(*InPropertyPin, *FunctionInPropertyPin);
	CompilerContext.MovePinLinksToIntermediate(*OutValidPin, *FunctionOutValidPin);
	CompilerContext.MovePinLinksToIntermediate(*OutValuePin, *FunctionOutValuePin);

	BreakAllNodeLinks();
}

void UK2_SulleyGetObjectPropertyByName::PinDefaultValueChanged(UEdGraphPin* Pin)
{
	if (Pin != NULL && Pin->PinName == FK2_SulleyGetObjectPropertyByName::InObjectPinName && Pin->Direction == EGPD_Input)
	{
		OnInObjectChanged();
	}
	else if (Pin != NULL && Pin->PinName == FK2_SulleyGetObjectPropertyByName::InPropertyPinName && Pin->Direction == EGPD_Input)
	{
		OnInPropertyChanged();
	}
}

void UK2_SulleyGetObjectPropertyByName::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);

	if (Pin != NULL && Pin->PinName == FK2_SulleyGetObjectPropertyByName::InObjectPinName && Pin->Direction == EGPD_Input)
	{
		OnInObjectChanged();
	}
	else if (Pin != NULL && Pin->PinName == FK2_SulleyGetObjectPropertyByName::InPropertyPinName && Pin->Direction == EGPD_Input)
	{
		OnInPropertyChanged();
	}
}

FText UK2_SulleyGetObjectPropertyByName::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("GetCameraPropertyByName_Title", "SulleyGetObjectPropertyByName");
}

FText UK2_SulleyGetObjectPropertyByName::GetTooltipText() const
{
	return LOCTEXT("GetCameraPropertyByName_Tooltip", "Gets the value of a specified property name of a provided object.");
}

FText UK2_SulleyGetObjectPropertyByName::GetMenuCategory() const
{
	return LOCTEXT("GetCameraPropertyByName_Menu", "SulleyAccessProperty");
}

void UK2_SulleyGetObjectPropertyByName::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();

	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FSlateIcon UK2_SulleyGetObjectPropertyByName::GetIconAndTint(FLinearColor& OutColor) const
{
	static FSlateIcon Icon("EditorStyle", "GraphEditor.OverrideFunction_16x");
	return Icon;
}

void UK2_SulleyGetObjectPropertyByName::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	UEdGraphPin* OldInObjectPin = nullptr;
	UEdGraphPin* OldInPropertyPin = nullptr;

	for (UEdGraphPin* Pin : OldPins)
	{
		if (Pin->GetFName() == FK2_SulleyGetObjectPropertyByName::InObjectPinName)
		{
			OldInObjectPin = Pin;
		}
		if (Pin->GetFName() == FK2_SulleyGetObjectPropertyByName::InPropertyPinName)
		{
			OldInPropertyPin = Pin;
		}
	}

	AllocateDefaultPins();

	UClass* InObjectClass = GetInObjectClass(OldInObjectPin);
	FName InPropertyName = GetInPropertyName(OldInPropertyPin);

	if (!InPropertyName.IsNone())
	{
		CreateOutValuePin(InObjectClass, InPropertyName);
	}
	
	RestoreSplitPins(OldPins);
}

void UK2_SulleyGetObjectPropertyByName::GetNodeAttributes(TArray<TKeyValuePair<FString, FString>>& OutNodeAttributes) const
{
	/** Get InObject's class. */
	UClass* InObjectClass = GetInObjectClass();

	/** Add node attributes. */
	const FString InObjectClassStr = InObjectClass ? InObjectClass->GetName() : TEXT("InvalidClass");
	OutNodeAttributes.Add(TKeyValuePair<FString, FString>(TEXT("Type"), TEXT("GetCameraPropertyByName")));
	OutNodeAttributes.Add(TKeyValuePair<FString, FString>(TEXT("Class"), GetClass()->GetName()));
	OutNodeAttributes.Add(TKeyValuePair<FString, FString>(TEXT("Name"), GetName()));
	OutNodeAttributes.Add(TKeyValuePair<FString, FString>(TEXT("InObjectClass"), InObjectClassStr));
}

class FNodeHandlingFunctor* UK2_SulleyGetObjectPropertyByName::CreateNodeHandler(class FKismetCompilerContext& CompilerContext) const
{
	return new FNodeHandlingFunctor(CompilerContext);
}

UEdGraphPin* UK2_SulleyGetObjectPropertyByName::GetExecPin() const
{
	UEdGraphPin* Pin = FindPinChecked(UEdGraphSchema_K2::PN_Execute);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2_SulleyGetObjectPropertyByName::GetThenPin() const
{
	UEdGraphPin* Pin = FindPinChecked(UEdGraphSchema_K2::PN_Then);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UK2_SulleyGetObjectPropertyByName::GetInObjectPin() const
{
	UEdGraphPin* Pin = FindPin(FK2_SulleyGetObjectPropertyByName::InObjectPinName, EGPD_Input);
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2_SulleyGetObjectPropertyByName::GetInPropertyPin() const
{
	UEdGraphPin* Pin = FindPin(FK2_SulleyGetObjectPropertyByName::InPropertyPinName, EGPD_Input);
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2_SulleyGetObjectPropertyByName::GetOutValidPin() const
{
	UEdGraphPin* Pin = FindPin(FK2_SulleyGetObjectPropertyByName::OutValidPinName, EGPD_Output);
	check(Pin == nullptr || Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UK2_SulleyGetObjectPropertyByName::GetOutValuePin()
{
	for (UEdGraphPin* Pin : Pins)
	{
		if (Pin->Direction == EGPD_Output &&
			Pin->GetName() != "then" &&
			Pin->GetName() != FK2_SulleyGetObjectPropertyByName::OutValidPinName.ToString())
		{
			return Pin;
		}
	}

	return nullptr;
}

UClass* UK2_SulleyGetObjectPropertyByName::GetInObjectClass(const UEdGraphPin* InPin) const
{
	UClass* InObjectClass = nullptr;

	const UEdGraphPin* InObjectPin = InPin;

	if (InObjectPin == nullptr)
	{
		InObjectPin = GetInObjectPin();
	}

	if (InObjectPin == nullptr)
	{
		return nullptr;
	}

	if (InObjectPin->DefaultObject && InObjectPin->LinkedTo.Num() == 0)
	{
		InObjectClass = CastChecked<UClass>(InObjectPin->DefaultObject->StaticClass());
	}
	else if (InObjectPin->LinkedTo.Num())
	{
		UEdGraphPin* ObjectSource = InObjectPin->LinkedTo[0];
		InObjectClass = ObjectSource ? Cast<UClass>(ObjectSource->PinType.PinSubCategoryObject.Get()) : nullptr;
	}

	return InObjectClass;
}

FName UK2_SulleyGetObjectPropertyByName::GetInPropertyName(const UEdGraphPin* InPin)
{
	FName InPropertyName = FName("");
	const UEdGraphPin* InPropertyPin = InPin;
	
	if (InPropertyPin == nullptr)
	{
		InPropertyPin = GetInPropertyPin();
	}

	if (InPropertyPin && InPropertyPin->LinkedTo.Num() == 0)
	{
		InPropertyName = FName(InPropertyPin->DefaultValue);
	}
	else if (InPropertyPin && InPropertyPin->LinkedTo.Num())
	{
		// Does not support external pin.
		UEdGraphPin* InPropertySource = InPropertyPin->LinkedTo[0];
		InPropertyName = InPropertySource ? FName(InPropertySource->DefaultValue) : FName("");
	}

	return InPropertyName;
}

void UK2_SulleyGetObjectPropertyByName::CreateOutValuePin(UClass* InObjectClass, FName InPropertyName)
{
	if (InObjectClass == nullptr)
	{
		return;
	}

	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	FProperty* Property = nullptr;

	for (TFieldIterator<FProperty> PropertyIt(InObjectClass, EFieldIteratorFlags::IncludeSuper); PropertyIt; ++PropertyIt)
	{
		FProperty* CurrentProperty = *PropertyIt;

		if (CurrentProperty->GetFName().ToString().Equals(InPropertyName.ToString()))
		{
			Property = CurrentProperty;
			break;
		}
	}

	if (Property != nullptr)
	{
		FEdGraphPinType PinType;
		K2Schema->ConvertPropertyToPinType(Property, PinType);
		UEdGraphPin* Pin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Boolean, InPropertyName);
		Pin->PinType = PinType;
	}
}

void UK2_SulleyGetObjectPropertyByName::OnInObjectChanged()
{
	UEdGraphPin* ValuePin = GetOutValuePin();

	if (ValuePin)
	{
		ValuePin->BreakAllPinLinks();
	}

	OnInPinChanged();
}

void UK2_SulleyGetObjectPropertyByName::OnInPropertyChanged()
{
	UEdGraphPin* ValuePin = GetOutValuePin();

	if (ValuePin)
	{
		ValuePin->BreakAllPinLinks();
	}

	OnInPinChanged();
}

void UK2_SulleyGetObjectPropertyByName::OnInPinChanged()
{
	Modify();

	TArray<UEdGraphPin*> OldPins;

	for (int i = 0; i < Pins.Num(); ++i)
	{
		UEdGraphPin* Pin = Pins[i];
		
		if (IsOutValuePin(Pin))
		{
			OldPins.Add(Pins[i]);
			/** Remove the old out value pin. */
			Pins.RemoveAt(i);
			/** Only one out value pin. Immediately break once found. */
			break;
		}
	}

	UClass* InObjectClass = GetInObjectClass();
	FName InPropertyName = GetInPropertyName();

	if (!InPropertyName.IsNone())
	{
		/** Create a new out value pin. */
		CreateOutValuePin(InObjectClass, InPropertyName);
	}

	RestoreSplitPins(OldPins);
	RewireOldPinsToNewPins(OldPins, Pins, nullptr);
	GetGraph()->NotifyGraphChanged();

	FBlueprintEditorUtils::MarkBlueprintAsModified(GetBlueprint());
}

bool UK2_SulleyGetObjectPropertyByName::IsOutValuePin(const UEdGraphPin* Pin)
{
	UEdGraphPin* ValuePin = GetOutValuePin();

	if (Pin && ValuePin && Pin == ValuePin)
	{
		return true;
	}

	return false;
}

#undef LOCTEXT_NAMESPACE