// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "GutsPlayerController.generated.h"

class UGutsAbilitySystemComponent;
class UInputMappingContext;
class UInputAction;
class UGutsInputConfig;
struct FInputActionValue;
/**
 * 
 */
UCLASS()
class GUTS_API AGutsPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultInputContext;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LookAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UGutsInputConfig> InputConfig;
	
	UGutsAbilitySystemComponent* GetASC();
	
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
private:
	void Move(const FInputActionValue& Value);
	void Look();
	
	void OnInputPressed(FGameplayTag InputTag);
	void OnInputHeld(FGameplayTag InputTag);
	void OnInputReleased(FGameplayTag InputTag);
	
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UGutsAbilitySystemComponent> AbilitySystemComponent;
	
	virtual void BeginPlay() override;
	
	virtual void SetupInputComponent() override;
};
