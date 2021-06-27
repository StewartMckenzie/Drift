// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DriftCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class DRIFT_API UDriftCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	class FDriftSavedMove : public FSavedMove_Character
	{
	public:

		typedef FSavedMove_Character Super;

		// Resets all saved variables.
		virtual void Clear() override;
		// Store input commands in the compressed flags.
		virtual uint8 GetCompressedFlags() const override;
		// This is used to check whether or not two moves can be combined into one.
		// Basically you just check to make sure that the saved variables are the same.
		virtual bool CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* Character, float MaxDelta) const override;
		// Sets up the move before sending it to the server. 
		virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;
		// Sets variables on character movement component before making a predictive correction.
		virtual void PrepMoveFor(class ACharacter* Character) override;

	private:
		uint8 SavedRequestToStartSprinting : 1;

	};


	class FDriftNetworkPredictionData_Client : public FNetworkPredictionData_Client_Character
	{
	public:
		typedef FNetworkPredictionData_Client_Character Super;

		// Constructor
		FDriftNetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement);

		//brief Allocates a new copy of our custom saved move
		virtual FSavedMovePtr AllocateNewMove() override;
	};


	public:






	uint8 RequestToStartSprinting : 1;
	uint8 RequestToStartADS : 1;

	virtual float GetMaxSpeed() const override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	// Sprint
	UFUNCTION(BlueprintCallable, Category = "Sprint")
	void StartSprinting();
	UFUNCTION(BlueprintCallable, Category = "Sprint")
	void StopSprinting();


};



