// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GutsGameplayAbility.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EGutsAbilityActivationPolicy : uint8
{
	OnInputTriggered,
	OnInputReleased,
	OnInputTimeOut,
};

UCLASS()
class GUTS_API UGutsGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category="Guts|Ability Activation")
	FGameplayTag StartUpInputTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guts|Ability Activation")
	EGutsAbilityActivationPolicy ActivationPolicy;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guts|Ability Activation")
	float ActivationTimeOutPeriod;
};
