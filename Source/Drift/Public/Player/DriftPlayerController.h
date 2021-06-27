// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DriftPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class DRIFT_API ADriftPlayerController : public APlayerController
{
	GENERATED_BODY()

	protected:
	virtual void OnPossess(APawn* InPawn) override;
};

