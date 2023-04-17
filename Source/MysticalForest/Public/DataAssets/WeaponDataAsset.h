// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponDataAsset.generated.h"

class ARangeWeaponActor;

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FAsyncSpawnWeapon, bool, bResult, FStringAssetReference, LoadRef, ARangeWeaponActor*, WeaponActor);

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Unknown,
    OneHandedWeapon,
    TwoHandedWeapon
};

USTRUCT(BlueprintType)
struct FWeaponDataBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponData")
	FName WeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponData")
	EWeaponType WeaponType;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponData")
	TAssetSubclassOf<ARangeWeaponActor> WeaponClass;
};

UCLASS(Blueprintable)
class MYSTICALFOREST_API UWeaponDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	FRangeWeaponData FindRangeWeaponData(const FName& RowName);
	
	UFUNCTION(BlueprintCallable, Category = "AsyncLoad|WeaponDataAsset", meta = (WorldContext = "WorldContextObject", DisplayName = "AsyncSpawnWeapon"))
	static bool AsyncCreateWeapon(TAssetSubclassOf<ARangeWeaponActor> Class, UObject* WorldContext, FTransform SpawnTransform, class AController* Controller, const FAsyncSpawnWeapon& CallBack);

private:
	
	/** Called when asset loading for actor spawn is finished */
	static void OnAsyncSpawnActorComplete(UObject* WorldContextObject, FStringAssetReference Reference, FTransform SpawnTransform, class AController* Controller, FAsyncSpawnWeapon CallBack);

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TMap<FName, FRangeWeaponData> RangeWeaponData;
};
