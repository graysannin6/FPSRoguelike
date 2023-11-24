// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSRoguelikeCharacter.h"
#include "FPSRoguelikeProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AFPSRoguelikeCharacter

AFPSRoguelikeCharacter::AFPSRoguelikeCharacter()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	isSprinting = false;
	isZoomedIn = false;
	weapon = nullptr;

}

void AFPSRoguelikeCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

}

//////////////////////////////////////////////////////////////////////////// Input

void AFPSRoguelikeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

	PlayerInputComponent->BindAction("Sprint",IE_Pressed,this,&AFPSRoguelikeCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint",IE_Released,this,&AFPSRoguelikeCharacter::StopSprinting);
	PlayerInputComponent->BindAction("Teleport", IE_Pressed, this, &AFPSRoguelikeCharacter::RandomTeleport);
	PlayerInputComponent->BindAction("ZoomIn",IE_Pressed,this,&AFPSRoguelikeCharacter::ZoomIn);
	PlayerInputComponent->BindAction("ZoomIn",IE_Released,this,&AFPSRoguelikeCharacter::ZoomOut);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFPSRoguelikeCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFPSRoguelikeCharacter::Look);
	}
	else
	{
		
	}
}


void AFPSRoguelikeCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AFPSRoguelikeCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AFPSRoguelikeCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool AFPSRoguelikeCharacter::GetHasRifle()
{
	return bHasRifle;
}

void AFPSRoguelikeCharacter::Sprint()
{
	if (auto characterMouvement = GetCharacterMovement())
	{
		characterMouvement->MaxWalkSpeed = 1500;
		isSprinting = true;
	}
	
}

void AFPSRoguelikeCharacter::StopSprinting()
{
	if (auto characterMouvement = GetCharacterMovement())
	{
		characterMouvement->MaxWalkSpeed = 600;
		isSprinting = false;
	}
}

void AFPSRoguelikeCharacter::ZoomIn()
{
	if (auto firstPersonCamera = GetFirstPersonCameraComponent())
	{
		firstPersonCamera->SetFieldOfView(70.0);
		isZoomedIn = true;
	}
	
}

void AFPSRoguelikeCharacter::ZoomOut()
{
	if (auto firstPersonCamera = GetFirstPersonCameraComponent())
	{
		firstPersonCamera->SetFieldOfView(100.0);
		isZoomedIn = false;
	}
}

void AFPSRoguelikeCharacter::RandomTeleport()
{
	FVector NewLocation = GetRandomLocation();
    TeleportTo(NewLocation, GetActorRotation());
}

FVector AFPSRoguelikeCharacter::GetRandomLocation()
{
     FVector Origin = GetActorLocation();
    float Range = 1000.0f; // Customize as needed

    FVector RandomPoint = Origin + FMath::VRand() * Range;

    // Set minimum Z value
    float MinZ = 100.0f; // Set this to the minimum Z value you want (e.g., ground level)
    if (RandomPoint.Z < MinZ)
    {
        RandomPoint.Z = MinZ;
    }

    // Additional logic to ensure the location is safe for teleportation
    // ...

    return RandomPoint;
}
