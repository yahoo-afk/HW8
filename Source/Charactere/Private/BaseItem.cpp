// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values
ABaseItem::ABaseItem()
{
	//틱사용 안함
	PrimaryActorTick.bCanEverTick = false;
	//신루트 설정
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);
	//콜리전이라는 이름의 스피어박스 달아주고
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	// 콜리전 설정을 오버랩 올 다이나믹으로해줌 움직이는 객체들 모두감지
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	// 신루트밑에붙임
	Collision->SetupAttachment(Scene);

	//스테틱매쉬 만들어서
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	//콜리전에 붙임
	StaticMesh->SetupAttachment(Collision);

	//*********이벤트 바인딩 아주중요함********
	// 콜리전으로 충돌이벤트 시작 끝 일때 호출되는 함수불러옴
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ABaseItem::OnItemOverLap);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ABaseItem::OnItemEndOverLap);
}

//오버랩 함수의 매개변수들 
// 온 아이템 오버랩함수의 인자정리

//인자를 이렇게 6개를 사용하면 언리얼엔진의 충돌함수인가? 하게되고명단에 올리고  
// 그걸 위에 Collision->OnComponentBeginOverlap.AddDynamic(this, &ABaseItem::OnItemOverLap);
// 이코드에서 연결시켜줌?
void ABaseItem::OnItemOverLap(
	// 내 아이템의 무슨 부위가 부딪혀졌나?
	UPrimitiveComponent* Overlappedcomp,
	// 나말고 다른 액터가 무었인가?
	AActor* OtherActor,
	//나말고 다른 엑터의 어느부위인가?
	UPrimitiveComponent* OtherComp,
	// 몇번째 인덱스??
	int32 OtherBodyIndex,
	//어떻게 부딪혔나(흝고 지나간건가?) ? true:false;
	bool bFromSweep,
	//충돌결과?? 부딪힌 위치나 각도
	const FHitResult& SweepResult)
{
	// 나말고다른엑터랑충돌했고 그엑터에 태그가 플레이어라면 
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{//오버랩 출력하고 아더엑터를활성화해라?
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, FString::Printf(TEXT("Overlap!!!!")));
		ActivateItem(OtherActor);
	}
}

void ABaseItem::OnItemEndOverLap(
	UPrimitiveComponent* OverlapComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{



}

void ABaseItem::ActivateItem(AActor* OverLapActor)
{
	UParticleSystemComponent* Particle = nullptr;

	if (PickupParticle)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			PickupParticle,
			GetActorLocation(),
			GetActorRotation(),
			false
		);
	}
	if(PickupSound)	
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			PickupSound,
			GetActorLocation()
		);
	}

	if (Particle && GetWorld())
	{
		FTimerHandle DestroyParticleTimerHandle;
		TWeakObjectPtr<UParticleSystemComponent> WeakParticle = Particle;

		GetWorld()->GetTimerManager().SetTimer(
			DestroyParticleTimerHandle,
			[WeakParticle]()
			{
				if (WeakParticle.IsValid())
				{
					WeakParticle->DestroyComponent();
				}
			},
			2.0f,
			false
		);
	}
}

// 에딧과 bp에등록
FName ABaseItem::GetItemType()const
{
	//아이템 타입을 반환
	return ItemType;
}

void ABaseItem::DestroyItem()
{
	Destroy();
}