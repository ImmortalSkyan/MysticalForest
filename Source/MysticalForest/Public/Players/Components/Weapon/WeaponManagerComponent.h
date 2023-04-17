// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataAssets/WeaponDataAsset.h"
#include "WeaponManagerComponent.generated.h"

class ABaseWeaponActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNewWeaponAdded, ABaseWeaponActor*, NewWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCurrentWeaponChanged, ABaseWeaponActor*, NewCurrentWeapon);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYSTICALFOREST_API UWeaponManagerComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	/** Find Weapon by key
	* @param
	* @Weapon weapon for add
	*/
	UFUNCTION()
	void AddWeaponToStorage(ABaseWeaponActor* Weapon);

	/** Find Weapon by key
	* @param
	* @Weapon weapon for remove
	*/
	UFUNCTION()
	void RemoveWeaponFromStorage(ABaseWeaponActor* Weapon);

	UFUNCTION()
	void OnRep_CurrentWeapon();
	
	/** Find Weapon by key
	* @param
	* @Key Weapon type for remove
	*/
	UFUNCTION()
	void RemoveWeaponByKey(EWeaponType Key);

	/** Find Weapon by key
	* @param
	* @bResult true if weapon be created, false if be error
	* @LoadRef String asset reference
	* @WeaponActor Actor when be spawned
	*/
	UFUNCTION()
    void OnCreateWeapon(bool bResult, FStringAssetReference LoadRef, ARangeWeaponActor* WeaponActor);

	/** Find Weapon by key
	 * @param
	 * @Key Weapon type for find 
	 */
	ABaseWeaponActor* FindWeaponByKey(EWeaponType Key);

	/** Find Weapon by key
	* @param
	* @Key Weapon type for contains 
	*/
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

private:

	/** Weapon data asset */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UWeaponDataAsset* WeaponData;

	/** curent weapon */
	UPROPERTY(ReplicatedUsing = "OnRep_CurrentWeapon", BlueprintReadOnly, Category = "Weapons", meta=(AllowPrivateAccess = "true"))
	ABaseWeaponActor* CurrentWeapon;

	/** weapon array */
	UPROPERTY(Replicated)
	TArray<ABaseWeaponActor*> Weapons;

	/** async spawn delegate */
	UPROPERTY()
	FAsyncSpawnWeapon AsyncSpawnWeaponDelegate;

public:

	/** Delegate to add new weapon */
	UPROPERTY()
	FNewWeaponAdded OnNewWeaponAddedDelegate;

	/** Delegate to current weapon */
	UPROPERTY()
	FCurrentWeaponChanged OnCurrentWeaponChangedDelegate;
};
