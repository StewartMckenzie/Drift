// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Heros/DriftHeroCharacter.h"
// #include "AI/GDHeroAIController.h"
#include "Camera/CameraComponent.h"
#include "Characters/Abilities/AttributeSets/DriftAttributeSetBase.h"
#include "Characters/DriftCharacterMovementComponent.h"
#include "Characters/Abilities/DriftAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/DecalComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/DriftPlayerState.h"


// Sets default values
ADriftHeroCharacter::ADriftHeroCharacter(const FObjectInitializer& ObjectInitializer):	Super(ObjectInitializer.SetDefaultSubobjectClass<UDriftCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	// Makes sure that the animations play on the Server so that we can use bone and socket transforms
	// to do things like spawning projectiles and other FX.
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(FName("NoCollision"));




}


// Called to bind functionality to input
void ADriftHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ADriftHeroCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADriftHeroCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ADriftHeroCharacter::LookUp);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ADriftHeroCharacter::LookUpRate);
	PlayerInputComponent->BindAxis("Turn", this, &ADriftHeroCharacter::Turn);
	PlayerInputComponent->BindAxis("TurnRate", this, &ADriftHeroCharacter::TurnRate);

	// Bind player input to the AbilitySystemComponent. Also called in OnRep_PlayerState because of a potential race condition.
	BindASCInput();
}

void ADriftHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ADriftPlayerState* PS = GetPlayerState<ADriftPlayerState>();

	if (PS)
	{

		// Set the ASC on the Server. Clients do this in OnRep_PlayerState()
		AbilitySystemComponent = Cast<UDriftAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		// AI won't have PlayerControllers so we can init again here just to be sure. No harm in initing twice for heroes that have PlayerControllers.
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

		// Set the AttributeSetBase for convenience attribute functions
		AttributeSetBase = PS->GetAttributeSetBase();

		// If we handle players disconnecting and rejoining in the future, we'll have to change this so that possession from rejoining doesn't reset attributes.
		// For now assume possession = spawn/respawn.
		InitializeAttributes();

		AddStartupEffects();

		AddCharacterAbilities();

		// ADriftPlayerController* PC = Cast<ADriftPlayerController>(GetController());
		// if (PC)
		// {
		// 	PC->CreateHUD();
		// }






	}
	
}

void ADriftHeroCharacter::BindASCInput()
{
	if (!ASCInputBound && AbilitySystemComponent.IsValid() && IsValid(InputComponent))
	{
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, FGameplayAbilityInputBinds(FString("ConfirmTarget"),
			FString("CancelTarget"), FString("EGDAbilityInputID"), static_cast<int32>(EGDAbilityInputID::Confirm), static_cast<int32>(EGDAbilityInputID::Cancel)));
		UE_LOG(LogTemp, Warning, TEXT("Input bound") );
		ASCInputBound = true;
	}

}



// Called when the game starts or when spawned
void ADriftHeroCharacter::BeginPlay()
{
	Super::BeginPlay();
	//spawn the weapon in world
	MeleeWeapon = GetWorld()->SpawnActor<ADriftWeaponComponent>(MeleeWeaponClass);

	if (MeleeWeapon) {	//attach it in our right hand
		MeleeWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocketR"));
		//claim ownership
		MeleeWeapon->SetOwner(this);
	
	}
}




USpringArmComponent* ADriftHeroCharacter::GetCameraBoom()
{
	return CameraBoom;
}

UCameraComponent* ADriftHeroCharacter::GetFollowCamera()
{
	return FollowCamera;
}

float ADriftHeroCharacter::GetStartingCameraBoomArmLength()
{
	return StartingCameraBoomArmLength;
}

FVector ADriftHeroCharacter::GetStartingCameraBoomLocation()
{
	return StartingCameraBoomLocation;
}


void ADriftHeroCharacter::TurningServer_Implementation(float Value)
{
	if(GetVelocity().IsZero())
	{

		if(Value >.3)
		{
			TurnRight=true;
			TurnLeft=false;
		}else if(Value <-.3)
		{
			TurnRight=false;
			TurnLeft=true;
		}else
		{
			TurnRight=false;
			TurnLeft=false;
		}
	}
}


void ADriftHeroCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void ADriftHeroCharacter::LookUpRate(float Value)
{
	AddControllerPitchInput(Value * BaseLookUpRate * GetWorld()->DeltaTimeSeconds);
}

void ADriftHeroCharacter::Turn(float Value)
{
	
	AddControllerYawInput(Value);

	TurningServer(Value);
}

void ADriftHeroCharacter::TurnRate(float Value)
{
	AddControllerYawInput(Value * BaseTurnRate * GetWorld()->DeltaTimeSeconds);
}

void ADriftHeroCharacter::MoveForward(float Value)
{

	AddMovementInput(UKismetMathLibrary::GetForwardVector(FRotator(0, GetControlRotation().Yaw, 0)), Value);
}

void ADriftHeroCharacter::MoveRight(float Value)
{
	AddMovementInput(UKismetMathLibrary::GetRightVector(FRotator(0, GetControlRotation().Yaw, 0)), Value);
}


void ADriftHeroCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	ADriftPlayerState* PS = GetPlayerState<ADriftPlayerState>();
	if (PS)
	{
		// Set the ASC for clients. Server does this in PossessedBy.
		AbilitySystemComponent = Cast<UDriftAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		// Init ASC Actor Info for clients. Server will init its ASC when it possesses a new Actor.
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);

		// Bind player input to the AbilitySystemComponent. Also called in SetupPlayerInputComponent because of a potential race condition.
		BindASCInput();

		// Set the AttributeSetBase for convenience attribute functions
		AttributeSetBase = PS->GetAttributeSetBase();

		// If we handle players disconnecting and rejoining in the future, we'll have to change this so that posession from rejoining doesn't reset attributes.
		// For now assume possession = spawn/respawn.
		InitializeAttributes();
	}
}



