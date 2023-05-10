// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyArtifact.h" 
#include "..\\Ascension\MyAscensionData.h"
#include "MyArtifactData.generated.h"

USTRUCT(BlueprintType)
struct FMyArtifactData
{
    GENERATED_BODY()

public:

    FMyArtifactData();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int X;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int Y;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int ArtifactStrength;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TEnumAsByte<MyArtifactGenerationType> GenerationType;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool hasModifier;

   
};