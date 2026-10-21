// Copyright Omar Alesharie


#include "Components/Combat/CombatComponent.h"
#include "TimerManager.h"
#include "Data/WeaponData.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Interfaces/PlayerInterface.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Weapon/Weapon.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include <Net/UnrealNetwork.h>

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	TraceLength = 30'000.0f;
	bAiming = false;
	bTriggerPressed = false;
}

const TObjectPtr<AWeapon> UCombatComponent::GetCurrentWeapon()
{
	return CurrentWeapon;
}

void UCombatComponent::InitializeWeaponWidgets() const
{
	if (IsValid(CurrentWeapon))
	{
		OnReticleChanged.Broadcast(CurrentWeapon->GetReticleDynamicMaterialInstance());
		OnAmmoCounterChanged.Broadcast(CurrentWeapon->GetAmmoCounterDynamicMaterialInstance(), CurrentWeapon->Ammo, CurrentWeapon->MagCapacity);
	}
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, Inventory);
	DOREPLIFETIME(UCombatComponent, CurrentWeapon);
	DOREPLIFETIME_CONDITION(UCombatComponent, bAiming, ELifetimeCondition::COND_SkipOwner);

}

void UCombatComponent::Initiate_CycleWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Cycle Weapon"), false);
}

void UCombatComponent::Initiate_FireWeapon_Pressed()
{
	if (!IsValid(CurrentWeapon)) return;
	bTriggerPressed = true;
	
	if (CurrentWeapon->Ammo > 0)
	{
		Local_FireWeapon();
	}
	else
	{
		CurrentWeapon->OutOfAmmoFireEffect();
	}
}

void UCombatComponent::Local_FireWeapon()
{
	if (!IsValid(CurrentWeapon))
	{
		return;
	}

	ensure(IsValid(WeaponData));

	UAnimMontage* Montage1P = WeaponData->FirstPersonMontages.FindChecked(CurrentWeapon->WeaponType).FireMontage;
	USkeletalMeshComponent* Mesh1P = IPlayerInterface::Execute_GetMesh1P(GetOwner());
	if (IsValid(Montage1P) && IsValid(Mesh1P))
	{
		Mesh1P->GetAnimInstance()->Montage_Play(Montage1P);
	}

	FHitResult Hit;
	CurrentWeapon->WeaponTrace(Hit, TraceLength);

	EPhysicalSurface ImpactSurfaceType = Hit.PhysMaterial.IsValid(false) ? Hit.PhysMaterial->SurfaceType.GetValue() : SurfaceType1;
	CurrentWeapon->Local_Fire(Hit.ImpactPoint, Hit.ImpactNormal, ImpactSurfaceType, true);

	GetWorld()->GetTimerManager().SetTimer(FireTimer, this, &ThisClass::FireTimerFinished, CurrentWeapon->FireTime);
	Server_FireWeapon(Hit);
}

void UCombatComponent::FireTimerFinished()
{
	if (!IsValid(CurrentWeapon)) return;

	if (bTriggerPressed && CurrentWeapon->FireType == EFireType::Auto && CurrentWeapon->Ammo > 0)
	{
		Local_FireWeapon();
	}
}


void UCombatComponent::Server_FireWeapon_Implementation(const FHitResult& Hit)
{
	if (!IsValid(CurrentWeapon)) return;
	if (GetNetMode() != NM_ListenServer || !Cast<APawn>(GetOwner())->IsLocallyControlled())
	{
		CurrentWeapon->Auth_Fire();
	}

	Multicast_FireWeapon(Hit, CurrentWeapon->Ammo);
}

void UCombatComponent::Multicast_FireWeapon_Implementation(const FHitResult& Hit, int32 AuthAmmo)
{
		APawn* OwningPawn = Cast<APawn>(GetOwner());
		if (OwningPawn->IsLocallyControlled())
		{
			// do locally-controlled stuff.
			CurrentWeapon->Rep_Fire(AuthAmmo);
		}
		else
		{
			ensure(IsValid(WeaponData));

			EPhysicalSurface ImpactSurfaceType = Hit.PhysMaterial.IsValid(false) ? Hit.PhysMaterial->SurfaceType.GetValue() : SurfaceType1;
			CurrentWeapon->Local_Fire(Hit.ImpactPoint, Hit.ImpactNormal, ImpactSurfaceType, false);

			UAnimMontage* Montage3P = WeaponData->ThirdPersonMontages.FindChecked(CurrentWeapon->WeaponType).FireMontage;
			USkeletalMeshComponent* Mesh3P = IPlayerInterface::Execute_GetMesh3P(GetOwner());
			if (IsValid(Montage3P) && IsValid(Mesh3P))
			{
				Mesh3P->GetAnimInstance()->Montage_Play(Montage3P);
			}
		}
}

void UCombatComponent::Initiate_FireWeapon_Released()
{
	bTriggerPressed = false;
}

void UCombatComponent::Initiate_ReloadWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Reload Weapon"), false);
}

void UCombatComponent::Initiate_AimWeapon_Pressed()
{
	Local_Aim(true);
	Server_Aim(true);
}

void UCombatComponent::Initiate_AimWeapon_Released()
{
	Local_Aim(false);
	Server_Aim(false);
}

void UCombatComponent::Server_Aim_Implementation(bool bPressed)
{
	Local_Aim(bPressed);
}

void UCombatComponent::Local_Aim(bool bPressed)
{
	bAiming = bPressed;
}



void UCombatComponent::Equip(AWeapon* WeaponToEquip)
{
	CurrentWeapon = WeaponToEquip;
	CurrentWeapon->AttachToOwningPawn();
}

void UCombatComponent::SpawnInventory()
{
	if (GetOwner()->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	for (TSubclassOf<AWeapon> WeaponClass : DefaultWeaponClasses)
	{
		AWeapon* NewWeapon = SpawnWeapon(WeaponClass);
		if (NewWeapon)
		{
			Inventory.AddUnique(NewWeapon);
		}
	}

	if (Inventory.Num() > 0)
	{
		Equip(Inventory[0]);
		InitializeWeaponWidgets();
	}
}

void UCombatComponent::DestroyInventory()
{
	for (AWeapon* Weapon : Inventory)
	{
		if (IsValid(Weapon))
		{
			Weapon->Destroy();
		}
	}
	Inventory.Empty();
}

void UCombatComponent::OnRep_CurrentWeapon(AWeapon* PreviousWeapon)
{
	if (!IsValid(CurrentWeapon))
	{
		return;
	}

	CurrentWeapon->AttachToOwningPawn();
	IPlayerInterface::Execute_WeaponReplicated(GetOwner());
	InitializeWeaponWidgets();
}

AWeapon* UCombatComponent::SpawnWeapon(TSubclassOf<AWeapon> WeaponClass) const
{
	AActor* OwningActor = GetOwner();
	if(!IsValid(OwningActor))
	{
		return nullptr;
	}

	if (OwningActor->GetLocalRole() != ROLE_Authority)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwningActor;
	SpawnParams.Instigator = Cast<APawn>(OwningActor);
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	return GetWorld()->SpawnActor<AWeapon>(WeaponClass, SpawnParams);
}







