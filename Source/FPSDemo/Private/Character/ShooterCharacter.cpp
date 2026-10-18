// Copyright Omar Alesharie


#include "Character/ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Data/WeaponData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/Combat/CombatComponent.h"
#include "EnhancedInputComponent.h"
#include "Components/InputComponent.h"
#include "Weapon/Weapon.h"


// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->MovementState.bCanCrouch = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 0.0f;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 15.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>("FirstPersonCamera");
	FirstPersonCameraComponent->SetupAttachment(CameraBoom);
	FirstPersonCameraComponent->bUsePawnControlRotation = false;

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>("CharacterMesh1P");
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bOnlyOwnerSee = true;
	Mesh1P->bOwnerNoSee = false;
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->bReceivesDecals = false;
	
	// OnlyTickPoseWhenRendered is a good option for optimizing performance, especially for first-person meshes that are not always visible to the player.
	Mesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;

	// Set the tick group for the Mesh1P component to TG_PrePhysics, which means it will tick before physics simulation starts.
	Mesh1P->PrimaryComponentTick.TickGroup = ETickingGroup::TG_PrePhysics;

	GetMesh()->bOnlyOwnerSee = false;
	GetMesh()->bOwnerNoSee = true;
	GetMesh()->bReceivesDecals = false;

	Combat = CreateDefaultSubobject<UCombatComponent>("Combat");
	Combat->SetIsReplicated(true);

	DefaultFOV = 90.0f;
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	FirstPersonCameraComponent->SetFieldOfView(DefaultFOV);
}

void AShooterCharacter::BeginDestroy()
{
	Super::BeginDestroy();

	if (IsValid(Combat))
	{
		Combat->DestroyInventory();
	}
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CalculateTurnInPlaceParameters();
	CalculateFABRIKSocketTransform();
}

void AShooterCharacter::CalculateTurnInPlaceParameters()
{
	// Get velocity, see if it's zero
	// See if we are falling

	// if standing still and not jumping
		// get current aim rotation
		// get delta aim rotation - the difference in rotation of my current aim rotation from the initial aim rotation
		// (initial aim rotation is calculated in BeginPlay)
		// Store the Yaw of the delta aim rotation (AO_Yaw)
		// if TurningStatus == NotTurning
			// Set InterpAO_Yaw to AO_Yaw
		// TurnInPlace() - interpolates the InterpAO_Yaw value to zero.

	// if running or jumping
		// reset initial aim rotation to the current actual aim rotation
		// AO_Yaw = 0
		// We also need a Movement Offset Yaw to feed to our strafing blendspaces
		// Get Base Aim Rotation
		// Get our Movement Rotation - this is the rotation of our Velocity
		// Movement Offset Yaw = the delta between our movement rotation and our aim rotation
		// TurningStatus = NotTurning

}

// Turn In Place
	// if AO_Yaw > 90
		// TurningStatus = Right
	// else if AO_Yaw < -90
		// TurningStatus = Left
	// if TurningStatus != NotTurning (in other words, we are turning left or right)
		// Interpolate InterpAO_Yaw down to zero.
		// AO_Yaw = InterpAO_Yaw
		// if Abs(AO_Yaw) < 5.f
			// TurningStatus = NotTurning
			// reset initial aim rotation to our actual aim rotation

void AShooterCharacter::CalculateFABRIKSocketTransform()
{
	if (IsValid(Combat) && IsValid(Combat->GetCurrentWeapon()) && IsValid(Combat->GetCurrentWeapon()->GetWeaponMesh3P()))
	{
		FABRIK_SocketTransform = Combat->GetCurrentWeapon()->GetWeaponMesh3P()->GetSocketTransform("FABRIK_Socket", ERelativeTransformSpace::RTS_World);

		FVector OutLocation;
		FRotator OutRotator;
		GetMesh()->TransformToBoneSpace("hand_r", FABRIK_SocketTransform.GetLocation(), FABRIK_SocketTransform.GetRotation().Rotator(), OutLocation, OutRotator);

		FABRIK_SocketTransform.SetLocation(OutLocation);
		FABRIK_SocketTransform.SetRotation(OutRotator.Quaternion());
	}
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* ShooterInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	ShooterInputComponent->BindAction(CycleWeaponAction, ETriggerEvent::Started, this, &AShooterCharacter::CycleWeapon);
	ShooterInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Started, this, &AShooterCharacter::FireWeapon_Pressed);
	ShooterInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Completed, this, &AShooterCharacter::FireWeapon_Released);
	ShooterInputComponent->BindAction(ReloadWeaponAction, ETriggerEvent::Started, this, &AShooterCharacter::ReloadWeapon);
	ShooterInputComponent->BindAction(AimWeaponAction, ETriggerEvent::Started, this, &AShooterCharacter::AimWeapon_Pressed);
	ShooterInputComponent->BindAction(AimWeaponAction, ETriggerEvent::Completed, this, &AShooterCharacter::AimWeapon_Released);
}

FName AShooterCharacter::GetWeaponAttachPoint_Implementation(const FGameplayTag& WeaponType) const
{
	checkf(Combat->WeaponData, TEXT("Combat->WeaponData is null. Make sure to set the WeaponData in the CombatComponent."));
	return Combat->WeaponData->GripPoints.FindChecked(WeaponType);
}

USkeletalMeshComponent* AShooterCharacter::GetMesh1P_Implementation() const
{
	return Mesh1P;
}

USkeletalMeshComponent* AShooterCharacter::GetMesh3P_Implementation() const
{
	return GetMesh();
}

FRotator AShooterCharacter::GetFixedAimRotation() const
{
	FRotator AimRotation = GetBaseAimRotation();
	
	if (AimRotation.Pitch > 90.f && !IsLocallyControlled())
	{
		// map pitch from [270, 360) to [-90,0]
		const FVector2D InRange(270.f, 360.f);
		const FVector2D OutRange(-90.f, 0.f);
		AimRotation.Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AimRotation.Pitch);
	}

	return AimRotation;
}

bool AShooterCharacter::HasCurrentWeapon() const
{
	return IsValid(Combat) && Combat->GetCurrentWeapon() != nullptr;
}

void AShooterCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (IsValid(Combat))
	{
		Combat->SpawnInventory();
	}
}

void AShooterCharacter::CycleWeapon()
{
	if (!Combat) return;
	Combat->Initiate_CycleWeapon();
}

void AShooterCharacter::FireWeapon_Pressed()
{
	if (!Combat) return;
	Combat->Initiate_FireWeapon_Pressed();
}

void AShooterCharacter::FireWeapon_Released()
{
	if (!Combat) return;
	Combat->Initiate_FireWeapon_Released();
}

void AShooterCharacter::ReloadWeapon()
{
	if (!Combat) return;
	Combat->Initiate_ReloadWeapon();
}

void AShooterCharacter::AimWeapon_Pressed()
{
	if (!Combat) return;
	Combat->Initiate_AimWeapon_Pressed();
	OnAim(true);
}

void AShooterCharacter::AimWeapon_Released()
{
	if (!Combat) return;
	Combat->Initiate_AimWeapon_Released();
	OnAim(false);
}

