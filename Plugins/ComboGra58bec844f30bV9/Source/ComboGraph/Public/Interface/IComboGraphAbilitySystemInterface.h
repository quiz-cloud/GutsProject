// Copyright 2022 Mickael Daniel. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IComboGraphAbilitySystemInterface.generated.h"

UINTERFACE()
class COMBOGRAPH_API UComboGraphAbilitySystemInterface : public UInterface
{
	GENERATED_BODY()
};

class COMBOGRAPH_API IComboGraphAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// ComboGraph calls this when the active node changes, giving ASC the candidate edge input data.
	virtual void UpdateNextComboGraphInputList(const TArray<UObject*>& ComboInputData) = 0;

	// ComboGraph calls this when the active node stops listening, preventing stale edge selection.
	virtual void ClearNextComboGraphInputList() = 0;
};
