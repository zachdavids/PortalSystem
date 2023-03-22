// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalSystemPlayerController.h"
#include "PortalManager.h"

APortalManager* APortalSystemPlayerController::GetPortalManager()
{
	return PortalManager;
}

void APortalSystemPlayerController::BeginPlay()
{
	PortalManager = GetWorld()->SpawnActor<APortalManager>(APortalManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	PortalManager->AttachToActor(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
	PortalManager->SetControllerOwner(this);
	PortalManager->SetPortalClass(PortalClass);
}

FMatrix APortalSystemPlayerController::GetCameraProjectionMatrix()
{
	FMatrix ProjectionMatrix;

	if (GetLocalPlayer() != nullptr)
	{
		FSceneViewProjectionData PlayerProjectionData;
		GetLocalPlayer()->GetProjectionData(
			GetLocalPlayer()->ViewportClient->Viewport,
			PlayerProjectionData,
			INDEX_NONE
		);

		ProjectionMatrix = PlayerProjectionData.ProjectionMatrix;
	}

	return ProjectionMatrix;
}
