// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpPad.h"
#include "IntroC_projectCharacter.h"

// Sets default values
AJumpPad::AJumpPad()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	forceJump = 800;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComponent;
	
	MeshComponent->OnComponentHit.AddDynamic(this, &AJumpPad::OnHit);
}

// Called when the game starts or when spawned
void AJumpPad::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AJumpPad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AJumpPad::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AIntroC_projectCharacter* character = Cast<AIntroC_projectCharacter>(OtherActor);
	character->LaunchCharacter(FVector(0,0,forceJump), false, true);
	GLog->Log("Jump");
}

