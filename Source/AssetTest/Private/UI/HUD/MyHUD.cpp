#include "UI/HUD/MyHUD.h"

#include "UI/MyAttributeWidget.h"

void AMyHUD::BeginPlay()
{
	Super::BeginPlay();

	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	Widget->AddToViewport();
}
