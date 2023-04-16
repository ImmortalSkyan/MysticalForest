// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponManagerComponent.generated.h"

class ABaseWeaponActor;

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Unknown,
	OneHandedWeapon,
	TwoHandedWeapon
};

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

	UFUNCTION(Server, Reliable)
	void Server_AddWeaponToStorage(EWeaponType Type, ABaseWeaponActor* Weapon);

	UFUNCTION(Server, Reliable)
	void Server_RemoveWeaponFromStorage(EWeaponType Type, ABaseWeaponActor* Weapon);

public:	
	// Sets default values for this component's properties
	UWeaponManagerComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

private:

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapons", meta=(AllowPrivateAccess = "true"))
	ABaseWeaponActor* CurrentWeapon;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapons", meta=(AllowPrivateAccess = "true"))
	TArray<FWeapons> Weapons;
};
