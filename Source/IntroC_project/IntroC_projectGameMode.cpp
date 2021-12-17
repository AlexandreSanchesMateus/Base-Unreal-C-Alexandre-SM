// Copyright Epic Games, Inc. All Rights Reserved.

#include "IntroC_projectGameMode.h"
#include "IntroC_projectCharacter.h"
#include "UObject/ConstructorHelpers.h"

AIntroC_projectGameMode::AIntroC_projectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	Delay = 3.f;
}

void AIntroC_projectGameMode::OnPlayerDeath(AController* Player)
{
	GLog->Log("Death");
	PlayerController = Player;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AIntroC_projectGameMode::TimerFunction, 3.0f, false, Delay);
}

void AIntroC_projectGameMode::TimerFunction()
{
	if (PlayerController) {
		GLog->Log("Restart Player");

		FVector Location = FVector(0, 0, 600);
		if (APawn* pawn = GetWorld()->SpawnActor<AIntroC_projectCharacter>(DefaultPawnClass, Location, FRotator::ZeroRotator)) {
			PlayerController->Possess(pawn);
		}

		PlayerController = nullptr;
		GetWorldTimerManager().ClearTimer(TimerHandle);
	}
}