// Copyright Omar Alesharie

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "WeaponData.generated.h"

/**
 * 
 */
UCLASS()
class FPSDEMO_API UWeaponData : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, Category = "FPSDemo|WeaponData|Weapon")
	TMap<FGameplayTag, FName> GripPoints;
};
