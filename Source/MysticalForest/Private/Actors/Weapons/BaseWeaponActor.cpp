// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Weapons/BaseWeaponActor.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABaseWeaponActor::ABaseWeaponActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	NetUpdateFrequency = 30.f;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABaseWeaponActor::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseWeaponActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABaseWeaponActor, bWeaponUsed, COND_SkipOwner);
}

void ABaseWeaponActor::OnRep_WeaponUsed()
{
	OnWeaponUsedDelegate.Broadcast(bWeaponUsed);
}

bool ABaseWeaponActor::UseWeapon()
{
	
}

void ABaseWeaponActor::StopUseWeapon()
{
	
}

void ABaseWeaponActor::StopRateDelay()
{
	
}

bool ABaseWeaponActor::IsAbleToUseWeapon()
{
	
}

