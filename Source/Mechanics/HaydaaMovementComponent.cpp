// Fill out your copyright notice in the Description page of Project Settings.


#include "HaydaaMovementComponent.h"
#include "GameFramework/Character.h"

#pragma region Things
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