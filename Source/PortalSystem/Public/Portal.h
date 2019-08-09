// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Portal.generated.h"

class UMaterial;
class UStaticMeshComponent;
class UTextureRenderTarget2D;
class USceneCaptureComponent2D;

UCLASS()
class PORTALSYSTEM_API APortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortal();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void UpdateCapture();

	void TeleportActor(AActor* ActorToTeleport);

	void SetTarget(APortal* NewTarget);

	APortal* GetTarget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Capture)
	USceneCaptureComponent2D* CaptureComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Capture)
	UTextureRenderTarget2D* RenderTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Capture)
	UMaterial* Material;

	UPROPERTY(VisibleDefaultsOnly, Category = Capture)
	UMaterialInstanceDynamic* MaterialInstance;

private:

	APortal* Target;
};
