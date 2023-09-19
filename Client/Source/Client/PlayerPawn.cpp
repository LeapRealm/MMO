#include "PlayerPawn.h"

#include "ClientGameInstance.h"
#include "ClientPacketHandler.h"
#include "Kismet/KismetMathLibrary.h"

APlayerPawn::APlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	TargetPosition = GetActorLocation();
	ClientGameInstance = Cast<UClientGameInstance>(GetGameInstance());
}

void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), TargetPosition, DeltaTime, Speed));
	MovePktTime += DeltaTime;
}

void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void APlayerPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void APlayerPawn::HandleMove(FVector2D Input)
{
	FRotator Rotation = FRotator(0.f, GetControlRotation().Yaw, 0.f);
	FVector Forward = UKismetMathLibrary::GetForwardVector(Rotation);
	FVector Right = UKismetMathLibrary::GetRightVector(Rotation);

	FVector Direction = (Input.X * Forward + Input.Y * Right).GetSafeNormal();
	AddActorWorldOffset(Direction * GetWorld()->DeltaTimeSeconds * Speed);
	TargetPosition = GetActorLocation();

	if (MovePktTime >= TargetMovePktTime || PrevInput != Input)
	{
		MovePktTime = 0.f;

		Protocol::C_MOVE MovePkt;
		Protocol::Vector3D* Position = MovePkt.mutable_position();

		FVector Pos = GetActorLocation();
		if (Input != FVector2D::ZeroVector)
			Pos += Direction * 75.f;
	
		Position->set_x(Pos.X);
		Position->set_y(Pos.Y);
		Position->set_z(Pos.Z);
		SEND_PACKET(MovePkt);
	}

	PrevInput = Input;
}
