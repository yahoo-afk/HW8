#include "HeaingItem.h"
#include "Charactere/SpartaCharacter.h"

AHeaingItem::AHeaingItem()
{
	HealAmount = 20;
	ItemType = "Healing";
}
void AHeaingItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
	//액터가 존재하고 액터가가지고있는 해시태그가 플레이어라면
	if (Activator && Activator->ActorHasTag("Player")) 
	{
		// 엑터가 캐릭터타입이라면
		if(ASpartaCharacter*PlayerCharacter = Cast<ASpartaCharacter>(Activator))
		{
			// 힐어마운트 호출 해서 체력회복
			PlayerCharacter->AddHealth(HealAmount);
		}
		//그후 아이템 소멸
		DestroyItem();
	}
}