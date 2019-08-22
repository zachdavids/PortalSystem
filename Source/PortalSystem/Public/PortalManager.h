// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortalManager.generated.h"

class APortal;
class UArrowComponent;
class USceneCaptureComponent2D;
class APortalSystemPlayerController;

UCLASS()
class PORTALSYSTEM_API APortalManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortalManager(const FObjectInitializer& ObjectInitializer);

	void Tick(float DeltaTime) override;

	// Called by character every frame
	void UpdatePortals();

	void SetControllerOwner(APortalSystemPlayerController* NewOwner);

	void SetPortalClass(TSubclassOf<class APortal> Class);

	void SpawnBluePortal(const FVector& Start, const FVector& End);

	void SpawnRedPortal(const FVector& Start, const FVector& End);

	APortal* SpawnPortal(const FVector& Start, const FVector& End);

	bool TraceCorner(const APortal* Portal, const FVector& Start, const FVector& End, const FVector& Forward, const FVector& Right, const FVector& Up, FHitResult& HitResult);

	bool VerifyPortalPlacement(const APortal* Portal, FVector& Origin);

	void FitPortalAroundTargetPortal(const APortal* Portal, const APortal* Target, const FVector& Forward, const FVector& Right, const FVector& Up, FVector& Origin);

	bool FitPortalOnSurface(const APortal* Portal, const FVector& Forward, const FVector& Right, const FVector& Up, const FVector& TopEdge, const FVector& BottomEdge, const FVector& RightEdge, const FVector& LeftEdge, TArray<FHitResult>& HitResults, TArray<int32>& HitIndex, FVector& Origin, int RecursionCount = 6, int HitCount = 0);


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

	/** Portal class to spawn */
	UPROPERTY()
	TSubclassOf<class APortal> PortalClass;
};
