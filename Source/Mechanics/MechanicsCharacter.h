// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MechanicsCharacter.generated.h"

class UHaydaaMovementComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);
enum bitwise : uint8
{
	FLAG_Alive = 0x01,
	FLAG_Dead = 0x02,
	FLAG_Injured = 0x04,
	FLAG_Dying = 0x08,
	// those are on up are reserved
	FLAG_5 = 0x10,
	FLAG_6 = 0x20,
	FLAG_7 = 0x40,
	FLAG_8 = 0x80,

	FLAG_AboutToDie = FLAG_Injured + FLAG_Dead + FLAG_Dying
};


UCLASS(config=Game)
class AMechanicsCharacter : public ACharacter
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	UHaydaaMovementComponent* HaydaaMovementComponent;
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;
public:
	AMechanicsCharacter(const FObjectInitializer& ObjectInitializer);
	UPROPERTY(EditAnywhere, Category = Hook)
	float StartOffset = 20.f;

	UPROPERTY(EditAnywhere, Category = Hook)
	float HookDistance = 1000.f;

	UPROPERTY(EditAnywhere, Category = Hook)
	FLinearColor LineTraceColor = FLinearColor::Red;

	UPROPERTY(EditAnywhere, Category = Hook)
	float Speed = 500.f;

	UPROPERTY(EditAnywhere, Category = Hook)
	float AcceptanceRadius = 70.f;
    
	UPROPERTY(EditAnywhere, Category = Hook)
	float JumpForce = 400.f;

	UPROPERTY(EditAnywhere, Category = Hook)
	float SphereRadius = 30.f;

	UPROPERTY(EditAnywhere, Category = Hook)
	FLinearColor SphereColor = FLinearColor::Gray;
	
	UPROPERTY()
	APlayerController* player;
	FHitResult HitResult;
	void Triggered();
	void Released();
	virtual void Tick(float DeltaSeconds) override;
	USplineComponent* Spline;

protected:
	
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();
	
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	uint8 what:2;

	FCollisionQueryParams GetIgnoreCharacterParams();
};

