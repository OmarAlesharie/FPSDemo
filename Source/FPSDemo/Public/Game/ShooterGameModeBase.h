// Copyright Omar Alesharie

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ShooterGameModeBase.generated.h"


UCLASS()
class FPSDEMO_API AShooterGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	void RequestRespawn(ACharacter* Character, AController* Controller);
};
