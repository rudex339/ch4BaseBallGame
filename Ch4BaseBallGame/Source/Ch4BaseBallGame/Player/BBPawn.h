// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BBPawn.generated.h"

UCLASS()
class CH4BASEBALLGAME_API ABBPawn : public APawn
{
	GENERATED_BODY()

public:

protected:
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;
	

};
