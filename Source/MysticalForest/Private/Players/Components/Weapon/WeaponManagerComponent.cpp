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
}

void UWeaponManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponManagerComponent, CurrentWeapon);
	DOREPLIFETIME_CONDITION(UWeaponManagerComponent, Weapons, COND_OwnerOnly)
	DOREPLIFETIME(UWeaponManagerComponent, SelectWeaponType);
}

void UWeaponManagerComponent::OnComponentCreated()
{
	Super::OnComponentCreated();

	if(GetOwnerRole() == ROLE_Authority) AsyncSpawnWeaponDelegate.BindUFunction(this, "OnCreateWeapon");
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

void UWeaponManagerComponent::OnRep_SelectWeaponType()
{
	OnSelectWeaponDelegate.Broadcast(SelectWeaponType, CurrentWeapon);
}

void UWeaponManagerComponent::SelectWeapon(EWeaponType NewType)
{
	Server_SelectWeapon(NewType);
	SelectWeaponType = ESelectWeaponType::Select;
	if(GetNetMode() != NM_ListenServer) OnSelectWeaponDelegate.Broadcast(SelectWeaponType, CurrentWeapon);
}

void UWeaponManagerComponent::Server_SelectWeapon_Implementation(EWeaponType NewType)
{
	//Dont start select weapon logic if player selected weapon right now
	if(SelectWeaponType != ESelectWeaponType::None) return;
	
	auto const TempWeapon = FindWeaponByKey(NewType);
	if(TempWeapon && TempWeapon != CurrentWeapon)
	{
		SelectWeaponType = ESelectWeaponType::Select;
		OnRep_SelectWeaponType();
			
		FName const FunName = "HideWeaponForSelect";
		float InRate = CurrentWeapon->GetSelectTime();
		
		FTimerDelegate TimerDel;
		TimerDel.BindUFunction(this, FunName, TempWeapon);
		GetWorld()->GetTimerManager().SetTimer(SelectWeaponHandle, TimerDel, TempWeapon->GetSelectTime(), false);
	}
}

void UWeaponManagerComponent::HideWeaponForSelect(ABaseWeaponActor* NewWeapon)
{
	GetWorld()->GetTimerManager().ClearTimer(SelectWeaponHandle);

	//need for detach weapon from right arm and attach to safe position
	OnMoveWeaponToSavePosDelegate.Broadcast(CurrentWeapon);
	
	CurrentWeapon = NewWeapon;
	OnRep_CurrentWeapon();
	
	FTimerDelegate TimerDel;
	TimerDel.BindUObject(this, &UWeaponManagerComponent::EquipWeaponBeforeSelect, NewWeapon);
	GetWorld()->GetTimerManager().SetTimer(SelectWeaponHandle, TimerDel, NewWeapon->GetSelectTime(), false);
}

void UWeaponManagerComponent::EquipWeaponBeforeSelect(ABaseWeaponActor* NewWeapon)
{
	GetWorld()->GetTimerManager().ClearTimer(SelectWeaponHandle);

	SelectWeaponType = ESelectWeaponType::None;
	OnRep_SelectWeaponType();
}

void UWeaponManagerComponent::HideWeapon()
{
	if(CurrentWeapon && SelectWeaponType == ESelectWeaponType::None)
	{
		Server_HideWeapon();
	}
}

void UWeaponManagerComponent::Server_HideWeapon_Implementation()
{
	if(CurrentWeapon && SelectWeaponType == ESelectWeaponType::None)
	{
		SelectWeaponType = ESelectWeaponType::Hide;
		OnRep_SelectWeaponType();

		GetWorld()->GetTimerManager().SetTimer(SelectWeaponHandle, this, &UWeaponManagerComponent::HideWeaponFinish, CurrentWeapon->GetSelectTime(), false);
	}
}

void UWeaponManagerComponent::HideWeaponFinish()
{
	GetWorld()->GetTimerManager().ClearTimer(SelectWeaponHandle);

	//need for detach weapon from right arm and attach to safe position
	OnMoveWeaponToSavePosDelegate.Broadcast(CurrentWeapon);
	
	CurrentWeapon = nullptr;
	OnRep_CurrentWeapon();

	SelectWeaponType = ESelectWeaponType::None;
}

void UWeaponManagerComponent::EquipWeapon(EWeaponType Type)
{
	Server_EquipWeapon(Type);
	SelectWeaponType = ESelectWeaponType::Equip;

	if(GetNetMode() != NM_ListenServer) OnSelectWeaponDelegate.Broadcast(SelectWeaponType, 	FindWeaponByKey(Type));
}

void UWeaponManagerComponent::Server_EquipWeapon_Implementation(EWeaponType Type)
{
	if(SelectWeaponType != ESelectWeaponType::None) return;
	
	auto const TempWeapon = FindWeaponByKey(Type);
	if(TempWeapon)
	{
		CurrentWeapon = TempWeapon;
		OnRep_CurrentWeapon();
		
		SelectWeaponType = ESelectWeaponType::Equip;
		OnRep_SelectWeaponType();

		GetWorld()->GetTimerManager().SetTimer(SelectWeaponHandle, this, &UWeaponManagerComponent::EquipWeaponFinish, CurrentWeapon->GetSelectTime(), false);
		return;
	}
	SelectWeaponType = ESelectWeaponType::None;
	OnRep_SelectWeaponType();
}

void UWeaponManagerComponent::EquipWeaponFinish()
{
	GetWorld()->GetTimerManager().ClearTimer(SelectWeaponHandle);
	
	SelectWeaponType = ESelectWeaponType::None;
	OnRep_SelectWeaponType();
}