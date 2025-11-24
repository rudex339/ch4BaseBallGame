// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BBPawn.h"
#include "Ch4BaseBallGame.h"


void ABBPawn::BeginPlay()
{
	Super::BeginPlay();

	FString NetRoleString = BBFunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("CXPawn::BeginPlay() %s [%s]"), *BBFunctionLibrary::GetNetModeString(this), *NetRoleString);
	BBFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}

void ABBPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	FString NetRoleString = BBFunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("CXPawn::PossessedBy() %s [%s]"), *BBFunctionLibrary::GetNetModeString(this), *NetRoleString);
	BBFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}

