// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"


// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComponent;

	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));

	if (MeshAsset.Succeeded()) {
		MeshComponent->SetStaticMesh(MeshAsset.Object);
		MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		MeshComponent->SetWorldScale3D(FVector(0.1f));
	}

	static ConstructorHelpers::FObjectFinder<UMaterial>MaterialAsset(TEXT("/Game/Material/M-Splash.M-Splash"));
	
	if (MaterialAsset.Succeeded()) {
		Material = MaterialAsset.Object;
	}

	ProjectileComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileComponent->InitialSpeed = 1500.0f;
	ProjectileComponent->ProjectileGravityScale = 0.5f;
	
	MeshComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	float SizeDecal = FMath::RandRange(30.0f, 20.0f);

	int Frame = FMath::RandRange(0, 3);

	float RColor = FMath::RandRange(0.0f, 1.0f);
	float GColor = FMath::RandRange(0.0f, 1.0f);
	float BColor = FMath::RandRange(0.0f, 1.0f);

	UMaterialInstanceDynamic* MaterialInstance = UMaterialInstanceDynamic::Create(Material, this);
	MaterialInstance->SetScalarParameterValue(FName(TEXT("Frame")), Frame);
	MaterialInstance->SetVectorParameterValue(FName(TEXT("Color")), FLinearColor(FVector4(RColor, GColor, BColor, 0.0f)));

	UGameplayStatics::SpawnDecalAtLocation(OtherActor, MaterialInstance, FVector(SizeDecal), Hit.Location, Hit.Normal.Rotation());
	
	this->Destroy();
}