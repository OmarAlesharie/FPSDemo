// Copyright Omar Alesharie

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/PlayerInterface.h"
#include "ShooterTypes/ShooterTypes.h"
#include "ShooterCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UCombatComponent;
class UInputAction;
class AWeapon;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeaponFirstReplicated, AWeapon*, Weapon, bool, bTargetingPlayer);

UCLASS()
class FPSDEMO_API AShooterCharacter : public ACharacter, public IPlayerInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called when the actor is being destroyed
	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnAim(bool bIsAiming);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// Called when the controller possesses this character
	virtual void PossessedBy(AController* NewController) override;

	// runs on the client whenever the PlayerState property replicates from the server
	virtual void OnRep_PlayerState() override;

#pragma region PlayerInterface
	/** IPlayerInterface implementation */
	virtual FName GetWeaponAttachPoint_Implementation(const FGameplayTag& WeaponType) const override;
	virtual USkeletalMeshComponent* GetMesh1P_Implementation() const override;
	virtual USkeletalMeshComponent* GetMesh3P_Implementation() const override;
	virtual void WeaponReplicated_Implementation() override;
	virtual AWeapon* GetCurrentWeapon_Implementation() override;
	virtual int32 GetReserveAmmo_Implementation() const override;
	virtual void Notify_CycleWeapon_Implementation() override;
	virtual void Notify_ReloadWeapon_Implementation() override;
	virtual void AddAmmo_Implementation(const FGameplayTag& WeaponType, int32 AmmoAmount) override;
#pragma endregion

	

	UFUNCTION(BlueprintCallable)
	FRotator GetFixedAimRotation() const;

	UPROPERTY(BlueprintReadOnly, Category = "FPSDemo|FABRIK")
	FTransform FABRIK_SocketTransform;

	UFUNCTION(BlueprintCallable)
	bool HasCurrentWeapon() const;

	UPROPERTY(BlueprintAssignable)
	FWeaponFirstReplicated OnWeaponFirstReplicated;

	bool HasWeaponFirstReplicated() const { return bWeaponFirstReplicated; }

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPSDemo|Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCombatComponent> Combat;

	// Mesh for 1st person view (arms; seen only by self)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	TObjectPtr < USkeletalMeshComponent> Mesh1P;

	// Camera boom positioning the camera behind the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPSDemo|Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	// Camera component for 1st person view (only seen by self)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPSDemo|Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCameraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPSDemo|Aiming", meta = (AllowPrivateAccess = "true"))
	float DefaultFOV;

	UPROPERTY(EditAnywhere, Category = "FPSDemo|Input")
	TObjectPtr<UInputAction> CycleWeaponAction;

	UPROPERTY(EditAnywhere, Category = "FPSDemo|Input")
	TObjectPtr<UInputAction> FireWeaponAction;

	UPROPERTY(EditAnywhere, Category = "FPSDemo|Input")
	TObjectPtr<UInputAction> ReloadWeaponAction;

	UPROPERTY(EditAnywhere, Category = "FPSDemo|Input")
	TObjectPtr<UInputAction> AimWeaponAction;

	void CycleWeapon();
	void FireWeapon_Pressed();
	void FireWeapon_Released();
	void ReloadWeapon();
	void AimWeapon_Pressed();
	void AimWeapon_Released();

	void CalculateFABRIKSocketTransform();
	void CalculateTurnInPlaceParameters(float DeltaTime);
	void TurnInPlace(float DeltaTime);

	bool bWeaponFirstReplicated;

	FRotator StartingAimRotation;
	float InterpAO_Yaw;

	UPROPERTY(BlueprintReadOnly, Category = "FPSDemo|TurnInPlace", meta = (AllowPrivateAccess = "true"))
	float AO_Yaw;

	UPROPERTY(BlueprintReadOnly, Category = "FPS|Strafing", meta = (AllowPrivateAccess = "true"))
	float MovementOffsetYaw;

	UPROPERTY(BlueprintReadOnly, Category = "FPSDemo|TurnInPlace", meta = (AllowPrivateAccess = "true"))
	ETurningInPlace TurningStatus;
};
