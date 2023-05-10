// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "..\\Construction\MyBuildingData.h"
#include "..\\Artifact\MyArtifactGenerationData.h"
#include "MyAscensionData.generated.h"

USTRUCT(BlueprintType)
struct FMyAscensionData
{
    GENERATED_BODY()
    
public:

    FMyAscensionData();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString ChallengeName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString genericWorldName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString WorldCharactristicsName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int difficultyLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int BlockSize;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TEnumAsByte<MyArtifactGenerationType> StartGenerationType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool HasPointsWins;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int WinPoints;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool HasWinArtifact;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool isFindWinArtifact;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int WinArtifactLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int GameBarLevel;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool GameBar;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int GameBarStartAddition;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int GameBarMultiplier;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int TurnBeforeMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool HasHole;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float HoleSizeProcentage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FMyArtifactGenerationData StartingArtifactData;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FMyArtifactGenerationData SpawningArtifactData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TArray<FString> startingModifiers;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool TreeCloseToRock;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int TreeRelationDistanceToRock;
};