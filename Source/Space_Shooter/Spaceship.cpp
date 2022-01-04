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
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ShipCamera"));
	CameraComponent->bUsePawnControlRotation = false;
	CameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	CameraComponent->OrthoWidth = 1024.0f;
	CameraComponent->AspectRatio = 3.0f / 4.0f;
	CameraComponent->SetWorldRotation(FRotator(0, 0, -90.0f));
	CameraComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	CurrSpeed = 10.0f;
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

	MoveForward(CurrSpeed*DeltaTime);
}

// Called to bind functionality to input
void ASpaceship::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAction("Boost", IE_Pressed, this, &ASpaceship::Boost);
	PlayerInputComponent->BindAction("Boost", IE_Released, this, &ASpaceship::ResetSpeed);
	PlayerInputComponent->BindAction("Brake", IE_Pressed, this, &ASpaceship::Brake);
	PlayerInputComponent->BindAction("Brake", IE_Released, this, &ASpaceship::ResetSpeed);


	PlayerInputComponent->BindAxis("Rotate", this, &ASpaceship::TurnAtRate);

}


void ASpaceship::Boost()
{
	CurrSpeed = MaxSpeed;
}

void ASpaceship::Brake()
{
	CurrSpeed = MinSpeed;
}
void ASpaceship::ResetSpeed()
{
	CurrSpeed = StartSpeed;
}
void ASpaceship::MoveForward(float AxisValue)
{
	UE_LOG(LogTemp, Warning, TEXT("Axis Value is %f"), AxisValue);
	// find out which way is forward
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	// get forward vector
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	UE_LOG(LogTemp, Warning, TEXT("Directin is is %s"), *Direction.ToString());
	FVector Pos = GetActorLocation();
	Pos.X += Direction.X * AxisValue;
	SetActorLocation(Pos);

}
void ASpaceship::TurnAtRate(float Rate)
{
	
}
