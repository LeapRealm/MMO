#include "Game/ClientPlayer.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AClientPlayer::AClientPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 1.f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bRunPhysicsWithNoController = true;

	CurrentPlayerInfo = new Protocol::PlayerInfo();
	DesiredPlayerInfo = new Protocol::PlayerInfo();

	MoveThresholdSquared = MoveThreshold * MoveThreshold;
}

AClientPlayer::~AClientPlayer()
{
	if (CurrentPlayerInfo)
	{
		delete CurrentPlayerInfo;
		CurrentPlayerInfo = nullptr;
	}

	if (DesiredPlayerInfo)
	{
		delete DesiredPlayerInfo;
		DesiredPlayerInfo = nullptr;
	}
}

void AClientPlayer::BeginPlay()
{
	Super::BeginPlay();
}

void AClientPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (IsMyPlayer() == false)
	{
		// TODO: 너무 많이 차이나면 순간이동
		FVector CurrentLocation = GetActorLocation();
		const Protocol::Transform& DesiredTransform = DesiredPlayerInfo->transform();
		const FVector DesiredLocation = FVector(DesiredTransform.x(), DesiredTransform.y(), DesiredTransform.z());
		
		if (FVector::DistSquaredXY(CurrentLocation, DesiredLocation) > MoveThresholdSquared)
		{
			FVector Direction = (DesiredLocation - CurrentLocation).GetSafeNormal();
			AddMovementInput(Direction);
		}

		SetActorRotation(FMath::RInterpConstantTo(GetActorRotation(), FRotator(0, DesiredTransform.yaw(), 0), DeltaTime, 360.f));
	}

	UpdateCurrentPlayerInfo();
}

void AClientPlayer::UpdateCurrentPlayerInfo()
{
	FVector Location = GetActorLocation();
	Protocol::Transform* Transform = CurrentPlayerInfo->mutable_transform();
	Transform->set_x(Location.X);
	Transform->set_y(Location.Y);
	Transform->set_z(Location.Z);

	if (bIsMyPlayer)
		Transform->set_yaw(GetControlRotation().Yaw);
	else
		Transform->set_yaw(GetActorRotation().Yaw);
}

void AClientPlayer::Init(bool IsMyPlayer, const Protocol::PlayerInfo& Info)
{
	bIsMyPlayer = IsMyPlayer;
	
	SetCurrentPlayerInfo(Info);
	SetDesiredPlayerInfo(Info);
}

void AClientPlayer::SetCurrentPlayerInfo(const Protocol::PlayerInfo& Info)
{
	if (CurrentPlayerInfo->objectid())
		assert(PlayerInfo->object_id() == Info.object_id());

	CurrentPlayerInfo->CopyFrom(Info);

	const Protocol::Transform& Transform = Info.transform();
	FVector Location(Transform.x(), Transform.y(), Transform.z());
	SetActorLocation(Location);
	
	FRotator Rotation(0.f, Transform.yaw(), 0.f);
	if (bIsMyPlayer)
		GetController()->SetControlRotation(Rotation);
	else
		SetActorRotation(Rotation);
}

void AClientPlayer::SetDesiredPlayerInfo(const Protocol::PlayerInfo& Info)
{
	if (CurrentPlayerInfo->objectid())
		assert(PlayerInfo->object_id() == Info.object_id());

	DesiredPlayerInfo->CopyFrom(Info);
}

void AClientPlayer::SetMoveState(Protocol::MoveState State)
{
	if (CurrentPlayerInfo->movestate() == State)
		return;

	CurrentPlayerInfo->set_movestate(State);
}
