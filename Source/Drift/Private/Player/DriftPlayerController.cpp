// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DriftPlayerController.h"
#include "AbilitySystemComponent.h"
#include "Characters/Heros/DriftHeroCharacter.h"
#include "Player/DriftPlayerState.h"

void ADriftPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ADriftPlayerState* PS = GetPlayerState<ADriftPlayerState>();
	if (PS)
	{
		// Init ASC with PS (Owner) and our new Pawn (AvatarActor)
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, InPawn);
	}
}
