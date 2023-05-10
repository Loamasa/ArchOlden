// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "..\\WorldGeneration\MyWorldBlockData.h"
#include "..\\Ascension\MyAscensionData.h"
#include "..\\Construction\MyModifierData.h"
#include "MySavedAscensionData.generated.h"


USTRUCT(BlueprintType)
struct FMySavedAscensionData
{
    GENERATED_BODY()

public: 
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TArray<FMyWorldBlockData> savedAreaData;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FMyAscensionData savedAscensionData;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TArray<FMyAscensionData> OldAscensionData;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TMap<FString, FMyModifierData> currentModifiers;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int currentRound;

public:
    FMySavedAscensionData();
};