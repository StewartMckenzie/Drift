// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "DriftPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class DRIFT_API ADriftPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

	public:
	ADriftPlayerState();

	// Implement IAbilitySystemInterface
	class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	class UDriftAttributeSetBase* GetAttributeSetBase() const;

	/**
	* Getters for attributes from DriftAttributeSetBase. Returns Current Value unless otherwise specified.
	*/

	UFUNCTION(BlueprintCallable, Category = "Drift|DriftPlayerState|Attributes")
	float GetMoveSpeed() const;


	protected:
	UPROPERTY()
	class UDriftAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	class UDriftAttributeSetBase* AttributeSetBase;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
