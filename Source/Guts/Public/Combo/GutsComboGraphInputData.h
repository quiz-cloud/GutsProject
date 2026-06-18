// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Interface/IComboGraphInputDataInterface.h"
#include "UObject/Object.h"
#include "GutsComboGraphInputData.generated.h"

class UComboGraphEdge;
class UGutsAbilitySystemComponent;

UENUM(BlueprintType)
enum class EGutsComboGraphInputActivationPolicy : uint8
{
	OnInputTriggered,
	OnInputReleaseMonitor,
	OnInputTimeoutMonitor
};

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class GUTS_API UGutsComboGraphInputData : public UObject, public IComboGraphInputDataInterface
{
	GENERATED_BODY()

public:
	// InputTag consumed by ASC when choosing a ComboGraph edge, mirroring X6's ComboInputData_ComboGraph.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (Categories = "Input"))
	FGameplayTag InputTag;

	// Optional tag used by conduit/start selection. Falls back to InputTag when unset.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (Categories = "Input"))
	FGameplayTag InitialInputTag;

	// X6-style policy: trigger immediately, trigger on short release, or trigger after hold timeout.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	EGutsComboGraphInputActivationPolicy ActivationPolicy = EGutsComboGraphInputActivationPolicy::OnInputTriggered;

	// Max short-press duration or hold timeout threshold, depending on ActivationPolicy.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (EditCondition = "ActivationPolicy == EGutsComboGraphInputActivationPolicy::OnInputReleaseMonitor || ActivationPolicy == EGutsComboGraphInputActivationPolicy::OnInputTimeoutMonitor", EditConditionHides))
	float DurationLimit = 0.f;

	// Runtime-only pointer filled when ASC duplicates edge input data from the current ComboGraph node.
	TWeakObjectPtr<UComboGraphEdge> OwnerEdge;

	virtual FGameplayTag GetInitialInputTag() const override;

	bool CheckCondition(const UGutsAbilitySystemComponent* ASC) const;

#if WITH_EDITOR
	virtual FString GetActivationPolicyName() const override;
	virtual FString GetInputTagName() const override;
#endif
};
