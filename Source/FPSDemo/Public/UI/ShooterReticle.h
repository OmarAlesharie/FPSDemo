// Copyright Omar Alesharie

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShooterReticle.generated.h"

class UImage;
class AWeapon;


UCLASS()
class FPSDEMO_API UShooterReticle : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Reticle;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_AmmoCounter;

private:

	TWeakObjectPtr<UMaterialInstanceDynamic> CurrentReticle_DynMatInst;
	TWeakObjectPtr<UMaterialInstanceDynamic> CurrentAmmoCounter_DynMatInst;

	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	UFUNCTION()
	void OnWeaponFirstReplicated(AWeapon* Weapon);

	UFUNCTION()
	void OnReticleChanged(UMaterialInstanceDynamic* ReticleDynMatInst);

	UFUNCTION()
	void OnAmmoCounterChanged(UMaterialInstanceDynamic* AmmoCounterDynMatInst, int32 RoundsCurrent, int32 RoundsMax);
};
