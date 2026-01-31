// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolme.h"
#include "Components/BoxComponent.h"

// Sets default values
ASpawnVolme::ASpawnVolme()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//틱함수 미사용
	PrimaryActorTick.bCanEverTick = false;
	//신 루트 하나만들고
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);
	// 박스 사이즈 정해주고
	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawningBox->SetupAttachment(Scene);

	ItemDataTable = nullptr;
}

//액터가 스폰될때 스폰범위지정
FVector ASpawnVolme::GetRandomPointInVolum()const
{
	//박스의 크기 
	FVector BoxExtent = SpawningBox->GetScaledBoxExtent();//박스크기
	//박스의 중심좌표
	FVector BoxOrigin = SpawningBox->GetComponentLocation();//중심좌표

	return BoxOrigin + FVector(
		// 랜덤으로 정해라  -x부터 +x 박스크기의의 
		FMath::FRandRange(-BoxExtent.X, BoxExtent.X),

		// 랜덤으로 정해라  -y부터 +y 박스크기의의 
		FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),

		// 랜덤으로 정해라  -z부터 + z박스크기의의 
		FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z)
	);
	//랜덤으로 생성해라
}

// 아이템 스폰 함수
AActor* ASpawnVolme::SpawnItem(TSubclassOf<AActor> ItemClass)
{
	// 아이템클레스가없다면 널포인트 리턴헤라
	if (!ItemClass)return nullptr;
	// 있다면 엑터 만들어서 아이템 클래스 받아오고 랜덤스폰함수 불러오고 회전값은 0으로
	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
		ItemClass,
		GetRandomPointInVolum(),
		FRotator::ZeroRotator
	);
	//그리고 만들어진 엑터 반환 
	return SpawnedActor;
}

//
AActor* ASpawnVolme::SpawnRandomItem()
{
	//아이템이 스폰랜덤 셀렉티드로으 에 겟아이템을 가져오고
	if (FItemSpawnRow* SelectedRow = GetRandomItem())
	{
		// 유클래스의 엑튜얼 클래스에 다시 만들어주고 
		if (UClass* ActualClass = SelectedRow->ItemClass.Get())
		{
			//해당클래스의 아이템을 스폰
			return SpawnItem(ActualClass);
		}
	}
	return nullptr;
}

//
FItemSpawnRow* ASpawnVolme::GetRandomItem()const
{
	// 아이템 데이터테이블이없다면 널포인트리턴
	if (!ItemDataTable) return nullptr;
	//아이템 스포 로우의 포인터 타입의 올로우 변수생성
	TArray<FItemSpawnRow*>AllRow;
	//FSTRING 변수 콘텍스트스트링으로 아이텐 스폰 콘텍스트를만들고
	static const FString ContextString(TEXT("ItemSpawnContext"));
	//테이블에서 정보가져옴	
	ItemDataTable->GetAllRows(ContextString, AllRow);
	// 얼로우가 비어있다면 널포인트
	if (AllRow.IsEmpty())return nullptr;
	// 최종찬스를 0으로 초기화
	float TotalChance = 0.0f;
	// 모든 아이템 배열을 로우에 담음	
	for (const FItemSpawnRow* Row : AllRow)
	{
		if (Row)
		{
			//확률을다더해서 100이되게
			TotalChance += Row->Spawnchance;
		}
	}
	//  0~ 전체 확률사이의 랜덤한값 하나생성
	const float RandValue = FMath::FRandRange(0.0f, TotalChance);
	// 누적 확률 계산용변수
	float AccmulateChance = 0.0f;
	//순서대로 확률 누적하면서
	for (FItemSpawnRow* Row : AllRow)
	{
		//랜던값이 들어오면 그아이템이 선택됨
		AccmulateChance += Row->Spawnchance;
		if (RandValue <= AccmulateChance)
		{
			return Row;
		}
	}
	// 예외처리널ptr;
	return nullptr;
}
