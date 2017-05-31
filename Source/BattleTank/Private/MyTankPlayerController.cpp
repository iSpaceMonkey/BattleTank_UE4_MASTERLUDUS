// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleTank.h"
#include "Tank.h"
#include "MyTankPlayerController.h"


void AMyTankPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (GetControlledTank()) {
		UE_LOG(LogTemp, Warning, TEXT("Player Controller a le Tank : %s"), * GetControlledTank()->GetName());
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Player Controller n'a pas le Tank"));
	}
}

void AMyTankPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AimTowardsCrosshair();
}

ATank * AMyTankPlayerController::GetControlledTank() const
{
	return Cast<ATank>(GetPawn());
}

void AMyTankPlayerController::AimTowardsCrosshair()
{
	if (!GetControlledTank()) { return; }
		// Get world location if linetrace through crosshair
		// If it hits the landscape
		// Tell controlled tank to aim at this point
	FVector HitLocation; // Out parameter
	if (GetSightRayHitLocation(HitLocation)) // Has "side-effect", is going to line trace
		{
			GetControlledTank()->AimAt(HitLocation);
		}
}

// Get world location of linetrace through crosshair, true if hits landscape
bool AMyTankPlayerController::GetSightRayHitLocation(FVector& HitLocation) const
{
	// Find the crosshair position in pixel coordinates
	int32 ViewportSizeX, ViewportSizeY;
	GetViewportSize(ViewportSizeX, ViewportSizeY);
	auto ScreenLocation = FVector2D(ViewportSizeX * CrosshairXLocation, ViewportSizeY * CrosshairYLocation);
	// "De-project" the screen position of the crosshair to a world direction
	FVector LookDirection;
	if (GetLookDirection(ScreenLocation, LookDirection))
		{
		UE_LOG(LogTemp, Warning, TEXT("Hit location: %s"), *HitLocation.ToString());
		// Line-trace along that LookDirection, and see what we hit (up to max range)
		GetLookVectorHitLocation(LookDirection, HitLocation);
		}

	return true;
}

bool AMyTankPlayerController::GetLookVectorHitLocation(FVector LookDirection, FVector& HitLocation) const
{
	FHitResult HitResult;
	auto StartLocation = PlayerCameraManager->GetCameraLocation();
	auto EndLocation = StartLocation + (LookDirection * LineTraceRange);
	if (GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		ECollisionChannel::ECC_Visibility)
		)
		{
		HitLocation = HitResult.Location;
		return true;
		}
	HitLocation = FVector(0);
	return false; // Line trace didn't succeed
}

bool AMyTankPlayerController::GetLookDirection(FVector2D ScreenLocation, FVector& LookDirection) const
{
	FVector CameraWorldLocation; // To be discarded
	return DeprojectScreenPositionToWorld(
		ScreenLocation.X,
		ScreenLocation.Y,
		CameraWorldLocation,
		LookDirection
		);
}