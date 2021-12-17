// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "IntroC_projectCharacter.h"
#include "TriggerBoxActor.generated.h"


/**
 * 
 */
UCLASS()
class INTROC_PROJECT_API ATriggerBoxActor : public ATriggerBox
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	ATriggerBoxActor();

	UFUNCTION()
	void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);
	
	UFUNCTION()
	void OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor);

	UFUNCTION()
	void TimerFunction();

	UPROPERTY()
	AIntroC_projectCharacter* Player;

	UPROPERTY(EditAnywhere)
	int AddValueToHealth;

	UPROPERTY()
	FTimerHandle TimerHandle;
};
