// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Actors/Weapons/BaseWeaponActor.h"
#include "DataAssets/WeaponDataAsset.h"
#include "WeaponManagerComponent.generated.h"

UENUM(BlueprintType)
enum class ESelectWeaponType : uint8
{
	None,
	Equip,
	Hide,
	Select
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNewWeaponAdded, ABaseWeaponActor*, NewWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCurrentWeaponChanged, ABaseWeaponActor*, NewCurrentWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMoveWeaponToSavePos, ABaseWeaponActor*, Weapon); // only server
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSelectWeaponDelegate, ESelectWeaponType, Type, ABaseWeaponActor*, CurWeapon);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYSTICALFOREST_API UWeaponManagerComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	
	UFUNCTION()
	void OnRep_SelectWeaponType();

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

	/** hide current weapon for hide on new weapon */
	UFUNCTION()
	void HideWeaponForSelect(ABaseWeaponActor* NewWeapon);
	
	UFUNCTION()
	void EquipWeaponBeforeSelect(ABaseWeaponActor* NewWeapon);

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
    
	UFUNCTION(Server, Reliable)
	void Server_SelectWeapon(EWeaponType NewType);

	UFUNCTION(Server, Reliable)
	void Server_HideWeapon();

	UFUNCTION()
    void HideWeaponFinish();
	
	UFUNCTION(Server, Reliable)
	void Server_EquipWeapon(EWeaponType NewType);

	UFUNCTION()
	void EquipWeaponFinish();

protected:

	virtual void OnComponentCreated() override;

public:	
	// Sets default values for this component's properties
	UWeaponManagerComponent();

	void SelectWeapon(EWeaponType NewType);
	void EquipWeapon(EWeaponType Type);
	void HideWeapon();
	
	UFUNCTION(BlueprintPure)
	ESelectWeaponType GetSelectType() const { return SelectWeaponType; }

	
	/** Find Weapon by key
	* @param
	* @Key Weapon type for find 
	*/
	UFUNCTION(BlueprintPure)
	ABaseWeaponActor* FindWeaponByKey(EWeaponType Key);

	/** Find Weapon by key
	* @param
	* @Key Weapon type for contains 
	*/
	UFUNCTION(BlueprintPure)
	bool ContainsWeaponByKey(EWeaponType Key);

	//TEST
	void CreateWeaponTest(AController* Controller);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** return weapon type of current weapon. if current weapon is not valid return EWeaponType::Unknown */
	UFUNCTION(BlueprintPure)
	EWeaponType GetCurrentWeaponType() const { return CurrentWeapon ? CurrentWeapon->GetWeaponType() : EWeaponType::Unknown; }

	UFUNCTION(BlueprintPure)
	ABaseWeaponActor* GetCurrentWeapon() const { return CurrentWeapon; }

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
	
	UPROPERTY(ReplicatedUsing = OnRep_SelectWeaponType)
	ESelectWeaponType SelectWeaponType;

	/** async spawn delegate */
	UPROPERTY()
	FAsyncSpawnWeapon AsyncSpawnWeaponDelegate;

	UPROPERTY()
	FTimerHandle SelectWeaponHandle;

public:

	/** Delegate to add new weapon */
	UPROPERTY(BlueprintAssignable)
	FNewWeaponAdded OnNewWeaponAddedDelegate;

	UPROPERTY()
	FMoveWeaponToSavePos OnMoveWeaponToSavePosDelegate;

	/** Delegate to current weapon */
	UPROPERTY(BlueprintAssignable)
	FCurrentWeaponChanged OnCurrentWeaponChangedDelegate;

	UPROPERTY(BlueprintAssignable)
	FSelectWeaponDelegate OnSelectWeaponDelegate;
};
