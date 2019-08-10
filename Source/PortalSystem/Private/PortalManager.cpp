// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalManager.h"
#include "Portal.h"
#include "PortalSystemCharacter.h"
#include "PortalSystemPlayerController.h"
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
	CaptureComponent->bUseCustomProjectionMatrix = false;
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
					CaptureComponent->SetWorldLocation(ConvertLocation(CameraManager->GetCameraLocation(), Target, Portal));
					CaptureComponent->SetWorldRotation(ConvertRotation(CameraManager->GetCameraRotation(), Target, Portal));
					CaptureComponent->ClipPlaneBase = Target->GetActorLocation();
					CaptureComponent->ClipPlaneBase = Target->GetActorForwardVector();
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
	FTransform PortalTransform = Portal->GetActorTransform();
	FVector PortalScale = PortalTransform.GetScale3D();
	PortalTransform.SetScale3D(FVector(-PortalScale.X, -PortalScale.Y, PortalScale.Z));

	return Target->GetActorTransform().TransformPosition(PortalTransform.InverseTransformPosition(Location));
}

FRotator APortalManager::ConvertRotation(FRotator Rotation, AActor* Portal, AActor* Target)
{
	FRotationMatrix R(Rotation);
	FVector AxisX = R.GetScaledAxis(EAxis::X);
	FVector AxisY = R.GetScaledAxis(EAxis::Y);

	FVector DirectionX = ConvertDirection(AxisX, Portal, Target);
	FVector DirectionY = ConvertDirection(AxisY, Portal, Target);

	return FRotationMatrix::MakeFromXY(DirectionX, DirectionY).Rotator();
}

FVector APortalManager::ConvertDirection(FVector Direction, AActor* Portal, AActor* Target)
{
	FVector InverseDirectionT = Portal->GetActorTransform().InverseTransformVectorNoScale(Direction);
	InverseDirectionT = FMath::GetReflectionVector(InverseDirectionT, (FVector(1.0f, 0, 0)));
	InverseDirectionT = FMath::GetReflectionVector(InverseDirectionT, (FVector(0, 1.0f, 0)));

	return Target->GetActorTransform().TransformVectorNoScale(InverseDirectionT);
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
