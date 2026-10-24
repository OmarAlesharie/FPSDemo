// Copyright Omar Alesharie


#include "Components/Elimination/EliminationComponent.h"

// Sets default values for this component's properties
UEliminationComponent::UEliminationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UEliminationComponent::OnRoundReported(AActor* Attacker, AActor* Victim, bool bHit, bool bHeadShot, bool bLethal)
{
	UE_LOG(LogTemp, Warning, TEXT("Hit: %d, HeadShot: %d, Lethal: %d"), bHit, bHeadShot, bLethal);
}

