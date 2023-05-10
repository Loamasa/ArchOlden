// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Map.h"
#include "Containers/EnumAsByte.h"
#include "Misc/OutputDeviceNull.h"
#include "..\\Ascension\MyAscensionData.h"
#include "..\\Ascension\MyAscensionManager.h"
#include "..\\Artifact\MyArtifactGenerationData.h"
#include "MyWorldBlockData.h"
#include "WorldGenerationData.h"
#include "MyWorldBlockEffect.h"
#include "MyWorldGenerator.generated.h"

UENUM(BlueprintType)
enum MyCorner
{
	UpperRight  UMETA(DisplayName = "Upper Right"),
	Upper		UMETA(DisplayName = "Upper"),
	UpperLeft   UMETA(DisplayName = "Upper Left"),
	Left		UMETA(DisplayName = "Left"),
	BelowRight	UMETA(DisplayName = "Below Right"),
	Right		UMETA(DisplayName = "Right"),
	BelowLeft	UMETA(DisplayName = "Below Left"),
	Below		UMETA(DisplayName = "Below"),
};

UCLASS()
class MOUNTAINEERS_API AMyWorldGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyWorldGenerator();

protected:

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
		void StartGenerateNewArea(const int X, const int Y, const FMyAscensionData AscensionData);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent) 
		TArray<FMyWorldBlockData> GenerateNewWorldOutput();

	UFUNCTION(BlueprintCallable)
	AMyWorldBlockEffect* getWorldBlockFromPool();
	UFUNCTION(BlueprintCallable)
	bool UnsubsribeWorldBockEffectToPool(AMyWorldBlockEffect* targetActiveWorldBlockEffect);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ExpandWorldBlockEffectPool(int ExpansionSize);

	UFUNCTION(BlueprintCallable)
		bool HasTargetBlock(const int X, const int Y);
	UFUNCTION(BlueprintCallable)
		FMyWorldBlockData GetTargetBlock(const int X, const int Y);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		bool InitializeWorldGenerationData();

	UFUNCTION(BlueprintCallable)
		void expandWorldAtClosestPosition(const int X, const int Y, const bool hasHole, const MyArtifactGenerationType targetGenerationType, const int Blockcount, float NaturalRecurcesModifier);

	UFUNCTION(BlueprintCallable)
		void OnAddArtifactToAscensionManager(TEnumAsByte<MyArtifactTypes> TargetArtifactType);
	UFUNCTION(BlueprintCallable)
		void OnRemoveArtifactInAscensionManager(TEnumAsByte<MyArtifactTypes> TargetArtifactType);

	UFUNCTION(BlueprintCallable)
		void onSetCurrentAreaData(const TArray<FMyWorldBlockData> areaData);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TMap<TEnumAsByte<MyWorldBlockType>, FMyWorldGenerationData> WorldGenerationDataList;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TEnumAsByte<MyArtifactGenerationType> currentWorldGenerationType;

	TMap<TPair<int, int>, FMyWorldBlockData> WorldBlockData; 	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Block Effects")
		TArray<AMyWorldBlockEffect*> WorldBlockEffectPooled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AMyAscensionManager* MyAscensionManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StartingArea")
		int NewAreaBlockCount = 200;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StartingArea")
		float holeSizeProcentage = 0.2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StartingArea")
		float blockDistanceX = 25;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StartingArea")
		float blockDistanceY = 25;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StartingArea")
		float cutOffProcentageRange = 0.1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FMyAscensionData currentAscension;
		

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Biome Properties")
		float ForrestCloseToRockProcentage = 0.7;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Biome Properties")
		int MaxMaxFrostClusterBlocks = 5;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Biome Properties")
		int MinAdjecentWaterBlock = 2;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Biome Properties")
		int MaxRockClusterBlock = 3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Biome Properties")
		int MinRockClusterBlock = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Biome Properties")
		int MaxFrostClusterBlock = 16;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Biome Properties")
		int MinFrostClusterBlock = 8;

	/*
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Biome size")
		int RockyGrassLandsBlockCount = 35;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Biome size")
		int GrassLandsBlockCount = 50;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Biome size")
		int RockLandsBlockCount = 25;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Biome size")
		int BigLakeBlockCount = 50;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Biome size")
		int SmallArtifactAreaBlockCount = 15;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Biome size")
		int TarnAreaBlockCount = 40;
	*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Biome size")
		int DepthDevider = 4;

private:

	//--- SPAWN NEW WORLD BLOCKS FUNCS ---
	
	//Create InitializeReady Cords
	void runNewWorldGenerationFuncsEachTick();
	TArray<FMyWorldBlockData> GenerateNewArea(TArray<TPair<int, int>>& InitializeReadyCoordinates, TPair<int, int> originCoordiates, int WorldBlockCount);
	void createNewWorldBlock(const TPair<int, int> originCoords, const int blockCount, TArray<TPair<int, int>>& InitializeReadyCoords);
	void placeNewRowWorldBlocks(const int loopCounter, const TPair<int, int> originCoords, int& currentBlockCount, MyCorner whichCorner, int& cutOff, TArray<TPair<int, int>>& InitializeReadyCoords);	

	void TryGenerateWorldHole(const TPair<int, int> originCoords, const int BlockCount, const int newAreaRadius);
	int getNewAreaRadius(int newAreaBlockcount);

	//Sort Water Funcs
	void sortWaterWorldBlocks(int waterCount, TArray<TPair<int, int>>& InitializeReadyCoordinates);
	TArray<TPair<int, int>> deductWaterValidFromInitializeReadyCoords(TArray<TPair<int, int>>& InitializeReadyCoordinates);
	TPair<int, int> GetValidAdjecentWaterCoordinate(TPair<int, int> StartingCoord, const TArray<TPair<int, int>>& InitializeReadyCoordinates);

	//Sort rock Funcs
	void sortRockWorldBlocks(int RockCount, TArray<TPair<int, int>>& InitializeReadyCoordinates); //NOTE: Usally spawns one more rock then inttended
	TPair<int, int> getValidSurroundingRockBlock(TPair<int, int> StartingCoord, bool& IsEdgeBlock, const TArray<TPair<int, int>>& InitializeReadyCoordinates);
	TPair<int, int> GetValidAdjecentRockCoordinate(TPair<int, int> StartingCoord, const TArray<TPair<int, int>>& InitializeReadyCoordinates);

	//Sort Frost Func
	void sortForstWorldBlocks(TArray<TPair<int, int>>&InitializeReadyCoordinates);
	TArray<TPair<int, int>> deductFrostValidFromInitializeReadyCoords(TArray<TPair<int, int>>& InitializeReadyCoordinates);
	int tryPlaceFrostOnAllAdjecent(const int BlockCount, const TPair<int, int> OriginCoords, TArray<TPair<int, int>>& frostReadyCoords/*, TArray<int>& CrystalBlockNumbers*/);
	bool TryFindAFrostCorner(TPair<int, int>& newCornerCoord, const TArray<TPair<int, int>>& frostReadyCoords);
	void assigneFrostBlock(TArray<TPair<int, int>>& frostReadyCoords, const TPair<int, int> TargetAssignRockCoords,/* TArray<int>& CrystalBlockNumbers, */const int remaningFrostBlocks);

	//Sort Grass Func
	void sortGrassWorldBlocks(TArray<TPair<int, int>>& InitializeReadyCoordinates);

	//Try Add Artifact
	void tryAssignArtifactToValidCurrentArea(int artifactCount, int artifactModifiersCount, int ArtifactAncientCount, int ArtifactWinCount);

	//Try Add Natural Resurces 
	void AssigneNaturalResurces();
	void assigneTreeToGrassBlocks(TArray<int>& GrassBlockIndexes);
	void assigneMineraToRockBlocks(TArray<int>& rockBlockIndexes);
	void assigneCrystalToFrostBlocks(TArray<int>& frostBlockIndexes);
	void assigneFishToWaterBlocks(TArray<int>& waterBlockIndexes);

	//Assign height differences
	void assignHeightDifference();
	void sortWorldBlocks();
	TPair<bool, int> GetResultOnSurroundingBlocks(const TPair<int, int> originCoord);
	TPair<bool, int> isSurroundingBlocksTypeAndHigher(const TPair<int, int> originCoord, MyWorldBlockType targetBlockType);
	TPair<bool, int> isSurroundingBlockTypeAndHigher(const TPair<int, int> originCoord, MyWorldBlockType targetBlockType, int radiusRange);

	//World block interface
	bool AssigneWorldBlockType(const TPair<int, int>& Coords, const MyWorldBlockType type, const MyNaturalResourcesType NaturalResourceType, TArray<TPair<int, int>>& InitializeReadyCoordinates);
	bool isEdgeBlock(const TPair<int, int> targetCoord);
	bool isAdjecentEdgeBlock(const TPair<int, int> targetCoord);
	bool isBlockTypeClose(const TPair<int, int> originCoord, MyWorldBlockType targetBlockType, int radiusRange);

	//--- SPAWN NEW AREA FUNCS ---

	void StartGenerateExpansionOfArea(TPair<int, int> OriginCoords, const int BlockCount, /*const int artifactCount, const int artifactModifierCount, const int ArtifactAncientCount, const int ArtifactWinCount, */const bool generateHole);
	TPair<int, int> closestEmptyArea(const TPair<int, int> OriginCoords);
	int tryCutOff(int X);

	void updateForNewArea();

	//--- WORLD BLOCK EFFECT --- 
	
private:
	bool startNewWorldGeneration = false;
	int TickCounter = 1;
	int WaterCount = 0;
	int RockCount = 0;

	bool GenerateWithHole = true;
	int spawningAreasBlockCount = 20;
	TPair<int, int> spawningNewAreaOrigin;
	
	TMap<int, int> CurrentCutOffsUpperRight; //Key = X   Value = CutOff

	TArray<TPair<int, int>> InitializeReadyCoordinate;

	TArray<FMyWorldBlockData> CurrentAreaData;
	TArray<TPair<int, int>> CurrentAreaCoordsInSpawnOrder;

	int currentArtifactCount = 0;
	int currentArtifactModifierCount = 0;
	int currentArtifactAncientCount = 0;
	int currentArtifactWincondition = 0;

	bool spawnCloseToRocks = true;
	int blockDistanceToRockForTrees = 2;
	float currentNaturalRecurcesModifier = 1.0;

	TArray<TEnumAsByte<MyArtifactGenerationType>> CurrentWorldGenerationArtifacts;
	TArray<TEnumAsByte<MyBuildingType>> CurrentBuildingArtifacts;
	int CurrentModifierArtifacts;

	UPROPERTY()
	TArray<AMyWorldBlockEffect*> WorldBlockEffectActive;
};