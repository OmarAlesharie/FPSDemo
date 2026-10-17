// Copyright Omar Alesharie


#include "Character/ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Data/WeaponData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/Combat/CombatComponent.h"
#include "EnhancedInputComponent.h"
#include "Components/InputComponent.h"


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
	GetMesh()->bCastDynamicShadow = false;

	Combat = CreateDefaultSubobject<UCombatComponent>("Combat");
	Combat->SetIsReplicated(true);

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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
}

void AShooterCharacter::AimWeapon_Released()
{
	if (!Combat) return;
	Combat->Initiate_AimWeapon_Released();
}

