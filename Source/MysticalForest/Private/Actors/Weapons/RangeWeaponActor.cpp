// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Weapons/RangeWeaponActor.h"
#include "Net/UnrealNetwork.h"

void ARangeWeaponActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ARangeWeaponActor, RangeWeaponData);
}

bool ARangeWeaponActor::IsAbleToUseWeapon()
{
    return Super::IsAbleToUseWeapon();
}

bool ARangeWeaponActor::UseWeapon()
{
    if(Super::UseWeapon())
    {
        DropLineTrace();
        return true;
    }
    StopUseWeapon();
    return false;
}

void ARangeWeaponActor::StopRateDelay()
{
    Super::StopRateDelay();

    if(GetCanAutoFire() && bWeaponUsed)
    {
        UseWeapon();
    }
}

void ARangeWeaponActor::StopUseWeapon()
{
    Super::StopUseWeapon();
}

void ARangeWeaponActor::DropLineTrace()
{
    
}