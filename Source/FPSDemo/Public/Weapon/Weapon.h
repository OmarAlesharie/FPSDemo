// Copyright Omar Alesharie

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Weapon.generated.h"

enum EPhysicalSurface : int;
class USkeletalMeshComponent;

UCLASS()
class FPSDEMO_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	virtual void OnRep_Instigator() override;

	USkeletalMeshComponent* GetWeaponMesh1P() const { return WeaponMesh1P; }
	USkeletalMeshComponent* GetWeaponMesh3P() const { return WeaponMesh3P; }

	void AttachToOwningPawn() const;
	void WeaponTrace(FHitResult& OutHit, float TraceLength);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FPSDemo|Weapon")
	FGameplayTag WeaponType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPSDemo|Aiming")
	float AimFOV;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS|Trace")
	float TraceRadius;

	void Local_Fire(const FVector& ImpactPoint, const FVector& ImpactNormal, TEnumAsByte<EPhysicalSurface> ImpactSurfaceType, bool bIsFirstPerson);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override; 

	UFUNCTION(BlueprintImplementableEvent)
	void FireEffects(const FVector& ImpactPoint, const FVector& ImpactNormal, EPhysicalSurface ImpactSurfaceType, bool bIsFirstPerson);

private:

	// Skeletal mesh for the weapon in first person view
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> WeaponMesh1P;

	// Skeletal mesh for the weapon in third person view
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> WeaponMesh3P;
	
	void SetMeshVisibility(APawn* OwningPawn) const;
};
