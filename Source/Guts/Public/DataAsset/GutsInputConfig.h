// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "GutsInputConfig.generated.h"

class UInputAction;
/**
 * 
 */
USTRUCT(BlueprintType)
struct FGutsInputAction
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	UInputAction* InputAction = nullptr;
	
	UPROPERTY(EditAnywhere)
	FGameplayTag InputTag = FGameplayTag();
};

UCLASS()
class GUTS_API UGutsInputConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TArray<FGutsInputAction> GutsInputActions;
};
