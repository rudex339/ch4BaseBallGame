// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BBGameModeBase.generated.h"

class ABBPlayerController;
/**
 * 
 */
UCLASS()
class CH4BASEBALLGAME_API ABBGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	ABBGameModeBase();
	virtual void OnPostLogin(AController* NewPlayer) override;
	virtual void BeginPlay() override;

	FString GenerateSecretNumber();
	bool IsGuessNumberString(const FString& InNumberString);
	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);

	void PrintChatMessageString(ABBPlayerController* InChattingPlayerController, const FString& InChatMessageString);
	void IncreaseGuessCount(ABBPlayerController* InChattingPlayerController);
	void ResetGame();
	void JudgeGame(ABBPlayerController* InChattingPlayerController, int InStrikeCount);

	void OnTimerTick();


protected:
	FString SecretNumberString;
	TArray<TObjectPtr<ABBPlayerController>> AllPlayerControllers;

	FTimerHandle TimerHandle;
	int CurrentTurnPlayerIndex;

};
