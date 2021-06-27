// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/DriftCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Drift/Public/Characters/DriftCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"



FNetworkPredictionData_Client* UDriftCharacterMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		// Return our custom client prediction class instead
		UDriftCharacterMovementComponent* MutableThis = const_cast<UDriftCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FDriftNetworkPredictionData_Client(*this);
	}

	return ClientPredictionData;
}

void UDriftCharacterMovementComponent::FDriftSavedMove::Clear()
{
	Super::Clear();

	SavedRequestToStartSprinting = false;
}

uint8 UDriftCharacterMovementComponent::FDriftSavedMove::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (SavedRequestToStartSprinting)
	{
		Result |= FLAG_Custom_0;
	}



	return Result;
}

bool UDriftCharacterMovementComponent::FDriftSavedMove::CanCombineWith(const FSavedMovePtr& NewMove,
	ACharacter* Character, float MaxDelta) const
{
	
	//Set which moves can be combined together. This will depend on the bit flags that are used.
	if (SavedRequestToStartSprinting != ((FDriftSavedMove*)&NewMove)->SavedRequestToStartSprinting)
	{
		return false;
	}



	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

void UDriftCharacterMovementComponent::FDriftSavedMove::SetMoveFor(ACharacter* Character, float InDeltaTime,
	FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	UDriftCharacterMovementComponent* CharacterMovement = Cast<UDriftCharacterMovementComponent>(Character->GetCharacterMovement());
	if (CharacterMovement)
	{
		SavedRequestToStartSprinting = CharacterMovement->RequestToStartSprinting;
	
	}
}

void UDriftCharacterMovementComponent::FDriftSavedMove::PrepMoveFor(ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	UDriftCharacterMovementComponent* CharacterMovement = Cast<UDriftCharacterMovementComponent>(Character->GetCharacterMovement());
	if (CharacterMovement)
	{
	}
}

UDriftCharacterMovementComponent::FDriftNetworkPredictionData_Client::FDriftNetworkPredictionData_Client(
	const UCharacterMovementComponent& ClientMovement)	: Super(ClientMovement)
{
	
}

FSavedMovePtr UDriftCharacterMovementComponent::FDriftNetworkPredictionData_Client::AllocateNewMove()
{
	return FSavedMovePtr(new FDriftSavedMove());
}


void UDriftCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	RequestToStartSprinting = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
}

float UDriftCharacterMovementComponent::GetMaxSpeed() const
{
	ADriftCharacterBase* Owner = Cast<ADriftCharacterBase>(GetOwner());
	if (!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() No Owner"), *FString(__FUNCTION__));
		return Super::GetMaxSpeed();
	}



	// if (Owner->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun"))))
	// {
	// 	return 0.0f;
	// }

	// if (RequestToStartSprinting)
	// {
	// 	return Owner->GetMoveSpeed() * SprintSpeedMultiplier;
	// }

	// if (RequestToStartADS)
	// {
	// 	return Owner->GetMoveSpeed() * ADSSpeedMultiplier;
	// }
	// UE_LOG(LogTemp, Warning, TEXT("Speed:  %f "), Owner->GetMoveSpeed() );
	return Owner->GetMoveSpeed();
}

void UDriftCharacterMovementComponent::StartSprinting()
{
	RequestToStartSprinting = true;
}

void UDriftCharacterMovementComponent::StopSprinting()
{
	RequestToStartSprinting = false;
}

