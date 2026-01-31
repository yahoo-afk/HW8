#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemSpawnRow.h"

#include "SpawnVolme.generated.h"


class UBoxComponent;

UCLASS()
class CHARACTERE_API ASpawnVolme : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnVolme();
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Spawning")
	USceneComponent* Scene;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Spawning")
	UBoxComponent* SpawningBox;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Spawning")
	UDataTable* ItemDataTable;

	UFUNCTION(BlueprintCallable,Category = "Spawning")
	AActor* SpawnRandomItem();
	
	FItemSpawnRow* GetRandomItem()const;

	AActor* SpawnItem(TSubclassOf<AActor> ItemClass);

	FVector GetRandomPointInVolum()const;
		
};
