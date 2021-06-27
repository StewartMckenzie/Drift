// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "DriftAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class DRIFT_API UDriftAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	public:
	bool CharacterAbilitiesGiven = false;
	bool StartupEffectsApplied = false;
};
