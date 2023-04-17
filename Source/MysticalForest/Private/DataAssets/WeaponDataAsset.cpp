// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/WeaponDataAsset.h"
#include "Actors/Weapons/RangeWeaponActor.h"
#include "Singleton/BaseSingleton.h"

FRangeWeaponData UWeaponDataAsset::FindRangeWeaponData(const FName& RowName)
{
    return RangeWeaponData.FindRef(RowName);
}

bool UWeaponDataAsset::AsyncCreateWeapon(const FName& RowName, UObject* WorldContext, FTransform SpawnTransform, AController* Controller, const FAsyncSpawnWeapon& CallBack)
{
    auto const TempData = FindRangeWeaponData(RowName);
    auto const Class = TempData.WeaponClass;
    if(Class.IsNull())
    {
        FString const InstigatorName = WorldContext ? *WorldContext->GetFullName() : TEXT("Unknown");
        UE_LOG(LogAssetData, Error, TEXT("UWeaponDataAsset::AsyncCreateWeapon --Class is null for %s"), *InstigatorName);
        return false;
    }

    FStreamableManager& AssetLoader = UBaseSingleton::Get().AssetLoader;
    FSoftObjectPath Ref = Class.ToSoftObjectPath();
    AssetLoader.RequestAsyncLoad(Ref, FStreamableDelegate::CreateUObject(this, &UWeaponDataAsset::OnAsyncSpawnActorComplete, RowName, WorldContext, Ref, SpawnTransform, Controller, CallBack));
    return true;
}

void UWeaponDataAsset::OnAsyncSpawnActorComplete(FName RowName, UObject* WorldContextObject, FStringAssetReference Reference, FTransform SpawnTransform, AController* Controller, FAsyncSpawnWeapon CallBack)
{
    ARangeWeaponActor* WeaponActor = nullptr;

    if(!Controller || !Controller->GetPawn()) return;

    UClass* Class = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), Controller->GetPawn(), *Reference.ToString()));

    if(!Class)
    {
        UE_LOG(LogAssetData, Error, TEXT("Actor class is null -- UWeaponDataAsset::OnAsyncSpawnActorComplete"));
    }
    else
    {
        FActorSpawnParameters Param;
        Param.Instigator = Controller->GetPawn();
        Param.Owner = Controller;
        Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        WeaponActor = WorldContextObject->GetWorld()->SpawnActor<ARangeWeaponActor>(Class, SpawnTransform, Param);
        WeaponActor->SetWeaponData(FindRangeWeaponData(RowName));
    }
    CallBack.ExecuteIfBound(WeaponActor != nullptr, Reference, WeaponActor);
}

