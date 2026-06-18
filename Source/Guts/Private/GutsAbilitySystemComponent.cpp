// Fill out your copyright notice in the Description page of Project Settings.


#include "GutsAbilitySystemComponent.h"

#include "Abilities/Tasks/ComboGraphAbilityTask_StartGraph.h"
#include "Combo/GutsComboGraphInputData.h"
#include "Graph/ComboGraphEdge.h"

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

	for (const UGutsComboGraphInputData* InputData : NextComboGraphInputList)
	{
		if (InputData && InputData->InputTag.IsValid() && InputTag.MatchesTag(InputData->InputTag))
		{
			// X6-style ComboGraph input collection: record the semantic tag press in ASC.
			// Edge selection happens later in ProcessAbilityInput, not inside the input callback.
			ComboGraphInputReleased(false);
			ComboGraphInputPressed(true, InputTag);
			break;
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

	for (const UGutsComboGraphInputData* InputData : NextComboGraphInputList)
	{
		if (InputData && InputData->InputTag.IsValid() && InputTag.MatchesTag(InputData->InputTag))
		{
			// Preserve release state for short-press policies; ProcessComboGraphInput will consume it once.
			ComboGraphInputPressed(false);
			ComboGraphInputReleased(true, InputTag);
			break;
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

	ProcessComboGraphInput(DeltaTime);

	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UGutsAbilitySystemComponent::UpdateNextComboGraphInputList(const TArray<UObject*>& ComboInputData)
{
	NextComboGraphInputList.Empty();

	// ComboGraph task pushes only the current node's reachable edge input objects.
	// Duplicate them under ASC so runtime fields like OwnerEdge do not dirty graph assets.
	for (UObject* InputDataObject : ComboInputData)
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

		if (UGutsComboGraphInputData* InputData = Cast<UGutsComboGraphInputData>(InputDataObject))
		{
			FObjectDuplicationParameters Params(InputData, this);
			UGutsComboGraphInputData* RuntimeInputData = Cast<UGutsComboGraphInputData>(StaticDuplicateObjectEx(Params));
			if (RuntimeInputData)
			{
				RuntimeInputData->OwnerEdge = OwnerEdge;
				NextComboGraphInputList.Add(RuntimeInputData);
			}
		}
	}
}

void UGutsAbilitySystemComponent::ClearNextComboGraphInputList()
{
	NextComboGraphInputList.Empty();
	ClearComboGraphInput();
}

void UGutsAbilitySystemComponent::ProcessComboGraphInput(float DeltaTime)
{
	// Guts copy of X6's ComboGraph edge selection: ASC evaluates the input tag,
	// press/release state, and duration policy, then confirms the chosen edge.
	if (NextComboGraphInputList.Num() <= 0)
	{
		ClearComboGraphInput();
		return;
	}

	if (bComboGraphInputPressed)
	{
		ComboGraphInputHeldTimeSeconds += DeltaTime;
	}

	if (!bComboGraphInputPressed && !bComboGraphInputReleased)
	{
		return;
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
		else if (InputData->ActivationPolicy == EGutsComboGraphInputActivationPolicy::OnInputTimeoutMonitor)
		{
			const bool bInputTagMatch = ComboGraphPressedTag.MatchesTag(InputData->InputTag);
			if (bInputTagMatch && ComboGraphInputHeldTimeSeconds > InputData->DurationLimit)
			{
				InputDataToActivate = InputData;
				break;
			}
		}
		else if (InputData->ActivationPolicy == EGutsComboGraphInputActivationPolicy::OnInputReleaseMonitor)
		{
			const bool bReleasedTagMatch = ComboGraphReleasedTag.MatchesTag(InputData->InputTag);
			if (bComboGraphInputReleased && bReleasedTagMatch && ComboGraphInputHeldTimeSeconds <= InputData->DurationLimit)
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
			// Hand the selected edge back to ComboGraph so its existing combo-window and transition rules run.
			if (StartGraphTask->ConfirmComboGraphInput(InputDataToActivate->OwnerEdge.Get()))
			{
				InputPressedSpecHandles.Reset();
				InputReleasedSpecHandles.Reset();
			}
		}
	}

	if (bComboGraphInputReleased)
	{
		ComboGraphInputHeldTimeSeconds = 0.f;
	}

	bComboGraphInputReleased = false;
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

void UGutsAbilitySystemComponent::ClearComboGraphInput()
{
	bComboGraphInputPressed = false;
	bComboGraphInputReleased = false;
	ComboGraphInputHeldTimeSeconds = 0.f;
	ComboGraphPressedTag = FGameplayTag();
	ComboGraphReleasedTag = FGameplayTag();
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
