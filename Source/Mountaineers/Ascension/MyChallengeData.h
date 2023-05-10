// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "MyChallengeData.generated.h"

USTRUCT(BlueprintType)
struct FMyChallengeData
{
    GENERATED_BODY()

public:

    FMyChallengeData();
     
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool HasPointsWins = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int WinPoints = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float ChallengePointMultiplier = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool HasWinArtifact = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int WinArtifactLevel = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float ChallengeArtifactMultiplier = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool FindWinArtifact = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool GameBar;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int GameBarStartAddition;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int GameBarMultiplier;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int TurnBeforeMultiplier;
};
