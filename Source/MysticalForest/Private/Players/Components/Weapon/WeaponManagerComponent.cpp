// Fill out your copyright notice in the Description page of Project Settings.


#include "Players/Components/Weapon/WeaponManagerComponent.h"
#include "Actors/Weapons/RangeWeaponActor.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UWeaponManagerComponent::UWeaponManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

// Called when the game starts
void UWeaponManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	if(GetOwnerRole() == ROLE_Authority) AsyncSpawnWeaponDelegate.BindUFunction(this, "OnCreateWeapon");
}

void UWeaponManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponManagerComponent, CurrentWeapon);
	DOREPLIFETIME_CONDITION(UWeaponManagerComponent, Weapons, COND_OwnerOnly);
}

void UWeaponManagerComponent::CreateWeaponTest(AController* Controller)
{
	if(GetOwnerRole() != ROLE_Authority) return;
	
	WeaponData->AsyncCreateWeapon("WeaponTest", GetWorld(), FTransform(FVector(0.f)), Controller, AsyncSpawnWeaponDelegate);
}

ABaseWeaponActor* UWeaponManagerComponent::FindWeaponByKey(EWeaponType Key)
{
	for(const auto& ByArray : Weapons)
	{
		if(ByArray->GetWeaponType() == Key)
		{
			return ByArray;
		}
	}
	return nullptr;
}

bool UWeaponManagerComponent::ContainsWeaponByKey(EWeaponType Key)
{
	bool Finder = Weapons.ContainsByPredicate([&](ABaseWeaponActor* Item) -> bool
	{
		return Item->GetWeaponType() == Key;
	});
	return Finder;
}

void UWeaponManagerComponent::OnCreateWeapon(bool bResult, FStringAssetReference LoadRef, ARangeWeaponActor* WeaponActor)
{
	if(bResult)
	{
		if(ContainsWeaponByKey(WeaponActor->GetWeaponType()))
		{
			RemoveWeaponByKey(WeaponActor->GetWeaponType());
		}
		AddWeaponToStorage(WeaponActor);
	}
}

void UWeaponManagerComponent::AddWeaponToStorage(ABaseWeaponActor* Weapon)
{
	if(GetOwnerRole() == ROLE_Authority)
	{
		Weapons.Add(Weapon);
		OnNewWeaponAddedDelegate.Broadcast(Weapon);
	}
}

void UWeaponManagerComponent::RemoveWeaponFromStorage(ABaseWeaponActor* Weapon)
{
	if(GetOwnerRole() == ROLE_Authority)
	Weapons.Remove(Weapon);
}

void UWeaponManagerComponent::RemoveWeaponByKey(EWeaponType Key)
{
	if(GetOwnerRole() != ROLE_Authority) return;
	
	Weapons.RemoveAll([&](ABaseWeaponActor* Item) -> bool
	{
		return Item->GetWeaponType() == Key;
	});
}

void UWeaponManagerComponent::OnRep_CurrentWeapon()
{
	OnCurrentWeaponChangedDelegate.Broadcast(CurrentWeapon);
}