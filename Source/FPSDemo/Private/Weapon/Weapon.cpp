// Copyright Omar Alesharie


#include "Weapon/Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Pawn.h"
#include <Interfaces/PlayerInterface.h>
#include <Kismet/KismetMathLibrary.h>
#include "FPSDemo/FPSDemo.h"
#include <KismetTraceUtils.h>
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bNetUseOwnerRelevancy = true;

	WeaponMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh1P");
	WeaponMesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	WeaponMesh1P->bReceivesDecals = false;
	WeaponMesh1P->CastShadow = false;
	WeaponMesh1P->SetHiddenInGame(true);
	SetRootComponent(WeaponMesh1P);

	WeaponMesh3P = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh3P");
	WeaponMesh3P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	WeaponMesh3P->bReceivesDecals = false;
	WeaponMesh3P->CastShadow = true;
	WeaponMesh3P->SetHiddenInGame(true);
	WeaponMesh3P->SetupAttachment(WeaponMesh1P);

	AimFOV = 65.0f;
	TraceRadius = 5.f;

	FireTime = 0.1f;
	MagCapacity = 10;
	Ammo = 5;
	StartingCarriedAmmo = 10;
	Sequence = 0;
	WeaponStatus = EWeaponStatus::Idle;
}

UMaterialInstanceDynamic* AWeapon::GetReticleDynamicMaterialInstance()
{
	if (!IsValid(DynMatInst_Reticle))
	{
		DynMatInst_Reticle = UMaterialInstanceDynamic::Create(ReticleMaterial, this);
	}
	return DynMatInst_Reticle;
}

UMaterialInstanceDynamic* AWeapon::GetAmmoCounterDynamicMaterialInstance()
{
	if (!IsValid(DynMatInst_AmmoCounter))
	{
		DynMatInst_AmmoCounter = UMaterialInstanceDynamic::Create(AmmoCounterMaterial, this);
	}
	return DynMatInst_AmmoCounter;
}

void AWeapon::AttachToOwningPawn(APawn* Pawn) const
{
	if (!IsValid(Pawn) || !Pawn->Implements<UPlayerInterface>())
	{
		return;
	}

	SetMeshVisibility(Pawn);

	const FName AttachPoint = IPlayerInterface::Execute_GetWeaponAttachPoint(Pawn, WeaponType);
	USkeletalMeshComponent* OwningPawnMesh1P = IPlayerInterface::Execute_GetMesh1P(Pawn);
	USkeletalMeshComponent* OwningPawnMesh3P = IPlayerInterface::Execute_GetMesh3P(Pawn);

	WeaponMesh1P->AttachToComponent(OwningPawnMesh1P, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
	WeaponMesh3P->AttachToComponent(OwningPawnMesh3P, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
}

void AWeapon::DetachFromOwningPawn()
{
	WeaponMesh1P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	WeaponMesh1P->SetHiddenInGame(true);

	WeaponMesh3P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	WeaponMesh3P->SetHiddenInGame(true);
}

void AWeapon::WeaponTrace(FHitResult& OutHit, float TraceLength)
{
	FCollisionQueryParams QueryParams;
	QueryParams.bReturnPhysicalMaterial = true;
	QueryParams.AddIgnoredActor(GetOwner());

	FCollisionResponseParams ResponseParams;
	ResponseParams.CollisionResponse.SetAllChannels(ECollisionResponse::ECR_Ignore);
	ResponseParams.CollisionResponse.SetResponse(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	ResponseParams.CollisionResponse.SetResponse(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	ResponseParams.CollisionResponse.SetResponse(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	ResponseParams.CollisionResponse.SetResponse(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Block);

	ensure(GetInstigator());
	if (APlayerController* PC = Cast<APlayerController>(GetInstigator()->GetController()); IsValid(PC))
	{
		FVector EyesWorldLocation;
		FRotator EyesWorldRotation;
		PC->GetActorEyesViewPoint(EyesWorldLocation, EyesWorldRotation);
		const FVector EyesWorldDirection = UKismetMathLibrary::GetForwardVector(EyesWorldRotation);

		const FVector Start = EyesWorldLocation;
		const FVector End = Start + EyesWorldDirection * TraceLength;

		const bool bHit = GetWorld()->SweepSingleByChannel(
			OutHit,
			Start,
			End,
			FQuat::Identity,
			FPSDemoTraceChannels::ECC_Weapon,
			FCollisionShape::MakeSphere(TraceRadius),
			QueryParams,
			ResponseParams);

		if (!bHit)
		{
			OutHit.ImpactPoint = End;
		}
		
		
	}
}

void AWeapon::Local_Fire(const FVector& ImpactPoint, const FVector& ImpactNormal, TEnumAsByte<EPhysicalSurface> ImpactSurfaceType, bool bIsFirstPerson)
{
	// local fire stuff...
	FireEffects(ImpactPoint, ImpactNormal, ImpactSurfaceType, bIsFirstPerson);

	if (GetInstigator()->IsLocallyControlled())
	{
		Ammo = FMath::Clamp(Ammo - 1, 0, MagCapacity);
		++Sequence;
	}
}

void AWeapon::Auth_Fire()
{
	Ammo = FMath::Clamp(Ammo - 1, 0, MagCapacity);
}

void AWeapon::Rep_Fire(int32 AuthAmmo)
{
	if (GetInstigator()->IsLocallyControlled())
	{
		Ammo = AuthAmmo;
		--Sequence;
		Ammo -= Sequence;
	}
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeapon::SetMeshVisibility(APawn* OwningPawn) const
{
	if(OwningPawn->IsLocallyControlled())
	{
		WeaponMesh1P->SetHiddenInGame(false);
		WeaponMesh3P->SetHiddenInGame(true);
	}
	else
	{
		WeaponMesh1P->SetHiddenInGame(true);
		WeaponMesh3P->SetHiddenInGame(false);
	}
}

