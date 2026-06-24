// Fill out your copyright notice in the Description page of Project Settings.


#include "GutsAbilitySystemComponent.h"

#include "Combo/GutsComboGraphInputData.h"
#include "GameplayAbilitySystem/GutsGameplayAbility.h"
#include "Graph/ComboGraphEdge.h"

void UGutsAbilitySystemComponent::GiveAbilityByClass(TArray<TSubclassOf<UGameplayAbility>> AbilityClasses)
{
	for (TSubclassOf<UGameplayAbility> AbilityClass : AbilityClasses)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass);
		if (const UGutsGameplayAbility* Ability = Cast<UGutsGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(Ability->StartUpInputTag);
			GiveAbility(AbilitySpec);
		}
	}
}

FGameplayAbilityInputSpecHandle UGutsAbilitySystemComponent::FindInputHeldSpecHandle(const FGameplayAbilitySpecHandle& InputSpecHandle)
{
	for (FGameplayAbilityInputSpecHandle& Handle : InputHeldSpecHandles)
	{
		if (InputSpecHandle == Handle.AbilitySpecHandle)
		{
			return Handle;
		}
	}
	return FGameplayAbilityInputSpecHandle();
}

void UGutsAbilitySystemComponent::ComboGraphInputPressed(bool bActivate, const FGameplayTag& InputTag)
{
	bComboGraphInputPressed = bActivate;
	ComboGraphPressedTag = InputTag;
}

void UGutsAbilitySystemComponent::ComboGraphInputReleased(bool bActivate, const FGameplayTag& InputTag)
{
	bComboGraphInputReleased = bActivate;
	ComboGraphReleasedTag = InputTag;
}

void UGutsAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (AbilitySpec.Ability && AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			FGameplayAbilityInputSpecHandle InputHandle;
			InputHandle.AbilitySpecHandle = AbilitySpec.Handle;
			
			InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
			InputHeldSpecHandles.AddUnique(InputHandle);
		}
	}

	if (NextComboGraphInputList.Num() > 0)
	{
		for (UGutsComboGraphInputData* InputData : NextComboGraphInputList)
		{
			if (InputTag.MatchesTagExact(InputData->InputTag))
			{
				ComboGraphInputPressed(true, InputTag);
			}
		}
	}

}

void UGutsAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (AbilitySpec.Ability && AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			FGameplayAbilityInputSpecHandle InputHandle =FindInputHeldSpecHandle(AbilitySpec.Handle);
			if (InputHandle.IsValid())
			{
				InputReleasedSpecHandles.AddUnique(InputHandle);
				InputHeldSpecHandles.Remove(InputHandle);
			}
		}
	}
	
	if (NextComboGraphInputList.Num() > 0)
	{
		for (UGutsComboGraphInputData* InputData : NextComboGraphInputList)
		{
			if (InputTag.MatchesTagExact(InputData->InputTag))
			{
				ComboGraphInputPressed(false);
				ComboGraphInputReleased(true, InputTag);
			}
		}
	}
}

void UGutsAbilitySystemComponent::ProcessAbilityInput(float DeltaTime)
{
	// Centralized input consumption point, matching X6's PlayerController -> ASC ProcessAbilityInput flow.
	for (FGameplayAbilityInputSpecHandle& InputHeldSpecHandle : InputHeldSpecHandles)
	{
		InputHeldSpecHandle.HeldTimeSeconds += DeltaTime;
	}

	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();
	
	for (FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			const UGutsGameplayAbility* GutsAbility = Cast<UGutsGameplayAbility>(AbilitySpec->Ability); 
			if (GutsAbility->ActivationPolicy == EGutsAbilityActivationPolicy::OnInputTriggered)
			{
				AbilitiesToActivate.AddUnique(SpecHandle);
			}
		}
	}
	
	for (FGameplayAbilityInputSpecHandle& InputSpecHandle : InputHeldSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(InputSpecHandle.AbilitySpecHandle))
		{
			const UGutsGameplayAbility* GutsAbility = Cast<UGutsGameplayAbility>(AbilitySpec->Ability); 
			if (GutsAbility->ActivationPolicy == EGutsAbilityActivationPolicy::OnInputTimeOut && InputSpecHandle.HeldTimeSeconds >= GutsAbility->ActivationTimeOutPeriod)
			{
				AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
			}
		}
	}
	
	for (FGameplayAbilityInputSpecHandle& InputSpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(InputSpecHandle.AbilitySpecHandle))
		{
			if (AbilitySpec->IsActive())
			{
				AbilitySpecInputReleased(*AbilitySpec);
			}
			
			const UGutsGameplayAbility* GutsAbility = Cast<UGutsGameplayAbility>(AbilitySpec->Ability); 
			if (GutsAbility->ActivationPolicy == EGutsAbilityActivationPolicy::OnInputReleased && InputSpecHandle.HeldTimeSeconds < GutsAbility->ActivationTimeOutPeriod)
			{
				AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
			}
		}
	}
	
	for (const FGameplayAbilitySpecHandle& Handle : AbilitiesToActivate)
	{
		TryActivateAbility(Handle);
	}
	
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UGutsAbilitySystemComponent::UpdateNextComboGraphInputList(const TArray<UObject*>& ComboInputDataList)
{
	NextComboGraphInputList.Reset();
	
	for (UObject* InputDataObject : ComboInputDataList)
	{
		if (!InputDataObject)
		{
			continue;
		}
		
		UComboGraphEdge* OwnerEdge = Cast<UComboGraphEdge>(InputDataObject->GetOuter());
		if (!OwnerEdge)
		{
			continue;
		}
		
		UGutsComboGraphInputData* InputData = Cast<UGutsComboGraphInputData>(InputDataObject);
		if (!InputData)
		{
			continue;
		}
		
		FObjectDuplicationParameters Params(InputData, this);
		UGutsComboGraphInputData* RuntimeInputData = Cast<UGutsComboGraphInputData>(StaticDuplicateObjectEx(Params));
		
		if (!RuntimeInputData)
		{
			continue;
		}
		
		RuntimeInputData->OwnerEdge = OwnerEdge;
		NextComboGraphInputList.Add(RuntimeInputData);
	}
}

void UGutsAbilitySystemComponent::ClearNextComboGraphInputList()
{
	NextComboGraphInputList.Reset();
	
	bComboGraphInputPressed = false;
	bComboGraphInputReleased = false;
	ComboGraphInputHeldTimeSeconds = 0.0f;
	ComboGraphPressedTag = FGameplayTag();
	ComboGraphReleasedTag = FGameplayTag();
}

