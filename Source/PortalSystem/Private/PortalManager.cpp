// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalManager.h"
#include "Portal.h"
#include "PortalSystemCharacter.h"
#include "PortalSystemPlayerController.h"
#include "EngineUtils.h"

// Sets default values
APortalManager::APortalManager(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void APortalManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APortalManager::UpdatePortals(float DeltaTime)
{
	for (TActorIterator<APortal> It(GetWorld()); It; ++It)
	{
		APortal* Portal = *It;
		if (Portal != nullptr)
		{
			Portal->UpdateCapture();
		}
	}
}

void APortalManager::RequestTeleport(APortal* Portal, AActor* TeleportTarget)
{

}

void APortalManager::SetControllerOwner(APortalSystemPlayerController* NewOwner)
{
	OwningController = NewOwner;
}

void APortalManager::SetBluePortal(APortal* NewPortal)
{
	if (BluePortal != nullptr)
	{
		BluePortal->Destroy();
	}

	BluePortal = NewPortal;

	if (RedPortal != nullptr)
	{
		BluePortal->SetTarget(RedPortal);
		RedPortal->SetTarget(BluePortal);
	}
}

void APortalManager::SetRedPortal(APortal* NewPortal)
{
	if (RedPortal != nullptr)
	{
		RedPortal->Destroy();
	}

	RedPortal = NewPortal;

	if (BluePortal != nullptr)
	{
		RedPortal->SetTarget(BluePortal);
		BluePortal->SetTarget(RedPortal);
	}
}
