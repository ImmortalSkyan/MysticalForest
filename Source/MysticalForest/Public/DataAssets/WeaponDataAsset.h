// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FWeaponDataBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponData")
	FName WeaponRowName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponData")
	int32 BaseDamage;

	/** delay before used */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponData")
	float SpeedOfUse;
};

USTRUCT(BlueprintType)
struct FRangeWeaponData : public FWeaponDataBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponData")
	int32 RangeOfUse;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponData")
	int32 MaxAmmoInStorage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponData")
	int32 MaxAmmoInWeapon;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponData")
	float ReloadTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponData")
	bool bCanAutoFire;
};

UCLASS(Blueprintable)
class MYSTICALFOREST_API UWeaponDataAsset : public UDataAsset
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FRangeWeaponData> RangeWeaponData;
};
