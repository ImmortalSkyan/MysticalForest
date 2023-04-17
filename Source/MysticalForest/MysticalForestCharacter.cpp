// Copyright Epic Games, Inc. All Rights Reserved.

#include "MysticalForestCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Players/Components/Weapon/WeaponManagerComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/KismetSystemLibrary.h"

AMysticalForestCharacter::AMysticalForestCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	bReplicates = true;
	NetUpdateFrequency = 60.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(GetMesh(), "head");
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	WeaponManagerComponent = CreateDefaultSubobject<UWeaponManagerComponent>(TEXT("WeaponMangerComponent"));

//	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	//FirstPersonMesh->SetupAttachment(FollowCamera);
}

void AMysticalForestCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("SelectOneHandWeapon", IE_Released, this, &AMysticalForestCharacter::StopJumping);
	PlayerInputComponent->BindAction("SelectTwoHandWeapon", IE_Released, this, &AMysticalForestCharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMysticalForestCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMysticalForestCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMysticalForestCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMysticalForestCharacter::LookUpAtRate);

	FInputActionHandlerSignature OneHandWeaponActionHandler;
	OneHandWeaponActionHandler.BindUFunction(this, "OnSelectWeapon", EWeaponType::OneHandedWeapon);
	BindActionBindings(OneHandWeaponActionHandler, "SelectOneHandWeapon");
	
	FInputActionHandlerSignature TwoHandWeaponActionHandler;
	OneHandWeaponActionHandler.BindUFunction(this, "OnSelectWeapon", EWeaponType::TwoHandedWeapon);
	BindActionBindings(OneHandWeaponActionHandler, "SelectTwoHandWeapon");
}

void AMysticalForestCharacter::BindActionBindings(const FInputActionHandlerSignature& ActionSignature, const FName& ActionName)
{
	FInputActionBinding ActionBinding(ActionName, IE_Released);
	ActionBinding.ActionDelegate = ActionSignature;
	InputComponent->AddActionBinding(ActionBinding);
}

void AMysticalForestCharacter::OnSelectWeapon(EWeaponType Type)
{
	/**
	* if weapon with new type is valid and
	* if new weapon type != current weapon type, start selected, else stop selected
	*/
	auto const TempWeapon = WeaponManagerComponent->FindWeaponByKey(Type);
	if(TempWeapon && TempWeapon != WeaponManagerComponent->GetCurrentWeapon())
	{
		WeaponManagerComponent->Server_SelectWeapon(Type);
		OnWeaponSelected.Broadcast(Type, WeaponManagerComponent->GetCurrentWeaponType());
	}	
}

void AMysticalForestCharacter::BeginPlay()
{
	Super::BeginPlay();

	WeaponManagerComponent->OnCurrentWeaponChangedDelegate.AddDynamic(this, &AMysticalForestCharacter::OnCurrentWeaponChangedEvent);

	if(GetLocalRole() == ROLE_Authority)
	WeaponManagerComponent->OnNewWeaponAddedDelegate.AddDynamic(this, &AMysticalForestCharacter::OnNewWeaponAddedEvent);
}

void AMysticalForestCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if(GetLocalRole() == ROLE_Authority) WeaponManagerComponent->CreateWeaponTest(Controller);
}

void AMysticalForestCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMysticalForestCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMysticalForestCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMysticalForestCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

USkeletalMeshComponent* AMysticalForestCharacter::GetLocalMesh() const
{
	return nullptr;
}

void AMysticalForestCharacter::OnCurrentWeaponChangedEvent(ABaseWeaponActor* NewWeapon)
{
	UKismetSystemLibrary::PrintString(this, FString(TEXT("OnCurrentWeaponChangedEvent")));
}

void AMysticalForestCharacter::OnNewWeaponAddedEvent(ABaseWeaponActor* NewWeapon)
{
	FString WeaponStr = UEnum::GetDisplayValueAsText(NewWeapon->GetWeaponType()).ToString();
	FString SocketStr = "SKT_" + WeaponStr + "Pos";
	SocketStr.RemoveSpacesInline();
	NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, *SocketStr);
	ForceNetUpdate();
}