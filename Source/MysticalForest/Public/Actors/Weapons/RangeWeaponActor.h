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
    
    virtual EWeaponType GetWeaponType() const override final { return RangeWeaponData.WeaponType; }
    virtual float GetSelectTime() const override final { return RangeWeaponData.TimeForSelect; }    

    void SetWeaponData(const FRangeWeaponData& Data) { RangeWeaponData = Data; }

protected:

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:

    UPROPERTY(Replicated)
    FRangeWeaponData RangeWeaponData;
};
