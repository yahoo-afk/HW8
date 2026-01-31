// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SpartaGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class CHARACTERE_API USpartaGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	USpartaGameInstance();

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="GameData")
	int32 TotalScore;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="GameData")
	int CurrentLevelIndex;

	UFUNCTION(BlueprintCallable,Category = "GameData")
	void AddTotalScore(int32 Amount);
};
