// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyBuildingData.generated.h"

UENUM(BlueprintType)
enum MyBuildingType
{
    Tree        UMETA(DisplayName = "Tree"),
    Fish        UMETA(DisplayName = "Fish"),
    Mineral     UMETA(DisplayName = "Mineral"),
    Crystal     UMETA(DisplayName = "Crystal"),

    House       UMETA(DisplayName = "House"),
    Mansion     UMETA(DisplayName = "Mansion"),
    CityCenter  UMETA(DisplayName = "CityCenter"),
    TownHall    UMETA(DisplayName = "TownHall"),

    Masonry     UMETA(DisplayName = "Masonry"),
    Factory     UMETA(DisplayName = "Factory"),
    Storage     UMETA(DisplayName = "Storage"),
    Mint        UMETA(DisplayName = "Mint"),

    Field       UMETA(DisplayName = "Field"),
    Farm        UMETA(DisplayName = "Farm"),
    Windmill    UMETA(DisplayName = "Windmill"),
    Reservoir   UMETA(DisplayName = "Reservoir"),

    Shrine      UMETA(DisplayName = "Shrine"),
    Monastery   UMETA(DisplayName = "Monastery"),
    Tempel      UMETA(DisplayName = "Tempel"),
    Sanctuary   UMETA(DisplayName = "Sanctuary"),

    FishingHut  UMETA(DisplayName = "FishingHut"),
    Harbor      UMETA(DisplayName = "Harbor"),
    Haven       UMETA(DisplayName = "Haven"),
    WaterFarm   UMETA(DisplayName = "WaterFarm"),

    Remnant     UMETA(DisplayName = "Remnant"),
    Archive     UMETA(DisplayName = "Archive"),
    Ark         UMETA(DisplayName = "Ark"),
    Eternal     UMETA(DisplayName = "Eternal"),

    None        UMETA(DisplayName = "None"),
};

UENUM(BlueprintType)
enum MyPlacmentProfile
{
    PP_GrassLands            UMETA(DisplayName = "PP_GrassLands"),
    PP_RocksOnly             UMETA(DisplayName = "PP_RocksOnly"),
    PP_GrassAndRock          UMETA(DisplayName = "PP_GrassAndRock"),
    PP_WaterOnly             UMETA(DisplayName = "PP_WaterOnly"),
    PP_GrassAndFrost         UMETA(DisplayName = "PP_GrassAndFrost"),
    PP_GrassFrostAndRock     UMETA(DisplayName = "PP_GrassFrostAndRock"),
    PP_FrostAndRock          UMETA(DisplayName = "PP_FrostAndRock"),
};

UENUM(BlueprintType)
enum MyRarity
{
    R_Negative              UMETA(DisplayName = "R_Negative"),
    R_Common                UMETA(DisplayName = "R_Common"),
    R_UnCommon              UMETA(DisplayName = "R_UnCommon"),
    R_Rare                  UMETA(DisplayName = "R_Rare"),
    R_Legendary             UMETA(DisplayName = "R_Legendary"),
};

UENUM(BlueprintType)
enum MyBuildingStage
{
    BS_StageOne              UMETA(DisplayName = "StageOne"),
    BS_StageTwo              UMETA(DisplayName = "StageTwo"),
    BS_StageThree            UMETA(DisplayName = "StageThree"),
    BS_StageFour             UMETA(DisplayName = "StageFour"),
    BS_StageFive             UMETA(DisplayName = "StageFive"),
    BS_StageSix              UMETA(DisplayName = "StageSix"),
    BS_StageSeven            UMETA(DisplayName = "StageSeven"),
    BS_StageEight            UMETA(DisplayName = "StageEight"),
};

UENUM(BlueprintType)
enum MyBuildingPack
{
    BP_City                 UMETA(DisplayName = "BP_City"),
    BP_Farm                 UMETA(DisplayName = "BP_Farm"),
    BP_Tempel               UMETA(DisplayName = "BP_Tempel"),
    BP_Factory              UMETA(DisplayName = "BP_Factory"),
    BP_Water                UMETA(DisplayName = "BP_Water"),
    BP_Ancient              UMETA(DisplayName = "BP_Ancient"),
    BP_Environment          UMETA(DisplayName = "BP_Environment"),

    BP_None                 UMETA(DisplayName = "BP_None"),
};

USTRUCT(BlueprintType)
struct FMyBuildingData
{
    GENERATED_BODY()

public:
    FMyBuildingData();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TEnumAsByte<MyBuildingPack> PackType;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TEnumAsByte<MyBuildingType> BuildingType;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TEnumAsByte<MyPlacmentProfile> PlacmentProfile;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float DetectionRadious = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int AvrageScore = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TEnumAsByte<MyBuildingStage> BuildingStage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TEnumAsByte<MyRarity> Rarity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TMap<FString, int> BuildingPoints;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString BuildingName;

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