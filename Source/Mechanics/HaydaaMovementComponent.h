// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HaydaaMovementComponent.generated.h"

UENUM(BlueprintType)
enum ECustomMovementMode : uint8
{
	CMOVE_None			UMETA(Hidden),
	CMOVE_Slide			UMETA(DisplayName = "Slide"),
	CMOVE_MAX			UMETA(Hidden),
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MECHANICS_API UHaydaaMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	UHaydaaMovementComponent();
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
	UPROPERTY(EditDefaultsOnly, Category = Walking) float Sprint_MaxWalkSpeed = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = Walking) float Sprint_DefaultMaxWalkSpeed = 500.f;
	UPROPERTY(EditDefaultsOnly, Category = Slide) float Slide_MinSpeed=400;
   	UPROPERTY(EditDefaultsOnly, Category = Slide) float Slide_EnterImpulse=400;
    UPROPERTY(EditDefaultsOnly, Category = Slide) float Slide_GravityForce=200;
    UPROPERTY(EditDefaultsOnly, Category = Slide) float Slide_Friction=.1;
	
	bool Safe_bWantsToSprit;
	
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	
protected:
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	
	UFUNCTION(BlueprintCallable) void SprintPressed();
	UFUNCTION(BlueprintCallable) void SprintReleased();
	
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

private:
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;
	bool IsMovingOnGround() const override;
	bool CanCrouchInCurrentState() const override;
	virtual void InitializeComponent() override;
	void EnterSlide();
	void ExitSlide();
	void PhysSlide(float deltaTime, int32 Iterations);
	bool GetSlideSurface(FHitResult& Hit) const;
	TObjectPtr<AMechanicsCharacter> HaydaaaCharacterOwner;

};

