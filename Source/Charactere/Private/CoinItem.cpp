// Fill out your copyright notice in the Description page of Project Settings.


#include "CoinItem.h"
#include "Engine/World.h"
#include "SpartaGameState.h"

ACoinItem::ACoinItem()
{
	PointValue = 0;
	ItemType = "DefaultCoin";
}

// 아이템 획득? 사용 효과
void ACoinItem::ActivateItem(AActor* Activator) // 이함수는 월드가 사라지면같이 사라짐
{												// 다른코드를보면 점수는 레벨이 새로생기면 이전레벨에서 생성된점수를 다시넘겨주는형식이라 
							
	Super::ActivateItem(Activator);//다음 레벨에서 다시호출되는 형식같음
	// 엑터가 존재하고 엑터의 태그가 플레이어라면
	if (Activator && Activator->ActorHasTag("Player"))
	{
		// 월드를 요청해서 받고
		if(UWorld* World =GetWorld()) //여기도레벨에서 초기화
		{
			//스파르타 게임스테이트의포인터로만든 게임스테이트라는 변수에 월드에 게임스테이트를 스파르타 게임스테이트로 변환해서 호출?
			if(ASpartaGameState*GameState = World->GetGameState<ASpartaGameState>())
			{
				// 점수도증가 코인종류에따라 20점 50점 획득
				GameState->AddScore(PointValue); // 코인을 먹고나면 오르는 점수는 그레벨에서만 적용됨
				// 코인아이템을 얻으면 갯수증가
				//GameState->OnCoinCollected();//코인갯수도 레벨에서만 적용
			}
		}
		//아이템 소멸
		DestroyItem();
	}
}