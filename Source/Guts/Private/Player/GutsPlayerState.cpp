// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GutsPlayerState.h"

#include "GutsAbilitySystemComponent.h"

AGutsPlayerState::AGutsPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UGutsAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

UAbilitySystemComponent* AGutsPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}



