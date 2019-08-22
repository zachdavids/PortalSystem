// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalManager.h"
#include "Portal.h"
#include "DrawDebugHelpers.h"
#include "PortalSystemCharacter.h"
#include "Components/ArrowComponent.h"
#include "PortalSystemPlayerController.h"
#include "MathLibrary.h"
#include "Components/SceneCaptureComponent2D.h"
#include "EngineUtils.h"

// Sets default values
APortalManager::APortalManager(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = ETickingGroup::TG_PostUpdateWork;

	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	SetRootComponent(RootComponent);

	CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(FName("CaptureComponent"));
	CaptureComponent->SetupAttachment(RootComponent);

	FPostProcessSettings CaptureSettings;
	CaptureSettings.bOverride_AmbientOcclusionQuality = true;
	CaptureSettings.bOverride_MotionBlurAmount = true;
	CaptureSettings.bOverride_SceneFringeIntensity = true;
	CaptureSettings.bOverride_GrainIntensity = true;
	CaptureSettings.bOverride_ScreenSpaceReflectionQuality = true;
	CaptureSettings.bOverride_ScreenPercentage = true;
	CaptureSettings.AmbientOcclusionQuality = 0.0f;
	CaptureSettings.MotionBlurAmount = 0.0f;
	CaptureSettings.SceneFringeIntensity = 0.0f;
	CaptureSettings.GrainIntensity = 0.0f;
	CaptureSettings.ScreenSpaceReflectionQuality = 0.0f;
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

void APortalManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdatePortals();
}

// Called when the game starts or when spawned
void APortalManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APortalManager::UpdatePortals()
{
	if (OwningController != nullptr)
	{
		APlayerCameraManager* CameraManager = OwningController->PlayerCameraManager;
		if (CameraManager != nullptr)
		{
			for (TActorIterator<APortal> It(GetWorld()); It; ++It)
			{
				APortal* Portal = *It;
				APortal* Target = Portal->GetTarget();
				if (Portal != nullptr && Target != nullptr)
				{
					FVector Position = UMathLibrary::ConvertLocation(CameraManager->GetCameraLocation(), Portal, Target);
					FRotator Rotation = UMathLibrary::ConvertRotation(CameraManager->GetCameraRotation(), Portal, Target);
					CaptureComponent->SetWorldLocationAndRotation(Position, Rotation);
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

void APortalManager::SetControllerOwner(APortalSystemPlayerController* NewOwner)
{
	OwningController = NewOwner;
}

void APortalManager::SetPortalClass(TSubclassOf<class APortal> Class)
{
	PortalClass = Class;
}

void APortalManager::SpawnBluePortal(const FVector& Start, const FVector& End)
{
	if (BluePortal != nullptr)
	{
		RedPortal->SetTarget(nullptr);
		BluePortal->Destroy();
	}

	BluePortal = SpawnPortal(RedPortal, FColor::Blue, Start, End);
}

void APortalManager::SpawnRedPortal(const FVector& Start, const FVector& End)
{
	if (RedPortal != nullptr)
	{
		BluePortal->SetTarget(nullptr);
		RedPortal->Destroy();
	}

    RedPortal = SpawnPortal(BluePortal, FColor::Red, Start, End);
}

APortal* APortalManager::SpawnPortal(APortal* Target, const FColor Color, const FVector& Start, const FVector& End)
{
	APortal* Portal = nullptr;

	UWorld* const World = GetWorld();
	if (World != nullptr && PortalClass != nullptr)
	{
		FHitResult OutHit;
		if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility))
		{
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			FVector Origin = OutHit.Location + OutHit.ImpactNormal;
			FRotator Rotation = OutHit.ImpactNormal.Rotation();
			if (FVector::DotProduct(OutHit.ImpactNormal, FVector::UpVector) > KINDA_SMALL_NUMBER)
			{
				Rotation.Roll -= GetActorRotation().Yaw;
			}
			else if (FVector::DotProduct(OutHit.ImpactNormal, FVector::UpVector) < -KINDA_SMALL_NUMBER)
			{
				Rotation.Roll += GetActorRotation().Yaw;
			}

			Portal = World->SpawnActor<APortal>(PortalClass, Origin, Rotation, ActorSpawnParams);
			Portal->SetPortalSurface(OutHit.GetActor());
			Portal->SetColor(Color);
			
			if (Target != nullptr)
			{
				Portal->SetTarget(Target);
				Target->SetTarget(Portal);
			}

			if (VerifyPortalPlacement(Portal, Origin))
			{
				Portal->SetActorLocation(Origin);
			}
			else
			{
				Target->SetTarget(nullptr);
				Portal->Destroy();
			}
		}
	}

	return Portal;
 }

bool APortalManager::VerifyPortalPlacement(const APortal* Portal, FVector& Origin)
{
	FVector OriginalOrigin = Origin;

	FVector Forward = Portal->GetActorForwardVector();
	FVector Right = Portal->GetActorRightVector();
	FVector Up = Portal->GetActorUpVector();

	//Check if surface is moving
	const AActor* Surface = Portal->GetPortalSurface();
	if (!Surface->GetVelocity().IsNearlyZero())
	{
		UE_LOG(LogTemp, Warning, TEXT("Portal surface moving"));
		return false;
	}

	//Check if portal is overlapping linked portal
	const APortal* Target = Portal->GetTarget();
	if (Target != nullptr)
	{
		FitPortalAroundTargetPortal(Portal, Target, Forward, Right, Up, Origin);
	}

	//Check if portal fits on surface
	FVector PortalExtent = Portal->CalculateComponentsBoundingBoxInLocalSpace().GetExtent();
	const FVector TopEdge = Up * PortalExtent.Z;
	const FVector BottomEdge = -TopEdge;
	const FVector RightEdge = Right * PortalExtent.Y;
	const FVector LeftEdge = -RightEdge;

	if (!FitPortalOnSurface(Portal, Forward, Right, Up, TopEdge, BottomEdge, RightEdge, LeftEdge, Origin))
	{
		UE_LOG(LogTemp, Warning, TEXT("Portal does not fit on surface"));
		return false;
	}

	return true;
}

void APortalManager::FitPortalAroundTargetPortal(const APortal* Portal, const APortal* Target, const FVector& Forward, const FVector& Right, const FVector& Up, FVector& Origin)
{
	FVector TargetForward = Target->GetActorForwardVector();

	//Reposition if portals are on the same face
	if (FVector::DotProduct(Forward, TargetForward) > 1.f - KINDA_SMALL_NUMBER)
	{
		FVector Distance = Origin - Target->GetActorLocation();
		FVector RightProjection = FVector::DotProduct(Distance, Right) * Right;
		FVector UpProjection = FVector::DotProduct(Distance, Up) * Up;

		float RightProjectionLength = RightProjection.Size();
		float UpProjectionLength = UpProjection.Size();

		if (RightProjectionLength < 1.0f)
		{
			RightProjection = Right;
		}

		FVector Size = Portal->CalculateComponentsBoundingBoxInLocalSpace().GetSize();
		if (UpProjectionLength < Size.Z && RightProjectionLength < Size.Y)
		{
			RightProjection.Normalize();
			Origin += RightProjection * (Size.Y - RightProjectionLength + 1.0f);
		}
	}
}

bool APortalManager::FitPortalOnSurface(const APortal* Portal, const FVector& Forward, const FVector& Right, const FVector& Up, const FVector& TopEdge, const FVector& BottomEdge, const FVector& RightEdge, const FVector& LeftEdge, FVector& Origin, int RecursionCount)
{
	if (RecursionCount == 0)
	{
		return false;
	}

	TArray<FVector> Corners;
	Corners.SetNumUninitialized(4);
	Corners[0] = Origin + TopEdge + LeftEdge;
	Corners[1] = Origin + TopEdge + RightEdge;
	Corners[2] = Origin + BottomEdge + LeftEdge;
	Corners[3] = Origin + BottomEdge + RightEdge;

	FHitResult HitResult;
	for (int i = 0; i < 4; ++i)
	{
		if (TraceCorner(Portal, Origin, Corners[i], Forward, Right, Up, HitResult))
		{
			float Distance = FMath::PointDistToLine(Corners[i], FVector::CrossProduct(HitResult.Normal, Forward), HitResult.Location) + 5.0f;
			Origin += HitResult.Normal * Distance;

			return FitPortalOnSurface(Portal, Forward, Right, Up, TopEdge, BottomEdge, RightEdge, LeftEdge, Origin, RecursionCount - 1);
		}
	}

	return true;
}

bool APortalManager::TraceCorner(const APortal* Portal, const FVector& Start, const FVector& End, const FVector& Forward, const FVector& Right, const FVector& Up, FHitResult& HitResult)
{
	bool bFoundHit = false;

	FCollisionQueryParams TraceParams;
	TraceParams.bFindInitialOverlaps = true;
	TraceParams.bTraceComplex = true;
	TraceParams.AddIgnoredActor(Portal);
	TraceParams.AddIgnoredActor(Portal->GetPortalSurface());

	if (OwningController != nullptr)
	{
		TraceParams.AddIgnoredActor(OwningController->GetCharacter());
	}

	//Check inner surface for intersections
	FHitResult InnerHitResult;
	if (GetWorld()->LineTraceSingleByChannel(InnerHitResult, Start - Forward, End - Forward, ECC_Visibility, TraceParams))
	{
		HitResult = InnerHitResult;
		bFoundHit = true;
	}

	//Check outer surface for intersections
	FHitResult OuterHitResult;
	if (GetWorld()->LineTraceSingleByChannel(OuterHitResult, Start + Forward, End + Forward, ECC_Visibility, TraceParams))
	{
		HitResult = OuterHitResult;
		bFoundHit = true;
	}

	//Determine the closer intersection, if any
	if (InnerHitResult.bBlockingHit && OuterHitResult.bBlockingHit)
	{
		HitResult = (InnerHitResult.Distance <= OuterHitResult.Distance) ? InnerHitResult : OuterHitResult;
	}

	//Check if corner overlaps surface, if not we reached the end of surface and fake it as collision
	FCollisionQueryParams VerticalTraceParams;
	TraceParams.bFindInitialOverlaps = true;
	TraceParams.bTraceComplex = true;
	TraceParams.AddIgnoredActor(Portal);

	if (OwningController != nullptr)
	{
		TraceParams.AddIgnoredActor(OwningController->GetCharacter());
	}

	FHitResult OverlapHitResult;

	float Fraction = 0.f;
	FVector Direction = End - Start;
	FVector Location(0.f);
	while (Fraction <= 1.0f + KINDA_SMALL_NUMBER)
	{
		Location = Start + (Direction * Fraction);
		if (!GetWorld()->LineTraceSingleByChannel(OverlapHitResult, Location + (Forward * 2), Location - (Forward * 2), ECC_Visibility, VerticalTraceParams))
		{
			//Found an edge now determine its normal
			FVector RightProjection = Direction.ProjectOnToNormal(Right);
			FVector UpProjection = Direction.ProjectOnToNormal(Up);

			int32 Vertical = GetWorld()->LineTraceSingleByChannel
			(
				OverlapHitResult, 
				Location + (RightProjection * 0.05) - (UpProjection * 0.05) + (Forward * 2),
				Location + (RightProjection * 0.05) - (UpProjection * 0.05) - (Forward * 2),
				ECC_Visibility, 
				VerticalTraceParams
			);

			int32 Horizontal = GetWorld()->LineTraceSingleByChannel
			(
				OverlapHitResult, 
				Location + (UpProjection * 0.05) - (RightProjection * 0.05) + (Forward * 2),
				Location + (UpProjection * 0.05) - (RightProjection * 0.05) - (Forward * 2),
				ECC_Visibility, 
				VerticalTraceParams
			);

			if (Vertical)
			{
				OverlapHitResult.Normal = -UpProjection.GetClampedToSize(0, 1.0f);
			}

			if (Horizontal)
			{
				OverlapHitResult.Normal = -RightProjection.GetClampedToSize(0, 1.0f);
			}

			//Corner, choose one
			if (!Vertical && !Horizontal)
			{
				OverlapHitResult.Normal = -UpProjection.GetClampedToSize(0, 1.0f);
			}

			OverlapHitResult.bBlockingHit = true;
			OverlapHitResult.Location = Location;
			OverlapHitResult.Distance = FVector::Distance(Start, OverlapHitResult.Location);
			OverlapHitResult.ImpactNormal = FVector::CrossProduct(OverlapHitResult.Normal, Forward);

			if (bFoundHit)
			{
				if (OverlapHitResult.Distance <= HitResult.Distance)
				{
					HitResult = OverlapHitResult;
				}
			}
			else
			{
				HitResult = OverlapHitResult;
			}

			bFoundHit = true;
			break;
		}

		OverlapHitResult.Reset();
		Fraction += 0.05;
	}

	return bFoundHit;
}
