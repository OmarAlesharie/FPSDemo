// Copyright Omar Alesharie


#include "Weapon/Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Pawn.h"
#include <Interfaces/PlayerInterface.h>

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bNetUseOwnerRelevancy = true;

	WeaponMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh1P");
	WeaponMesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	WeaponMesh1P->bReceivesDecals = false;
	WeaponMesh1P->CastShadow = false;
	WeaponMesh1P->SetHiddenInGame(true);
	SetRootComponent(WeaponMesh1P);

	WeaponMesh3P = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh3P");
	WeaponMesh3P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	WeaponMesh3P->bReceivesDecals = false;
	WeaponMesh3P->CastShadow = true;
	WeaponMesh3P->SetHiddenInGame(true);
	WeaponMesh3P->SetupAttachment(WeaponMesh1P);


}

void AWeapon::OnRep_Instigator()
{
	Super::OnRep_Instigator();
	AttachToOwningPawn();
}

void AWeapon::AttachToOwningPawn() const
{
	APawn* OwningPawn = GetInstigator();
	if (!IsValid(OwningPawn) || !OwningPawn->Implements<UPlayerInterface>())
	{
		return;
	}

	SetMeshVisibility(OwningPawn);

	const FName AttachPoint = IPlayerInterface::Execute_GetWeaponAttachPoint(OwningPawn, WeaponType);
	USkeletalMeshComponent* OwningPawnMesh1P = IPlayerInterface::Execute_GetMesh1P(OwningPawn);
	USkeletalMeshComponent* OwningPawnMesh3P = IPlayerInterface::Execute_GetMesh3P(OwningPawn);

	WeaponMesh1P->AttachToComponent(OwningPawnMesh1P, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
	WeaponMesh3P->AttachToComponent(OwningPawnMesh3P, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeapon::SetMeshVisibility(APawn* OwningPawn) const
{
	if(OwningPawn->IsLocallyControlled())
	{
		WeaponMesh1P->SetHiddenInGame(false);
		WeaponMesh3P->SetHiddenInGame(true);
	}
	else
	{
		WeaponMesh1P->SetHiddenInGame(true);
		WeaponMesh3P->SetHiddenInGame(false);
	}
}

