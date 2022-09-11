// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSOpenWorldEnemy.h"

// Sets default values
AFPSOpenWorldEnemy::AFPSOpenWorldEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Creating AIPerceptionStimuliComponent
	AIPerceptionStimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("AIPerceptionStimuliComponent"));

	//Creating Equipment component for AI
	TP_Equipment = CreateDefaultSubobject<UTP_Equipment>(TEXT("TP_Equipment"));

	//GetController()->SetGenericTeamID

}

// Called when the game starts or when spawned
void AFPSOpenWorldEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFPSOpenWorldEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFPSOpenWorldEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AFPSOpenWorldEnemy::Kill()
{
	if (DeathSequence != nullptr && bIsAlive)
		GetMesh()->PlayAnimation(DeathSequence, false); 

	bIsAlive = false;

	//Timer for destroying dead body
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this] {Destroy(); });
	GetWorldTimerManager().SetTimer(DeathTimer, TimerDelegate, DeathBodyExistTime, false);

	//Disable physics on dead body
	GetCapsuleComponent()->SetCollisionProfileName("NoCollision");
	GetMesh()->SetCollisionProfileName("NoCollision");

}

