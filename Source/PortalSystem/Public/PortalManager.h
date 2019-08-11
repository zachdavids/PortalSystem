// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortalManager.generated.h"

class APortal;
class USceneCaptureComponent2D;
class APortalSystemPlayerController;

UCLASS()
class PORTALSYSTEM_API APortalManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortalManager(const FObjectInitializer& ObjectInitializer);

	// Called by character every frame
	void UpdatePortals(float DeltaTime);

	void SetControllerOwner(APortalSystemPlayerController* NewOwner);

	void SetBluePortal(APortal* NewPortal);

	void SetRedPortal(APortal* NewPortal);

	UFUNCTION(BlueprintCallable, Category = "Portal")
	void RequestTeleport(APortal* Portal, AActor* TeleportTarget);

	FVector ConvertLocation(FVector Location, AActor* Portal, AActor* Target);

	FRotator ConvertRotation(FRotator Rotation, AActor* Portal, AActor* Target);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Capture)
	USceneCaptureComponent2D* CaptureComponent;

private:

	APortal* BluePortal;

	APortal* RedPortal;

	UPROPERTY()
	APortalSystemPlayerController* OwningController;
};
