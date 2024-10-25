// Fill out your copyright notice in the Description page of Project Settings.


#include "HaydaaMovementComponent.h"

#include "MechanicsCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

#pragma region Things
UHaydaaMovementComponent::UHaydaaMovementComponent()
{
	NavAgentProps.bCanCrouch = true;
}

bool UHaydaaMovementComponent::FSavedMove_Haydaa::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter,
                                                                 float MaxDelta) const
{
	FSavedMove_Haydaa* Haydaa = static_cast<FSavedMove_Haydaa*>(NewMove.Get());
	
	if (Saved_bWantsToSprit != Haydaa->Saved_bWantsToSprit)
	{
		return false;
	}
	return FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UHaydaaMovementComponent::FSavedMove_Haydaa::Clear()
{
	FSavedMove_Character::Clear();
	Saved_bWantsToSprit = 0;
}

uint8 UHaydaaMovementComponent::FSavedMove_Haydaa::GetCompressedFlags() const
{
	uint8 Result = FSavedMove_Character::GetCompressedFlags();
	if (Saved_bWantsToSprit) Result |=  FLAG_Custom_0;
	return Result;
}

void UHaydaaMovementComponent::FSavedMove_Haydaa::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel,
	class FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);
	UHaydaaMovementComponent* H = Cast<UHaydaaMovementComponent>(C->GetCharacterMovement());
	
	Saved_bWantsToSprit = H->Safe_bWantsToSprit;
	
}

void UHaydaaMovementComponent::FSavedMove_Haydaa::PrepMoveFor(ACharacter* C)
{
	FSavedMove_Character::PrepMoveFor(C);
	UHaydaaMovementComponent* H = Cast<UHaydaaMovementComponent>(C->GetCharacterMovement());
	H->Safe_bWantsToSprit = Saved_bWantsToSprit;
}

UHaydaaMovementComponent::FNetworkPredictionData_Client_Haydaa::FNetworkPredictionData_Client_Haydaa(
	const UCharacterMovementComponent& ClientMovement) : Super(ClientMovement)
{
}

FSavedMovePtr UHaydaaMovementComponent::FNetworkPredictionData_Client_Haydaa::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_Haydaa);
}

FNetworkPredictionData_Client* UHaydaaMovementComponent::GetPredictionData_Client() const
{
	check (PawnOwner != nullptr)
	if (ClientPredictionData == nullptr)
	{
		UHaydaaMovementComponent* MutableThis = const_cast<UHaydaaMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Haydaa(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}
	return ClientPredictionData;
}

void UHaydaaMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
	Safe_bWantsToSprit = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
}
#pragma endregion

void UHaydaaMovementComponent::SprintPressed()
{
	Safe_bWantsToSprit = true;
}

void UHaydaaMovementComponent::SprintReleased()
{
	Safe_bWantsToSprit = false;
}

// Movement Update in sonunda cagriliyor
void UHaydaaMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation,
	const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
	if (Safe_bWantsToSprit)
	{
		MaxWalkSpeed = Sprint_MaxWalkSpeed;
	}
	else
	{
		MaxWalkSpeed = Sprint_DefaultMaxWalkSpeed;
	}
}

void UHaydaaMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	/* Ornek
	if (MovementMode == MOVE_Walking && !bWantsToCrouch && Safe_bPrevWantsToCrouch)
	{
		FHitResult PotentialSlideSurface;
		if (Velocity.SizeSquared() > pow(Slide_MinSpeed, 2) && GetSlideSurface(PotentialSlideSurface))
		{
			EnterSlide();
		}
	}

	if (IsCustomMovementMode(CMOVE_Slide) && !bWantsToCrouch)
	{
		SetMovementMode(MOVE_Walking);
	}
	
	*/
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

void UHaydaaMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);
	switch (CustomMovementMode)
	{
	case CMOVE_Slide:
		PhysSlide(deltaTime, Iterations);
		break;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"))
	}
}

bool UHaydaaMovementComponent::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

bool UHaydaaMovementComponent::IsMovingOnGround() const
{
	return Super::IsMovingOnGround() && IsCustomMovementMode(CMOVE_Slide);;
}

bool UHaydaaMovementComponent::CanCrouchInCurrentState() const
{
	return Super::CanCrouchInCurrentState();
}

void UHaydaaMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
	HaydaaaCharacterOwner = Cast<AMechanicsCharacter>(GetOwner());
}

void UHaydaaMovementComponent::EnterSlide()
{
	bWantsToCrouch = true;
	Velocity += Velocity.GetSafeNormal2D() * Slide_EnterImpulse;
	SetMovementMode(MOVE_Custom, CMOVE_Slide);
}

void UHaydaaMovementComponent::ExitSlide()
{
	bWantsToCrouch = false;
	
	FQuat NewRotation = FRotationMatrix::MakeFromXZ(UpdatedComponent->GetForwardVector().GetSafeNormal2D(), FVector::UpVector).ToQuat();
	FHitResult Hit;
	SafeMoveUpdatedComponent(FVector::ZeroVector, NewRotation, true, Hit);
	SetMovementMode(MOVE_Walking);
}

void UHaydaaMovementComponent::PhysSlide(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}
	
	RestorePreAdditiveRootMotionVelocity();

	FHitResult SurfaceHit;
	if (!GetSlideSurface(SurfaceHit) || Velocity.SizeSquared() < pow(Slide_MinSpeed, 2))
	{
		ExitSlide();
		StartNewPhysics(deltaTime, Iterations);
		return;
	}

	// Surface Gravity
	Velocity += Slide_GravityForce * FVector::DownVector * deltaTime;

	// Strafe
	if (FMath::Abs(FVector::DotProduct(Acceleration.GetSafeNormal(), UpdatedComponent->GetRightVector())) > .5)
	{
		Acceleration = Acceleration.ProjectOnTo(UpdatedComponent->GetRightVector());
	}
	else
	{
		Acceleration = FVector::ZeroVector;
	}

	// Calc Velocity
	if(!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		CalcVelocity(deltaTime, Slide_Friction, true, GetMaxBrakingDeceleration());
	}
	ApplyRootMotionToVelocity(deltaTime);

	// Perform Move
	Iterations++;
	bJustTeleported = false;
	
	FVector OldLocation = UpdatedComponent->GetComponentLocation();
	FQuat OldRotation = UpdatedComponent->GetComponentRotation().Quaternion();
	FHitResult Hit(1.f);
	FVector Adjusted = Velocity * deltaTime;
	FVector VelPlaneDir = FVector::VectorPlaneProject(Velocity, SurfaceHit.Normal).GetSafeNormal();
	FQuat NewRotation = FRotationMatrix::MakeFromXZ(VelPlaneDir, SurfaceHit.Normal).ToQuat();
	SafeMoveUpdatedComponent(Adjusted, NewRotation, true, Hit);

	if (Hit.Time < 1.f)
	{
		HandleImpact(Hit, deltaTime, Adjusted);
		SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
	}

	FHitResult NewSurfaceHit;
	if (!GetSlideSurface(NewSurfaceHit) || Velocity.SizeSquared() < pow(Slide_MinSpeed, 2))
	{
		ExitSlide();
	}
	
	// Update Outgoing Velocity & Acceleration
	if( !bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
	}
}

bool UHaydaaMovementComponent::GetSlideSurface(FHitResult& Hit) const
{
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector End = Start + CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.f * FVector::DownVector;
	FName ProfileName = TEXT("BlockAll");
	return GetWorld()->LineTraceSingleByProfile(Hit, Start, End, ProfileName, HaydaaaCharacterOwner->GetIgnoreCharacterParams());
}

