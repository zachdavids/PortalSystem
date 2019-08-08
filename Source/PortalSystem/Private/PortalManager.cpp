// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalManager.h"
#include "Portal.h"
#include "PortalSystemCharacter.h"
#include "PortalSystemPlayerController.h"

// Sets default values
APortalManager::APortalManager(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APortalManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APortalManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
}

void APortalManager::SetRedPortal(APortal* NewPortal)
{
	if (RedPortal != nullptr)
	{
		RedPortal->Destroy();
	}

	RedPortal = NewPortal;
}
