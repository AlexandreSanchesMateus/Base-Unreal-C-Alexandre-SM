// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerBoxActor.h"
#include "DrawDebugHelpers.h"

ATriggerBoxActor::ATriggerBoxActor()
{
	OnActorBeginOverlap.AddDynamic(this, &ATriggerBoxActor::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &ATriggerBoxActor::OnOverlapEnd);

	AddValueToHealth = 5;
}

void ATriggerBoxActor::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	Player = Cast<AIntroC_projectCharacter>(OtherActor);

	if (Player) {

		GLog->Log("In");
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ATriggerBoxActor::TimerFunction, 3.0f, true, 1.f);
	}
}

void ATriggerBoxActor::OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor)
{
	if (Player == OtherActor) {
		
		GLog->Log("Out");
		GetWorldTimerManager().ClearTimer(TimerHandle);
	}
}

void ATriggerBoxActor::TimerFunction()
{
	if (Player->CurrentHealth > 0) {
		Player->DamagePlayer(AddValueToHealth);
	}
}

void ATriggerBoxActor::BeginPlay() {
	Super::BeginPlay();

	DrawDebugBox(GetWorld(), GetActorLocation(), GetComponentsBoundingBox().GetExtent(), FColor::Green, true, -1, 0, 2);
}