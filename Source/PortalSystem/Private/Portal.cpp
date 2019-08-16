// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"
#include "MathLibrary.h"
#include "DrawDebugHelpers.h"
#include "PortalSystemCharacter.h"
#include "PortalSystemPlayerController.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
APortal::APortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	RootComponent->Mobility = EComponentMobility::Static;
	SetRootComponent(RootComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("MeshComponent"));
	MeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	MeshComponent->SetupAttachment(RootComponent);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(FName("BoxComponent"));
	BoxComponent->SetBoxExtent(FVector(20, 100, 150));
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &APortal::OnOverlapBegin);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &APortal::OnOverlapEnd);
	BoxComponent->SetupAttachment(RootComponent);
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

	MaterialInstance = UMaterialInstanceDynamic::Create(Material, this);
	MaterialInstance->SetTextureParameterValue(FName("RenderTargetTexture"), RenderTarget);
	MeshComponent->SetMaterial(0, MaterialInstance);
}

// Called every frame
void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Overlapping)
	{
		TeleportActors();
	}
}

void APortal::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(APortalSystemCharacter::StaticClass()))
	{
		Overlapping = true;
		if (PortalSurface != nullptr)
		{
			PortalSurface->SetActorEnableCollision(false);
		}
	}
}

void APortal::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(APortalSystemCharacter::StaticClass()))
	{
		Overlapping = false;
		if (PortalSurface != nullptr)
		{
			PortalSurface->SetActorEnableCollision(true);
		}
	}
}

void APortal::TeleportActors()
{
	APortalSystemPlayerController* PlayerController = Cast<APortalSystemPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PlayerController != nullptr)
	{
		APortalSystemCharacter* Character = Cast<APortalSystemCharacter>(PlayerController->GetCharacter());
		if (Character != nullptr)
		{
			if (UMathLibrary::CheckIsCrossing(Character->GetActorLocation(), GetActorLocation(), GetActorForwardVector(), bLastInFront, LastPosition))
			{
				UE_LOG(LogTemp, Warning, TEXT("Teleported to %s"), *Target->GetName());
				FVector CurrentVelocity = Character->GetCharacterMovement()->Velocity;

				FHitResult HitResult;
				FVector Location = UMathLibrary::ConvertLocation(Character->GetActorLocation(), this, Target);
				FRotator Rotation = UMathLibrary::ConvertRotation(Character->GetActorRotation(), this, Target);
				Character->SetActorLocationAndRotation(Location, Rotation, false, &HitResult, ETeleportType::TeleportPhysics);
				PlayerController->SetControlRotation(UMathLibrary::ConvertRotation(PlayerController->GetControlRotation(), this, Target));

				//TODO Add Previous Velocity
				Character->GetCharacterMovement()->Velocity = FVector::ZeroVector;

				LastPosition = Location;

				Overlapping = false;
			}
		}
	}
}

void APortal::SetTarget(APortal* NewTarget)
{
	if (NewTarget != nullptr)
	{
		Target = NewTarget;
	}
}

void APortal::SetPortalSurface(AActor* Surface)
{
	PortalSurface = Surface;
}

void APortal::SetColor(FColor Color)
{
	MaterialInstance->SetVectorParameterValue(FName("Color"), Color);
}

APortal* APortal::GetTarget()
{
	return Target;
}

UTextureRenderTarget2D* APortal::GetRenderTarget()
{
	return RenderTarget;
}