// Fill out your copyright notice in the Description page of Project Settings.


#include "Spaceship.h"
#include "Components/ArrowComponent.h"
#include "PaperSpriteComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ASpaceship::ASpaceship()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ShipBase"));
	}
	ShipDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("ShipDirection"));
	ShipDirection->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	ShipSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("ShipSprite"));
	ShipSprite->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	ShipTip = CreateDefaultSubobject<UChildActorComponent>(TEXT("ShipTip"));
	ShipTip->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	
	
}

// Called when the game starts or when spawned
void ASpaceship::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpaceship::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASpaceship::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

