#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyHUD.generated.h"

class UMyAttributeWidget;

/**
 * 
 */
UCLASS()
class ASSETTEST_API AMyHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<UMyAttributeWidget>  OverlayWidget;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UMyAttributeWidget> OverlayWidgetClass;
};
