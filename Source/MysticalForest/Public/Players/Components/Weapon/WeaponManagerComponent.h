// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataAssets/WeaponDataAsset.h"
#include "WeaponManagerComponent.generated.h"

class ABaseWeaponActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNewWeaponAdded, ABaseWeaponActor*, NewWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCurrentWeaponChanged, ABaseWeaponActor*, NewCurrentWeapon);

USTRUCT(BlueprintType)
struct FWeapons
{
	GENERATED_BODY()
	
	FWeapons() : Key(EWeaponType::Unknown), Value(nullptr) {}
	FWeapons(EWeaponType Type, ABaseWeaponActor* Weapon) : Key(Type), Value(Weapon) {}
	
	EWeaponType Key;
	ABaseWeaponActor* Value;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYSTICALFOREST_API UWeaponManagerComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	UFUNCTION()
	void AddWeaponToStorage(ABaseWeaponActor* Weapon);

	UFUNCTION()
	void RemoveWeaponFromStorage(ABaseWeaponActor* Weapon);

	UFUNCTION()
	void OnRep_CurrentWeapon();

	void RemoveWeaponByKey(EWeaponType Key);
	
	UFUNCTION()
    void OnCreateWeapon(bool bResult, FStringAssetReference LoadRef, ARangeWeaponActor* WeaponActor);
	
	ABaseWeaponActor* FindWeaponByKey(EWeaponType Key);
	bool ContainsWeaponByKey(EWeaponType Key);

public:	
	// Sets default values for this component's properties
	UWeaponManagerComponent();

	//TEST
	void CreateWeaponTest(AController* Controller);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UWeaponDataAsset* WeaponData;

	UPROPERTY(ReplicatedUsing = "OnRep_CurrentWeapon", BlueprintReadOnly, Category = "Weapons", meta=(AllowPrivateAccess = "true"))
	ABaseWeaponActor* CurrentWeapon;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapons", meta=(AllowPrivateAccess = "true"))
	TArray<ABaseWeaponActor*> Weapons;

	UPROPERTY()
	FAsyncSpawnWeapon AsyncSpawnWeaponDelegate;

public:
	
	UPROPERTY()
	FNewWeaponAdded OnNewWeaponAddedDelegate;

	UPROPERTY()
	FCurrentWeaponChanged OnCurrentWeaponChangedDelegate;
};
