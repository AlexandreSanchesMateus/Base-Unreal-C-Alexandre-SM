// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "IntroC_projectGameMode.generated.h"

UCLASS(minimalapi)
class AIntroC_projectGameMode : public AGameModeBase
{
	GENERATED_BODY()
	

public:
	
	UPROPERTY(EditAnywhere)
	float Delay;

	UPROPERTY(EditAnywhere)
	FTimerHandle TimerHandle;

	UPROPERTY(EditAnywhere)
	AController* PlayerController;

	AIntroC_projectGameMode();

	UFUNCTION(BlueprintCallable)
	virtual void OnPlayerDeath(AController* Player);

	UFUNCTION()
	void TimerFunction();
};



