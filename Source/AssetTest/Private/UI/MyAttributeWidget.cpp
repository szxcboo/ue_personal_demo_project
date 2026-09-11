#include "UI/MyAttributeWidget.h"

void UMyAttributeWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	
	BP_OnWidgetControllerSet();
}
