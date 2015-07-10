

#include "NetGameCPP.h"
#include "NetGameCPPHUD.h"

ANetGameCPPHUD::ANetGameCPPHUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UFont>HUDFontOb(TEXT("Font'/Game/Font/OpenSans48.OpenSans48'"));
	MyFont = HUDFontOb.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D>HUDTextureOb(TEXT("Texture2D'/Game/Textures/HUDInterface.HUDInterface'"));
	MyTexture = HUDTextureOb.Object;
}

void ANetGameCPPHUD::DrawHUD()
{
	// Query the character at each iteration
	ANetGameCPPCharacter *TheChar = Cast<ANetGameCPPCharacter>(GetOwningPlayerController()->GetPawn());

	if (!TheChar)
		return;

	// Draweing the attribute's frame
	int32 X = Canvas->SizeX - MyTexture->GetSizeX() - 20;
	DrawTexture(MyTexture, (float)X, 20.0f, (float)MyTexture->GetSizeX(), (float)MyTexture->GetSizeY(), 0, 0, 1, 1);

	// Drawing the attributes
	float w, h;

	const FString TextHealth = FString::FromInt(FMath::CeilToInt(TheChar->Health));
	GetTextSize(TextHealth, w, h, MyFont);
	DrawText(TextHealth, FLinearColor(1.0f, 1.0f, 1.0f, 0.5f), (float)(X + 102) - w / 2, 138.0f - h / 2, MyFont);

	const FString TextBombs = FString::FromInt(TheChar->BombCount);
	GetTextSize(TextBombs, w, h, MyFont);
	DrawText(TextBombs, FLinearColor(1.0f, 1.0f, 1.0f, 0.5f), (float)(X + 263) - w / 2, 138.0f - h / 2, MyFont);

	float Top = 138.0f;

	for ( TActorIterator<ANetGameCPPCharacter> Itr(GetWorld()); Itr; ++Itr )
	{
		FLinearColor c;

		ANetGameCPPCharacter *curr = *Itr;

		if (curr == TheChar)
			c = FLinearColor(1.0f, 0.f, 0.f, 0.5f);
		else
			c = FLinearColor(1.0f, 1.f, 1.f, 0.5f);

		const FString TextIDValue = FString::FromInt(curr->TSId);
		GetTextSize(TextIDValue, w, h, MyFont);
		DrawText(TextIDValue, c, 50.f - w / 2, Top - h / 2, MyFont);

		Top += 50.f;
	}
}


