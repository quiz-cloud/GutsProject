// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GutsCharacter.h"
#include "GutsAbilitySystemComponent.h"
#include "AbilitySystemComponent.h"
#include "Player/GutsPlayerState.h"

//Serve
void AGutsCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	InitAbilityActorInfo();
}

//Client
void AGutsCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	if (GetPlayerState<AGutsPlayerState>())
	{
		InitAbilityActorInfo();
	}
}

void AGutsCharacter::InitAbilityActorInfo()
{
	AGutsPlayerState* GutsPlayerState = GetPlayerState<AGutsPlayerState>();
	UAbilitySystemComponent* ASC = GutsPlayerState->GetAbilitySystemComponent();
	ASC->InitAbilityActorInfo(GutsPlayerState, this);
	AbilitySystemComponent = Cast<UGutsAbilitySystemComponent>(ASC);
}

UGutsAbilitySystemComponent* AGutsCharacter::GetASC()
{
	if (AbilitySystemComponent == nullptr)
	{
		const AGutsPlayerState* GutsPlayerState = GetPlayerState<AGutsPlayerState>();
		UAbilitySystemComponent* ASC = GutsPlayerState->GetAbilitySystemComponent();
		AbilitySystemComponent = Cast<UGutsAbilitySystemComponent>(ASC);
	}
	return AbilitySystemComponent;
}
