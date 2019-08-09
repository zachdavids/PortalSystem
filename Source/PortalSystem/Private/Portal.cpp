// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"
#include "PortalSystemPlayerController.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneCaptureComponent2D.h"

// Sets default values
APortal::APortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	SetRootComponent(RootComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);

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
void APortal::BeginPlay()
{
	Super::BeginPlay();

	RenderTarget = NewObject<UTextureRenderTarget2D>(this, UTextureRenderTarget2D::StaticClass(), FName("RenderTarget"));
	RenderTarget->InitAutoFormat(1920, 1080);
	RenderTarget->AddressX = TextureAddress::TA_Wrap;
	RenderTarget->AddressY = TextureAddress::TA_Wrap;
	RenderTarget->bAutoGenerateMips = false;
	RenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA16f;
	RenderTarget->Filter = TextureFilter::TF_Bilinear;

	CaptureComponent->TextureTarget = RenderTarget;

	MaterialInstance = UMaterialInstanceDynamic::Create(Material, this);
	MaterialInstance->SetTextureParameterValue(FName("RenderTargetTexture"), RenderTarget);
	MeshComponent->SetMaterial(0, MaterialInstance);
}

// Called every frame
void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APortal::UpdateCapture()
{
	if (Target != nullptr)
	{
		APortalSystemPlayerController* PlayerController = Cast<APortalSystemPlayerController>(GetWorld()->GetFirstPlayerController());
		if (PlayerController != nullptr)
		{
			APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
			if (CameraManager != nullptr)
			{
				FVector Direction = CameraManager->GetCameraLocation() - GetActorLocation();
				FVector Dots;
				Dots.X = FVector::DotProduct(Direction, GetActorForwardVector());
				Dots.Y = FVector::DotProduct(Direction, GetActorRightVector());
				Dots.Z = FVector::DotProduct(Direction, GetActorUpVector());
				FVector NewDirection = Dots.X * Target->GetActorForwardVector()	+ Dots.Y * Target->GetActorRightVector() + Dots.Z * Target->GetActorUpVector();
				FVector NewLocation = Target->GetActorLocation() + NewDirection;
				CaptureComponent->SetWorldLocation(NewLocation);

				FTransform CameraTransform = CameraManager->GetTransformComponent()->GetComponentTransform();
				FTransform SourceTransform = GetActorTransform();
				FTransform TargetTransform = Target->GetActorTransform();
				FQuat LocalQuat = SourceTransform.GetRotation().Inverse() * CameraTransform.GetRotation();
				FQuat NewWorldQuat = TargetTransform.GetRotation() * LocalQuat;
				CaptureComponent->SetWorldRotation(NewWorldQuat);

				CaptureComponent->TextureTarget = RenderTarget;
				CaptureComponent->CustomProjectionMatrix = PlayerController->GetCameraProjectionMatrix();
				CaptureComponent->CaptureScene();
			}
		}
	}
}

void APortal::TeleportActor(AActor* ActorToTeleport)
{

}

void APortal::SetTarget(APortal* NewTarget)
{
	Target = NewTarget;
}

APortal* APortal::GetTarget()
{
	return Target;
}

