// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "..\\Construction\MyBuildingData.h"
#include "MyArtifactGenerationData.generated.h"

UENUM(BlueprintType)
enum MyArtifactGenerationType
{
    AGT_RockyGrassLands		 UMETA(DisplayName = "Rocky Plains"),
    AGT_GrassLands			 UMETA(DisplayName = "Plains"),
    AGT_RockLands			 UMETA(DisplayName = "Rock Lands"),
    AGT_BigLake				 UMETA(DisplayName = "Big Lake"),
    AGT_SmallArtifactArea	 UMETA(DisplayName = "Small Gem"),
    AGT_Tarn                 UMETA(DisplayName = "Tarn"),

    AGT_None				 UMETA(DisplayName = "None"),
};

USTRUCT(BlueprintType)
struct FMyArtifactGenerationData
{
	GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TArray<TEnumAsByte<MyArtifactGenerationType>> WorldGenerationArtifacts;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int ModifierArtifacts;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TArray<TEnumAsByte<MyBuildingType>> BuildingArtifacts;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool isFindWinArtifact;

	FMyArtifactGenerationData();
};