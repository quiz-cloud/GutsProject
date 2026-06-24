// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Interface/ComboGraphInputDataInterface.h"
#include "UObject/NoExportTypes.h"
#include "GutsComboGraphInputData.generated.h"

class UGutsAbilitySystemComponent;
class UComboGraphEdge;
/**
 * 
 */
UENUM(BlueprintType)
enum class EGutsComboGraphInputActivationPolicy : uint8
{
	OnInputTriggered,
	OnInputTimeOutMonitor,
	OnInputReleasedMonitor,
};

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class GUTS_API UGutsComboGraphInputData : public UObject, public IComboGraphInputDataInterface
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	FGameplayTag InputTag;
	
	//Do What?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	FGameplayTag InitialInputTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	EGutsComboGraphInputActivationPolicy ActivationPolicy = EGutsComboGraphInputActivationPolicy::OnInputTriggered;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	float DurationLimit = 0.2f;
	
	TWeakObjectPtr<UComboGraphEdge> OwnerEdge;
	
	FGameplayTag GetInitialInputTag() const;

	static bool CheckCondition(const UGutsAbilitySystemComponent* ASC);
};
