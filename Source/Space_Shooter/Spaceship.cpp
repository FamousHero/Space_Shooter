// Fill out your copyright notice in the Description page of Project Settings.


#include "Spaceship.h"
#include "Components/ArrowComponent.h"
#include "PaperSpriteComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"


// Sets default values
ASpaceship::ASpaceship()
{
	

 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicatingMovement(true);
	
	//SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root Component")));

	ShipSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("ShipSprite"));
	SetRootComponent(ShipSprite);
	//ShipSprite->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	ShipDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("ShipDirection"));
	ShipDirection->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);


	ShipTip = CreateDefaultSubobject<UChildActorComponent>(TEXT("ShipTip"));
	ShipTip->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	

	//!SharedCamera ? CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ShipCamera")) : CameraComponent = SharedCamera;
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ShipCamera"));
	CameraComponent->bUsePawnControlRotation = false;
	CameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	CameraComponent->OrthoWidth = 2048.0f;
	CameraComponent->AspectRatio = 3.0f / 4.0f;
	CameraComponent->SetWorldRotation(FRotator(0, 0, -90.0f));

	CameraComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	CurrSpeed = 45.0f;

}

void ASpaceship::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASpaceship, CurrSpeed);
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

////////////////////////////////////////
//Movement
void ASpaceship::Boost()
{
	
	if (GetLocalRole() < ROLE_Authority)
	{
		FString message = FString::Printf(TEXT("called by client"));
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, message);
		Server_SetCurrSpeed(MaxSpeed);
	}
	else
	{
		CurrSpeed = MaxSpeed;
	}
}

void ASpaceship::Brake()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_SetCurrSpeed(MinSpeed);
	}
	else{
	CurrSpeed = MinSpeed;
	}
}
void ASpaceship::ResetSpeed()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_SetCurrSpeed(StartSpeed);
	}
	else{
	CurrSpeed = StartSpeed;
	}
}

void ASpaceship::OnRep_CurrSpeed()
{
	FVector Pos = GetActorLocation();

	FString message = FString::Printf(TEXT("currSpeed is %f | pos is %f %f %f"), CurrSpeed, Pos.X, Pos.Y, Pos.Z);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, message);
	//Do client stuff for each client (or check if its the owner
}

bool ASpaceship::Server_SetCurrSpeed_Validate(float NewSpeed)
{
	return true;
}

void ASpaceship::Server_SetCurrSpeed_Implementation(float NewSpeed)
{
	CurrSpeed = NewSpeed;
}
void ASpaceship::MoveForward(float AllowedSpeed)
{

	//if you are an actor on the server
	if ((GetLocalRole() == ROLE_Authority))
	{
	// find out which way is forward
		const FRotator Rotation = GetActorRotation();
		const FRotator PitchRotation(Rotation.Pitch, 0, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(PitchRotation).GetUnitAxis(EAxis::X);

		FVector Pos = GetActorLocation();
		FMath::Abs(GetActorQuat().Y) < .707f ? Pos.X += Direction.X * AllowedSpeed: Pos.X -= Direction.X * AllowedSpeed;
		Pos.Z += Direction.Z * AllowedSpeed;
		SetActorLocation(Pos, true);
		Client_SetLocation(GetActorLocation());
		FString message = FString::Printf(TEXT(" loc is currently %f, %f, %f"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, FString(TEXT("SERVER:"))+ GetName() + message);
		
	}
	else if(GetLocalRole() != ROLE_Authority)
	{
		FString message = FString::Printf(TEXT(" loc is currently %f, %f, %f"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString(TEXT("Client:")) + GetName() + message);
		if (GetController())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString(TEXT("Client controller was found for "))+GetName() );

		}
	}
}


void ASpaceship::Client_SetLocation_Implementation(FVector NewLocation)
{
	SetActorLocation(NewLocation);
}

void ASpaceship::Server_UpdateRotator_Implementation(FQuat Rotation)
{
	AddActorLocalRotation(Rotation);
	Client_SetRotation(GetActorQuat());
}
bool ASpaceship::Server_UpdateRotator_Validate(FQuat Rotation)
{
	return true;
}


void ASpaceship::Client_SetRotation_Implementation(FQuat NewRotation)
{
	SetActorRotation(NewRotation);
}

//////////////////////////////////////////////////
//Rotation

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
	if ((GetLocalRole() != ROLE_Authority) && !FMath::IsNearlyZero(Rate, KINDA_SMALL_NUMBER))
	{
		Server_UpdateRotator(RotationToAdd.Quaternion());
	}
	else if (!FMath::IsNearlyZero(Rate, KINDA_SMALL_NUMBER))
	{
		AddActorLocalRotation(RotationToAdd.Quaternion());	
	}
}

/*
bool ASpaceship::Server_UpdateLocation_Validate(FVector Location)
{
	
	return true;
}

void ASpaceship::Server_UpdateLocation_Implementation(FVector Location)
{
	UE_LOG(LogTemp, Warning, TEXT("in loc imp"));
	SetActorLocation(Location, true);
}
*/