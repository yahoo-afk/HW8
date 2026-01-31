// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpartaCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
struct FInputActionValue;

UCLASS()
class CHARACTERE_API ASpartaCharacter : public ACharacter
{
	GENERATED_BODY()
	// 스린모드는 고정된것처럼 보임 항상 정면에서보임
	//월드모드는 캐릭터의 움직임에따라 글씨도돌아감 글씨가 안보일수도있음

public:
	ASpartaCharacter();

	//수정을 불가능한게맞는데 내부속성은 조정가능함
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "UI")
	UWidgetComponent* OverHeadWidget;


	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth()const;

	UFUNCTION(BlueprintCallable,Category="Health")
	void AddHealth(float Amount);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Hp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHp;

	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser
	)override;


	UFUNCTION()
	void Move(const FInputActionValue& Value);//참조로 가져오는 이유는 
	//객체의 모든 데이터를 복사해서 가져오기때문에 참조로 복사비용줄이기
	UFUNCTION()
	void StartJump(const FInputActionValue& Value);
	UFUNCTION()
	void StopJump(const FInputActionValue& Value);
	UFUNCTION()
	void Look(const FInputActionValue& Value);
	UFUNCTION()
	void StartSprint(const FInputActionValue& Value);
	UFUNCTION()
	void StopSprint(const FInputActionValue& Value);



	void OnDeath();
	void UpdateOverHeadHP();

private:
	float NomalSpeed;
	float SprintSeedMultiplier;
	float SprintSpeed;


};
