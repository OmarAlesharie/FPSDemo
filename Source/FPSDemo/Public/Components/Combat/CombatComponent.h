// Copyright Omar Alesharie

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class UWeaponData;
class AWeapon;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPSDEMO_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();

	const TObjectPtr<AWeapon> GetCurrentWeapon();

protected:
	
	UPROPERTY(Transient, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentWeapon)
	TObjectPtr<AWeapon> CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "FPSDemo|Weapon")
	float TraceLength;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Cycle to the next weapon in the inventory
	void Initiate_CycleWeapon();
	void Initiate_FireWeapon_Pressed();
	void Initiate_FireWeapon_Released();
	void Initiate_ReloadWeapon();
	void Initiate_AimWeapon_Pressed();
	void Initiate_AimWeapon_Released();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPSDemo|Weapon")
	TObjectPtr<UWeaponData> WeaponData;

	void Equip(AWeapon* WeaponToEquip);
	void SpawnInventory();
	void DestroyInventory();

	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bAiming;

private:

	UFUNCTION()
	void OnRep_CurrentWeapon(AWeapon* PreviousWeapon);

	UPROPERTY(Transient, Replicated)
	TArray<TObjectPtr<AWeapon>> Inventory;

	UPROPERTY(EditDefaultsOnly, Category = "FPSDemo|Weapon")
	TArray<TSubclassOf<AWeapon>> DefaultWeaponClasses;

	AWeapon* SpawnWeapon(TSubclassOf<AWeapon> WeaponClass) const;

	UFUNCTION(Server, Reliable)
	void Server_Aim(bool bPressed);

	UFUNCTION(Server, Reliable)
	void Server_FireWeapon(const FHitResult& Hit);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_FireWeapon(const FHitResult& Hit);

	void Local_Aim(bool bPressed);
	void Local_FireWeapon();
};
