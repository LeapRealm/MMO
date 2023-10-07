#include "Game/MonsterBase.h"

#include "Client.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

AMonsterBase::AMonsterBase()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 88.0f);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -100.f), FRotator(0.f, -90.f, 0.f));
}

AMonsterBase::~AMonsterBase()
{
	if (CurrentMonsterInfo)
	{
		delete CurrentMonsterInfo;
		CurrentMonsterInfo = nullptr;
	}

	if (DesiredMonsterInfo)
	{
		delete DesiredMonsterInfo;
		DesiredMonsterInfo = nullptr;
	}
}

void AMonsterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AMonsterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const Protocol::Transform& DesiredTransform = DesiredMonsterInfo->transform();
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

	UpdateCurrentMonsterInfo();
}

void AMonsterBase::UpdateCurrentMonsterInfo()
{
	FVector Location = GetActorLocation();
	Protocol::Transform* Transform = CurrentMonsterInfo->mutable_transform();
	Transform->set_x(Location.X);
	Transform->set_y(Location.Y);
	Transform->set_z(Location.Z);
	Transform->set_yaw(GetActorRotation().Yaw);
}

void AMonsterBase::Init(const Protocol::MonsterInfo& Info)
{
	SetCurrentMonsterInfo(Info);
	SetDesiredMonsterInfo(Info);
}

void AMonsterBase::Attack()
{
	PlayAnimMontage(AttackMontage);
}

void AMonsterBase::SetCurrentMonsterInfo(const Protocol::MonsterInfo& Info)
{
	if (CurrentMonsterInfo->objectid())
		assert(CurrentMonsterInfo->object_id() == Info.object_id());

	CurrentMonsterInfo->CopyFrom(Info);

	const Protocol::Transform& Transform = Info.transform();
	FVector Location(Transform.x(), Transform.y(), Transform.z());
	SetActorLocation(Location);
	
	FRotator Rotation(0.f, Transform.yaw(), 0.f);
	SetActorRotation(Rotation);
}

void AMonsterBase::SetDesiredMonsterInfo(const Protocol::MonsterInfo& Info)
{
	if (DesiredMonsterInfo->objectid())
		assert(DesiredMonsterInfo->object_id() == Info.object_id());

	float prevYaw = DesiredMonsterInfo->transform().yaw();
	DesiredMonsterInfo->CopyFrom(Info);

	FVector A = FVector(CurrentMonsterInfo->transform().x(), CurrentMonsterInfo->transform().y(), 0.f);
	FVector B = FVector(DesiredMonsterInfo->transform().x(), DesiredMonsterInfo->transform().y(), 0.f);
	if (UKismetMathLibrary::Vector_Distance(A, B) > 0.f)
		DesiredMonsterInfo->mutable_transform()->set_yaw(UKismetMathLibrary::FindLookAtRotation(A, B).Yaw);
	else
		DesiredMonsterInfo->mutable_transform()->set_yaw(prevYaw);
}
