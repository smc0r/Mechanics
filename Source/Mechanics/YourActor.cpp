// Fill out your copyright notice in the Description page of Project Settings.


#include "YourActor.h"


// Sets default values
AYourActor::AYourActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AYourActor::BeginPlay()
{
	Super::BeginPlay();
	UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(StaticMeshToEdit->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (!StaticMesh) return;
	FPositionVertexBuffer& yes = StaticMesh->GetStaticMesh()->GetRenderData()->LODResources[0].VertexBuffers.PositionVertexBuffer;
	for (size_t i = 0; i < yes.GetNumVertices(); ++i)
	{
		const FVector3f& f3f = yes.VertexPosition(i);
		const FVector location = {f3f.X, f3f.Y, f3f.Z};
		DrawDebugSphere(GetWorld(), location, radius, segment, color, true);
	}
	
}

// Called every frame
void AYourActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

