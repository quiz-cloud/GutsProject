// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GutsCharacterBase.h"
#include "GutsCharacter.generated.h"

class UGutsAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class GUTS_API AGutsCharacter : public AGutsCharacterBase
{
	GENERATED_BODY()
	
public:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

private:
	void InitAbilityActorInfo();
};
