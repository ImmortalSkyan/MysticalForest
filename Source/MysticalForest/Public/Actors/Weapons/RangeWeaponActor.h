// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Weapons/BaseWeaponActor.h"
#include "RangeWeaponActor.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class MYSTICALFOREST_API ARangeWeaponActor : public ABaseWeaponActor
{
	GENERATED_BODY()

public:
    
    UFUNCTION(BlueprintPure)
    virtual EWeaponType GetWeaponType() const override final { return RangeWeaponData.WeaponType; }

private:

    FRangeWeaponData RangeWeaponData;
};
