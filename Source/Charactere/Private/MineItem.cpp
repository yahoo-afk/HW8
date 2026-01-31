// Fill out your copyright notice in the Description page of Project Settings.


#include "MineItem.h"
#include "Components/SphereComponent.h"
#include "SpartaGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AMineItem::AMineItem()
{
	//폭발데미지 30 
	ExplosionDamage = 30;
	//폭발 반경
	ExplosionRadius = 400;
	//폭팔 딜레이
	ExplosionDelay = 5.f;
	//테이블에 올라갈 FNAME이름
	ItemType = "Mine";
	bHaaExploded = false;

	// 폭발콜리전을 스피어 컴포넌트로 만들고
	ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExpoisionCollision"));
	// 스피어컴포넌트 300으로 설정
	ExplosionCollision->InitSphereRadius(ExplosionRadius);
	//콜리전 설정 오버랩다이나믹으로 설정 이건 오버랩시 발동되는 종류중에하나
	ExplosionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	//콜리전을 신에달아줌
	ExplosionCollision->SetupAttachment(Scene);
}
void AMineItem::ActivateItem(AActor* Activator)
{
	if (bHaaExploded) return;
	Super::ActivateItem(Activator);
	ASpartaGameState* LevelIndex = Cast<ASpartaGameState>(GetWorld()->GetGameState());
	if (LevelIndex)
	{
		ExplosionRadius *= LevelIndex->CurrentLevelIndex + 1;
		if (ExplosionCollision)
		{
			ExplosionCollision->SetSphereRadius(ExplosionRadius);
		}
	}
	//월드에서 매니저한테 이시간 으로 요청하고 5초후에 마인 아이템 함수 호출해줘 반복은 안할래
	GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandle, this, &AMineItem::Explode, ExplosionDelay, false);
	bHaaExploded = true;
}

void AMineItem::Explode()
{
	UParticleSystemComponent* Particle = nullptr;
	if (ExplosionParticle)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ExplosionParticle,
			GetActorLocation(),
			GetActorRotation(),
			false
		);
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			ExplosionSound,
			GetActorLocation()
		);
	}


	//TArray안에 엑터들의 주소를담는 상자들을 하나만들어서
	TArray<AActor*> OverlappingActor;
	//익스플로전 콜리전으로겟 오버랩핑액터함수호출 이건 언리얼 자체함수인듯? 역할은 모르겟음
	ExplosionCollision->GetOverlappingActors(OverlappingActor); //애네는 마인이라느 아이템이 소멸될때 다같이죽는거같음
	// 반복문 맥터의 주소로 오버랩된액터를 확인하고
	for (AActor* Actor : OverlappingActor)
	{
		//액터가 존재하고 액터의 해쉬태그가 플레이어라면
		if (Actor && Actor->ActorHasTag("Player"))
		{
			// 무슨 함수인지는 모르겠지만
			UGameplayStatics::ApplyDamage(
				// 액터의주소(플레이어의주소)
				Actor,
				// 폭발데미지
				ExplosionDamage,
				//널??은 모르겠다
				nullptr,
				// 액터를가르키고
				this,
				// 원형틀을 가져오고
				UDamageType::StaticClass()
			);
		}
	}
	//아이템 소멸
	DestroyItem();

	if (Particle)
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