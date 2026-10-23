// Copyright Omar Alesharie

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

struct FGameplayTag;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FPSDEMO_API IPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "FPSDemo|PlayerInterface")
	FName GetWeaponAttachPoint(const FGameplayTag& WeaponType) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "FPSDemo|PlayerInterface")
	USkeletalMeshComponent* GetMesh1P() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "FPSDemo|PlayerInterface")
	USkeletalMeshComponent* GetMesh3P() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void WeaponReplicated();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AWeapon* GetCurrentWeapon();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetReserveAmmo() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Notify_CycleWeapon();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Notify_ReloadWeapon();
};
