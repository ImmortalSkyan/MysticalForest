// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Actors/Weapons/BaseWeaponActor.h"
#include "GameFramework/Character.h"
#include "MysticalForestCharacter.generated.h"

class UWeaponManagerComponent;

UCLASS(config=Game)
class AMysticalForestCharacter : public ACharacter
{
	GENERATED_BODY()

private:

	UFUNCTION()
	void OnCurrentWeaponChangedEvent(ABaseWeaponActor* NewWeapon);

	UFUNCTION()
	void OnNewWeaponAddedEvent(ABaseWeaponActor* NewWeapon);

	UFUNCTION()
	virtual void PossessedBy(AController* NewController) override;

public:
	AMysticalForestCharacter();

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	USkeletalMeshComponent* GetLocalMesh() const;

protected:

	void MoveForward(float Value);
	void MoveRight(float Value);
	
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;

public:

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

private:
	
	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "WeaponManager", meta = (AllowPrivateAccess = "true"))
	UWeaponManagerComponent* WeaponManagerComponent;
	
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SkeletalMesh", meta = (AllowPrivateAccess = "true"))
//	USkeletalMeshComponent* FirstPersonMesh;
};

