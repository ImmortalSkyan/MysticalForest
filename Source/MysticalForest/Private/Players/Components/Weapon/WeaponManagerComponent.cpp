// Fill out your copyright notice in the Description page of Project Settings.


#include "Players/Components/Weapon/WeaponManagerComponent.h"
#include "Actors/Weapons/BaseWeaponActor.h"
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
	
}

void UWeaponManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponManagerComponent, CurrentWeapon);
	DOREPLIFETIME_CONDITION(UWeaponManagerComponent, Weapons, COND_OwnerOnly);
}

bool UWeaponManagerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bRecorded = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	if(((Channel->Connection->QueuedBits) + Bunch->GetNumBits() + Channel->Connection->SendBuffer.GetNumBits()) >= 0)
	{
		return bRecorded;
	}
	TArray<UObject*> ReplicatedArray;
	if(CurrentWeapon) ReplicatedArray.Add(CurrentWeapon);
	for(auto const ByArray : Weapons) if(ByArray.Value) ReplicatedArray.Add(ByArray.Value);

	bRecorded = Channel->ReplicateSubobjectList(ReplicatedArray, *Bunch, *RepFlags);
	return bRecorded;
}

void UWeaponManagerComponent::Server_AddWeaponToStorage_Implementation(EWeaponType Type, ABaseWeaponActor* Weapon)
{
	Weapons.Add(FWeapons(Type, Weapon));
}

void UWeaponManagerComponent::Server_RemoveWeaponFromStorage_Implementation(EWeaponType Type, ABaseWeaponActor* Weapon)
{
	Weapons.RemoveAll([&](FWeapons Item) -> bool
	{
		return Item.Key == Type;
	});
}
