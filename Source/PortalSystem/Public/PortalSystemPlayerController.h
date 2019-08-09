// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PortalSystemPlayerController.generated.h"

class APortalManager;

UCLASS()
class PORTALSYSTEM_API APortalSystemPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	APortalManager* GetPortalManager();

	FMatrix GetCameraProjectionMatrix();

protected:
	virtual void BeginPlay() override;

	APortalManager* PortalManager;
};
