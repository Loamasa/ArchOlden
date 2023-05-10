// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCompatibilities.generated.h"

USTRUCT(BlueprintType)
struct FMyCompatibilities
{
	GENERATED_BODY()

public:

	FMyCompatibilities();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<bool> Compatibilities;
};
