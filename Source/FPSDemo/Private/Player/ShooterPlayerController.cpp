// Copyright Omar Alesharie


#include "Player/ShooterPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "GameFramework/Character.h"

AShooterPlayerController::AShooterPlayerController()
{
	bReplicates = true;
	bPawnAlive = true;
}

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem && ShooterIMC)
	{
		Subsystem->AddMappingContext(ShooterIMC, 0);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ShooterIMC is not set or Subsystem is null."));
	}
}

void AShooterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* ShooterInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	ShooterInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShooterPlayerController::Move);
	ShooterInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShooterPlayerController::Look);
	ShooterInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AShooterPlayerController::Crouch);
	ShooterInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AShooterPlayerController::Jump);
}

void AShooterPlayerController::Move(const FInputActionValue& Value)
{
	if (!bPawnAlive) return;

	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	const FRotator TheControlRotation = GetControlRotation();
	const FRotator YawRotation(0.f, TheControlRotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, LookAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, LookAxisVector.X);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ControlledPawn is null."));
	}
}

void AShooterPlayerController::Look(const FInputActionValue& Value)
{
	if (!bPawnAlive) return;

	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
}

void AShooterPlayerController::Crouch()
{
	if(!IsValid(GetCharacter()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Character is not valid."));
		return;
	}

	if (!bPawnAlive) return;

	if (UCharacterMovementComponent* CMC = GetCharacter()->GetCharacterMovement(); IsValid(CMC))
	{
		CMC->bWantsToCrouch = !CMC->bWantsToCrouch;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterMovementComponent is not valid."));
	}
}

void AShooterPlayerController::Jump()
{
	if (!IsValid(GetCharacter()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Character is not valid."));
		return;
	}
	if (!bPawnAlive) return;

	UCharacterMovementComponent* CMC = GetCharacter()->GetCharacterMovement();
	if (!IsValid(CMC)) 
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterMovementComponent is not valid."));
		return;
	}

	if (CMC->bWantsToCrouch)
	{
		CMC->bWantsToCrouch = false;
	}
	else
	{
		GetCharacter()->Jump();
	}
}