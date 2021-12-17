// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Projectile.generated.h"


UCLASS()
class INTROC_PROJECT_API AProjectile : public AActor
{
	GENERATED_BODY()

	UPROPERTY()
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY()
	class UProjectileMovementComponent* ProjectileComponent;

	UPROPERTY(EditAnywhere)
	class UMaterial* Material;
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
