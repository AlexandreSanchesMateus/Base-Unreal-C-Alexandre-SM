// Copyright Epic Games, Inc. All Rights Reserved.

#include "IntroC_projectCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include "IntroC_projectGameMode.h"
#include "Projectile.h"

//////////////////////////////////////////////////////////////////////////
// AIntroC_projectCharacter

AIntroC_projectCharacter::AIntroC_projectCharacter()
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	WalkSpeed = 175.f;
	RunSpeed = 500.f;
	Health = 100;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AIntroC_projectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AIntroC_projectCharacter::InteractPressed);

	PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &AIntroC_projectCharacter::Throw);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AIntroC_projectCharacter::OnStartRun);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AIntroC_projectCharacter::OnStopRun);

	PlayerInputComponent->BindAxis("MoveForward", this, &AIntroC_projectCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AIntroC_projectCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AIntroC_projectCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AIntroC_projectCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AIntroC_projectCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AIntroC_projectCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AIntroC_projectCharacter::OnResetVR);
}

void AIntroC_projectCharacter::InteractPressed()
{
	if (!primitiveComponent) {
		FVector Loc;
		FRotator Rot;
		FHitResult outHit;

		GetController()->GetPlayerViewPoint(Loc, Rot);

		FVector Start = Loc;
		FVector End = Start + (Rot.Vector() * 2000);

		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(this);

		FCollisionObjectQueryParams ObjectPrams;
		ObjectPrams.AddObjectTypesToQuery(ECC_PhysicsBody);

		//GetWorld()->LineTraceSingleByChannel(outHit, Start, End, ECC_PhysicsBody, TraceParams);
		if (GetWorld()->LineTraceSingleByObjectType(outHit, Start, End, ObjectPrams, TraceParams)) {
			DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.0f);

			primitiveComponent = Cast<UPrimitiveComponent>(outHit.GetComponent());

			primitiveComponent->SetSimulatePhysics(false);

			outHit.GetActor()->AttachToComponent(SceneComponent, FAttachmentTransformRules::KeepWorldTransform);
			outHit.GetActor()->SetActorLocation(SceneComponent->GetComponentLocation());
			outHit.GetActor()->SetActorRotation(this->GetActorRotation());
		}
		else {
			DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f);
		}
		
	}
	else {
		primitiveComponent->SetSimulatePhysics(true);
		primitiveComponent->GetAttachmentRootActor()->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		primitiveComponent = nullptr;
	}
}


void AIntroC_projectCharacter::PlayerDeath()
{
	// Ragdoll
	USkeletalMeshComponent* PlayerMesh = this->GetMesh();
	PlayerMesh->SetSimulatePhysics(true);
	PlayerMesh->SetCollisionProfileName(FName(TEXT("Ragdoll")));
	this->GetCapsuleComponent()->DestroyComponent();

	AController* PlayerController = Cast<AController>(this->GetController());
	AIntroC_projectGameMode* GameMode = Cast<AIntroC_projectGameMode>((AIntroC_projectGameMode*)GetWorld()->GetAuthGameMode());

	GameMode->OnPlayerDeath(PlayerController);

	this->UnPossessed();
}


void AIntroC_projectCharacter::DamagePlayer(int damage)
{
	CurrentHealth -= damage;

	GLog->Log("Damage to "+ FString(this->GetName()) + " : " + FString::FromInt(damage));

	if (CurrentHealth <= 0) {
		CurrentHealth = 0;
		this->PlayerDeath();
	}
	else if (CurrentHealth > Health) {
		CurrentHealth = Health;
	}
}


void AIntroC_projectCharacter::Throw()
{
	if (!primitiveComponent) {

		FVector Direction = FollowCamera->GetForwardVector();
		FVector Location = SceneComponent->GetComponentLocation();

		GetWorld()->SpawnActor<AProjectile>(Location, Direction.Rotation());
	}
}


void AIntroC_projectCharacter::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = Health;


	FTransform rebuilt = FTransform();

	FVector pos = SceneComponent->GetComponentLocation() - this->GetActorLocation();
	rebuilt.SetTranslation(-pos);
	SceneComponent->SetRelativeTransform(rebuilt);

	FTransform initialTransform = FTransform();

	FVector initialPosition = FVector(120, 0, 0);
	initialTransform.SetTranslation(initialPosition);
	SceneComponent->SetRelativeTransform(initialTransform);



	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AIntroC_projectCharacter::OnStartRun()
{
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}


void AIntroC_projectCharacter::OnStopRun()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}


void AIntroC_projectCharacter::OnResetVR()
{
	// If IntroC_project is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in IntroC_project.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AIntroC_projectCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AIntroC_projectCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AIntroC_projectCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AIntroC_projectCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AIntroC_projectCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AIntroC_projectCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
