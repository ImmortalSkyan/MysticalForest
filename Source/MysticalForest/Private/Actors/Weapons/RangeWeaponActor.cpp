// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Weapons/RangeWeaponActor.h"
#include "Net/UnrealNetwork.h"

void ARangeWeaponActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ARangeWeaponActor, RangeWeaponData);
}
