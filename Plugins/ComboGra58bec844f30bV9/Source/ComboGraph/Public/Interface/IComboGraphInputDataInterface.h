// Copyright 2022 Mickael Daniel. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "IComboGraphInputDataInterface.generated.h"

UINTERFACE()
class COMBOGRAPH_API UComboGraphInputDataInterface : public UInterface
{
	GENERATED_BODY()
};

class COMBOGRAPH_API IComboGraphInputDataInterface
{
	GENERATED_BODY()

public:
	// Project-defined edge input data exposes the tag used for initial/conduit selection.
	virtual FGameplayTag GetInitialInputTag() const = 0;

#if WITH_EDITOR
	virtual FString GetActivationPolicyName() const = 0;
	virtual FString GetInputTagName() const = 0;
#endif
};
