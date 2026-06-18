// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "DataAsset/GutsInputConfig.h"
#include "GutsInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class GUTS_API UGutsInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
	
public:
	template<class UserClass, typename PressFuncType, typename ReleaseFuncType, typename HeldFuncType>
	void BindAbilityAction(UGutsInputConfig* InputConfig, UserClass* Object, PressFuncType PressFunc, HeldFuncType HeldFunc, ReleaseFuncType ReleaseFunc )
	{
		for (FGutsInputAction& Action : InputConfig->GutsInputActions)
		{
			if (Action.InputAction && Action.InputTag.IsValid())
			{
				if (PressFunc)
				{
					BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressFunc, Action.InputTag);
				}
				if (HeldFunc)
				{
					BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, Action.InputTag);
				}
				if (ReleaseFunc)
				{
					BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleaseFunc, Action.InputTag);
				}
			}
		}
	};
};
