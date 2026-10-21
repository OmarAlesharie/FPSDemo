// Copyright Omar Alesharie

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "CombatComponent.generated.h"

class UMaterialInstanceDynamic;
class UWeaponData;
class AWeapon;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FReticleChanged, UMaterialInstanceDynamic*, ReticleDynMatInst, const FReticleParams&, ReticleParams, bool, bCurrentlyTargetingPlayer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAmmoCounterChanged, UMaterialInstanceDynamic*, AmmoCounterDynMatInst, int32, RoundsCurrent, int32, RoundsMax);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRoundFired, int32, RoundsCurrent, int32, RoundsMax);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAimingStatusChanged, bool, bIsAiming);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTargetingPlayerStatusChanged, bool, bIsAiming);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPSDEMO_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();

	const TObjectPtr<AWeapon> GetCurrentWeapon();

	void InitializeWeaponWidgets() const;

protected:
	
	UPROPERTY(Transient, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentWeapon)
	TObjectPtr<AWeapon> CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "FPSDemo|Weapon")
	float TraceLength;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category = "FPSDemo|Combat")
	static UCombatComponent* FindCombatComponent(const AActor* Actor) { return (IsValid(Actor) ? Actor->FindComponentByClass<UCombatComponent>() : nullptr); }

	// Cycle to the next weapon in the inventory
	void Initiate_CycleWeapon();
	void Initiate_FireWeapon_Pressed();
	void Initiate_FireWeapon_Released();
	void Initiate_ReloadWeapon();
	void Initiate_AimWeapon_Pressed();
	void Initiate_AimWeapon_Released();

	UPROPERTY(BlueprintAssignable)
	FReticleChanged OnReticleChanged;

	UPROPERTY(BlueprintAssignable)
	FAmmoCounterChanged OnAmmoCounterChanged;

	UPROPERTY(BlueprintAssignable)
	FRoundFired OnRoundFired;

	UPROPERTY(BlueprintAssignable)
	FAimingStatusChanged OnAimingStatusChanged;

	UPROPERTY(BlueprintAssignable)
	FTargetingPlayerStatusChanged OnTargetingPlayerStatusChanged;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPSDemo|Weapon")
	TObjectPtr<UWeaponData> WeaponData;

	void Equip(AWeapon* WeaponToEquip);
	void SpawnInventory();
	void DestroyInventory();

	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bAiming;

private:

	bool bHitPlayer;
	bool bHitPlayerLastFrame;
	bool bTriggerPressed;
	FTimerHandle FireTimer;
	void FireTimerFinished();

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
	void Multicast_FireWeapon(const FHitResult& Hitt, int32 AuthAmmo);

	void Local_Aim(bool bPressed);
	void Local_FireWeapon();
};
