#include "Game/ClientPlayer.h"

#include "Client.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AClientPlayer::AClientPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GetCapsuleComponent()->InitCapsuleSize(42.f, 88.0f);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bRunPhysicsWithNoController = false;
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
		const Protocol::Transform& DesiredTransform = DesiredPlayerInfo->transform();
		FVector CurrentLocation = GetActorLocation();
		FVector DesiredLocation = FVector(DesiredTransform.x(), DesiredTransform.y(), DesiredTransform.z());

		float Distance = FVector::Distance(CurrentLocation, DesiredLocation);
		float DistanceZ = FMath::Abs(CurrentLocation.Z - DesiredLocation.Z);
		if (Distance > 200.f)
		{
			SetActorLocation(DesiredLocation);
		}
		else
		{
			float Speed = 7.5f;
			if (DistanceZ > 3.f || Distance < 20.f)
				Speed *= 2.f;
		
			SetActorLocation(FMath::VInterpTo(CurrentLocation, DesiredLocation, DeltaTime, Speed));
		}
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), FRotator(0, DesiredTransform.yaw(), 0), DeltaTime, 6.5f));
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
