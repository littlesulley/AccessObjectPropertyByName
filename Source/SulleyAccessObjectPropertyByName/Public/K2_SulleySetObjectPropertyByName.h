// Sulley. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2_SulleySetObjectPropertyByName.generated.h"


UCLASS()
class SULLEYACCESSOBJECTPROPERTYBYNAME_API UK2_SulleySetObjectPropertyByName : public UK2Node
{
	GENERATED_BODY()

public:
	//~ Begin UObject Interface.
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ End UObject Interface.

	//~ Begin UEdGraphNode Interface.
	virtual void AllocateDefaultPins() override;
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	//~ End UEdGraphNode Interface.

	//~ Begin UK2Node Interface
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual bool IsNodeSafeToIgnore() const override { return true; }
	virtual bool IsNodePure() const override { return false; }
	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
	virtual void GetNodeAttributes(TArray<TKeyValuePair<FString, FString>>& OutNodeAttributes) const override;
	virtual FText GetMenuCategory() const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual class FNodeHandlingFunctor* CreateNodeHandler(class FKismetCompilerContext& CompilerContext) const override;
	//~ End UK2Node Interface

	//~ Begin helper functions to get pins
	UEdGraphPin* GetExecPin() const;
	UEdGraphPin* GetThenPin() const;
	UEdGraphPin* GetInObjectPin() const;
	UEdGraphPin* GetInPropertyPin() const;
	UEdGraphPin* GetOutValidPin() const;
	UEdGraphPin* GetOutValuePin();
	UEdGraphPin* GetInValuePin();
	//~ End helper functions to get pins

protected:
	UClass* GetInObjectClass(const UEdGraphPin* InPin = nullptr) const;
	FName GetInPropertyName(const UEdGraphPin* InPin = nullptr);
	void CreateOutValuePin(UClass* InObjectClass, FName InPropertyName);
	void CreateInValuePin(UClass* InObjectClass, FName InPropertyName);
	void OnInObjectChanged();
	void OnInPropertyChanged();
	void OnInPinChanged();
	bool IsOutValuePin(const UEdGraphPin* Pin);
	bool IsInValuePin(const UEdGraphPin* Pin);
};
