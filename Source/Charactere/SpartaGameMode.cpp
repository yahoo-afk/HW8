#include "SpartaGameMode.h"
#include "SpartaCharacter.h"
#include "SpartaPlayerController.h"
#include "SpartaGameState.h"
ASpartaGameMode::ASpartaGameMode()
{										// 스태틱 클래스는 원본틀 그자체를 가져오는방식?
											//아니그럼 원본가져와서 좋은게 뭔데? 메모리라도아껴짐?	
											// //메모리 많이아껴짐 왜아껴지는데
											// 100번 만들때 틀 하나로 만드는거랑 매번 틀만드는거랑 뭐가더 메모리가 덜들겠냐	
	DefaultPawnClass = ASpartaCharacter::StaticClass(); 
	PlayerControllerClass = ASpartaPlayerController::StaticClass();
	GameStateClass = ASpartaGameState::StaticClass();
}