// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BBPlayerController.generated.h"

class UBBChatInputWidget;
class UUserWidget;
/**
 * 
 */
UCLASS()
class CH4BASEBALLGAME_API ABBPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ABBPlayerController();

	virtual void BeginPlay() override;

	void SetChatMessageString(const FString& InChatMessageString);

	void PrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Server, Reliable)
	void ServerRPCPrintChatMessageString(const FString& InChatMessageString);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;


protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBBChatInputWidget> ChatInputWidgetClass;

	UPROPERTY()
	TObjectPtr<UBBChatInputWidget> ChatInputWidgetInstance;

	FString ChatMessageString;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> NotificationTextWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> NotificationTextWidgetInstance;
public:
	UPROPERTY(Replicated, BlueprintReadOnly)
	FText NotificationText;
};
