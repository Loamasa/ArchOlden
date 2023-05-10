// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGenericWorldData.generated.h"

USTRUCT(BlueprintType)
struct FMyGenericWorldData
{
    GENERATED_BODY()

public:

    FMyGenericWorldData();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int StartBlockSize = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int StartGenerationArtifactCount = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int StartModifierArtifactCount = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int StartBuildingArtifactCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int SpawningGenerationArtifactCount = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int SpawningModifierArtifactCount = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int SpawningBuildingArtifactCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool TreeCloseToRock;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int TreeRelationDistanceToRock;
};
