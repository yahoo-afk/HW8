// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "HeaingItem.generated.h"

/**
 * 
 */
UCLASS()
class CHARACTERE_API AHeaingItem : public ABaseItem
{
	GENERATED_BODY()
public:
	AHeaingItem();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category ="Item")
	int32 HealAmount;

	virtual void ActivateItem(AActor* Activator) override;

};
