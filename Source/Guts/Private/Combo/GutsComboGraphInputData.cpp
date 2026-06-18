// Fill out your copyright notice in the Description page of Project Settings.

#include "Combo/GutsComboGraphInputData.h"

FGameplayTag UGutsComboGraphInputData::GetInitialInputTag() const
{
	return InitialInputTag.IsValid() ? InitialInputTag : InputTag;
}

bool UGutsComboGraphInputData::CheckCondition(const UGutsAbilitySystemComponent* ASC) const
{
	return ASC != nullptr;
}

#if WITH_EDITOR
FString UGutsComboGraphInputData::GetActivationPolicyName() const
{
	return UEnum::GetValueAsString(ActivationPolicy);
}

FString UGutsComboGraphInputData::GetInputTagName() const
{
	return InputTag.ToString();
}
#endif
