// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GutsCharacterBase.h"

#include "GutsAbilitySystemComponent.h"


UAbilitySystemComponent* AGutsCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AGutsCharacterBase::AddStartAbilities() const
{
	if (!HasAuthority()) return;
	
	UGutsAbilitySystemComponent* GutsASC = Cast<UGutsAbilitySystemComponent>(AbilitySystemComponent);
	GutsASC->GiveAbilityByClass(StartAbilities);
}
