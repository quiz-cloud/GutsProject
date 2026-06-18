// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Interface/IComboGraphAbilitySystemInterface.h"
#include "GutsAbilitySystemComponent.generated.h"

class UGutsComboGraphInputData;
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
class GUTS_API UGutsAbilitySystemComponent : public UAbilitySystemComponent, public IComboGraphAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ProcessAbilityInput(float DeltaTime);

	virtual void UpdateNextComboGraphInputList(const TArray<UObject*>& ComboInputData) override;
	virtual void ClearNextComboGraphInputList() override;
protected:
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
	TArray<FGameplayAbilityInputSpecHandle> InputHeldSpecHandles;
	TArray<FGameplayAbilityInputSpecHandle> InputReleasedSpecHandles;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UGutsComboGraphInputData>> NextComboGraphInputList;

	bool bComboGraphInputPressed = false;
	bool bComboGraphInputReleased = false;
	float ComboGraphInputHeldTimeSeconds = 0.f;
	FGameplayTag ComboGraphPressedTag;
	FGameplayTag ComboGraphReleasedTag;
	
private:
	void ProcessComboGraphInput(float DeltaTime);
	void ComboGraphInputPressed(bool bActivate, const FGameplayTag& InputTag = FGameplayTag());
	void ComboGraphInputReleased(bool bActivate, const FGameplayTag& InputTag = FGameplayTag());
	void ClearComboGraphInput();
	FGameplayAbilityInputSpecHandle FindInputHeldSpecHandle(const FGameplayAbilitySpecHandle& InputSpecHandle);
};
