// Copyright Omar Alesharie


#include "Components/Combat/CombatComponent.h"
#include "Data/WeaponData.h"
#include "Weapon/Weapon.h"
#include "Engine/Engine.h"
#include <Net/UnrealNetwork.h>

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
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
}

void UCombatComponent::Initiate_CycleWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Cycle Weapon"), false);
}

void UCombatComponent::Initiate_FireWeapon_Pressed()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Fire Weapon Pressed"), false);
}

void UCombatComponent::Initiate_FireWeapon_Released()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Fire Weapon Released"), false);
}

void UCombatComponent::Initiate_ReloadWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Reload Weapon"), false);
}

void UCombatComponent::Initiate_AimWeapon_Pressed()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Aim Weapon Pressed"), false);
}

void UCombatComponent::Initiate_AimWeapon_Released()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Aim Weapon Released"), false);
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
		Inventory[0]->AttachToOwningPawn();
	}
}

void UCombatComponent::DestroyInventory()
{
	// Todo: Destroy all weapons in the inventory
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
	SpawnParams.Instigator = OwningActor->GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	return OwningActor->GetWorld()->SpawnActor<AWeapon>(WeaponClass, SpawnParams);
}
