// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HaydaaMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MECHANICS_API UHaydaaMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	class FSavedMove_Haydaa : public FSavedMove_Character
	{
		typedef FSavedMove_Character Super;
		
		uint8 Saved_bWantsToSprit:1;
		
		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		virtual void Clear() override;
		virtual uint8 GetCompressedFlags() const override;
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;
		virtual void PrepMoveFor(ACharacter* C) override;
	};

	class FNetworkPredictionData_Client_Haydaa : public FNetworkPredictionData_Client_Character
	{
	public:
		FNetworkPredictionData_Client_Haydaa(const UCharacterMovementComponent& ClientMovement);

		typedef FNetworkPredictionData_Client_Character Super;

		virtual FSavedMovePtr AllocateNewMove() override;
	};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Walking) float Sprint_MaxWalkSpeed = 1000.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Walking) float Sprint_DefaultMaxWalkSpeed = 500.f;

	bool Safe_bWantsToSprit;
	
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	
protected:
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	
	UFUNCTION(BlueprintCallable) void SprintPressed();
	UFUNCTION(BlueprintCallable) void SprintReleased();
	
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
};

