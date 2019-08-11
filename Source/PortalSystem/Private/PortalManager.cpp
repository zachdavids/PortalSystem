// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalManager.h"
#include "Portal.h"
#include "PortalSystemCharacter.h"
#include "PortalSystemPlayerController.h"
#include "DrawDebugHelpers.h"
#include "Components/SceneCaptureComponent2D.h"
#include "EngineUtils.h"

// Sets default values
APortalManager::APortalManager(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(FName("CaptureComponent"));
	CaptureComponent->SetupAttachment(RootComponent);

	FPostProcessSettings CaptureSettings;
	CaptureSettings.bOverride_AmbientOcclusionQuality = true;
	CaptureSettings.bOverride_MotionBlurAmount = true;
	CaptureSettings.bOverride_SceneFringeIntensity = true;
	CaptureSettings.bOverride_GrainIntensity = true;
	CaptureSettings.bOverride_ScreenSpaceReflectionQuality = true;
	CaptureSettings.AmbientOcclusionQuality = 0.0f;
	CaptureSettings.MotionBlurAmount = 0.0f;
	CaptureSettings.SceneFringeIntensity = 0.0f;
	CaptureSettings.GrainIntensity = 0.0f;
	CaptureSettings.ScreenSpaceReflectionQuality = 0.0f;
	CaptureSettings.bOverride_ScreenPercentage = true;
	CaptureSettings.ScreenPercentage = 100.0f;

	CaptureComponent->bCaptureEveryFrame = false;
	CaptureComponent->bCaptureOnMovement = false;
	CaptureComponent->LODDistanceFactor = 3;
	CaptureComponent->TextureTarget = nullptr;
	CaptureComponent->bEnableClipPlane = true;
	CaptureComponent->bUseCustomProjectionMatrix = true;
	CaptureComponent->CaptureSource = ESceneCaptureSource::SCS_SceneColorSceneDepth;
	CaptureComponent->PostProcessSettings = CaptureSettings;
}

// Called when the game starts or when spawned
void APortalManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APortalManager::UpdatePortals(float DeltaTime)
{
	APortalSystemPlayerController* PlayerController = Cast<APortalSystemPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PlayerController != nullptr)
	{
		APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
		if (CameraManager != nullptr)
		{
			for (TActorIterator<APortal> It(GetWorld()); It; ++It)
			{
				APortal* Portal = *It;
				APortal* Target = Portal->GetTarget();
				if (Portal != nullptr && Target != nullptr)
				{
					FVector Location = ConvertLocation(CameraManager->GetCameraLocation(), Portal, Target);
					CaptureComponent->SetWorldLocation(Location);
					FRotator Rotation = ConvertRotation(CameraManager->GetCameraRotation(), Portal, Target);
					CaptureComponent->SetWorldRotation(Rotation);
					CaptureComponent->ClipPlaneNormal = Target->GetActorForwardVector();
					CaptureComponent->ClipPlaneBase = Target->GetActorLocation() + (CaptureComponent->ClipPlaneNormal * -1.5f);;
					CaptureComponent->CustomProjectionMatrix = OwningController->GetCameraProjectionMatrix();
					CaptureComponent->TextureTarget = Portal->GetRenderTarget();
					CaptureComponent->CaptureScene();
				}
			}
		}
	}
}

void APortalManager::RequestTeleport(APortal* Portal, AActor* TeleportTarget)
{

}

FVector APortalManager::ConvertLocation(FVector Location, AActor* Portal, AActor* Target)
{
	FVector Direction = Location - Portal->GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();

	FVector Dots;
	Dots.X = FVector::DotProduct(Direction, Portal->GetActorForwardVector());
	Dots.Y = FVector::DotProduct(Direction, Portal->GetActorRightVector());
	Dots.Z = FVector::DotProduct(Direction, Portal->GetActorUpVector());

	FVector NewDirection = Dots.X * -Target->GetActorForwardVector()
		+ Dots.Y * -Target->GetActorRightVector()
		+ Dots.Z * Target->GetActorUpVector();

	return TargetLocation + NewDirection;
}

FRotator APortalManager::ConvertRotation(FRotator Rotation, AActor* Portal, AActor* Target)
{
	FTransform SourceTransform = Portal->GetActorTransform();
	FTransform TargetTransform = Target->GetActorTransform();
	Rotation.Yaw -= 180.0f;
	FQuat QuatRotation = FQuat(Rotation);

	FQuat LocalQuat = SourceTransform.GetRotation().Inverse() * QuatRotation;
	FQuat NewWorldQuat = TargetTransform.GetRotation() * LocalQuat;
	FRotator Final = NewWorldQuat.Rotator();
	return Final;
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
		RedPortal->SetTarget(nullptr);
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
		BluePortal->SetTarget(nullptr);
	}

	RedPortal = NewPortal;

	if (BluePortal != nullptr)
	{
		RedPortal->SetTarget(BluePortal);
		BluePortal->SetTarget(RedPortal);
	}
}
