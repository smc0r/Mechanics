// Copyright Epic Games, Inc. All Rights Reserved.

#include "MechanicsCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Components/SplineComponent.h"
#include "HaydaaMovementComponent.h"
#include "Dataflow/DataflowSettings.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AMechanicsCharacter

AMechanicsCharacter::AMechanicsCharacter(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer.SetDefaultSubobjectClass<UHaydaaMovementComponent>
	(ACharacter::CharacterMovementComponentName))
{
	HaydaaMovementComponent = Cast<UHaydaaMovementComponent>(GetCharacterMovement());
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	Spline = CreateDefaultSubobject<USplineComponent>("yaaaani");
}

void AMechanicsCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	
	// what |= FLAG_Injured;
	// what |= 0x01;
	// if (GEngine) GEngine->AddOnScreenDebugMessage(1, 10.f, FColor::MakeRandomColor(), FString::Printf(TEXT("Num: %d"), what));
	// what &= ~0x01;
	// what |= 0x02;
	// if (GEngine) GEngine->AddOnScreenDebugMessage(2, 10.f, FColor::MakeRandomColor(), FString::Printf(TEXT("Num: %d"), what));
	// what |= 0x01;
	// if (GEngine) GEngine->AddOnScreenDebugMessage(3, 10.f, FColor::MakeRandomColor(), FString::Printf(TEXT("Num: %d"), what));
	// what = ~what;
	// if (GEngine) GEngine->AddOnScreenDebugMessage(4, 10.f, FColor::MakeRandomColor(), FString::Printf(TEXT("Num: %d"), what));
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMechanicsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMechanicsCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMechanicsCharacter::Look);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AMechanicsCharacter::Released);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &AMechanicsCharacter::Triggered);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}
#pragma region Trigger Things
void AMechanicsCharacter::Triggered()
{
	if (player == nullptr)
	{
		player = Cast<APlayerController>(GetController());
	}
	else
	{
		FVector2D Viewport;
		GEngine->GameViewport->GetViewportSize(Viewport);
		FVector Location;
		FVector Direction;
		player->DeprojectScreenPositionToWorld(Viewport.X / 2, Viewport.Y / 2, Location, Direction);
		Location += Direction * StartOffset;
		FVector End = Location + Direction *  HookDistance;
		TArray<AActor*> ToIgnore;
		ToIgnore.Add(this);
		UKismetSystemLibrary::LineTraceSingle(this,
			Location,
			End,
			ETraceTypeQuery::TraceTypeQuery1,
			false,
			ToIgnore,
			EDrawDebugTrace::ForOneFrame,
			HitResult,
			true,
			LineTraceColor,
			FLinearColor::Black);
	}
	DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, SphereRadius, 12.f, SphereColor.ToFColor(true));
}
#pragma endregion 



void AMechanicsCharacter::Released()
{
	if (HitResult.GetActor() == nullptr) return;
	const FVector To = HitResult.ImpactPoint - GetActorLocation();
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		GetCharacterMovement()->Velocity = Speed * To.GetSafeNormal();
		GetCharacterMovement()->BrakingDecelerationFlying = 0;
	}
}

void AMechanicsCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (GetCharacterMovement() && GetCharacterMovement()->MovementMode == MOVE_Flying)
	{
		float Distance = (HitResult.ImpactPoint - GetActorLocation()).Size();
		if (Distance <= AcceptanceRadius)
		{
			LaunchCharacter(FVector(0.f, 0.f, 400.f), false, true);
			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
		if (HaydaaMovementComponent)
		{
			FVector A = HaydaaMovementComponent->GetCurrentAcceleration();
			FString Message = FString::Printf(TEXT("X: %f, \nY: %f, \nX: %f"),A.X, A.Y, A.Z);
			GEngine->AddOnScreenDebugMessage(312, 2.f, FColor::Orange, Message);
		}
	}
	// FVector HitNormal = (GetCharacterMovement()->CurrentFloor.HitResult.Normal);
	// const FVector& Velocity = GetCharacterMovement()->Velocity;
	// FVector XAxis = FVector::VectorPlaneProject(Velocity, HitNormal);
	// FQuat Rotation = FRotationMatrix::MakeFromXZ(XAxis, HitNormal).ToQuat();
	// if (Velocity.Size() > 100.f)
	// {
	// 	SetActorRotation(Rotation);	
	// }
	
}

void AMechanicsCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
	
}

void AMechanicsCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}

}
