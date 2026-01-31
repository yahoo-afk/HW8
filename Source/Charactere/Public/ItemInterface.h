// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UItemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class CHARACTERE_API IItemInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//지뢰 힐링 코인 
	//힐링 코인 즉발
	//지뢰는 범위내에 오버랩되면 5초뒤 폭발 5초뒤오버랩 유지라면 데미지
	UFUNCTION()
	virtual void OnItemOverLap(
		UPrimitiveComponent* Overlappedcomp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) = 0;
		//오버랩이 발생한 자기자신 (this포인터)느낌?
		// 나와 부딪힌 엑터 
		//부딪힌 엑터와붙어있던 컴포넌트
		//애네 세개는 물리 시뮬연관이라 일단패스
	UFUNCTION()
	virtual void OnItemEndOverLap
	(UPrimitiveComponent* OverlapComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex) = 0;
	virtual void ActivateItem(AActor* OverLapActor) = 0;
	virtual FName GetItemType()const = 0;


	 

};
