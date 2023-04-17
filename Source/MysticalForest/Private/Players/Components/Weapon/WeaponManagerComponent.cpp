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

	auto const TempData = WeaponData->FindRangeWeaponData("WeaponTest");
	if(TempData.WeaponClass.IsNull()) return;
	
	WeaponData->AsyncCreateWeapon(TempData.WeaponClass, GetWorld(), FTransform(FVector(0.f)), Controller, AsyncSpawnWeaponDelegate);
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

bool UWeaponManagerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bRecorded = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	if((Channel->Connection->QueuedBits + Bunch->GetNumBits() + Channel->Connection->SendBuffer.GetNumBits()) >= 0)
	{
		return bRecorded;
	}
	TArray<UObject*> ReplicatedArray;
	if(CurrentWeapon) ReplicatedArray.Add(CurrentWeapon);
	for(auto const ByArray : Weapons) if(ByArray) ReplicatedArray.Add(ByArray);

	bRecorded = Channel->ReplicateSubobjectList(ReplicatedArray, *Bunch, *RepFlags);
	return bRecorded;
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

