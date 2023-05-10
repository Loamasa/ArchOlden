// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyWorldBlockEffect.generated.h"

UCLASS()
class MOUNTAINEERS_API AMyWorldBlockEffect : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyWorldBlockEffect();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
		int ActivePoolIndex = 0;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetActivePoolIndex(int index);
	int GetActivePoolIndex();
};
