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

	SetReplicates(true);
	SetReplicatingMovement(bReplicates);

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
	CameraComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

}


void ASpaceship::OnConstruction(const FTransform& Transform)
{
	CurrSpeed = StartSpeed;
	BaseTurnRate = 45.0f;
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

	float AllowedSpeed = CurrSpeed * DeltaTime;
	MoveForward(AllowedSpeed);
}

// Called to bind functionality to input
void ASpaceship::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAction("Boost", IE_Pressed, this, &ASpaceship::Boost);
	PlayerInputComponent->BindAction("Boost", IE_Released, this, &ASpaceship::ResetSpeed);
	PlayerInputComponent->BindAction("Brake", IE_Pressed, this, &ASpaceship::Brake);
	PlayerInputComponent->BindAction("Brake", IE_Released, this, &ASpaceship::ResetSpeed);


	PlayerInputComponent->BindAxis("Rotate", this, &ASpaceship::Rotate);

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

void ASpaceship::MoveForward(float AllowedSpeed)
{
	// find out which way is forward
	const FRotator Rotation = GetActorRotation();
	const FRotator PitchRotation(Rotation.Pitch, 0, 0);
	UE_LOG(LogTemp, Warning, TEXT("Current Pitch is %f %f %f"), PitchRotation.Pitch, PitchRotation.Roll, PitchRotation.Yaw);
	UE_LOG(LogTemp, Warning, TEXT("Current Quat X is %f"), GetActorQuat().Y);

	// get forward vector
	const FVector Direction = FRotationMatrix(PitchRotation).GetUnitAxis(EAxis::X);

	//UE_LOG(LogTemp, Warning, TEXT("Direction is is %s"), *Direction.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Added %f to Z"), Direction.Z);
	FVector Pos = GetActorLocation();
	FMath::Abs(GetActorQuat().Y) < .707f ? Pos.X += Direction.X * AllowedSpeed: Pos.X -= Direction.X * AllowedSpeed;
	Pos.Z += Direction.Z * AllowedSpeed;
	SetActorLocation(Pos);

}


void ASpaceship::Rotate(float Rate)
{
	FRotator RotationToAdd;
	if (!FMath::IsNearlyZero(Rate, KINDA_SMALL_NUMBER))
	{
		float AllowedRotation = BaseTurnRate * GetWorld()->GetDeltaSeconds();
		if (Rate < 0)
			RotationToAdd.Pitch = -AllowedRotation;
		else
			RotationToAdd.Pitch = AllowedRotation;
		
	}
	AddActorLocalRotation(RotationToAdd.Quaternion());
}
