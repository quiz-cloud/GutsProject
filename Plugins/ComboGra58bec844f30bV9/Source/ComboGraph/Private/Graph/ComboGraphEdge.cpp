// Copyright 2022 Mickael Daniel. All Rights Reserved.

#include "Graph/ComboGraphEdge.h"

#include "Graph/ComboGraphNodeAnimBase.h"
#include "Net/UnrealNetwork.h"

void UComboGraphEdge::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UComboGraphEdge, CurrentInputActionInstance);
}

bool UComboGraphEdge::IsConfirmed() const
{
	return bConfirmed;
}

void UComboGraphEdge::SetIsConfirmed(const bool bInIsConfirmed)
{
	bConfirmed = bInIsConfirmed;
}

ETriggerEvent UComboGraphEdge::GetEnhancedInputTriggerEvent() const
{
	// Convert out internal enum to the real Input Trigger Event for Enhanced Input
	return TriggerEvent == EComboGraphTransitionInputEvent::Started ? ETriggerEvent::Started
		: TriggerEvent == EComboGraphTransitionInputEvent::Triggered ? ETriggerEvent::Triggered
		: TriggerEvent == EComboGraphTransitionInputEvent::Canceled ? ETriggerEvent::Canceled
		: ETriggerEvent::Triggered;
}

bool UComboGraphEdge::IsUsingCanceledTriggerEvent() const
{
	return GetEnhancedInputTriggerEvent() == ETriggerEvent::Canceled;
}

UComboGraphAbilityTask_StartGraph* UComboGraphEdge::K2_GetOwningTask() const
{
	if (const UComboGraphNodeAnimBase* StartAnimNode = Cast<UComboGraphNodeAnimBase>(StartNode))
	{
		if (UComboGraphAbilityTask_StartGraph* Task = StartAnimNode->K2_GetOwningTask())
		{
			return Task;
		}
	}

	if (const UComboGraphNodeAnimBase* EndAnimNode = Cast<UComboGraphNodeAnimBase>(EndNode))
	{
		return EndAnimNode->K2_GetOwningTask();
	}

	return nullptr;
}

FInputActionInstance UComboGraphEdge::GetCurrentInputActionInstance() const
{
	return CurrentInputActionInstance;
}

void UComboGraphEdge::SetCurrentInputActionInstance(const FInputActionInstance& InCurrentInputActionInstance)
{
	CurrentInputActionInstance = InCurrentInputActionInstance;
}

#if WITH_EDITOR
void UComboGraphEdge::SetNodeTitle(const FText& InTitle)
{
	NodeTitle = InTitle;
}
#endif
