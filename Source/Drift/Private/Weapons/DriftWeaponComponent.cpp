// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/DriftWeaponComponent.h"

// Sets default values
ADriftWeaponComponent::ADriftWeaponComponent()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//create a root component
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// assign a mesh for our weapon and attach it to the root
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	Mesh->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void ADriftWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADriftWeaponComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

