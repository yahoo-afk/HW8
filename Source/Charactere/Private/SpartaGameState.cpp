// Fill out your copyright notice in the Description page of Project Settings.
#include "SpartaGameState.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnVolme.h"
#include "CoinItem.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
//생성시 초기화
ASpartaGameState::ASpartaGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	LevelDuration = 30;
	CurrentLevelIndex = 0;
	MaxLevels = 3;

}
//부모클래스에 비긴플레이 호출후 레벨스타트함수호출
void ASpartaGameState::BeginPlay()
{
	Super::BeginPlay();

	StartLevel();

	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&ASpartaGameState::UpdateHUD,
		0.1f,
		true
	);
}
// 점수 호출
int32 ASpartaGameState::GetScore()const
{
	return Score;
}

//레벨이 변경될때 점수를 더하는 함수
void ASpartaGameState::AddScore(int32 Amount)
{
	//게임 인스턴트의 변수를 만들어서 현재 게임 인스턴스를 가져오고 nullptr이아니라면
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		//내가만든 스파르타인스턴타입으로 형변환을 해주고
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		// 널포인트가아니라면
		if (SpartaGameInstance)
		{
			// 토탈 점수를 불러온다 
			SpartaGameInstance->AddTotalScore(Amount);
		}
	}

}

//레벨 시작시 호출되는 함수
void ASpartaGameState::StartLevel()
{
	// 아이템 호출 3번 하는데 이걸 FTimer변수인 웨이브타미어로 10초마다 호출이라..

	WaveSpawnItem();

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaController->ShowGameHUD();
		}
	}

	// 유게임 인스턴슬로 변수를 만들고  현재 인스턴트 호출해서 담아줌
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		//그리고 내가만든 인스턴스랑 같은 타입이라면
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		// 스파르타 인스턴스가 비어있지않다면
		if (SpartaGameInstance)
		{
			//현재 레벨 인덱스를 스파르타 게임의 인스턴스의 인덱스로 교환
			CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
		}
	}


	//스폰카운터 0;
	SpawnedCoinCount = 0;
	//먹은 코인갯수 0;
	CollectedCoinCount = 0;

	//배열상자를 하나만들어 엑터의주소를 담음
	TArray<AActor*> FoundVolumes;
	// 유게임 플레이 월드에있는 스폰볼륨엑터를 담고 엑터의 주소를 담음?
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolme::StaticClass(), FoundVolumes);
	//아이템 40개 스폰
	const int32 ItemToSpawn = 30;

	for (int32 i = 0; i < ItemToSpawn; i++)
	{
		//아이템이 한개 이상일때만
		if (FoundVolumes.Num() > 0)
		{
			// 스폰볼륨의 포인터타임 변수에 파운드 볼륨하고 같다면
			ASpawnVolme* SpawnVolume = Cast<ASpawnVolme>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				//스폰액터에 스폰볼륨의 랜덤 아이템 함수를 불러오고?
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				// 스폰액터가있고 스폰엑터가 코인아이템의 원형이라면
				if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					// 코인아이템의 생성갯수를늘린다?
					SpawnedCoinCount++;
				}
			}
		}
	}


	GetWorld()->GetTimerManager().SetTimer(
		WaveTimer,
		this,
		&ASpartaGameState::WaveSpawnItem,
		10.0f,
		true
	);


	// 타이머 매니저함수를 가져와서 타이머셋팅 
	GetWorldTimerManager().SetTimer(
		//F타이머핸들의 변수 레벨 타이머 변수만들고
		LevelTimerHandle,
		// 호출한 자기함수?
		this,
		/// 스파르타 게임스테이트에있는타임업함수 호출
		&ASpartaGameState::OnLevelTimeUp,
		//30초
		LevelDuration,
		//반복 X
		false
	);
}

void ASpartaGameState::OnLevelTimeUp()
{
	//시간이 종료되면 앤드레벨호출
	EndLevel();
}

void ASpartaGameState::EndLevel()
{
	//레벨이끝나면 타이머 초기화
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);
	GetWorldTimerManager().ClearTimer(WaveTimer);
	WaveCount = 0;

	// 인스턴스가져와서 
	if (UGameInstance* GameInstance = GetGameInstance())
	{//캐스팅해주고 점수전레벨이전하고 맵인덱스 더해주고 인덱스 변환
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		// 스타트게임 인스턴스가 널포인트가아니라면
		if (SpartaGameInstance)
		{
			// 점수더하는 함수호출
			AddScore(Score);
			// 현재 인덱스 늘리기
			CurrentLevelIndex++;
			// 늘린인덱스로 맵변경하기
			SpartaGameInstance->CurrentLevelIndex = CurrentLevelIndex;
		}
	}
	// 인덱스가 더커져버리면 게임오버리턴
	if (CurrentLevelIndex >= MaxLevels)
	{
		OnGameOver();
		return;
	}
	//레벨 FNAME들을 저장한 레벨맵네임을 통해서 현제 인덱스를 불러오고
	if (LevelMapName.IsValidIndex(CurrentLevelIndex))
	{
		//오픈레벨 함수에서 월드를 불러오고 레벨맵이름안에 현재 인덱스번호를통해 
		// 현재 레벨을 열어라가되는건가?
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapName[CurrentLevelIndex]);
	}
	else
	{
		//맵이없다면 게임오버?
		OnGameOver();
	}
}


void ASpartaGameState::OnCoinCollected()
{
	//함수호출되면 코인획득갯수증가
	CollectedCoinCount++;
	UE_LOG(LogTemp, Warning, TEXT("Coin Collected: %d / %d")
		, CollectedCoinCount, SpawnedCoinCount);
	// 스폰된 코인이 0보다 많고 획득한코인이 생성된코인보다 많거나 같으면
	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
	{
		//레벨 종료 다음레벨로
		EndLevel();
	}
}
void ASpartaGameState::OnGameOver()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaController->SetPause(true);
			SpartaController->ShowMainMenu(true);
		}
	}
}

void ASpartaGameState::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
			{
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time:%.1f"), RemainingTime)));
				}

				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
						if (SpartaGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score:%d"), SpartaGameInstance->TotalScore)));
						}
					}
				}

				if (UTextBlock* LevelIndex = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
				{
					LevelIndex->SetText(FText::FromString(FString::Printf(TEXT("Level:%d"), CurrentLevelIndex + 1)));
				}
			}
		}
	}
}

void ASpartaGameState::WaveSpawnItem()
{
	if (WaveCount >= 3)return;
	else
	{
		TArray<AActor*> FoundVolumes;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolme::StaticClass(), FoundVolumes);
		const int32 WaveSpawnItem = 10;
		for (int32 i = 0; i < WaveSpawnItem; i++)
		{
			if (FoundVolumes.Num() > 0)
			{
				ASpawnVolme* SpawnVolume = Cast<ASpawnVolme>(FoundVolumes[0]);
				if (SpawnVolume)
				{

					AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();

					if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
					{
						SpawnedCoinCount++;
					}
				}
			}
		}
		FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
		if (CurrentLevelName != TEXT("MenuLevel"))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString(TEXT("아이템이 추가로 생성되었습니다!")));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT(" 현재 웨이브: %d"), WaveCount + 1));
			WaveCount++;
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("레벨에 따라 폭탄의 법위가 늘어납니다 웨이브마다 아이템이 랜덤으로 10개 생성됩니다")));
		}
	}
}