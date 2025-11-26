// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BBGameModeBase.h"
#include "BBGameStateBase.h"
#include "Player/BBPlayerController.h"
#include "EngineUtils.h"
#include "Player/BBPlayerState.h"

ABBGameModeBase::ABBGameModeBase():
	CurrentTurnPlayerIndex(0)
{	
}

void ABBGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	ABBPlayerController* BBPlayerController = Cast<ABBPlayerController>(NewPlayer);
	if (IsValid(BBPlayerController) == true)
	{
		BBPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));
		AllPlayerControllers.Add(BBPlayerController);

		ABBPlayerState* BBPS = BBPlayerController->GetPlayerState<ABBPlayerState>();
		if (IsValid(BBPS) == true)
		{
			BBPS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
		}

		ABBGameStateBase* BBGameStateBase = GetGameState<ABBGameStateBase>();
		if (IsValid(BBGameStateBase) == true)
		{
			BBGameStateBase->MulticastRPCBroadcastLoginMessage(BBPS->PlayerNameString);
		}
	}
}

FString ABBGameModeBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0; });

	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	return Result;
}

bool ABBGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	//check baseball input?
	bool bCanPlay = false;

	do {
		
		if (InNumberString.Len() != 3)
		{
			break;
		}

		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			if (FChar::IsDigit(C) == false || C == '0')
			{
				bIsUnique = false;
				break;
			}

			UniqueDigits.Add(C);
		}

		if (bIsUnique == false)
		{
			break;
		}

		bCanPlay = true;

	} while (false);

	return bCanPlay;
}

FString ABBGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}

void ABBGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	SecretNumberString = GenerateSecretNumber();
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ABBGameModeBase::OnTimerTick, 30.f, true);
}

void ABBGameModeBase::PrintChatMessageString(ABBPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	int Index = InChatMessageString.Len() - 3;
	FString GuessNumberString = InChatMessageString.RightChop(Index);	
	ABBPlayerState* BBPS = InChattingPlayerController
		->GetPlayerState<ABBPlayerState>();
	if (!IsValid(BBPS))return;

	if (IsGuessNumberString(GuessNumberString) == true && BBPS->turn)
	{
		IncreaseGuessCount(InChattingPlayerController);
		FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);		
		for (TActorIterator<ABBPlayerController> It(GetWorld()); It; ++It)//월드의 모든 컨트롤러를 순회
		{
			ABBPlayerController* BBPlayerController = *It;
			if (IsValid(BBPlayerController) == true)
			{
				FString CombinedMessageString = InChatMessageString + TEXT(" -> ") + JudgeResultString;
				BBPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);
				int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
				JudgeGame(InChattingPlayerController, StrikeCount);
			}
		}
		GetWorldTimerManager().ClearTimer(TimerHandle);
		OnTimerTick();
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ABBGameModeBase::OnTimerTick, 30.f, true);
	}
	else
	{
		for (TActorIterator<ABBPlayerController> It(GetWorld()); It; ++It)//월드의 모든 컨트롤러를 순회
		{
			ABBPlayerController* BBPlayerController = *It;
			if (IsValid(BBPlayerController) == true)
			{
				BBPlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
			}
		}
	}
}

void ABBGameModeBase::IncreaseGuessCount(ABBPlayerController* InChattingPlayerController)
{
	ABBPlayerState* BBPS = InChattingPlayerController->GetPlayerState<ABBPlayerState>();
	if (IsValid(BBPS) == true)
	{
		BBPS->CurrentGuessCount++;
	}
}

void ABBGameModeBase::ResetGame()
{
	GetWorldTimerManager().ClearTimer(TimerHandle);
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ABBGameModeBase::OnTimerTick, 30.f, true);

	CurrentTurnPlayerIndex = 0;

	SecretNumberString = GenerateSecretNumber();

	for (const auto& BBPlayerController : AllPlayerControllers)
	{
		ABBPlayerState* BBPS = BBPlayerController->GetPlayerState<ABBPlayerState>();
		if (IsValid(BBPS) == true)
		{
			BBPS->CurrentGuessCount = 0;
		}
	}

	for (const auto& BBPlayerController : AllPlayerControllers)
	{
		BBPlayerController->RemainTime = 30.f;
	}

	ABBPlayerState* BBPS = AllPlayerControllers[CurrentTurnPlayerIndex]
		->GetPlayerState<ABBPlayerState>();
	if (IsValid(BBPS) == true)
	{
		BBPS->turn = false;
	}

}

void ABBGameModeBase::JudgeGame(ABBPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (3 == InStrikeCount)
	{
		ABBPlayerState* BBPS = InChattingPlayerController->GetPlayerState<ABBPlayerState>();
		for (const auto& BBPlayerController : AllPlayerControllers)
		{
			if (IsValid(BBPS) == true)
			{
				FString CombinedMessageString = BBPS->PlayerNameString + TEXT(" has won the game.");
				BBPlayerController->NotificationText = FText::FromString(CombinedMessageString);

				ResetGame();
			}
		}
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& BBPlayerController : AllPlayerControllers)
		{
			ABBPlayerState* BBPS = BBPlayerController->GetPlayerState<ABBPlayerState>();
			if (IsValid(BBPS) == true)
			{
				if (BBPS->CurrentGuessCount <BBPS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}
		if (true == bIsDraw)
		{
			for (const auto& BBPlayerController : AllPlayerControllers)
			{
				BBPlayerController->NotificationText = FText::FromString(TEXT("Draw..."));
			}
			ResetGame();
		}
	}
}

void ABBGameModeBase::OnTimerTick() {
	//CurrentTurnPlayerIndex 위치 의 turn false
	for (const auto& BBPlayerController : AllPlayerControllers)
	{
		BBPlayerController->RemainTime = 30.f;
	}

	ABBPlayerState* BBPS = AllPlayerControllers[CurrentTurnPlayerIndex]
		->GetPlayerState<ABBPlayerState>();
	if (IsValid(BBPS) == true)
	{
		BBPS->turn = false;
	}

	CurrentTurnPlayerIndex++;
	CurrentTurnPlayerIndex %= AllPlayerControllers.Num();

	//CurrentTurnPlayerIndex 위치 의 turn true
	AllPlayerControllers[CurrentTurnPlayerIndex]
		->ClientRPCPrintChatMessageString(TEXT("your turn"));
	BBPS = AllPlayerControllers[CurrentTurnPlayerIndex]
		->GetPlayerState<ABBPlayerState>();
	if (IsValid(BBPS) == true)
	{
		BBPS->turn = true;
	}

}