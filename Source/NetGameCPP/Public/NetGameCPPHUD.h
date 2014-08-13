

#pragma once

#include "GameFramework/HUD.h"
#include "NetGameCPPCharacter.h"
#include "NetGameCPPHUD.generated.h"

/**
 * 
 */
UCLASS()
class NETGAMECPP_API ANetGameCPPHUD : public AHUD
{
	GENERATED_UCLASS_BODY()

	
	UPROPERTY()
	UFont* MyFont;

	UPROPERTY()
	UTexture2D* MyTexture;

	virtual void DrawHUD() override;
};
