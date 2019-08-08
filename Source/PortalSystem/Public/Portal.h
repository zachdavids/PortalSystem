// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Portal.generated.h"

UCLASS()
class PORTALSYSTEM_API APortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortal();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void TeleportActor(AActor* ActorToTeleport);

	APortal* GetTarget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite)
	UMaterialInstanceDynamic* MaterialInstance;

private:

	APortal* Target;
};
