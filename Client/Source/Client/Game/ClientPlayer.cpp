#include "Game/ClientPlayer.h"

#include "Components/CapsuleComponent.h"

AClientPlayer::AClientPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
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
		const Protocol::Transform& DesiredTransform = DesiredPlayerInfo->transform();
		FVector DesiredLocation = FVector(DesiredTransform.x(), DesiredTransform.y(), DesiredTransform.z());

		if (FVector::Distance(GetActorLocation(), DesiredLocation) < 50.f)
		{
			SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), DesiredLocation, DeltaTime, 300.f));
		}
		else
		{
			SetActorLocation(FMath::VInterpTo(GetActorLocation(), DesiredLocation, DeltaTime, 6.f));
		}
		// SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), FVector(FVector2D(DesiredLocation), GetActorLocation().Z), DeltaTime, 350.f));
		// if (DesiredLocation.Z > GetActorLocation().Z)
		// 	SetActorLocation(FMath::VInterpTo(GetActorLocation(), FVector(FVector2D(GetActorLocation()), (DesiredLocation.Z)), DeltaTime, 700.f));
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
