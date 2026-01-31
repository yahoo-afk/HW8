#include "SpartaCharacter.h"
#include "SpartaPlayerController.h"
#include "SpartaGameState.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"

ASpartaCharacter::ASpartaCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.0f;
	//카메라랑 거리
	SpringArmComp->bUsePawnControlRotation = true;
	// 캐릭터 이동할때 카메라 회전도같이 하게 켜달라고하는거
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	OverHeadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidget"));
	OverHeadWidget->SetupAttachment(GetMesh());
	OverHeadWidget->SetWidgetSpace(EWidgetSpace::Screen);

	NomalSpeed = 600;
	SprintSeedMultiplier = 2;
	SprintSpeed = NomalSpeed * SprintSeedMultiplier;

	GetCharacterMovement()->MaxWalkSpeed = NomalSpeed;

	MaxHp = 100;
	Hp = MaxHp;
}


// 그럼애는 언제호출되는거냐? 
// 내가 선택한 게임모드 플레이어 컨트롤러에 애를 집어넣으면 
// 그게임모드에서 캐릭터가 생성될때마다 애를 가지고 태어남 
	//이인풋 컨트롤러는 오래된 컨트롤러임그래서 이걸 신형으로 
	// 바꿔줌
	//그냥처음부터 신형꽂으면 되는거아님?
	// 그게안됨 언리얼은 너무 방대해서 기존컨트롤러를 
	// 메인으로두고 너네가 필요하면 cast해서 
	// 사용하라고 권장하는거임.
//그래서 이제이거 nullptr이라면 
// 조건문안으로 안보내줄것
//형변환이 실패하면
// nullptr이들어가게됨;

//nullptr도아니고 0도아니라서 들어왔다 
// 그리고나서 애를 스파르타 형식으로바꾸고 나서 
//널이 아니라면 다음 이프문으로 보낸다?
// 이동입력액션IA호출
///이벤트 발동했을때
// 호출된 함수의 객채를
// 연결을시키겠다?
void ASpartaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::Move
				);
			}
			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::StartJump
				);


				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaCharacter::StopJump
				);

			}
			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::Look
				);
			}
			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::StartSprint
				);


				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaCharacter::StopSprint
				);

			}
		}
	}
}
void ASpartaCharacter::BeginPlay()
{
	Super::BeginPlay();
	UpdateOverHeadHP();
}


// 이친구안에는 필요로하는게 대부분있으니꺼내서 사용할수있게 Value라는 변수를 만들어줄게
	//이번엔 FVector 2D가필요한가보내 자 movinput내가 Fvector2D로바꿔줄께 이제써 
		//애는 점프가속도를 0으로만들어버림
		//중력하고는 다른역할임
void ASpartaCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller) return;// 컨트롤러있냐? 없으면 종료

	const FVector2D MoveInput = Value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}
void ASpartaCharacter::StartJump(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		Jump();
	}
}
void ASpartaCharacter::StopJump(const FInputActionValue& Value)
{
	if (!Value.Get<bool>())
	{
		StopJumping();
	}
}
void ASpartaCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookInput = Value.Get<FVector2D>();
	AddControllerYawInput(LookInput.X); //마우스 x축
	AddControllerPitchInput(LookInput.Y); // 마우스 Y축
}
void ASpartaCharacter::StartSprint(const FInputActionValue& Value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}
void ASpartaCharacter::StopSprint(const FInputActionValue& Value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NomalSpeed;
	}
}

float ASpartaCharacter::GetHealth()const
{
	return Hp;
}

float ASpartaCharacter::TakeDamage(
	float DamageAmount,
	struct FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser
)
{		//실제데미지
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Hp = FMath::Clamp(Hp - DamageAmount, 0.0f, MaxHp);
	UpdateOverHeadHP();

	if (Hp <= 0)
	{
		OnDeath();
	}
	return ActualDamage;
}

void ASpartaCharacter::OnDeath()
{
	ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
	if(SpartaGameState)
	{
		SpartaGameState->OnGameOver();
	}
} 

void ASpartaCharacter::AddHealth(float Amount)
{
	Hp = FMath::Clamp(Hp + Amount, 0.0f, MaxHp);
	UpdateOverHeadHP();

}

void ASpartaCharacter::UpdateOverHeadHP()
{
	if (!OverHeadWidget)return;

	UUserWidget* OverHeadWidgetInctance = OverHeadWidget->GetUserWidgetObject();
	if (!OverHeadWidgetInctance) return;

	if (UTextBlock* HPText = Cast<UTextBlock>(OverHeadWidgetInctance->GetWidgetFromName(TEXT("OverHeadHP"))))
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f/%.0f"), Hp, MaxHp)));
	}
}