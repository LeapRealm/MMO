#include "PlayerPawn.h"

#include "ClientGameInstance.h"
#include "ClientPacketHandler.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

APlayerPawn::APlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	SetRootComponent(CapsuleComponent);
	CapsuleComponent->SetCapsuleHalfHeight(88.f);
	CapsuleComponent->SetCapsuleRadius(35.f);
	CapsuleComponent->SetCollisionProfileName(FName("Pawn"));
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CapsuleComponent->SetCollisionObjectType(ECC_Pawn);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->TargetArmLength = 600.f;
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->bDoCollisionTest = false;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(GetRootComponent());
	SkeletalMeshComponent->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));
}

void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	ClientGameInstance = Cast<UClientGameInstance>(GetGameInstance());

	SimulatedPlayerTargetPosition = GetActorLocation();
	TargetRotation = GetActorRotation();
}

void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bPlayerControlled)
	{
		FHitResult HitResult;
		AddActorWorldOffset(ControlledPlayerVelocity * DeltaTime, true, &HitResult);

		if (HitResult.bBlockingHit)
			ControlledPlayerVelocity = FVector::ZeroVector;
		
		if (PrevHitResult.bBlockingHit == false && HitResult.bBlockingHit)
			ShouldSendMovePkt = true;
		
		PrevHitResult = HitResult;
		
		if (MovePktTime >= TargetMovePktTime || ShouldSendMovePkt)
			SendMovePacket();
		MovePktTime += DeltaTime;
	}
	else
	{
		SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), SimulatedPlayerTargetPosition, DeltaTime, MoveSpeed));
	}
	
	SetActorRotation(FMath::RInterpConstantTo(GetActorRotation(), TargetRotation, DeltaTime, RotationSpeed));
}

void APlayerPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	bPlayerControlled = true;
}

void APlayerPawn::HandleMove(FVector2D Input)
{
	FRotator Rotation = FRotator(0.f, GetControlRotation().Yaw, 0.f);
	FVector Forward = UKismetMathLibrary::GetForwardVector(Rotation);
	FVector Right = UKismetMathLibrary::GetRightVector(Rotation);
	
	FVector Direction = (Input.X * Forward + Input.Y * Right).GetSafeNormal();
	if (Input != FVector2D::ZeroVector)
		TargetRotation = FRotator(0.f, UKismetMathLibrary::Conv_VectorToRotator(Direction).Yaw, 0.f);

	ControlledPlayerVelocity = Direction * MoveSpeed;
	
	if (PrevInput != Input)
	{
		PrevInput = Input;
		ShouldSendMovePkt = true;
	}
}

void APlayerPawn::SendMovePacket()
{
	ShouldSendMovePkt = false;
	MovePktTime = 0.f;

	Protocol::C_MOVE MovePkt;
	Protocol::Vector3D* Position = MovePkt.mutable_transform()->mutable_position();
	MovePkt.mutable_transform()->set_yaw(TargetRotation.Yaw);
	
	FVector TargetPos = GetActorLocation() + (ControlledPlayerVelocity * 0.5f);
	Position->set_x(TargetPos.X);
	Position->set_y(TargetPos.Y);
	Position->set_z(TargetPos.Z);
	SEND_PACKET(MovePkt);
}
