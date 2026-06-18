// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GutsPlayerController.h"

#include"EnhancedInputSubsystems.h"
#include "GutsAbilitySystemComponent.h"
#include "GutsInputComponent.h"
#include "Player/GutsPlayerState.h"

void AGutsPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UGutsInputComponent* GutsInputComponent = CastChecked<UGutsInputComponent>(InputComponent);
	GutsInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGutsPlayerController::Move);
	GutsInputComponent->BindAbilityAction(InputConfig, this, &AGutsPlayerController::OnInputPressed, &AGutsPlayerController::OnInputHeld, &AGutsPlayerController::OnInputReleased);
}

UGutsAbilitySystemComponent* AGutsPlayerController::GetASC()
{
	if (AbilitySystemComponent == nullptr)
	{
		const AGutsPlayerState* GutsPlayerState = GetPlayerState<AGutsPlayerState>();
		UAbilitySystemComponent* ASC = GutsPlayerState->GetAbilitySystemComponent();
		AbilitySystemComponent = Cast<UGutsAbilitySystemComponent>(ASC);
	}
	return AbilitySystemComponent;
}

void AGutsPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UGutsAbilitySystemComponent* GutsASC = GetASC())
	{
		// X6-style input pipeline: collect Enhanced Input callbacks first, then consume them once per frame in ASC.
		GutsASC->ProcessAbilityInput(DeltaTime);
	}
	
	Super::PostProcessInput(DeltaTime, bGamePaused);
}

void AGutsPlayerController::Move(const FInputActionValue& Value)
{
	const FVector2D InputAxisVector = Value.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AGutsPlayerController::Look()
{
}

void AGutsPlayerController::OnInputPressed(FGameplayTag InputTag)
{
	if (!InputTag.IsValid())return;
	
	GetASC()->AbilityInputTagPressed(InputTag);
}

void AGutsPlayerController::OnInputHeld(FGameplayTag InputTag)
{
}

void AGutsPlayerController::OnInputReleased(FGameplayTag InputTag)
{
	if (!InputTag.IsValid())return;
    	
	GetASC()->AbilityInputTagReleased(InputTag);
}

void AGutsPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultInputContext, 0);
	}
}
