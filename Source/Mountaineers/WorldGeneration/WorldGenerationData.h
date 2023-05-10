// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyWorldBlockData.h"
#include "WorldGenerationData.generated.h"

UENUM(BlueprintType)
enum MyBuildingSquareSize
{
    OneBlock    UMETA(DisplayName = "OneBlock"),
    TwoBlock    UMETA(DisplayName = "TwoBlock"),
    FourBlock   UMETA(DisplayName = "FourBlock"),
};

USTRUCT(BlueprintType)
struct FMyWorldGenerationData
{
    GENERATED_BODY()
    
public:

    //UPROPERTY(EditAnywhere, BlueprintReadWrite)
        //FMyWorldBlockData WorldBlockType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TEnumAsByte<MyWorldBlockType> WorldBlockType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float BlockMin; 
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float BlockMax;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float NaturalResourceMin;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float NaturalResourceMax;
};
