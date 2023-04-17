// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataAssets/WeaponDataAsset.h"
#include "BaseWeaponActor.generated.h"

UCLASS(Abstract)
class MYSTICALFOREST_API ABaseWeaponActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseWeaponActor();

	UFUNCTION(BlueprintPure, Category = "Getter")
	USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	
	UFUNCTION(BlueprintPure)
	virtual EWeaponType GetWeaponType() const { return EWeaponType::Unknown; }
	
	UFUNCTION(BlueprintPure)
	virtual float GetSelectTime() const { return 0.f; }
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponMesh", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMesh;
};
