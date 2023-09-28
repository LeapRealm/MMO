#include "Game/ClientPlayer.h"

#include "Components/CapsuleComponent.h"

AClientPlayer::AClientPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GetCapsuleComponent()->InitCapsuleSize(42.f, 88.0f);
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
		const Protocol::Transform& DesiredTransform = DesiredPlayerInfo->transform();
		FVector DesiredLocation = FVector(DesiredTransform.x(), DesiredTransform.y(), DesiredTransform.z());
		float Dist2DSquared = FVector::DistSquared2D(GetActorLocation(), DesiredLocation);
		float DistZ = FMath::Abs(GetActorLocation().Z - DesiredLocation.Z);

		FVector2D NewLocation2D;
		if (Dist2DSquared < MoveThresholdSquared * 2.f)
			NewLocation2D = FMath::Vector2DInterpTo(FVector2D(GetActorLocation()), FVector2D(DesiredLocation), DeltaTime, 4.5f);
		else if (Dist2DSquared < MoveThresholdSquared * 8.f)
			NewLocation2D = FMath::Vector2DInterpConstantTo(FVector2D(GetActorLocation()), FVector2D(DesiredLocation), DeltaTime, 800.f);
		else
			NewLocation2D = FVector2D(DesiredLocation);

		float NewLocationZ;
		if (DistZ < MoveThreshold)
			NewLocationZ = FMath::FInterpConstantTo(GetActorLocation().Z, DesiredLocation.Z, DeltaTime, 180.f);
		else if (DistZ < MoveThreshold * 2.f)
			NewLocationZ = FMath::FInterpTo(GetActorLocation().Z, DesiredLocation.Z, DeltaTime, 4.5f);
		else if (DistZ < MoveThreshold * 8.f)
			NewLocationZ = FMath::FInterpConstantTo(GetActorLocation().Z, DesiredLocation.Z, DeltaTime, 800.f);
		else
			NewLocationZ = DesiredLocation.Z;
		
		SetActorLocation(FVector(NewLocation2D, NewLocationZ));
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
