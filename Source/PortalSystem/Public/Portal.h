// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Portal.generated.h"

class UMaterial;
class UBoxComponent;
class USceneComponent;
class UStaticMeshComponent;
class UTextureRenderTarget2D;

UCLASS()
class PORTALSYSTEM_API APortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortal();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void TeleportActors();

	void SetTarget(APortal* NewTarget);

	APortal* GetTarget();

	UTextureRenderTarget2D* GetRenderTarget();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Portal)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Portal)
	UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere, Category = Capture)
	UTextureRenderTarget2D* RenderTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Capture)
	UMaterial* Material;

	UPROPERTY(EditAnywhere, Category = Capture)
	UMaterialInstanceDynamic* MaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Capture)
	APortal* Target;

private:

	FVector LastPosition;

	bool bLastInFront;

	bool Overlapping;

	TArray<AActor*> OverlappingActors;
};