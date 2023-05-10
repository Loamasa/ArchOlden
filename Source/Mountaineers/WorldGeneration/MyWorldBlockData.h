// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Map.h"
#include "Containers/EnumAsByte.h"
#include "..\\Ascension\MyAscensionData.h"
#include "..\\Construction\MyBuildingData.h"
#include "MyWorldBlockData.generated.h"

UENUM(BlueprintType)
enum MyWorldBlockType
{
    Grass   UMETA(DisplayName = "Grass"),
    Water   UMETA(DisplayName = "Water"),
    Frost   UMETA(DisplayName = "Frost"),
    Rock    UMETA(DisplayName = "Rock"),
    WBNone  UMETA(DisplayName = "None"),
    Hole    UMETA(DisplayName = "Hole"),
};

UENUM(BlueprintType)
enum MyNaturalResourcesType
{
    NR_Tree    UMETA(DisplayName = "Tree"),
    NR_Fish    UMETA(DisplayName = "Fish"),
    NR_Mineral UMETA(DisplayName = "Mineral"),
    NR_Crystal UMETA(DisplayName = "Crystal"),
    NR_None    UMETA(DisplayName = "None"),
};

USTRUCT(BlueprintType)
struct FMyWorldBlockData
{
    GENERATED_BODY()

public:

    FMyWorldBlockData();

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
        int CordX = 0;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
        int CordY = 0;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
        int Depth = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
        bool HasArtifact = false;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
        bool HasArtifactModifier = false;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
        bool HasArtifactAncient = false;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
        bool HasArtifactWin = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TEnumAsByte<MyArtifactGenerationType> WorldGenerationArtifacts = MyArtifactGenerationType::AGT_None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool ModifierArtifacts = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TEnumAsByte<MyBuildingType> BuildingArtifacts = MyBuildingType::None;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
        bool HasNaturalResoruces = false;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
        TEnumAsByte<MyNaturalResourcesType> NaturalResourcesType = MyNaturalResourcesType::NR_None;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
        TArray<TEnumAsByte<MyNaturalResourcesType>> NaturalResources;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
        TEnumAsByte<MyWorldBlockType> WorldBlockType = MyWorldBlockType::WBNone;
};