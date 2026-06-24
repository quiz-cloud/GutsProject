// Fill out your copyright notice in the Description page of Project Settings.


#include "Combo/GutsComboGraphInputData.h"

FGameplayTag UGutsComboGraphInputData::GetInitialInputTag() const
{
	return InitialInputTag.IsValid() ? InitialInputTag : InputTag;
}

bool UGutsComboGraphInputData::CheckCondition(const UGutsAbilitySystemComponent* ASC)
{
	return ASC != nullptr;
}
