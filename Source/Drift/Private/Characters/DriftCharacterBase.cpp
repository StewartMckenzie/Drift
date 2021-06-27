// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/DriftCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Characters/DriftCharacterMovementComponent.h"
#include "Characters/Abilities/AttributeSets/DriftAttributeSetBase.h"
#include "Characters/Abilities/DriftGameplayAbility.h"
#include "Characters/Abilities/DriftAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
// Sets default values


ADriftCharacterBase::ADriftCharacterBase(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer.SetDefaultSubobjectClass<UDriftCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);

	bAlwaysRelevant = true;

	

}

UAbilitySystemComponent* ADriftCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

UDriftAttributeSetBase* ADriftCharacterBase::GetAttributeSetBase() const
{
	return AttributeSetBase.Get();
}

void ADriftCharacterBase::BeginPlay()
{
	Super::BeginPlay();

}

UDriftCharacterMovementComponent* ADriftCharacterBase::GetMyMovementComponent() const
{
	return static_cast<UDriftCharacterMovementComponent*>(GetCharacterMovement());
}

int32 ADriftCharacterBase::GetAbilityLevel(EGDAbilityInputID AbilityID) const
{
	return 1;
}


void ADriftCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ADriftCharacterBase, TurnRight, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ADriftCharacterBase, TurnLeft, COND_None, REPNOTIFY_Always);
}

int32 ADriftCharacterBase::GetCharacterLevel() const
{
	if (AttributeSetBase.IsValid())
	{
		return static_cast<int32>(AttributeSetBase->GetCharacterLevel());
	}

	return 0;
}

float ADriftCharacterBase::GetMoveSpeed() const
{
	
	
	if (AttributeSetBase.IsValid())
	{
		// UE_LOG(LogTemp, Warning, TEXT("Speed:  %f "), AttributeSetBase->GetMoveSpeed());
		return AttributeSetBase->GetMoveSpeed();
	}
	// UE_LOG(LogTemp, Warning, TEXT("No") );
	return 0.0f;
}

float ADriftCharacterBase::GetMoveSpeedBaseValue() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMoveSpeedAttribute().GetGameplayAttributeData(AttributeSetBase.Get())->GetBaseValue();
	}

	return 0.0f;
}

void ADriftCharacterBase::AddCharacterAbilities()
{
	// Grant abilities, but only on the server	
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || AbilitySystemComponent->CharacterAbilitiesGiven)
	{
		return;
	}

	for (TSubclassOf<UDriftGameplayAbility>& StartupAbility : CharacterAbilities)
	{
		AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(StartupAbility, GetAbilityLevel(StartupAbility.GetDefaultObject()->AbilityID), static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
	}

	AbilitySystemComponent->CharacterAbilitiesGiven = true;
}

void ADriftCharacterBase::InitializeAttributes()
{
	if (!AbilitySystemComponent.IsValid())
	{
		return;
	}

	if (!DefaultAttributes)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing DefaultAttributes for %s. Please fill in the character's Blueprint."), *FString(__FUNCTION__), *GetName());
		return;
	}

	// Can run on Server and Client
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, GetCharacterLevel(), EffectContext);
	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
	}
}

void ADriftCharacterBase::AddStartupEffects()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || AbilitySystemComponent->StartupEffectsApplied)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
	{
		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, GetCharacterLevel(), EffectContext);
		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
		}
	}

	AbilitySystemComponent->StartupEffectsApplied = true;
}



