// Fill out your copyright notice in the Description page of Project Settings.


#include "MathLibrary.h"
#include "Portal.h"
#include "GameFramework/Actor.h"

FVector UMathLibrary::ConvertLocation(FVector const& Location, AActor* Portal, AActor* Target)
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

FRotator UMathLibrary::ConvertRotation(FRotator const& Rotation, AActor* Portal, AActor* Target)
{
	FVector RotationAdjustment(0.f, 0.f, -180.f);
	FVector LocalAdjustment = FVector::ZeroVector;

	if (FVector::DotProduct(Portal->GetActorForwardVector(), FVector::UpVector) > KINDA_SMALL_NUMBER)
	{
		LocalAdjustment.X = FMath::UnwindDegrees(Portal->GetTransform().GetRotation().Euler().X);
		LocalAdjustment.Y = 180.f;
		RotationAdjustment.Z += LocalAdjustment.X;
	}
	else if (FVector::DotProduct(Portal->GetActorForwardVector(), FVector::UpVector) < -KINDA_SMALL_NUMBER)
	{
		LocalAdjustment.X = FMath::UnwindDegrees(Portal->GetTransform().GetRotation().Euler().X);
		LocalAdjustment.Y = -180.f;
		RotationAdjustment.Z -= LocalAdjustment.X;
	}

	FQuat QuatRotation = FQuat::MakeFromEuler(RotationAdjustment) * FQuat(Rotation);
	FQuat LocalQuat = FQuat::MakeFromEuler(LocalAdjustment) * Portal->GetActorTransform().GetRotation().Inverse() * QuatRotation;
	FQuat NewWorldQuat = Target->GetActorTransform().GetRotation() * LocalQuat;
	return NewWorldQuat.Rotator();
}

bool UMathLibrary::CheckIsInFront(FVector const& Point, FVector const& PortalLocation, FVector const& PortalNormal)
{
	FPlane PortalPlane = FPlane(PortalLocation, PortalNormal);

	return PortalPlane.PlaneDot(Point) >= 0;
}

bool UMathLibrary::CheckIsCrossing(FVector const& Point, FVector const& PortalLocation, FVector const& PortalNormal, bool& out_LastInFront, FVector& out_LastPosition)
{
	FPlane PortalPlane = FPlane(PortalLocation, PortalNormal);
	FVector IntersectionPoint;
	bool IsIntersecting = FMath::SegmentPlaneIntersection(out_LastPosition,	Point, PortalPlane, IntersectionPoint);
	bool IsInFront = CheckIsInFront(Point, PortalLocation, PortalNormal);
	bool IsCrossing = false;

	if (IsIntersecting && !IsInFront && out_LastInFront)
	{
		IsCrossing = true;
	}

	out_LastInFront = IsInFront;
	out_LastPosition = Point;

	return IsCrossing;
}

