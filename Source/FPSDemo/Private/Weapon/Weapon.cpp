// Copyright Omar Alesharie


#include "Weapon/Weapon.h"
#include "Components/SkeletalMeshComponent.h"

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

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

