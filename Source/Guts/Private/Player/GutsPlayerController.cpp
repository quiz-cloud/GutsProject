// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GutsPlayerController.h"

#include "GutsInputComponent.h"
#include"EnhancedInputSubsystems.h"

void AGutsPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UGutsInputComponent* GutsInputComponent = CastChecked<UGutsInputComponent>(InputComponent);
	GutsInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGutsPlayerController::Move);
}

void AGutsPlayerController::Move(const FInputActionValue& Value)
{
	FVector2D InputAxisVector = Value.Get<FVector2D>(); 
	FRotator Rotation = GetControlRotation();
	FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
	
	FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AGutsPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(DefaultInputContext, 0);
	}
}
