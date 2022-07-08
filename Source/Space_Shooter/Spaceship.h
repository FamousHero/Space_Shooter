// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Spaceship.generated.h"

UCLASS()
class SPACE_SHOOTER_API ASpaceship : public APawn
{
	GENERATED_BODY()


	virtual void OnConstruction(const FTransform& Transform) override;


public:
	// Sets default values for this pawn's properties
	ASpaceship();
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when player wants to rotate this pawn actor
	void Rotate(float Rate);


	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_UpdateRotator(FQuat Rotation);
	bool Server_UpdateRotator_Validate(FQuat Rotation);
	void Server_UpdateRotator_Implementation(FQuat Rotation);


	void MoveForward(float AllowedSpeed);

	/*
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_UpdateLocation(FVector Position);
	bool Server_UpdateLocation_Validate(FVector Position);
	void Server_UpdateLocation_Implementation(FVector Position);
	*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ship")
	float MaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ship")
	float MinSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ship")
	float StartSpeed;

	UPROPERTY(ReplicatedUsing=OnRep_CurrSpeed, VisibleAnywhere, BlueprintReadOnly, Category = "Ship")
	float CurrSpeed;

	UFUNCTION()
	void OnRep_CurrSpeed();

	void Boost();

	void Brake();

	void ResetSpeed();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetCurrSpeed(float NewSpeed);
	bool Server_SetCurrSpeed_Validate(float NewSpeed);
	void Server_SetCurrSpeed_Implementation(float NewSpeed);

	UFUNCTION(Client, Unreliable)
	void Client_SetLocation(FVector NewLocation);
	void Client_SetLocation_Implementation(FVector NewLocation);

	UFUNCTION(Client, Unreliable)
	void Client_SetRotation(FQuat NewRotation);
	void Client_SetRotation_Implementation(FQuat NewRotation);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// Helpful debug tool - which way is the tank facing?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship", meta = (AllowPrivateAccess = "true"))
		class UArrowComponent* ShipDirection;

	// Sprite for the spaceship body.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship", meta = (AllowPrivateAccess = "true"))
		class UPaperSpriteComponent* ShipSprite;

	//Tip of the ship from where to spawn bullets
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship", meta = (AllowPrivateAccess = "true"))
		UChildActorComponent* ShipTip;
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ship")
	class UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ship")
	float BaseTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ship")
	uint8 bReverseDirection : 1;

};
