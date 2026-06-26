// Fill out your copyright notice in the Description page of Project Settings.


#include "GutsAbilitySystemComponent.h"

#include "Abilities/Tasks/ComboGraphAbilityTask_StartGraph.h"
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

void UGutsAbilitySystemComponent::ComboGraphInputPressedCached(bool bActivate, const FGameplayTag& InputTag)
{
	bComboGraphInputPressedCached = bActivate;
	ComboGraphPressedTag = InputTag;
}

void UGutsAbilitySystemComponent::ComboGraphInputReleasedCached(const FGameplayTag& InputTag)
{
	bComboGraphInputReleasedCached = true;
	ComboGraphReleasedTag = InputTag;
}

void UGutsAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	if (bCanDoComboGraphInputCached)
	{
		bComboGraphInputPressedCached = true;
	}
	else if (NextComboGraphInputList.Num() > 0)
	{
		for (UGutsComboGraphInputData* InputData : NextComboGraphInputList)
		{
			if (InputTag.MatchesTag(InputData->InputTag))
			{
				ComboGraphInputPressed(true, InputTag);
			}
		}
	}
	else
	{
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
	}

}

void UGutsAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	if (bCanDoComboGraphInputCached)
	{
		bComboGraphInputReleasedCached = true;
	}
	else if (NextComboGraphInputList.Num() > 0)
	{
		for (UGutsComboGraphInputData* InputData : NextComboGraphInputList)
		{
			if (InputTag.MatchesTag(InputData->InputTag))
			{
				ComboGraphInputPressed(false);
				ComboGraphInputReleased(true, InputTag);
			}
		}
	}
	else
	{
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
	}
}

void UGutsAbilitySystemComponent::ProcessComboGraphInput(float DeltaTime)
{
	if (NextComboGraphInputList.Num() <= 0)
	{
		ClearComboGraphInput();
		return ;
	}
	
	if (bComboGraphInputPressed)
	{
		ComboGraphInputHeldTimeSeconds += DeltaTime;
	}
	
	if (!bComboGraphInputPressed && !bComboGraphInputReleased)
	{
		return ;
	}
	
	UGutsComboGraphInputData* InputDataToActivate = nullptr;
	for (UGutsComboGraphInputData* InputData : NextComboGraphInputList)
	{
		if (!InputData || !InputData->InputTag.IsValid() || !InputData->CheckCondition(this))
		{
			continue;
		}
		
		if (InputData->ActivationPolicy == EGutsComboGraphInputActivationPolicy::OnInputTriggered)
		{
			if (bComboGraphInputPressed && ComboGraphPressedTag.MatchesTag(InputData->InputTag))
			{
				InputDataToActivate = InputData;
				break;
			}
		}
		else if (InputData->ActivationPolicy == EGutsComboGraphInputActivationPolicy::OnInputReleasedMonitor)
		{
			if (bComboGraphInputReleased && ComboGraphReleasedTag.MatchesTag(InputData->InputTag) &&ComboGraphInputHeldTimeSeconds <= InputData->DurationLimit)
			{
				InputDataToActivate = InputData;
				break;
			}
		}
		else if (InputData->ActivationPolicy == EGutsComboGraphInputActivationPolicy::OnInputTimeOutMonitor)
		{
			if (bComboGraphInputPressed && ComboGraphInputHeldTimeSeconds > InputData->DurationLimit && ComboGraphPressedTag.MatchesTag(InputData->InputTag))
			{
				InputDataToActivate = InputData;
				break;
			}
		}
	}
	
	if (InputDataToActivate && InputDataToActivate->OwnerEdge.IsValid())
	{
		if (UComboGraphAbilityTask_StartGraph* StartGraphTask = InputDataToActivate->OwnerEdge->K2_GetOwningTask())
		{
			if (StartGraphTask->ConfirmComboGraphInput(InputDataToActivate->OwnerEdge.Get()))
			{
				InputPressedSpecHandles.Reset();
				InputReleasedSpecHandles.Reset();
			}
		}
	}
	
	if (bComboGraphInputReleased)
	{
		ComboGraphInputHeldTimeSeconds = 0.0f;
	}
	bComboGraphInputReleased = false;
}

void UGutsAbilitySystemComponent::ClearComboGraphInput()
{
	bComboGraphInputPressed = false;
	bComboGraphInputReleased = false;
	ComboGraphInputHeldTimeSeconds = 0.0f;
	ComboGraphPressedTag = FGameplayTag();
	ComboGraphReleasedTag = FGameplayTag();
}

void UGutsAbilitySystemComponent::ProcessAbilityInput(float DeltaTime)
{
	for (FGameplayAbilityInputSpecHandle& InputHeldSpecHandle : InputHeldSpecHandles)
	{
		InputHeldSpecHandle.HeldTimeSeconds += DeltaTime;
	}

	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();
	
	ProcessComboGraphInput(DeltaTime);
	
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
			if (GutsAbility->ActivationPolicy == EGutsAbilityActivationPolicy::OnInputTimeOut && InputSpecHandle.HeldTimeSeconds > GutsAbility->ActivationTimeOutPeriod)
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
			if (GutsAbility->ActivationPolicy == EGutsAbilityActivationPolicy::OnInputReleased && InputSpecHandle.HeldTimeSeconds <= GutsAbility->ActivationTimeOutPeriod)
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
	
	ClearComboGraphInput();
}

