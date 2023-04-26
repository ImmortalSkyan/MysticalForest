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
    virtual float GetSpeedOfUse() const override final { return RangeWeaponData.SpeedOfUse; }
    virtual bool GetCanAutoFire() const override final { return RangeWeaponData.bCanAutoFire; }

    void SetWeaponData(const FRangeWeaponData& Data) { RangeWeaponData = Data; }

protected:

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void StopUseWeapon() override;
    virtual void StopRateDelay() override;
    virtual bool IsAbleToUseWeapon() override;
    virtual bool UseWeapon() override;
    virtual void DropLineTrace();

private:

    UPROPERTY(Replicated)
    FRangeWeaponData RangeWeaponData;
};
