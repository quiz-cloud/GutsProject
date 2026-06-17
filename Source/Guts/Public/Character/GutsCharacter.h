// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Character/GutsCharacterBase.h"
#include "GutsCharacter.generated.h"

class UGutsAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class GUTS_API AGutsCharacter : public AGutsCharacterBase, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UGutsAbilitySystemComponent> AbilitySystemComponent;
	
	void InitAbilityActorInfo();
};
