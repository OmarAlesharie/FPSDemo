// Copyright Omar Alesharie

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/PlayerInterface.h"
#include "ShooterCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UCombatComponent;
class UInputAction;

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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** IPlayerInterface implementation */
	virtual FName GetWeaponAttachPoint_Implementation(const FGameplayTag& WeaponType) const override;

	// Called when the controller possesses this character
	virtual void PossessedBy(AController* NewController) override;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCombatComponent> Combat;

	// Mesh for 1st person view (arms; seen only by self)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	TObjectPtr < USkeletalMeshComponent> Mesh1P;

	// Camera boom positioning the camera behind the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	// Camera component for 1st person view (only seen by self)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCameraComponent;

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

};
