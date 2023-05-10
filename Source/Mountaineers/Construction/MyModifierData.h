// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyBuildingData.h"
#include "MyModifierData.generated.h"

USTRUCT(BlueprintType)
struct FMyModifierData
{
    GENERATED_BODY()

public:
    FMyModifierData();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ModifierName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<MyBuildingType> BuildingType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<MyBuildingStage> buildingStage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<MyRarity> Rarity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DetectionRadious = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FString, int> BuildingPoints;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Tree = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Mineral = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Crystal = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Fish = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int House = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Mansion = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int CityCenter = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int TownHall = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Masonry = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Factory = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Storage = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Mint = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Field = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Farm = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Windmill = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Reservoir = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Shrine = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Monastrey = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Tempel = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Sanctuary = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int FishingHut = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Harbor = 0; 
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Haven = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int WaterFarm = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Remnant = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Archive = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Ark = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Eternal = 0;
};