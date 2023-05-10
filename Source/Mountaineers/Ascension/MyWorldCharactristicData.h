// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CoreMinimal.h"
#include "..\\Artifact\MyArtifactGenerationData.h"
#include "..\\Construction\MyBuildingData.h"
#include "MyWorldCharactristicData.generated.h"

USTRUCT(BlueprintType)
struct FMyWorldCharactristicData
{
    GENERATED_BODY()

public:

    FMyWorldCharactristicData();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TEnumAsByte<MyArtifactGenerationType> StartGenerationType;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TEnumAsByte<MyArtifactGenerationType> FirstSpawningGenerationArtifact;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TArray<TEnumAsByte<MyArtifactGenerationType>> ValidStartGenerationArtifact;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TArray<TEnumAsByte<MyBuildingType>> ValidStartBuildingArtifact;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TArray<TEnumAsByte<MyArtifactGenerationType>> ValidSpawningGenerationArtifact;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TArray<TEnumAsByte<MyBuildingType>> ValidSpawningBuildingArtifact;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float StartHoleProcentage = 0;
};