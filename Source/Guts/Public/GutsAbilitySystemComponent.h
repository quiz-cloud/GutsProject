// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GutsAbilitySystemComponent.generated.h"

USTRUCT()
struct FGameplayAbilityInputSpecHandle
{
	GENERATED_BODY()
	
	UPROPERTY()
	FGameplayAbilitySpecHandle AbilitySpecHandle;
	
	float HeldTimeSeconds = 0.f;
	
	bool operator==(const FGameplayAbilityInputSpecHandle& Other) const
	{
		return AbilitySpecHandle == Other.AbilitySpecHandle;
	}
	
	bool operator!=(const FGameplayAbilityInputSpecHandle& other) const
	{
		return AbilitySpecHandle != other.AbilitySpecHandle;
	}
	
	bool IsValid() const
	{
		return AbilitySpecHandle.IsValid();
	};
};
/**
 * 
 */
UCLASS()
class GUTS_API UGutsAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	void GiveAbilityByClass(TArray<TSubclassOf<UGameplayAbility>> AbilityClasses);
	
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ProcessAbilityInput(float DeltaTime);

protected:
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
	TArray<FGameplayAbilityInputSpecHandle> InputHeldSpecHandles;
	TArray<FGameplayAbilityInputSpecHandle> InputReleasedSpecHandles;

private:
	FGameplayAbilityInputSpecHandle FindInputHeldSpecHandle(const FGameplayAbilitySpecHandle& InputSpecHandle);
};
