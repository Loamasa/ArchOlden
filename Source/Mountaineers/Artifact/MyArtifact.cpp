// Fill out your copyright notice in the Description page of Project Settings.


#include "MyArtifact.h"

// Sets default values
AMyArtifact::AMyArtifact()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyArtifact::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyArtifact::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

