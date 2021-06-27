// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "Drift/Drift.h"
#include "DriftCharacterBase.generated.h"


class UDriftCharacterMovementComponent;
UCLASS()
class DRIFT_API ADriftCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// // Sets default values for this character's properties
	ADriftCharacterBase(const class FObjectInitializer& ObjectInitializer);
	// Implement IAbilitySystemInterface
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	class UDriftAttributeSetBase* GetAttributeSetBase() const;
	// Gets the character's MyCustomMovementComponent
	UFUNCTION(BlueprintCallable, Category = "Movement")
	UDriftCharacterMovementComponent* GetMyMovementComponent() const;

	// Switch on AbilityID to return individual ability levels. Hardcoded to 1 for every ability in this project.
	UFUNCTION(BlueprintCallable, Category = "Drift|DriftCharacter")
	virtual int32 GetAbilityLevel(EGDAbilityInputID AbilityID) const;


	UFUNCTION(BlueprintCallable, Category = "Drift|DriftCharacter|Attributes")
	int32 GetCharacterLevel() const;
	
	UFUNCTION(BlueprintCallable, Category = "Drift|DriftCharacter|Attributes")
	float GetMoveSpeed() const;

	// Gets the Base value of MoveSpeed
	UFUNCTION(BlueprintCallable, Category = "Drift|DriftCharacter|Attributes")
	float GetMoveSpeedBaseValue() const;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	TWeakObjectPtr<class UDriftAbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<class UDriftAttributeSetBase> AttributeSetBase;


	UPROPERTY(BlueprintReadOnly, EditAnywhere,Category = "Drift|DriftCharacterTest")
	FText CharacterName;

	// Death Animation
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Drift|Animation")
	UAnimMontage* DeathMontage;
	
	// Default abilities for this Character. These will be removed on Character death and regiven if Character respawns.
	UPROPERTY(BlueprintReadOnly, EditAnywhere,Category = "Drift|Abilities")
	TArray<TSubclassOf<class UDriftGameplayAbility>> CharacterAbilities;
	
	// Default attributes for a character for initializing on spawn/respawn.
	// This is an instant GE that overrides the values for attributes that get reset on spawn/respawn.
	UPROPERTY(BlueprintReadOnly, EditAnywhere,Category = "Drift|Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	// These effects are only applied one time on startup
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Drift|Abilities")
	TArray<TSubclassOf<class UGameplayEffect>> StartupEffects;
	
	// Grant abilities on the Server. The Ability Specs will be replicated to the owning client.
	virtual void AddCharacterAbilities();
	
	// Initialize the Character's attributes. Must run on Server but we run it on Client too
	// so that we don't have to wait. The Server's replication to the Client won't matter since
	// the values should be the same.
	virtual void InitializeAttributes();

	virtual void AddStartupEffects();
};
