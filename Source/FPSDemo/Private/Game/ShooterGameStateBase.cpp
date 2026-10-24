// Copyright Omar Alesharie


#include "Game/ShooterGameStateBase.h"


class AShooterGameStateBase;

AShooterGameStateBase::AShooterGameStateBase()
{
	bHasFirstBloodBeenHad = false;
}

bool AShooterGameStateBase::HasFirstBloodBeenHad() const
{
	return bHasFirstBloodBeenHad;
}

void AShooterGameStateBase::UpdateLeader()
{
	bHasFirstBloodBeenHad = true;
}
