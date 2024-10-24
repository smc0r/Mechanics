// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "YourActor.generated.h"

UCLASS()
class MECHANICS_API AYourActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AYourActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, Category = Vertex) AActor* StaticMeshToEdit;
	UPROPERTY(EditAnywhere, Category = Vertex) float radius;
	UPROPERTY(EditAnywhere, Category = Vertex) int32 segment;
	UPROPERTY(EditAnywhere, Category = Vertex) FColor color;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
