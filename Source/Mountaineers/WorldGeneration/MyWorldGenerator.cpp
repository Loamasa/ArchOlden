// Fill out your copyright notice in the Description page of Project Settings.

#include "MyWorldGenerator.h"

// Sets default values
AMyWorldGenerator::AMyWorldGenerator()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMyWorldGenerator::Tick(float DeltaSeconds)
{
	if (startNewWorldGeneration) {
		runNewWorldGenerationFuncsEachTick();
	}
}

//--- CREATE INTITLIZE READY WORLD BLOCKS ---

void AMyWorldGenerator::runNewWorldGenerationFuncsEachTick()
{	
	switch (TickCounter) {
	case 1:
		TickCounter++;
		InitializeWorldGenerationData();
		break;

	case 2:
		TickCounter++;
		//InitializeWorldGenerationData();
		WaterCount = spawningAreasBlockCount * FMath::FRandRange(WorldGenerationDataList.Find(MyWorldBlockType::Water)->BlockMin, WorldGenerationDataList.Find(MyWorldBlockType::Water)->BlockMax);
		RockCount = spawningAreasBlockCount * FMath::FRandRange(WorldGenerationDataList.Find(MyWorldBlockType::Rock)->BlockMin, WorldGenerationDataList.Find(MyWorldBlockType::Rock)->BlockMax);
		break;

	case 3:
		TickCounter++;
		if(GenerateWithHole)
		TryGenerateWorldHole(spawningNewAreaOrigin, spawningAreasBlockCount, getNewAreaRadius(spawningAreasBlockCount));
		break;
		
	case 4:
		TickCounter++;
		GenerateNewArea(InitializeReadyCoordinate, spawningNewAreaOrigin, spawningAreasBlockCount);
		break;

	case 5:
		TickCounter++;
		sortWaterWorldBlocks(WaterCount, InitializeReadyCoordinate); //FIX: waterValidInitilizeReadyCoords[placingIndex] - Can crash the game 
		break;

	case 6:
		TickCounter++;
		sortRockWorldBlocks(RockCount, InitializeReadyCoordinate);
		break;

	case 7:
		TickCounter++;
		sortForstWorldBlocks(InitializeReadyCoordinate);
		break;

	case 8:
		TickCounter++;
		sortGrassWorldBlocks(InitializeReadyCoordinate);
		break;

	case 9:
		TickCounter++;
		tryAssignArtifactToValidCurrentArea(currentArtifactCount, currentArtifactModifierCount, currentArtifactAncientCount, currentArtifactWincondition);
		break;

	case 10:
		TickCounter++;
		AssigneNaturalResurces();
		break;

	case 11:
		TickCounter++;
		assignHeightDifference();
		break;

	default:
		GenerateNewWorldOutput();
		startNewWorldGeneration = false;
		TickCounter = 1;
		InitializeReadyCoordinate.Empty();
		break;
	}
}

void AMyWorldGenerator::StartGenerateNewArea(const int X, const int Y, const FMyAscensionData AscensionData)
{
	currentAscension = AscensionData;

	spawningAreasBlockCount = AscensionData.BlockSize;
	spawningNewAreaOrigin = TPairInitializer<int, int>(X, Y);
	startNewWorldGeneration = true;

	currentWorldGenerationType = AscensionData.StartGenerationType;

	currentArtifactCount = AscensionData.StartingArtifactData.WorldGenerationArtifacts.Num();
	currentArtifactModifierCount = 0;// AscensionData.StartingArtifactData.ModifierArtifacts;
	currentArtifactAncientCount = AscensionData.StartingArtifactData.BuildingArtifacts.Num();

	//if (AscensionData.isFindWinArtifact == false)
		//currentArtifactWincondition = AscensionData.HasWinArtifact;
	//else
		currentArtifactWincondition = 0;

	CurrentModifierArtifacts = 0; // AscensionData.StartingArtifactData.ModifierArtifacts;
	CurrentBuildingArtifacts = AscensionData.StartingArtifactData.BuildingArtifacts;
	CurrentWorldGenerationArtifacts = AscensionData.StartingArtifactData.WorldGenerationArtifacts; 
	
	blockDistanceToRockForTrees = AscensionData.TreeRelationDistanceToRock;
	spawnCloseToRocks = AscensionData.TreeCloseToRock;

	GenerateWithHole = AscensionData.HasHole;

	updateForNewArea();
}

void AMyWorldGenerator::StartGenerateExpansionOfArea(TPair<int, int> OriginCoords, const int BlockCount, /* const int artifactCount, const int artifactModifierCount, const int ArtifactAncientCount, const int ArtifactWinCount,*/ const bool generateHole)
{
	CurrentModifierArtifacts = 0;
	CurrentBuildingArtifacts.Empty(); 
	CurrentWorldGenerationArtifacts.Empty();

	spawningAreasBlockCount = BlockCount;
	spawningNewAreaOrigin = OriginCoords;

	FMyArtifactGenerationData targetArtifactGenerationData = MyAscensionManager->onGetArtifactGenerationData();

	currentArtifactCount = targetArtifactGenerationData.WorldGenerationArtifacts.Num();
	currentArtifactModifierCount = 0;// targetArtifactGenerationData.ModifierArtifacts;
	currentArtifactAncientCount = targetArtifactGenerationData.BuildingArtifacts.Num();
	currentArtifactWincondition = 0;//ArtifactWinCount;

	CurrentModifierArtifacts = 0;// targetArtifactGenerationData.ModifierArtifacts;
	CurrentBuildingArtifacts = targetArtifactGenerationData.BuildingArtifacts;
	CurrentWorldGenerationArtifacts = targetArtifactGenerationData.WorldGenerationArtifacts;


	UE_LOG(LogTemp, Warning, TEXT("MyWorldGenerator - StartGenerateExpansionOfArea: ArtCount: %d,  ModArt: %d, GenArt: %d, BuildArt: %d, winArt: %d"), currentArtifactCount, CurrentModifierArtifacts, CurrentBuildingArtifacts.Num(), CurrentWorldGenerationArtifacts.Num(), currentArtifactWincondition);

	GenerateWithHole = generateHole;

	updateForNewArea();
	startNewWorldGeneration = true;
}

void AMyWorldGenerator::updateForNewArea()
{
	InitializeReadyCoordinate.Empty();
	CurrentAreaData.Empty();
	CurrentCutOffsUpperRight.Empty();
}

void AMyWorldGenerator::expandWorldAtClosestPosition(const int X, const int Y, const bool hasHole, const MyArtifactGenerationType targetGenerationType, const int Blockcount, float NaturalRecurcesModifier)
{
	TPair<int, int> newAreaOrigin = closestEmptyArea(TPairInitializer<int, int>(X, Y));
	int targetBlockCount = Blockcount;

	currentWorldGenerationType = targetGenerationType;

	currentNaturalRecurcesModifier = NaturalRecurcesModifier;

	StartGenerateExpansionOfArea(newAreaOrigin, targetBlockCount,/* 0, 0, 0, 0,*/ hasHole);
}

AMyWorldBlockEffect* AMyWorldGenerator::getWorldBlockFromPool()
{
	if (1 >= WorldBlockEffectPooled.Num()) {
		ExpandWorldBlockEffectPool(10);
	}

	int TargeTPoolIndex = WorldBlockEffectPooled.Num() - 1;
	int newActiveIndex = WorldBlockEffectActive.Add(WorldBlockEffectPooled[TargeTPoolIndex]);
	WorldBlockEffectPooled.RemoveAt(TargeTPoolIndex);

	return WorldBlockEffectActive[newActiveIndex];
}

bool AMyWorldGenerator::UnsubsribeWorldBockEffectToPool(AMyWorldBlockEffect* targetActiveWorldBlockEffect)
{
	if (targetActiveWorldBlockEffect->IsValidLowLevel()) {

		int TargetActiveIndex = targetActiveWorldBlockEffect->GetActivePoolIndex();
		WorldBlockEffectPooled.Add(WorldBlockEffectActive[TargetActiveIndex]);
		WorldBlockEffectActive.RemoveAt(TargetActiveIndex);

		return true;
	}
	else {
		return false;
	}
}

void AMyWorldGenerator::ExpandWorldBlockEffectPool_Implementation(int ExpansionSize)
{

}

TArray<FMyWorldBlockData> AMyWorldGenerator::GenerateNewWorldOutput_Implementation()
{
	return CurrentAreaData;
}
 
bool AMyWorldGenerator::InitializeWorldGenerationData_Implementation()
{
	return true;
}

//--- SORT WORLD BLOCKS ---

//--- Water Sort
void AMyWorldGenerator::sortWaterWorldBlocks(int waterCount, TArray<TPair<int, int>>& InitializeReadyCoordinates)
{
	int remaningWaterBlocks = waterCount;
	TArray<TPair<int, int>> waterValidInitilizeReadyCoords = deductWaterValidFromInitializeReadyCoords(InitializeReadyCoordinates);

	if (0 >= waterValidInitilizeReadyCoords.Num()) {
		UE_LOG(LogTemp, Warning, TEXT("MyWorldGeneration - SortWAterWorldBlocks: No valid intilize Ready Coords"));
		InitializeReadyCoordinate.Append(waterValidInitilizeReadyCoords);
		return;
	}

	while (remaningWaterBlocks > 0)
	{
		//Randomize how many blocks to place, use remaningWaterBlocks as MAX if MaxAdjecentValue is over RemaingWaterBlocks 
		int tempMaxAdjecentWaterBlocks = MaxMaxFrostClusterBlocks;
		if (MaxMaxFrostClusterBlocks > remaningWaterBlocks) {
			tempMaxAdjecentWaterBlocks = remaningWaterBlocks;
		}

		int placingBlocksCount = FMath::RandRange(MinAdjecentWaterBlock, tempMaxAdjecentWaterBlocks);
		int placingIndex = FMath::RandRange(0, (waterValidInitilizeReadyCoords.Num() - 1));

		TPair<int, int> TargetAssignWaterCoords;

		if (waterValidInitilizeReadyCoords.IsValidIndex(placingIndex)) {
			TargetAssignWaterCoords = waterValidInitilizeReadyCoords[placingIndex];
		} else {
			break;
		}

		for (int i = 0; placingBlocksCount >= i; i++) {

			if (!waterValidInitilizeReadyCoords.IsValidIndex(placingIndex))
				break;

			if (!waterValidInitilizeReadyCoords.Contains(TargetAssignWaterCoords))
				break;

			if (isEdgeBlock(TargetAssignWaterCoords))
				break;

			AssigneWorldBlockType(TargetAssignWaterCoords, MyWorldBlockType::Water, MyNaturalResourcesType::NR_None, waterValidInitilizeReadyCoords);
			//UE_LOG(LogTemp, Warning, TEXT("DON´T Spawn Water on This Number: %d"), remaningWaterBlocks);

			TargetAssignWaterCoords = GetValidAdjecentWaterCoordinate(TargetAssignWaterCoords, waterValidInitilizeReadyCoords);
			remaningWaterBlocks--;
		}
	}

	InitializeReadyCoordinate.Append(waterValidInitilizeReadyCoords);
}

TArray<TPair<int, int>> AMyWorldGenerator::deductWaterValidFromInitializeReadyCoords(TArray<TPair<int, int>>& InitializeReadyCoordinates)
{
	TArray<TPair<int, int>> waterReadyCoordinates;

	for (int i = (InitializeReadyCoordinates.Num() - 1); i >= 0; i--) {
		
		if (!isEdgeBlock(InitializeReadyCoordinates[i])) {
			waterReadyCoordinates.Add(InitializeReadyCoordinates[i]);

			InitializeReadyCoordinates.RemoveAt(i);
		}		
	}

	return waterReadyCoordinates;
}

TPair<int, int> AMyWorldGenerator::GetValidAdjecentWaterCoordinate(TPair<int, int> OldCoordinate, const TArray<TPair<int, int>>& NoEdgesInitializeReadyCoordinates)
{
	int adjecentBlockCount = 4;
	int StartingAdjecentTile = FMath::RandRange(1, 4);
	TPair<int, int> newCoord = OldCoordinate;
	
	while (adjecentBlockCount > 0)
	{
		if (StartingAdjecentTile == 1) {
			TPair<int, int> newCoords = TPairInitializer<int, int>(OldCoordinate.Key + 1, OldCoordinate.Value);
			if (!NoEdgesInitializeReadyCoordinates.Contains(newCoords)) {
				//UE_LOG(LogTemp, Warning, TEXT("out of bound or edge: %d %d"), newCoord.Key + 1, newCoord.Value);
			}
			else {
				newCoord.Key++;
				break;
			}
		}
		else if (StartingAdjecentTile == 2) {

			TPair<int, int> newCoords = TPairInitializer<int, int>(OldCoordinate.Key - 1, OldCoordinate.Value);
			if (!NoEdgesInitializeReadyCoordinates.Contains(newCoords)) {
				//UE_LOG(LogTemp, Warning, TEXT("out of bound or edge: %d %d"), newCoord.Key - 1, newCoord.Value);
			}
			else {
				newCoord.Key--;
				break;
			}

		}
		else if (StartingAdjecentTile == 3) {

			TPair<int, int> newCoords = TPairInitializer<int, int>(OldCoordinate.Key, OldCoordinate.Value + 1);
			if (!NoEdgesInitializeReadyCoordinates.Contains(newCoords)) {
				//UE_LOG(LogTemp, Warning, TEXT("out of bound or edge: %d %d"), newCoord.Key, newCoord.Value + 1);
			}
			else {
				newCoord.Value++;
				break;
			}

		}
		else {

			TPair<int, int> newCoords = TPairInitializer<int, int>(OldCoordinate.Key, OldCoordinate.Value - 1);
			if (!NoEdgesInitializeReadyCoordinates.Contains(newCoords)) {
				//UE_LOG(LogTemp, Warning, TEXT("Out of bound or edge: %d %d"), newCoord.Key, newCoord.Value - 1);
			}
			else {
				newCoord.Value--;
				break;
			}

		}

		if (StartingAdjecentTile < 3)
			StartingAdjecentTile++;
		else
			StartingAdjecentTile = 1;

		adjecentBlockCount--;
	}

	return newCoord;
}

//--- Rock Sort
void AMyWorldGenerator::sortRockWorldBlocks(int TargetRockCount, TArray<TPair<int, int>>& InitializeReadyCoordinates)
{
	int rockSpawnCounter = 0;
	int remaningRockBlocks = TargetRockCount;
	bool isCornerBlock = false;

	if (0 >= InitializeReadyCoordinates.Num()) {
		UE_LOG(LogTemp, Warning, TEXT("World Generator - SortRockWordlBlocks: InitializeReadyCoords is EMPTY"));
		return;
	}

	//UE_LOG(LogTemp, Warning, TEXT("ROCK ROCK ROCK in: %d"), RockCount);

	while (remaningRockBlocks > 0)
	{
		int tempMaxRockclusterBlocks = MaxRockClusterBlock;
		if (tempMaxRockclusterBlocks > remaningRockBlocks) {
			tempMaxRockclusterBlocks = remaningRockBlocks;
		}

		int placingBlocksCount = FMath::RandRange(MinRockClusterBlock, tempMaxRockclusterBlocks);
		int placingIndex = FMath::RandRange(0, (InitializeReadyCoordinates.Num() - 1));
		TPair<int, int> TargetAssignRockCoords = InitializeReadyCoordinates[placingIndex];

		for (int i = 0; placingBlocksCount >= i; i++) {

			if (!InitializeReadyCoordinates.IsValidIndex(placingIndex))
				break;

			AssigneWorldBlockType(TargetAssignRockCoords, MyWorldBlockType::Rock, MyNaturalResourcesType::NR_None, InitializeReadyCoordinates);
			//UE_LOG(LogTemp, Warning, TEXT("DON´T Spawn Water on This Number: %d"), remaningWaterBlocks);

			if(isCornerBlock == false)
			TargetAssignRockCoords = getValidSurroundingRockBlock(TargetAssignRockCoords, isCornerBlock, InitializeReadyCoordinates);
			else {
				isCornerBlock = false;
				//UE_LOG(LogTemp, Warning, TEXT("I AM ADJECENT"));
				TargetAssignRockCoords = GetValidAdjecentRockCoordinate(TargetAssignRockCoords, InitializeReadyCoordinates);
			}

			remaningRockBlocks--;
		}
	}
}

TPair<int, int> AMyWorldGenerator::getValidSurroundingRockBlock(TPair<int, int> StartingCoord, bool& IsCornerBlock, const TArray<TPair<int, int>>& InitializeReadyCoordinates)
{
	TPairInitializer<int, int> tempCoordinate = StartingCoord;
	int SurroundingBlockNum = FMath::RandRange(1, 8);
	int surroundingBlockCount = 8;

	while (surroundingBlockCount > 0)
	{
		if (SurroundingBlockNum == 1) {
			TPair<int, int> newCoords = TPairInitializer<int, int>(tempCoordinate.Key + 1, tempCoordinate.Value + 1);
			if (!InitializeReadyCoordinates.Contains(newCoords)) {
				//UE_LOG(LogTemp, Warning, TEXT("out of bound or edge: %d %d"), newCoord.Key + 1, newCoord.Value);
			}
			else {

				IsCornerBlock = true;

				tempCoordinate.Key++;
				tempCoordinate.Value++;
				break;
			}
		}
		else if (SurroundingBlockNum == 2) {
			
			TPair<int, int> newCoords = TPairInitializer<int, int>(tempCoordinate.Key, tempCoordinate.Value + 1);
			if (!InitializeReadyCoordinates.Contains(newCoords)) {
				//UE_LOG(LogTemp, Warning, TEXT("out of bound or edge: %d %d"), newCoord.Key + 1, newCoord.Value);
			}
			else {
				tempCoordinate.Value++;
				break;
			}

			
		}
		else if (SurroundingBlockNum == 3) {

			TPair<int, int> newCoords = TPairInitializer<int, int>(tempCoordinate.Key - 1, tempCoordinate.Value + 1);
			if (!InitializeReadyCoordinates.Contains(newCoords)) {
				//UE_LOG(LogTemp, Warning, TEXT("out of bound or edge: %d %d"), newCoord.Key + 1, newCoord.Value);
			}
			else {

				IsCornerBlock = true;

				tempCoordinate.Key--;
				tempCoordinate.Value++;
				break;
			}

		}
		else if (SurroundingBlockNum == 4) {

			TPair<int, int> newCoords = TPairInitializer<int, int>(tempCoordinate.Key + 1, tempCoordinate.Value);
			if (!InitializeReadyCoordinates.Contains(newCoords)) {
				//UE_LOG(LogTemp, Warning, TEXT("out of bound or edge: %d %d"), newCoord.Key + 1, newCoord.Value);
			}
			else {
				tempCoordinate.Key++;
				break;
			}


		}
		else if (SurroundingBlockNum == 5) {

			TPair<int, int> newCoords = TPairInitializer<int, int>(tempCoordinate.Key - 1, tempCoordinate.Value);
			if (!InitializeReadyCoordinates.Contains(newCoords)) {
				//UE_LOG(LogTemp, Warning, TEXT("out of bound or edge: %d %d"), newCoord.Key + 1, newCoord.Value);
			}
			else {
				tempCoordinate.Key--;
				break;
			}


		}
		else if (SurroundingBlockNum == 6) {

			TPair<int, int> newCoords = TPairInitializer<int, int>(tempCoordinate.Key, tempCoordinate.Value - 1);
			if (!InitializeReadyCoordinates.Contains(newCoords)) {
				//UE_LOG(LogTemp, Warning, TEXT("out of bound or edge: %d %d"), newCoord.Key + 1, newCoord.Value);
			}
			else {
				tempCoordinate.Value--;
				break;
			}

		} else if (SurroundingBlockNum == 7) {

			TPair<int, int> newCoords = TPairInitializer<int, int>(tempCoordinate.Key + 1, tempCoordinate.Value - 1);
			if (!InitializeReadyCoordinates.Contains(newCoords)) {
				//UE_LOG(LogTemp, Warning, TEXT("out of bound or edge: %d %d"), newCoord.Key + 1, newCoord.Value);
			}
			else {

				IsCornerBlock = true;

				tempCoordinate.Key++;
				tempCoordinate.Value--;
				break;
			}
		}
		else if (SurroundingBlockNum == 8) {

			TPair<int, int> newCoords = TPairInitializer<int, int>(tempCoordinate.Key - 1, tempCoordinate.Value - 1);
			if (!InitializeReadyCoordinates.Contains(newCoords)) {
				//UE_LOG(LogTemp, Warning, TEXT("out of bound or edge: %d %d"), newCoord.Key + 1, newCoord.Value);
			}
			else {

				IsCornerBlock = true;

				tempCoordinate.Key--;
				tempCoordinate.Value--;
				break;
			}

		}

	if (SurroundingBlockNum < 7)
		SurroundingBlockNum++;
	else
		SurroundingBlockNum = 1;

	surroundingBlockCount--;
}

	return tempCoordinate;
}

TPair<int, int> AMyWorldGenerator::GetValidAdjecentRockCoordinate(TPair<int, int> StartingCoord, const TArray<TPair<int, int>>& InitializeReadyCoordinates)
{
	int adjecentBlockCount = 4;
	int StartingAdjecentTile = FMath::RandRange(1, 4);
	TPair<int, int> newCoord = StartingCoord;

	while (adjecentBlockCount > 0)
	{
		if (StartingAdjecentTile == 1) {
			TPair<int, int> newCoords = TPairInitializer<int, int>(StartingCoord.Key + 1, StartingCoord.Value);
			if (!InitializeReadyCoordinates.Contains(newCoords)) {
				//UE_LOG(LogTemp, Warning, TEXT("out of bound or edge: %d %d"), newCoord.Key + 1, newCoord.Value);
			}
			else {
				newCoord.Key++;
				break;
			}
		}
		else if (StartingAdjecentTile == 2) {

			TPair<int, int> newCoords = TPairInitializer<int, int>(StartingCoord.Key - 1, StartingCoord.Value);
			if (!InitializeReadyCoordinates.Contains(newCoords)) {
				//UE_LOG(LogTemp, Warning, TEXT("out of bound or edge: %d %d"), newCoord.Key - 1, newCoord.Value);
			}
			else {
				newCoord.Key--;
				break;
			}

		}
		else if (StartingAdjecentTile == 3) {

			TPair<int, int> newCoords = TPairInitializer<int, int>(StartingCoord.Key, StartingCoord.Value + 1);
			if (!InitializeReadyCoordinates.Contains(newCoords)) {
				//UE_LOG(LogTemp, Warning, TEXT("out of bound or edge: %d %d"), newCoord.Key, newCoord.Value + 1);
			}
			else {
				newCoord.Value++;
				break;
			}

		}
		else {

			TPair<int, int> newCoords = TPairInitializer<int, int>(StartingCoord.Key, StartingCoord.Value - 1);
			if (!InitializeReadyCoordinates.Contains(newCoords)) {
				//UE_LOG(LogTemp, Warning, TEXT("Out of bound or edge: %d %d"), newCoord.Key, newCoord.Value - 1);
			}
			else {
				newCoord.Value--;
				break;
			}

		}

		if (StartingAdjecentTile < 3)
			StartingAdjecentTile++;
		else
			StartingAdjecentTile = 1;

		adjecentBlockCount--;
	}

	return newCoord;
}

//--- Frost Sort
void AMyWorldGenerator::sortForstWorldBlocks(TArray<TPair<int, int>>& InitializeReadyCoordinates)
{
	//UE_LOG(LogTemp, Warning, TEXT("FROST FROST FROST in: %d"), FrostCount);

	TArray<TPair<int, int>> frostReadyCoords = deductFrostValidFromInitializeReadyCoords(InitializeReadyCoordinates);

	int FrostCount = frostReadyCoords.Num() * FMath::FRandRange(WorldGenerationDataList.Find(MyWorldBlockType::Frost)->BlockMin, WorldGenerationDataList.Find(MyWorldBlockType::Frost)->BlockMax);
	
	if (0 > frostReadyCoords.Num()) {
		InitializeReadyCoordinate.Append(frostReadyCoords);
		return;
	}

	int remaningFrostBlocks = FrostCount;
	
	//UE_LOG(LogTemp, Warning, TEXT("FROST COUNT: %d"), FrostCount);

	while (remaningFrostBlocks > 0)
	{
		//Randomize how many blocks to place, use remaningWaterBlocks as MAX if MaxAdjecentValue is over RemaingWaterBlocks 
		int tempMaxFrostClusterBlocks = MaxFrostClusterBlock;
		if (MaxMaxFrostClusterBlocks > remaningFrostBlocks) {
			tempMaxFrostClusterBlocks = remaningFrostBlocks;
		}
		
		int placingBlocksCount = FMath::RandRange(MinFrostClusterBlock, tempMaxFrostClusterBlocks);
		int placingIndex = FMath::RandRange(0, (frostReadyCoords.Num() - 1));
		TPair<int, int> OriginCoord;

		if (frostReadyCoords.IsValidIndex(placingIndex)) {
			OriginCoord = frostReadyCoords[placingIndex];
		} else {
			break;
		}


		//UE_LOG(LogTemp, Warning, TEXT("ORGIN FROST BLOCK: %d %d"), OriginCoord.Key, OriginCoord.Value);

		while(placingBlocksCount > 0) {	
			int placedAdjecentBlocks = 0;
			
			assigneFrostBlock(frostReadyCoords, OriginCoord, remaningFrostBlocks);
			placingBlocksCount--;
			//AssigneTypeToBlock(OriginCoord, MyWorldBlockType::Frost, frostReadyCoords);

			//if(placingBlocksCount > 4)
			//placedAdjecentBlocks = tryPlaceFrostOnAllAdjecent(4, OriginCoord, frostReadyCoords, CrystalBlockNumbers);
			//else
			placedAdjecentBlocks = tryPlaceFrostOnAllAdjecent(remaningFrostBlocks, OriginCoord, frostReadyCoords/*, CrystalBlockNumbers*/);

			placingBlocksCount = placingBlocksCount - placedAdjecentBlocks;
			remaningFrostBlocks = remaningFrostBlocks - placedAdjecentBlocks;

			if (TryFindAFrostCorner(OriginCoord, frostReadyCoords) == false) {
				//UE_LOG(LogTemp, Warning, TEXT("Fail to find new frostCorner"));
				break;
			}
		}
	}

	InitializeReadyCoordinates.Append(frostReadyCoords);
}

int AMyWorldGenerator::tryPlaceFrostOnAllAdjecent(const int BlockCount, const TPair<int, int> OriginCoords, TArray<TPair<int, int>>& frostReadyCoords /*, TArray<int>& CrystalBlockNumbers*/)
{
	int blockLeft = 0;

	if (BlockCount > 4)
		blockLeft = 4; //corners
	else
		blockLeft = BlockCount;

	int adjecentBlockCount = FMath::RandRange(1, 4);
	int adjecentCounter = 1;
	int BlockCounter = BlockCount;


	while (true)
	{
		if (adjecentBlockCount == 1) {

			TPair<int, int> newCoords = TPairInitializer<int, int>(OriginCoords.Key + 1, OriginCoords.Value);
			if (!frostReadyCoords.Contains(newCoords)) {
				//UE_LOG(LogTemp, Warning, TEXT("Frost invalid: %d %d %d"), newCoords.Key, newCoords.Value, adjecentBlockCount);
			}
			else {
				//UE_LOG(LogTemp, Warning, TEXT("spawn Frost Block here: %d %d %d"), newCoords.Key, newCoords.Value, adjecentBlockCount);
				assigneFrostBlock(frostReadyCoords, newCoords, BlockCounter);
				//UE_LOG(LogTemp, Warning, TEXT("BLOCK COUNTER: %d"), BlockCounter);
				blockLeft--;
				BlockCounter--;
			}

			adjecentCounter++;
		}
		else if (adjecentBlockCount == 2) {

			TPair<int, int> newCoords = TPairInitializer<int, int>(OriginCoords.Key - 1, OriginCoords.Value);
			if (!frostReadyCoords.Contains(newCoords)) {
				//UE_LOG(LogTemp, Warning, TEXT("Frost invalid: %d %d %d"), newCoords.Key, newCoords.Value, adjecentBlockCount);
			}
			else {
				//UE_LOG(LogTemp, Warning, TEXT("spawn Frost Block here: %d %d %d"), newCoords.Key, newCoords.Value, adjecentBlockCount);
				assigneFrostBlock(frostReadyCoords, newCoords, BlockCounter);
				//UE_LOG(LogTemp, Warning, TEXT("BLOCK COUNTER: %d"), BlockCounter);
				blockLeft--;
				BlockCounter--;
			}

			adjecentCounter++;
		}
		else if (adjecentBlockCount == 3) {
			TPair<int, int> newCoords = TPairInitializer<int, int>(OriginCoords.Key, OriginCoords.Value + 1);
			if (!frostReadyCoords.Contains(newCoords)) {
				//UE_LOG(LogTemp, Warning, TEXT("Frost invalid: %d %d %d"), newCoords.Key , newCoords.Value, adjecentBlockCount);
			}
			else {
				//UE_LOG(LogTemp, Warning, TEXT("spawn Frost Block here: %d %d %d"), newCoords.Key, newCoords.Value, adjecentBlockCount);
				assigneFrostBlock(frostReadyCoords, newCoords, BlockCounter);
				//UE_LOG(LogTemp, Warning, TEXT("BLOCK COUNTER: %d"), BlockCounter);
				blockLeft--;
				BlockCounter--;
			}

			adjecentCounter++;
		}
		else {
			TPair<int, int> newCoords = TPairInitializer<int, int>(OriginCoords.Key, OriginCoords.Value - 1);
			if (!frostReadyCoords.Contains(newCoords)) {
				//UE_LOG(LogTemp, Warning, TEXT("Frost invalid: %d %d %d"), newCoords.Key, newCoords.Value, adjecentBlockCount);
			}
			else {
				//UE_LOG(LogTemp, Warning, TEXT("spawn Frost Block here: %d %d %d"), newCoords.Key, newCoords.Value, adjecentBlockCount);
				assigneFrostBlock(frostReadyCoords, newCoords, BlockCounter);
				//UE_LOG(LogTemp, Warning, TEXT("BLOCK COUNTER: %d"), BlockCounter);
				blockLeft--;
				BlockCounter--;
			}

			adjecentCounter++;
		}

		if (blockLeft <= 0)
			break;

		if (adjecentCounter > 4)
			break;

		adjecentBlockCount++;
		if (adjecentBlockCount > 4)
			adjecentBlockCount = 1;
	}

	return blockLeft;
}

bool AMyWorldGenerator::TryFindAFrostCorner(TPair<int, int>& newCornerCoord, const TArray<TPair<int, int>>& frostReadyCoords)
{
	int Counter = FMath::RandRange(1, 4);
	int CornerCounter = 1;
	while (true)
	{
		if (Counter == 1) {

			TPair<int, int> newCoords = TPairInitializer<int, int>(newCornerCoord.Key + 1, newCornerCoord.Value + 1);
			if (!frostReadyCoords.Contains(newCoords)) {
			}
			else {
				newCornerCoord = newCoords;
				return true;
			}

			CornerCounter++;
		}
		else if (Counter == 2) {

			TPair<int, int> newCoords = TPairInitializer<int, int>(newCornerCoord.Key - 1, newCornerCoord.Value + 1);
			if (!frostReadyCoords.Contains(newCoords)) {
				//UE_LOG(LogTemp, Warning, TEXT("Frost invalid: %d %d %d"), newCoords.Key, newCoords.Value, adjecentBlockCount);
			}
			else {
				//UE_LOG(LogTemp, Warning, TEXT("spawn Frost Block here: %d %d %d"), newCoords.Key, newCoords.Value, adjecentBlockCount);
				newCornerCoord = newCoords;
				return true;
			}

			CornerCounter++;
		}
		else if (Counter == 3) {
			TPair<int, int> newCoords = TPairInitializer<int, int>(newCornerCoord.Key + 1, newCornerCoord.Value - 1);
			if (!frostReadyCoords.Contains(newCoords)) {
				//UE_LOG(LogTemp, Warning, TEXT("Frost invalid: %d %d %d"), newCoords.Key , newCoords.Value, adjecentBlockCount);
			}
			else {
				//UE_LOG(LogTemp, Warning, TEXT("spawn Frost Block here: %d %d %d"), newCoords.Key, newCoords.Value, adjecentBlockCount);
				newCornerCoord = newCoords;
				return true;
			}

			CornerCounter++;
		}
		else {
			TPair<int, int> newCoords = TPairInitializer<int, int>(newCornerCoord.Key - 1, newCornerCoord.Value - 1);
			if (!frostReadyCoords.Contains(newCoords)) {
				//UE_LOG(LogTemp, Warning, TEXT("Frost invalid: %d %d %d"), newCoords.Key, newCoords.Value, adjecentBlockCount);
			}
			else {
				//UE_LOG(LogTemp, Warning, TEXT("spawn Frost Block here: %d %d %d"), newCoords.Key, newCoords.Value, adjecentBlockCount);
				newCornerCoord = newCoords;
				return true;
			}

			CornerCounter++;
		}

		if (CornerCounter > 4)
			break;

		Counter++;
		if (Counter > 4)
			Counter = 1;
	}

	return false;
}

void AMyWorldGenerator::assigneFrostBlock(TArray<TPair<int, int>>& frostReadyCoords, const TPair<int, int> TargetAssignRockCoords, const int remaningFrostBlocks)
{
	bool ShouldSpawnCrystal = false;

	if (ShouldSpawnCrystal == true) {
		AssigneWorldBlockType(TargetAssignRockCoords, MyWorldBlockType::Frost, MyNaturalResourcesType::NR_Crystal, frostReadyCoords);
		//UE_LOG(LogTemp, Warning, TEXT("Spawn frost on This Number: %d"), remaningFrostBlocks);
	}
	else {
		AssigneWorldBlockType(TargetAssignRockCoords, MyWorldBlockType::Frost, MyNaturalResourcesType::NR_None, frostReadyCoords);
		//UE_LOG(LogTemp, Warning, TEXT("DON´T Spawn frost on This Number: %d"), remaningFrostBlocks);
	}

}

TArray<TPair<int, int>> AMyWorldGenerator::deductFrostValidFromInitializeReadyCoords(TArray<TPair<int, int>>& InitializeReadyCoordinates)
{
	TArray<TPair<int, int>> deductedValidFrostReadyCoords;

	for (int i = (InitializeReadyCoordinates.Num() - 1); i >= 0; i--) {
		if (InitializeReadyCoordinates[i].Key > 0) {
			deductedValidFrostReadyCoords.Add(InitializeReadyCoordinates[i]);
			InitializeReadyCoordinates.RemoveAt(i);
		}
	}

	return deductedValidFrostReadyCoords;
}

//--- Sort Grass
void AMyWorldGenerator::sortGrassWorldBlocks(TArray<TPair<int, int>>& InitializeReadyCoordinates)
{
	//UE_LOG(LogTemp, Warning, TEXT("GRASS GRASS GRASS: %d"), InitializeReadyCoordinate.Num());

	int counter = InitializeReadyCoordinates.Num();
	for (int i = (counter - 1); 0 <= i; i--) {
		TPair<int, int> blockCoords = InitializeReadyCoordinates[i];

		AssigneWorldBlockType(blockCoords, MyWorldBlockType::Grass, MyNaturalResourcesType::NR_None, InitializeReadyCoordinates);
	}
}

bool AMyWorldGenerator::AssigneWorldBlockType(const TPair<int, int>& Coords, const MyWorldBlockType type, const MyNaturalResourcesType NaturalResourceType, TArray<TPair<int, int>>& InitializeReadyCoordinates)
{
	if (InitializeReadyCoordinates.Contains(Coords) == false) {
		UE_LOG(LogTemp, Warning, TEXT("WORLD BLOCK SORTING (AssigneTypeToBlock): Could not find Coord in Initilize Ready Coord List. Coords: %d %d"), Coords.Key, Coords.Value);
		return false;
	}

	if (HasTargetBlock(Coords.Key, Coords.Value) == false) {
		UE_LOG(LogTemp, Warning, TEXT("WORLD BLOCK SORTING (AssigneTypeToBlock): Could not find Coord in WorldBlockData"));
		return false;
	}

	if (NaturalResourceType != MyNaturalResourcesType::NR_None)
		WorldBlockData.Find(Coords)->NaturalResourcesType = NaturalResourceType;
	else
		WorldBlockData.Find(Coords)->NaturalResourcesType = MyNaturalResourcesType::NR_None;

	WorldBlockData.Find(Coords)->WorldBlockType = type;
	CurrentAreaData.Add(GetTargetBlock(Coords.Key, Coords.Value));

	InitializeReadyCoordinates.RemoveSingle(Coords);

	return true;
}

//--- Artifact
void AMyWorldGenerator::tryAssignArtifactToValidCurrentArea(int artifactCount, int artifactModifiersCount, int ArtifactAncientCount, int ArtifactWinCount)
{		
	UE_LOG(LogTemp, Warning, TEXT("MyWorldGenerator - tryAssignArtifactToValidCurrentArea: ModArt: %d, GenArt: %d, BuildArt: %d, winArt %d"),  artifactModifiersCount, artifactCount, ArtifactAncientCount, ArtifactWinCount);

	if (0 >= CurrentAreaData.Num()) {
		UE_LOG(LogTemp, Warning, TEXT("MyWorldGenerator - tryAssignArtifactToValidCurrentArea: CurrentAreaData is empty"));
		return;
	}

	int allArtifactCounter = artifactCount + artifactModifiersCount + ArtifactAncientCount + ArtifactWinCount;

	//UE_LOG(LogTemp, Warning, TEXT("MyWorldGenerator - tryAssignArtifactToValidCurrentArea: ArtCount: %d,  ModArt: %d, GenArt: %d, BuildArt: %d"), allArtifactCounter, artifactModifiersCount, artifactCount, ArtifactAncientCount);

	for (int i = 0; allArtifactCounter > i; i++)
	{
		int StartIndex = FMath::RandRange(0, CurrentAreaData.Num() - 1);
		int potentialArtifactIndex = StartIndex;
		bool shouldFindNextIndex = true;

		while (shouldFindNextIndex)
		{
			if (CurrentAreaData[potentialArtifactIndex].HasNaturalResoruces == false &&
				CurrentAreaData[potentialArtifactIndex].WorldBlockType != MyWorldBlockType::Water &&
				CurrentAreaData[potentialArtifactIndex].NaturalResourcesType == MyNaturalResourcesType::NR_None &&
				isAdjecentEdgeBlock(TPairInitializer<int, int>(CurrentAreaData[potentialArtifactIndex].CordX, CurrentAreaData[potentialArtifactIndex].CordY)) == false &&
				CurrentAreaData[potentialArtifactIndex].HasArtifact == false
				){
				
				if (artifactCount > 0) {

					CurrentAreaData[potentialArtifactIndex].HasArtifact = true;
					WorldBlockData.Find(TPairInitializer<int, int>(CurrentAreaData[potentialArtifactIndex].CordX, CurrentAreaData[potentialArtifactIndex].CordY))->HasArtifact = true;
					CurrentAreaData[potentialArtifactIndex].WorldGenerationArtifacts = CurrentWorldGenerationArtifacts[artifactCount - 1];
					WorldBlockData.Find(TPairInitializer<int, int>(CurrentAreaData[potentialArtifactIndex].CordX, CurrentAreaData[potentialArtifactIndex].CordY))->WorldGenerationArtifacts = CurrentWorldGenerationArtifacts[artifactCount - 1];
					
					artifactCount--;
				}
				else if (artifactModifiersCount > 0) {
					
					CurrentAreaData[potentialArtifactIndex].HasArtifact = true;
					WorldBlockData.Find(TPairInitializer<int, int>(CurrentAreaData[potentialArtifactIndex].CordX, CurrentAreaData[potentialArtifactIndex].CordY))->HasArtifact = true;

					CurrentAreaData[potentialArtifactIndex].ModifierArtifacts = true;
					WorldBlockData.Find(TPairInitializer<int, int>(CurrentAreaData[potentialArtifactIndex].CordX, CurrentAreaData[potentialArtifactIndex].CordY))->ModifierArtifacts = true;

					artifactModifiersCount--;
				}
				else if (ArtifactAncientCount > 0) {
					
					CurrentAreaData[potentialArtifactIndex].HasArtifact = true;
					WorldBlockData.Find(TPairInitializer<int, int>(CurrentAreaData[potentialArtifactIndex].CordX, CurrentAreaData[potentialArtifactIndex].CordY))->HasArtifact = true;

					CurrentAreaData[potentialArtifactIndex].BuildingArtifacts = CurrentBuildingArtifacts[ArtifactAncientCount - 1];
					WorldBlockData.Find(TPairInitializer<int, int>(CurrentAreaData[potentialArtifactIndex].CordX, CurrentAreaData[potentialArtifactIndex].CordY))->BuildingArtifacts = CurrentBuildingArtifacts[ArtifactAncientCount - 1];

					ArtifactAncientCount--;
				}
				else if (ArtifactWinCount > 0) {

					CurrentAreaData[potentialArtifactIndex].HasArtifact = true;
					WorldBlockData.Find(TPairInitializer<int, int>(CurrentAreaData[potentialArtifactIndex].CordX, CurrentAreaData[potentialArtifactIndex].CordY))->HasArtifact = true;

					CurrentAreaData[potentialArtifactIndex].HasArtifactWin = true;
					WorldBlockData.Find(TPairInitializer<int, int>(CurrentAreaData[potentialArtifactIndex].CordX, CurrentAreaData[potentialArtifactIndex].CordY))->HasArtifactWin = true;

					ArtifactWinCount--;
				}

				shouldFindNextIndex = false;
			}
			else {
				potentialArtifactIndex++;

				if (potentialArtifactIndex >= CurrentAreaData.Num() - 1) {
					potentialArtifactIndex = 0;
				}

				if (potentialArtifactIndex == StartIndex) {
					UE_LOG(LogTemp, Warning, TEXT("MyWorldGenerator - tryAssignArtifactToValidCurrentArea: Failed to assign Artifact"));

					shouldFindNextIndex = false;
				}


				
			}
		}

	}
}

//--- SPAWN NEW AREA ---

TArray<FMyWorldBlockData> AMyWorldGenerator::GenerateNewArea(TArray<TPair<int, int>>& InitializeReadyCoordinates, TPair<int, int> originCoordiates, int WorldBlockCount)
{
	createNewWorldBlock(originCoordiates, WorldBlockCount, InitializeReadyCoordinates);

	//UE_LOG(LogTemp, Warning, TEXT("Size of InitializeReadyCoordinate: %d"), InitializeReadyCoordinate.Num());

	return CurrentAreaData;
}

void AMyWorldGenerator::createNewWorldBlock(const TPair<int, int> originCoords, const int blockCount, TArray<TPair<int, int>>& InitializeReadyCoords)
{
	//UE_LOG(LogTemp, Warning, TEXT("RADIUS: %d"), getNewAreaRadius(blockCount));

	CurrentAreaCoordsInSpawnOrder.Empty();

	FMyWorldBlockData TempWorldBlock;

	int currentBlockCount = blockCount;
	int loopCounter = 1;

	//UE_LOG(LogTemp, Warning, TEXT("ORIGIN COORDS: %d"), originCoords.Key, originCoords.Value);

	int UpperRightCutOff = 0;
	int UpperLeftCutOff = 0;
	int belowRightCutOff = 0;
	int belowLeftCutOff = 0;

	while (true) {

		placeNewRowWorldBlocks(loopCounter, originCoords, currentBlockCount, MyCorner::UpperRight, UpperRightCutOff, InitializeReadyCoords);
		placeNewRowWorldBlocks(loopCounter, originCoords, currentBlockCount, MyCorner::BelowRight, belowLeftCutOff, InitializeReadyCoords);
		placeNewRowWorldBlocks(loopCounter, originCoords, currentBlockCount, MyCorner::BelowLeft, belowRightCutOff, InitializeReadyCoords);
		placeNewRowWorldBlocks(loopCounter, originCoords, currentBlockCount, MyCorner::UpperLeft, UpperLeftCutOff, InitializeReadyCoords);

		if (0 >= currentBlockCount) { break; }

		loopCounter++;
	}
}

void AMyWorldGenerator::placeNewRowWorldBlocks(const int loopCounter, const TPair<int, int> originCoords, int& currentBlockCount, MyCorner whichCorner, int& cutOff, TArray<TPair<int, int>>& InitializeReadyCoords)
{
	FMyWorldBlockData TempWorldBlock;

	for (int j = 1; loopCounter >= j; j++) {

		if (0 >= currentBlockCount) { break; }

		TPair<int, int> targetCoord;

		switch (whichCorner)
		{
		case UpperRight:

			targetCoord = TPairInitializer<int, int>(j, -((loopCounter - j) - tryCutOff(j)));

			break;
		case UpperLeft:
			targetCoord = TPairInitializer<int, int>(-(loopCounter - j), -(j - tryCutOff(-(loopCounter - j))));
			break;
		case BelowRight:
			targetCoord = TPairInitializer<int, int>(j, ((loopCounter - j) - tryCutOff(j)));
			break;
		case BelowLeft:
			targetCoord = TPairInitializer<int, int>(-(loopCounter - j), ((j - 1) - tryCutOff(-(loopCounter - j))));
			break;
		default:
			break;
		}

		targetCoord.Key = originCoords.Key + targetCoord.Key;
		targetCoord.Value = originCoords.Value + targetCoord.Value;

		if (!WorldBlockData.Contains(targetCoord)) {
			InitializeReadyCoords.Add(targetCoord);

			TempWorldBlock.CordX = targetCoord.Key;
			TempWorldBlock.CordY = targetCoord.Value;

			//TPair<int, int> InlilizeReadyCoord = targetCoord;
			WorldBlockData.Add(targetCoord, TempWorldBlock);

			CurrentAreaCoordsInSpawnOrder.Add(targetCoord);

			currentBlockCount--;

		}
		else {
			//UE_LOG(LogTemp, Warning, TEXT("World Generator - FAILED TO INITILIZE: X: %d Y: %d"), TempWorldBlock.CordX, TempWorldBlock.CordY);
		}
	}
}

int AMyWorldGenerator::tryCutOff(int X)
{
	int cutOffValue = 0;

	if (cutOffProcentageRange > FMath::FRandRange(0, 1)) {

		if (CurrentCutOffsUpperRight.Contains(X)) {
			(*CurrentCutOffsUpperRight.Find(X))++;
			cutOffValue = *CurrentCutOffsUpperRight.Find(X);
		}
		else {
			CurrentCutOffsUpperRight.Add(X, 1);
		}

	}
	else {

		if (CurrentCutOffsUpperRight.Contains(X)) {
			cutOffValue = *CurrentCutOffsUpperRight.Find(X);
		}

	}

	return cutOffValue;
}

void AMyWorldGenerator::TryGenerateWorldHole(const TPair<int, int> originCoords, const int blockCount, const int newAreaRadius)
{
	TArray<TPair<int, int>> HolesInitializeReadyCoords;

	//UE_LOG(LogTemp, Warning, TEXT("Radius: %d Origin X: %d Y: %d"), HolesInitializeReadyCoords.Num(), originCoords.Key, originCoords.Value);

	TPair<int, int> HoleOriginCoord = TPairInitializer<int, int>((originCoords.Key + FMath::RandRange(-newAreaRadius, newAreaRadius)), (originCoords.Value + FMath::RandRange(-newAreaRadius, newAreaRadius)));


	createNewWorldBlock(HoleOriginCoord, (blockCount * holeSizeProcentage), HolesInitializeReadyCoords);

	//UE_LOG(LogTemp, Warning, TEXT("Hole Size: %d Hole Origin X: %d Y: %d"), HolesInitializeReadyCoords.Num(), HoleOriginCoord.Key, HoleOriginCoord.Value);

	int counter = HolesInitializeReadyCoords.Num();
	for (int i = (counter - 1); 0 <= i; i--) {
		AssigneWorldBlockType(HolesInitializeReadyCoords[i], MyWorldBlockType::Hole, MyNaturalResourcesType::NR_None, HolesInitializeReadyCoords);
	}

	CurrentAreaData.Empty();
}

int AMyWorldGenerator::getNewAreaRadius(int newAreaBlockcount)
{
	int currentRadius = 0;

	while (newAreaBlockcount >= 0) {
		currentRadius++;
		newAreaBlockcount = newAreaBlockcount - (currentRadius * 4);
	}

	return (currentRadius);
}

//--- Expan area



TPair<int, int> AMyWorldGenerator::closestEmptyArea(const TPair<int, int> OriginCoords)
{
	bool hasFoundEdge = false;
	int loopCounter = 1;
	TPair<int, int> CurrentCoord; 

	while (hasFoundEdge == false) {

		for (int j = 1; loopCounter >= j; j++) {
			CurrentCoord = TPairInitializer<int, int>(j, -(loopCounter - j));			

			CurrentCoord.Key = OriginCoords.Key + CurrentCoord.Key;
			CurrentCoord.Value = OriginCoords.Value + CurrentCoord.Value;

			if (WorldBlockData.Contains(CurrentCoord) == false) {
				//UE_LOG(LogTemp, Warning, TEXT("CHOOOOOOSE THIS ONE: X: %d Y: %d"), CurrentCoord.Key, CurrentCoord.Value);
				hasFoundEdge = true;
				break;
			}
			else {
				//UE_LOG(LogTemp, Warning, TEXT("Searching: X: %d Y: %d"), CurrentCoord.Key, CurrentCoord.Value);
			}
		} 

		if (hasFoundEdge == true)
			break;

		for (int j = 1; loopCounter >= j; j++) {
			CurrentCoord = TPairInitializer<int, int>(-(loopCounter - j), -j);

			CurrentCoord.Key = CurrentCoord.Key + OriginCoords.Key;
			CurrentCoord.Value = CurrentCoord.Value + OriginCoords.Value;

			if (WorldBlockData.Contains(CurrentCoord) == false) {
				//UE_LOG(LogTemp, Warning, TEXT("CHOOOOOOSE THIS ONE: X: %d Y: %d"), CurrentCoord.Key, CurrentCoord.Value);
				hasFoundEdge = true;
				break;
			}
			else {
				//UE_LOG(LogTemp, Warning, TEXT("Searching: X: %d Y: %d"), CurrentCoord.Key, CurrentCoord.Value);
			}
		}

		if (hasFoundEdge == true)
			break;

		for (int j = 1; loopCounter >= j; j++) {
			CurrentCoord = TPairInitializer<int, int>(j, (loopCounter - j));

			CurrentCoord.Key = CurrentCoord.Key + OriginCoords.Key;
			CurrentCoord.Value = CurrentCoord.Value + OriginCoords.Value;

			if (WorldBlockData.Contains(CurrentCoord) == false) {
				//UE_LOG(LogTemp, Warning, TEXT("CHOOOOOOSE THIS ONE: X: %d Y: %d"), CurrentCoord.Key, CurrentCoord.Value);
				hasFoundEdge = true;
				break;
			}
			else {
				//UE_LOG(LogTemp, Warning, TEXT("Searching: X: %d Y: %d"), CurrentCoord.Key, CurrentCoord.Value);
			}
		}

		if (hasFoundEdge == true)
			break;

		for (int j = 1; loopCounter >= j; j++) {
			CurrentCoord = TPairInitializer<int, int>(-(loopCounter - j), (j - 1));

			CurrentCoord.Key = CurrentCoord.Key + OriginCoords.Key;
			CurrentCoord.Value = CurrentCoord.Value + OriginCoords.Value;

			if (WorldBlockData.Contains(CurrentCoord) == false) {
				//UE_LOG(LogTemp, Warning, TEXT("CHOOOOOOSE THIS ONE: X: %d Y: %d"), CurrentCoord.Key, CurrentCoord.Value);
				hasFoundEdge = true;
				break;
			}
			else {
				//UE_LOG(LogTemp, Warning, TEXT("Searching: X: %d Y: %d"), CurrentCoord.Key, CurrentCoord.Value);
			}
		}

		if (hasFoundEdge == true)
			break;

		loopCounter++;
	}

	//UE_LOG(LogTemp, Warning, TEXT("SEND IN THIS COORD: X: %d Y: %d"), CurrentCoord.Key, CurrentCoord.Value);

	return CurrentCoord;
}

//--- BLOCK LIST MANAGER ---

void AMyWorldGenerator::AssigneNaturalResurces()
{
	if (CurrentAreaData.IsValidIndex(0) == false) {UE_LOG(LogTemp, Warning, TEXT("WORLD BLOCK SORTING (AssigneNaturalResurces()): CurrentAreaData size is none ")) return; }

	UE_LOG(LogTemp, Warning, TEXT("WORLD BLOCK SORTING (AssigneNaturalResurces()): CurrentAreaData size: %d"), CurrentAreaData.Num());

	TArray<int> ValidGrassIndexses;
	TArray<int> ValidFrostIndexses;
	TArray<int> ValidRockIndexses;
	TArray<int> ValidWaterIndexses;

	for (int i = 0; CurrentAreaData.Num() > i; i++) {
		if (CurrentAreaData[i].WorldBlockType == MyWorldBlockType::Grass &&
			CurrentAreaData[i].HasArtifact == false)
			ValidGrassIndexses.Add(i);
		if (CurrentAreaData[i].WorldBlockType == MyWorldBlockType::Frost &&
			CurrentAreaData[i].HasArtifact == false)
			ValidFrostIndexses.Add(i);
		if (CurrentAreaData[i].WorldBlockType == MyWorldBlockType::Rock &&
			CurrentAreaData[i].HasArtifact == false)
			ValidRockIndexses.Add(i);
		if (CurrentAreaData[i].WorldBlockType == MyWorldBlockType::Water &&
			CurrentAreaData[i].HasArtifact == false)
			ValidWaterIndexses.Add(i);
	}

	assigneTreeToGrassBlocks(ValidGrassIndexses);
	assigneMineraToRockBlocks(ValidRockIndexses);
	assigneCrystalToFrostBlocks(ValidFrostIndexses);
	assigneFishToWaterBlocks(ValidWaterIndexses);
}

void AMyWorldGenerator::assigneTreeToGrassBlocks(TArray<int>& GrassBlockIndexes)
{
	if (0 >= GrassBlockIndexes.Num())
		return;

	float NaturalResourceMax = FMath::Clamp<float>(WorldGenerationDataList.Find(MyWorldBlockType::Grass)->NaturalResourceMax * currentNaturalRecurcesModifier, 0.0f, 1.0f);
	float NaturalResourceMin = FMath::Clamp<float>(WorldGenerationDataList.Find(MyWorldBlockType::Grass)->NaturalResourceMin * currentNaturalRecurcesModifier, 0.0f, NaturalResourceMax);

	//int treeCount = GrassBlockIndexes.Num() * FMath::FRandRange(WorldGenerationDataList.Find(MyWorldBlockType::Grass)->NaturalResourceMin, WorldGenerationDataList.Find(MyWorldBlockType::Grass)->NaturalResourceMax);
	int treeCount = GrassBlockIndexes.Num() * FMath::FRandRange(NaturalResourceMin, NaturalResourceMax);

	TArray<int> GrassBlockIndexesTemp = GrassBlockIndexes;

	//Remove grass blocks inrealtion to Rock blocks
	for (int i = GrassBlockIndexes.Num() - 1; i > 0; i--) {

		if (isBlockTypeClose(TPairInitializer<int, int>(CurrentAreaData[GrassBlockIndexes[i]].CordX, CurrentAreaData[GrassBlockIndexes[i]].CordY), MyWorldBlockType::Rock, blockDistanceToRockForTrees) == spawnCloseToRocks) {
			
			if(ForrestCloseToRockProcentage > FMath::RandRange(0, 1))
			GrassBlockIndexes.RemoveAt(i);

		}
	}

	if (treeCount > (GrassBlockIndexes.Num() * 4)) {
		UE_LOG(LogTemp, Warning, TEXT("World Generator - assigneTreeToGrassBlocks: To little Grass Blocks for Trees - GrassBlocks: %d Trees: %d"), GrassBlockIndexes.Num(), treeCount);
		GrassBlockIndexes = GrassBlockIndexesTemp;
	}

	for (int i = 0; treeCount > i; i++) {
	int targetResurcesIndex = FMath::RandRange(0, GrassBlockIndexes.Num() - 1);

	//Only four per block
	if (CurrentAreaData.IsValidIndex(GrassBlockIndexes[targetResurcesIndex])) {
		if (CurrentAreaData[GrassBlockIndexes[targetResurcesIndex]].NaturalResources.Num() >= 4) {
			GrassBlockIndexes.RemoveAt(targetResurcesIndex);
			targetResurcesIndex = FMath::RandRange(0, GrassBlockIndexes.Num() - 1);
		}
	}

	CurrentAreaData[GrassBlockIndexes[targetResurcesIndex]].HasNaturalResoruces = true;
	CurrentAreaData[GrassBlockIndexes[targetResurcesIndex]].NaturalResources.Add(MyNaturalResourcesType::NR_Tree);
	CurrentAreaData[GrassBlockIndexes[targetResurcesIndex]].NaturalResourcesType = MyNaturalResourcesType::NR_Tree;

	WorldBlockData.Find(TPairInitializer<int, int>(CurrentAreaData[GrassBlockIndexes[targetResurcesIndex]].CordX, CurrentAreaData[GrassBlockIndexes[targetResurcesIndex]].CordY))->HasNaturalResoruces = true;
	WorldBlockData.Find(TPairInitializer<int, int>(CurrentAreaData[GrassBlockIndexes[targetResurcesIndex]].CordX, CurrentAreaData[GrassBlockIndexes[targetResurcesIndex]].CordY))->NaturalResources.Add(MyNaturalResourcesType::NR_Tree);
	WorldBlockData.Find(TPairInitializer<int, int>(CurrentAreaData[GrassBlockIndexes[targetResurcesIndex]].CordX, CurrentAreaData[GrassBlockIndexes[targetResurcesIndex]].CordY))->NaturalResourcesType = MyNaturalResourcesType::NR_Tree;

	if (CurrentAreaData[GrassBlockIndexes[targetResurcesIndex]].NaturalResources.Num() + 1 >= 4) {
		GrassBlockIndexes.RemoveAt(targetResurcesIndex);
	}
	}
}

void AMyWorldGenerator::assigneMineraToRockBlocks(TArray<int>& rockBlockIndexes)
{
	if (0 >= rockBlockIndexes.Num())
		return;


	float NaturalResourceMax = FMath::Clamp<float>(WorldGenerationDataList.Find(MyWorldBlockType::Rock)->NaturalResourceMax * currentNaturalRecurcesModifier, 0.0f, 1.0f);
	float NaturalResourceMin = FMath::Clamp<float>(WorldGenerationDataList.Find(MyWorldBlockType::Rock)->NaturalResourceMin * currentNaturalRecurcesModifier, 0.0f, NaturalResourceMax);

	int MineralCount = rockBlockIndexes.Num() * FMath::FRandRange(NaturalResourceMin, NaturalResourceMax);

	//int MineralCount = rockBlockIndexes.Num() * FMath::FRandRange(WorldGenerationDataList.Find(MyWorldBlockType::Rock)->NaturalResourceMin, WorldGenerationDataList.Find(MyWorldBlockType::Rock)->NaturalResourceMax);

	for (int i = 0; MineralCount > i; i++) {
		int targetResurcesIndex = FMath::RandRange(0, rockBlockIndexes.Num() - 1);

		//UE_LOG(LogTemp, Warning, TEXT("Mineral Spawning Index: %d"), targetResurcesIndex);

		if (CurrentAreaData[rockBlockIndexes[targetResurcesIndex]].NaturalResources.Num() > 0) {

			rockBlockIndexes.RemoveAt(targetResurcesIndex);
			targetResurcesIndex = FMath::RandRange(0, rockBlockIndexes.Num() - 1);

			//UE_LOG(LogTemp, Warning, TEXT("NEW Mineral Spawning Index: %d"), targetResurcesIndex);
		}

		//UE_LOG(LogTemp, Warning, TEXT("NEW Mineral Spawning Index value: %d"), rockBlockIndexes[targetResurcesIndex]);

		CurrentAreaData[rockBlockIndexes[targetResurcesIndex]].HasNaturalResoruces = true;
		CurrentAreaData[rockBlockIndexes[targetResurcesIndex]].NaturalResources.Add(MyNaturalResourcesType::NR_Mineral);
		CurrentAreaData[rockBlockIndexes[targetResurcesIndex]].NaturalResourcesType = MyNaturalResourcesType::NR_Mineral;

		WorldBlockData.Find(TPairInitializer<int, int>(CurrentAreaData[rockBlockIndexes[targetResurcesIndex]].CordX, CurrentAreaData[rockBlockIndexes[targetResurcesIndex]].CordY))->HasNaturalResoruces = true;
		WorldBlockData.Find(TPairInitializer<int, int>(CurrentAreaData[rockBlockIndexes[targetResurcesIndex]].CordX, CurrentAreaData[rockBlockIndexes[targetResurcesIndex]].CordY))->NaturalResources.Add(MyNaturalResourcesType::NR_Mineral);
		WorldBlockData.Find(TPairInitializer<int, int>(CurrentAreaData[rockBlockIndexes[targetResurcesIndex]].CordX, CurrentAreaData[rockBlockIndexes[targetResurcesIndex]].CordY))->NaturalResourcesType = MyNaturalResourcesType::NR_Mineral;

		if (CurrentAreaData[rockBlockIndexes[targetResurcesIndex]].NaturalResources.Num() > 0) {
			rockBlockIndexes.RemoveAt(targetResurcesIndex);
		}
	}
}

void AMyWorldGenerator::assigneCrystalToFrostBlocks(TArray<int>& frostBlockIndexes)
{
	if (0 >= frostBlockIndexes.Num())
		return;

	float NaturalResourceMax = FMath::Clamp<float>(WorldGenerationDataList.Find(MyWorldBlockType::Frost)->NaturalResourceMax * currentNaturalRecurcesModifier, 0.0f, 1.0f);
	float NaturalResourceMin = FMath::Clamp<float>(WorldGenerationDataList.Find(MyWorldBlockType::Frost)->NaturalResourceMin * currentNaturalRecurcesModifier, 0.0f, NaturalResourceMax);

	int CrystalCount = frostBlockIndexes.Num() * FMath::FRandRange(NaturalResourceMin, NaturalResourceMax);

	//int CrystalCount = frostBlockIndexes.Num() * FMath::FRandRange(WorldGenerationDataList.Find(MyWorldBlockType::Frost)->NaturalResourceMin, WorldGenerationDataList.Find(MyWorldBlockType::Frost)->NaturalResourceMax);

	UE_LOG(LogTemp, Warning, TEXT("Crystal count block: %d "), CrystalCount);

	for (int i = 0; CrystalCount > i; i++) {
		int targetResurcesIndex = FMath::RandRange(0, frostBlockIndexes.Num() - 1);

		//Only four per block
		if (CurrentAreaData[frostBlockIndexes[targetResurcesIndex]].NaturalResources.Num() > 0) {
			frostBlockIndexes.RemoveAt(targetResurcesIndex);
			targetResurcesIndex = FMath::RandRange(0, frostBlockIndexes.Num() - 1);
		}

		CurrentAreaData[frostBlockIndexes[targetResurcesIndex]].HasNaturalResoruces = true;
		CurrentAreaData[frostBlockIndexes[targetResurcesIndex]].NaturalResources.Add(MyNaturalResourcesType::NR_Crystal);
		CurrentAreaData[frostBlockIndexes[targetResurcesIndex]].NaturalResourcesType = MyNaturalResourcesType::NR_Crystal;

		WorldBlockData.Find(TPairInitializer<int, int>(CurrentAreaData[frostBlockIndexes[targetResurcesIndex]].CordX, CurrentAreaData[frostBlockIndexes[targetResurcesIndex]].CordY))->HasNaturalResoruces = true;
		WorldBlockData.Find(TPairInitializer<int, int>(CurrentAreaData[frostBlockIndexes[targetResurcesIndex]].CordX, CurrentAreaData[frostBlockIndexes[targetResurcesIndex]].CordY))->NaturalResources.Add(MyNaturalResourcesType::NR_Crystal);
		WorldBlockData.Find(TPairInitializer<int, int>(CurrentAreaData[frostBlockIndexes[targetResurcesIndex]].CordX, CurrentAreaData[frostBlockIndexes[targetResurcesIndex]].CordY))->NaturalResourcesType = MyNaturalResourcesType::NR_Crystal;


		if (CurrentAreaData[frostBlockIndexes[targetResurcesIndex]].NaturalResources.Num() > 0) {
			frostBlockIndexes.RemoveAt(targetResurcesIndex);
		}
	}
}

void AMyWorldGenerator::assigneFishToWaterBlocks(TArray<int>& waterBlockIndexes)
{
	if (0 >= waterBlockIndexes.Num())
		return;

	float NaturalResourceMax = FMath::Clamp<float>(WorldGenerationDataList.Find(MyWorldBlockType::Water)->NaturalResourceMax * currentNaturalRecurcesModifier, 0.0f, 1.0f);
	float NaturalResourceMin = FMath::Clamp<float>(WorldGenerationDataList.Find(MyWorldBlockType::Water)->NaturalResourceMin * currentNaturalRecurcesModifier, 0.0f, NaturalResourceMax);

	int FishCount = waterBlockIndexes.Num() * FMath::FRandRange(NaturalResourceMin, NaturalResourceMax);

	//int FishCount = waterBlockIndexes.Num() * FMath::FRandRange(WorldGenerationDataList.Find(MyWorldBlockType::Water)->NaturalResourceMin, WorldGenerationDataList.Find(MyWorldBlockType::Water)->NaturalResourceMax);

	for (int i = 0; FishCount > i; i++) {
		int targetResurcesIndex = FMath::RandRange(0, waterBlockIndexes.Num() - 1);

		//Only four per block
		if (CurrentAreaData[waterBlockIndexes[targetResurcesIndex]].NaturalResources.Num() > 0) {
			waterBlockIndexes.RemoveAt(targetResurcesIndex);
			targetResurcesIndex = FMath::RandRange(0, waterBlockIndexes.Num() - 1);
		}

		CurrentAreaData[waterBlockIndexes[targetResurcesIndex]].HasNaturalResoruces = true;
		CurrentAreaData[waterBlockIndexes[targetResurcesIndex]].NaturalResources.Add(MyNaturalResourcesType::NR_Fish);
		CurrentAreaData[waterBlockIndexes[targetResurcesIndex]].NaturalResourcesType = MyNaturalResourcesType::NR_Fish;

		WorldBlockData.Find(TPairInitializer<int, int>(CurrentAreaData[waterBlockIndexes[targetResurcesIndex]].CordX, CurrentAreaData[waterBlockIndexes[targetResurcesIndex]].CordY))->HasNaturalResoruces = true;
		WorldBlockData.Find(TPairInitializer<int, int>(CurrentAreaData[waterBlockIndexes[targetResurcesIndex]].CordX, CurrentAreaData[waterBlockIndexes[targetResurcesIndex]].CordY))->NaturalResources.Add(MyNaturalResourcesType::NR_Fish);
		WorldBlockData.Find(TPairInitializer<int, int>(CurrentAreaData[waterBlockIndexes[targetResurcesIndex]].CordX, CurrentAreaData[waterBlockIndexes[targetResurcesIndex]].CordY))->NaturalResourcesType = MyNaturalResourcesType::NR_Fish;


		if (CurrentAreaData[waterBlockIndexes[targetResurcesIndex]].NaturalResources.Num() > 0) {
			waterBlockIndexes.RemoveAt(targetResurcesIndex);
		}
	}
}

//--- HEIGHT DIFFERENCE ---

void AMyWorldGenerator::assignHeightDifference()
{
	for (int i = 0; CurrentAreaData.Num() > i; i++) {
		int height = FMath::Abs(CurrentAreaData[i].CordX) + FMath::Abs(CurrentAreaData[i].CordY);
		CurrentAreaData[i].Depth = height / DepthDevider;
		WorldBlockData.Find(TPairInitializer<int, int>(CurrentAreaData[i].CordX, CurrentAreaData[i].CordY))->Depth = height / DepthDevider;
	}
	
	sortWorldBlocks();
}

void AMyWorldGenerator::sortWorldBlocks()
{
	CurrentAreaData.Empty();
	for (int i = CurrentAreaCoordsInSpawnOrder.Num() - 1; i >= 0; i--) {
		CurrentAreaData.Add(*WorldBlockData.Find(CurrentAreaCoordsInSpawnOrder[i]));
	}

	for (int i = CurrentAreaData.Num() - 1; i >= 0; i--) {

		TPair<int, int> currentCoord = TPairInitializer<int, int>(CurrentAreaData[i].CordX, CurrentAreaData[i].CordY);
		TPair<bool, int> AdjecentCheckResult;

		switch (CurrentAreaData[i].WorldBlockType)
		{
		case MyWorldBlockType::Water:
			AdjecentCheckResult = isSurroundingBlocksTypeAndHigher(currentCoord, MyWorldBlockType::Water);

			if (AdjecentCheckResult.Key == true) {
				CurrentAreaData[i].Depth = AdjecentCheckResult.Value;
				WorldBlockData.Find(currentCoord)->Depth = AdjecentCheckResult.Value;
			}

			break;

		case MyWorldBlockType::Grass:

			AdjecentCheckResult = isSurroundingBlocksTypeAndHigher(currentCoord, MyWorldBlockType::Water);

			if (AdjecentCheckResult.Key == true) {
				CurrentAreaData[i].Depth = AdjecentCheckResult.Value;
				WorldBlockData.Find(currentCoord)->Depth = AdjecentCheckResult.Value;
			}

			break;
		case MyWorldBlockType::Frost:

			AdjecentCheckResult = isSurroundingBlocksTypeAndHigher(currentCoord, MyWorldBlockType::Water);

			if (AdjecentCheckResult.Key == true) {
				CurrentAreaData[i].Depth = AdjecentCheckResult.Value;
				WorldBlockData.Find(currentCoord)->Depth = AdjecentCheckResult.Value;
			}

			break;

		case MyWorldBlockType::Rock:

			AdjecentCheckResult = GetResultOnSurroundingBlocks(currentCoord);

			if (AdjecentCheckResult.Key == true) {
				CurrentAreaData[i].Depth = AdjecentCheckResult.Value;
				WorldBlockData.Find(currentCoord)->Depth = AdjecentCheckResult.Value;


			}

			break;

		default:
			break;
		}
	
	}

}

TPair<bool, int> AMyWorldGenerator::GetResultOnSurroundingBlocks(const TPair<int, int> originCoord)
{
	bool hasFoundblockType = false;
	int HighhestHitHeight = 0;

	TPairInitializer<int, int> tempCoordinate = originCoord;		
	TPair<bool, int> resultHit = TPairInitializer<bool, int>(false, 0);

	tempCoordinate.Key++;
	tempCoordinate.Value++;
	if (WorldBlockData.Contains(tempCoordinate)) { // +1 +1
		if (WorldBlockData.Find(originCoord)->Depth > WorldBlockData.Find(tempCoordinate)->Depth) {

			HighhestHitHeight = WorldBlockData.Find(tempCoordinate)->Depth;

			resultHit = TPairInitializer<bool, int>(true, HighhestHitHeight);
		}
	}

	// 0 +1
	tempCoordinate = originCoord;
	tempCoordinate.Value++;
	if (WorldBlockData.Contains(tempCoordinate)) { 
		if (WorldBlockData.Find(originCoord)->Depth > WorldBlockData.Find(tempCoordinate)->Depth) {

			if (WorldBlockData.Find(tempCoordinate)->Depth > HighhestHitHeight) {
				HighhestHitHeight = WorldBlockData.Find(tempCoordinate)->Depth;
			}
			 

			resultHit = TPairInitializer<bool, int>(true, HighhestHitHeight);
		}
	}
	
	// -1 +1
	tempCoordinate = originCoord;
	tempCoordinate.Key--;
	tempCoordinate.Value++;
	if (WorldBlockData.Contains(tempCoordinate)) {
		if (WorldBlockData.Find(originCoord)->Depth > WorldBlockData.Find(tempCoordinate)->Depth) {

			if (WorldBlockData.Find(tempCoordinate)->Depth > HighhestHitHeight) {
				HighhestHitHeight = WorldBlockData.Find(tempCoordinate)->Depth;
			}


			resultHit = TPairInitializer<bool, int>(true, HighhestHitHeight);
		}
	}

	// +1 0
	tempCoordinate = originCoord;
	tempCoordinate.Key++;
	if (WorldBlockData.Contains(tempCoordinate)) {
		if (WorldBlockData.Find(originCoord)->Depth > WorldBlockData.Find(tempCoordinate)->Depth) {

			if (WorldBlockData.Find(tempCoordinate)->Depth > HighhestHitHeight) {
				HighhestHitHeight = WorldBlockData.Find(tempCoordinate)->Depth;
			}


			resultHit = TPairInitializer<bool, int>(true, HighhestHitHeight);
		}
	}

	// -1 0
	tempCoordinate = originCoord;
	tempCoordinate.Key--;
	if (WorldBlockData.Contains(tempCoordinate)) {
		if (WorldBlockData.Find(originCoord)->Depth > WorldBlockData.Find(tempCoordinate)->Depth) {

			if (WorldBlockData.Find(tempCoordinate)->Depth > HighhestHitHeight) {
				HighhestHitHeight = WorldBlockData.Find(tempCoordinate)->Depth;
			}


			resultHit = TPairInitializer<bool, int>(true, HighhestHitHeight);
		}
	}

	// 0 -1
	tempCoordinate = originCoord;
	tempCoordinate.Value--;
	if (WorldBlockData.Contains(tempCoordinate)) {
		if (WorldBlockData.Find(originCoord)->Depth > WorldBlockData.Find(tempCoordinate)->Depth) {

			if (WorldBlockData.Find(tempCoordinate)->Depth > HighhestHitHeight) {
				HighhestHitHeight = WorldBlockData.Find(tempCoordinate)->Depth;
			}


			resultHit = TPairInitializer<bool, int>(true, HighhestHitHeight);
		}
	}

	// +1 -1
	tempCoordinate = originCoord;
	tempCoordinate.Key++;
	tempCoordinate.Value--;
	if (WorldBlockData.Contains(tempCoordinate)) {
		if (WorldBlockData.Find(originCoord)->Depth > WorldBlockData.Find(tempCoordinate)->Depth) {

			if (WorldBlockData.Find(tempCoordinate)->Depth > HighhestHitHeight) {
				HighhestHitHeight = WorldBlockData.Find(tempCoordinate)->Depth;
			}


			resultHit = TPairInitializer<bool, int>(true, HighhestHitHeight);
		}
	}

	// -1 -1
	tempCoordinate = originCoord;
	tempCoordinate.Key--;
	tempCoordinate.Value--;
	if (WorldBlockData.Contains(tempCoordinate)) {
		if (WorldBlockData.Find(originCoord)->Depth > WorldBlockData.Find(tempCoordinate)->Depth) {

			if (WorldBlockData.Find(tempCoordinate)->Depth > HighhestHitHeight) {
				HighhestHitHeight = WorldBlockData.Find(tempCoordinate)->Depth;
			}


			resultHit = TPairInitializer<bool, int>(true, HighhestHitHeight);
		}
	}


	return resultHit;
}

TPair<bool, int> AMyWorldGenerator::isSurroundingBlocksTypeAndHigher(const TPair<int, int> originCoord, MyWorldBlockType targetBlockType)
{
	int HeigestHitDepth = WorldBlockData.Find(originCoord)->Depth;

	TPairInitializer<int, int> tempCoordinate = originCoord;
	TPair<bool, int> resultHit = TPairInitializer<bool, int>(false, 0);

	tempCoordinate.Key++;
	tempCoordinate.Value++;
	if (WorldBlockData.Contains(tempCoordinate)) { // +1 +1
		
		if (WorldBlockData.Find(tempCoordinate)->WorldBlockType == targetBlockType &&
			WorldBlockData.Find(originCoord)->Depth > WorldBlockData.Find(tempCoordinate)->Depth
			) {

			if (HeigestHitDepth > WorldBlockData.Find(tempCoordinate)->Depth) {
				HeigestHitDepth = WorldBlockData.Find(tempCoordinate)->Depth;
			}

			resultHit = TPairInitializer<bool, int>(true, HeigestHitDepth);
		}
	}

	// 0 +1
	tempCoordinate = originCoord;
	tempCoordinate.Value++;
	if (WorldBlockData.Contains(tempCoordinate)) {
		
		if (WorldBlockData.Find(tempCoordinate)->WorldBlockType == targetBlockType &&
			WorldBlockData.Find(originCoord)->Depth > WorldBlockData.Find(tempCoordinate)->Depth
			) {

			if (HeigestHitDepth > WorldBlockData.Find(tempCoordinate)->Depth) {
				HeigestHitDepth = WorldBlockData.Find(tempCoordinate)->Depth;
			}


			resultHit = TPairInitializer<bool, int>(true, HeigestHitDepth);
		}
	}

	// -1 +1
	tempCoordinate = originCoord;
	tempCoordinate.Key--;
	tempCoordinate.Value++;
	if (WorldBlockData.Contains(tempCoordinate)) {

		if (WorldBlockData.Find(tempCoordinate)->WorldBlockType == targetBlockType &&
			WorldBlockData.Find(originCoord)->Depth > WorldBlockData.Find(tempCoordinate)->Depth
			) {

			if (HeigestHitDepth > WorldBlockData.Find(tempCoordinate)->Depth) {
				HeigestHitDepth = WorldBlockData.Find(tempCoordinate)->Depth;
			}


			resultHit = TPairInitializer<bool, int>(true, HeigestHitDepth);
		}
	}

	// +1 0
	tempCoordinate = originCoord;
	tempCoordinate.Key++;
	if (WorldBlockData.Contains(tempCoordinate)) {
		
		if (WorldBlockData.Find(tempCoordinate)->WorldBlockType == targetBlockType &&
			WorldBlockData.Find(originCoord)->Depth > WorldBlockData.Find(tempCoordinate)->Depth
			) {

			if (HeigestHitDepth > WorldBlockData.Find(tempCoordinate)->Depth) {
				HeigestHitDepth = WorldBlockData.Find(tempCoordinate)->Depth;
			}


			resultHit = TPairInitializer<bool, int>(true, HeigestHitDepth);
		}
	}

	// -1 0
	tempCoordinate = originCoord;
	tempCoordinate.Key--;
	if (WorldBlockData.Contains(tempCoordinate)) {
		
		if (WorldBlockData.Find(tempCoordinate)->WorldBlockType == targetBlockType &&
			WorldBlockData.Find(originCoord)->Depth > WorldBlockData.Find(tempCoordinate)->Depth
			) {

			if (HeigestHitDepth > WorldBlockData.Find(tempCoordinate)->Depth) {
				HeigestHitDepth = WorldBlockData.Find(tempCoordinate)->Depth;
			}


			resultHit = TPairInitializer<bool, int>(true, HeigestHitDepth);
		}
	}

	// 0 -1
	tempCoordinate = originCoord;
	tempCoordinate.Value--;
	if (WorldBlockData.Contains(tempCoordinate)) {
		
		if (WorldBlockData.Find(tempCoordinate)->WorldBlockType == targetBlockType &&
			WorldBlockData.Find(originCoord)->Depth > WorldBlockData.Find(tempCoordinate)->Depth
			) {

			if (HeigestHitDepth > WorldBlockData.Find(tempCoordinate)->Depth) {
				HeigestHitDepth = WorldBlockData.Find(tempCoordinate)->Depth;
			}


			resultHit = TPairInitializer<bool, int>(true, HeigestHitDepth);
		}
	}

	// +1 -1
	tempCoordinate = originCoord;
	tempCoordinate.Key++;
	tempCoordinate.Value--;
	if (WorldBlockData.Contains(tempCoordinate)) {

		if (WorldBlockData.Find(tempCoordinate)->WorldBlockType == targetBlockType &&
			WorldBlockData.Find(originCoord)->Depth > WorldBlockData.Find(tempCoordinate)->Depth
			) {

			if (HeigestHitDepth > WorldBlockData.Find(tempCoordinate)->Depth) {
				HeigestHitDepth = WorldBlockData.Find(tempCoordinate)->Depth;
			}


			resultHit = TPairInitializer<bool, int>(true, HeigestHitDepth);
		}
	}

	// -1 -1
	tempCoordinate = originCoord;
	tempCoordinate.Key--;
	tempCoordinate.Value--;
	if (WorldBlockData.Contains(tempCoordinate)) {
		if (WorldBlockData.Find(tempCoordinate)->WorldBlockType == targetBlockType &&
			WorldBlockData.Find(originCoord)->Depth > WorldBlockData.Find(tempCoordinate)->Depth
			) {

			if (HeigestHitDepth > WorldBlockData.Find(tempCoordinate)->Depth) {
				HeigestHitDepth = WorldBlockData.Find(tempCoordinate)->Depth;
			}


			resultHit = TPairInitializer<bool, int>(true, HeigestHitDepth);
		}
	}


	return resultHit;
}

TPair<bool, int> AMyWorldGenerator::isSurroundingBlockTypeAndHigher(const TPair<int, int> originCoord, MyWorldBlockType targetBlockType, int radiusRange)
{
	bool hasFoundblockType = false;
	int loopCounter = 1;
	TPair<int, int> CurrentCoord;
	int HitHeight = 0;

	while (hasFoundblockType == false && radiusRange >= loopCounter) {

		for (int j = 1; loopCounter >= j; j++) {
			CurrentCoord = TPairInitializer<int, int>(j, -(loopCounter - j));

			CurrentCoord.Key = originCoord.Key + CurrentCoord.Key;
			CurrentCoord.Value = originCoord.Value + CurrentCoord.Value;

			if (WorldBlockData.Contains(CurrentCoord)) {

				if (WorldBlockData.Find(CurrentCoord)->WorldBlockType == targetBlockType &&
					WorldBlockData.Find(originCoord)->Depth > WorldBlockData.Find(CurrentCoord)->Depth 
					) {
					//UE_LOG(LogTemp, Warning, TEXT("CHOOOOOOSE THIS ONE: X: %d Y: %d"), CurrentCoord.Key, CurrentCoord.Value);
					HitHeight = WorldBlockData.Find(CurrentCoord)->Depth;
					hasFoundblockType = true;
					break;
				}
				else {
					//UE_LOG(LogTemp, Warning, TEXT("No close block: X: %d Y: %d"), CurrentCoord.Key, CurrentCoord.Value);
				}

			}
		}

		if (hasFoundblockType == true)
			break;

		for (int j = 1; loopCounter >= j; j++) {
			CurrentCoord = TPairInitializer<int, int>(-(loopCounter - j), -j);

			CurrentCoord.Key = CurrentCoord.Key + originCoord.Key;
			CurrentCoord.Value = CurrentCoord.Value + originCoord.Value;

			if (WorldBlockData.Contains(CurrentCoord)) {
				if (WorldBlockData.Find(CurrentCoord)->WorldBlockType == targetBlockType &&
					WorldBlockData.Find(originCoord)->Depth > WorldBlockData.Find(CurrentCoord)->Depth
					) {
					//UE_LOG(LogTemp, Warning, TEXT("CHOOOOOOSE THIS ONE: X: %d Y: %d"), CurrentCoord.Key, CurrentCoord.Value);
					HitHeight = WorldBlockData.Find(CurrentCoord)->Depth;
					hasFoundblockType = true;
					break;
				}
				else {
					//UE_LOG(LogTemp, Warning, TEXT("Searching: X: %d Y: %d"), CurrentCoord.Key, CurrentCoord.Value);
				}

			}
		}

		if (hasFoundblockType == true)
			break;

		for (int j = 1; loopCounter >= j; j++) {
			CurrentCoord = TPairInitializer<int, int>(j, (loopCounter - j));

			CurrentCoord.Key = CurrentCoord.Key + originCoord.Key;
			CurrentCoord.Value = CurrentCoord.Value + originCoord.Value;

			if (WorldBlockData.Contains(CurrentCoord)) {
				if (WorldBlockData.Find(CurrentCoord)->WorldBlockType == targetBlockType &&
					WorldBlockData.Find(originCoord)->Depth > WorldBlockData.Find(CurrentCoord)->Depth
					) {
					//UE_LOG(LogTemp, Warning, TEXT("CHOOOOOOSE THIS ONE: X: %d Y: %d"), CurrentCoord.Key, CurrentCoord.Value);
					HitHeight = WorldBlockData.Find(CurrentCoord)->Depth;
					hasFoundblockType = true;
					break;
				}
				else {
					//UE_LOG(LogTemp, Warning, TEXT("Searching: X: %d Y: %d"), CurrentCoord.Key, CurrentCoord.Value);
				}
			}
		}

		if (hasFoundblockType == true)
			break;

		for (int j = 1; loopCounter >= j; j++) {
			CurrentCoord = TPairInitializer<int, int>(-(loopCounter - j), (j - 1));

			CurrentCoord.Key = CurrentCoord.Key + originCoord.Key;
			CurrentCoord.Value = CurrentCoord.Value + originCoord.Value;

			if (WorldBlockData.Contains(CurrentCoord)) {
				if (WorldBlockData.Find(CurrentCoord)->WorldBlockType == targetBlockType &&
					WorldBlockData.Find(originCoord)->Depth > WorldBlockData.Find(CurrentCoord)->Depth
					) {
					//UE_LOG(LogTemp, Warning, TEXT("CHOOOOOOSE THIS ONE: X: %d Y: %d"), CurrentCoord.Key, CurrentCoord.Value);
					HitHeight = WorldBlockData.Find(CurrentCoord)->Depth;
					hasFoundblockType = true;
					break;
				}
				else {
					//UE_LOG(LogTemp, Warning, TEXT("Searching: X: %d Y: %d"), CurrentCoord.Key, CurrentCoord.Value);
				}
			}
		}

		if (hasFoundblockType == true)
			break;

		loopCounter++;
	}

	//UE_LOG(LogTemp, Warning, TEXT("SEND IN THIS COORD: X: %d Y: %d"), CurrentCoord.Key, CurrentCoord.Value);

	return TPairInitializer<bool, int>(hasFoundblockType, HitHeight);
}

//--- WORLD BLOCK INTERFACE ---

bool AMyWorldGenerator::HasTargetBlock(const int X, const int Y)
{
	return WorldBlockData.Contains(TPairInitializer<const int, const int>(X, Y));
}

FMyWorldBlockData AMyWorldGenerator::GetTargetBlock(const int X, const int Y)
{
	if (WorldBlockData.Contains(TPairInitializer<const int, const int>(X, Y))) {

		FMyWorldBlockData TempWorldBlockData = WorldBlockData.FindRef(TPairInitializer<const int, const int>(X, Y));

		return TempWorldBlockData;
	}
	else {
		return 	FMyWorldBlockData();
	}
}

bool AMyWorldGenerator::isEdgeBlock(const TPair<int, int> targetCoord)
{
	TPairInitializer<int, int> tempCoordinate = targetCoord;

	tempCoordinate.Key++;
	tempCoordinate.Value++;
	if (!WorldBlockData.Contains(tempCoordinate)) // +1 +1
		return true;
	else if (WorldBlockData.Find(tempCoordinate)->WorldBlockType == MyWorldBlockType::Hole)
		return true;

	tempCoordinate = targetCoord;
	tempCoordinate.Value++;
	if (!WorldBlockData.Contains(tempCoordinate)) // 0 +1
		return true;
	else if (WorldBlockData.Find(tempCoordinate)->WorldBlockType == MyWorldBlockType::Hole)
		return true;

	tempCoordinate = targetCoord;
	tempCoordinate.Key--;
	tempCoordinate.Value++;
	if (!WorldBlockData.Contains(tempCoordinate)) // -1 +1
		return true;
	else if (WorldBlockData.Find(tempCoordinate)->WorldBlockType == MyWorldBlockType::Hole)
		return true;

	tempCoordinate = targetCoord;
	tempCoordinate.Key++;
	if (!WorldBlockData.Contains(tempCoordinate)) // +1 0
		return true;
	else if (WorldBlockData.Find(tempCoordinate)->WorldBlockType == MyWorldBlockType::Hole)
		return true;

	tempCoordinate = targetCoord;
	tempCoordinate.Key--;
	if (!WorldBlockData.Contains(tempCoordinate)) // -1 0
		return true;
	else if (WorldBlockData.Find(tempCoordinate)->WorldBlockType == MyWorldBlockType::Hole)
		return true;

	tempCoordinate = targetCoord;
	tempCoordinate.Value--;
	if (!WorldBlockData.Contains(tempCoordinate)) // 0 -1c
		return true;
	else if (WorldBlockData.Find(tempCoordinate)->WorldBlockType == MyWorldBlockType::Hole)
		return true;

	tempCoordinate = targetCoord;
	tempCoordinate.Key++;
	tempCoordinate.Value--;
	if (!WorldBlockData.Contains(tempCoordinate)) // +1 -1
		return true;
	else if (WorldBlockData.Find(tempCoordinate)->WorldBlockType == MyWorldBlockType::Hole)
		return true;

	tempCoordinate = targetCoord;
	tempCoordinate.Key--;
	tempCoordinate.Value--;
	if (!WorldBlockData.Contains(tempCoordinate)) // -1 -1
		return true;
	else if (WorldBlockData.Find(tempCoordinate)->WorldBlockType == MyWorldBlockType::Hole)
		return true;

	return false;
}

bool AMyWorldGenerator::isAdjecentEdgeBlock(const TPair<int, int> targetCoord)
{
	TPairInitializer<int, int> tempCoordinate = targetCoord;

	tempCoordinate = targetCoord;
	tempCoordinate.Value++;
	if (!WorldBlockData.Contains(tempCoordinate)) // 0 +1
		return true;
	else if (WorldBlockData.Find(tempCoordinate)->WorldBlockType == MyWorldBlockType::Hole)
		return true;

	tempCoordinate = targetCoord;
	tempCoordinate.Key++;
	if (!WorldBlockData.Contains(tempCoordinate)) // +1 0
		return true;
	else if (WorldBlockData.Find(tempCoordinate)->WorldBlockType == MyWorldBlockType::Hole)
		return true;

	tempCoordinate = targetCoord;
	tempCoordinate.Key--;
	if (!WorldBlockData.Contains(tempCoordinate)) // -1 0
		return true;
	else if (WorldBlockData.Find(tempCoordinate)->WorldBlockType == MyWorldBlockType::Hole)
		return true;

	tempCoordinate = targetCoord;
	tempCoordinate.Value--;
	if (!WorldBlockData.Contains(tempCoordinate)) // 0 -1
		return true;
	else if (WorldBlockData.Find(tempCoordinate)->WorldBlockType == MyWorldBlockType::Hole)
		return true;

	return false;
}

bool AMyWorldGenerator::isBlockTypeClose(const TPair<int, int> originCoord, MyWorldBlockType targetBlockType, int radiusRange)
{
	bool hasFoundblockType = false;
	int loopCounter = 1;
	TPair<int, int> CurrentCoord;

	while (hasFoundblockType == false && radiusRange >= loopCounter) {

		for (int j = 1; loopCounter >= j; j++) {
			CurrentCoord = TPairInitializer<int, int>(j, -(loopCounter - j));

			CurrentCoord.Key = originCoord.Key + CurrentCoord.Key;
			CurrentCoord.Value = originCoord.Value + CurrentCoord.Value;

			if (WorldBlockData.Contains(CurrentCoord)) {
				if (WorldBlockData.Find(CurrentCoord)->WorldBlockType == targetBlockType) {
					//UE_LOG(LogTemp, Warning, TEXT("Found Close block: X: %d Y: %d"), CurrentCoord.Key, CurrentCoord.Value);
					hasFoundblockType = true;
					break;
				}
				else {
					//UE_LOG(LogTemp, Warning, TEXT("No close block: X: %d Y: %d"), CurrentCoord.Key, CurrentCoord.Value);
				}

			}
		}

		if (hasFoundblockType == true)
			break;

		for (int j = 1; loopCounter >= j; j++) {
			CurrentCoord = TPairInitializer<int, int>(-(loopCounter - j), -j);

			CurrentCoord.Key = CurrentCoord.Key + originCoord.Key;
			CurrentCoord.Value = CurrentCoord.Value + originCoord.Value;

			if (WorldBlockData.Contains(CurrentCoord)) {
				if (WorldBlockData.Find(CurrentCoord)->WorldBlockType == targetBlockType) {
					//UE_LOG(LogTemp, Warning, TEXT("CHOOOOOOSE THIS ONE: X: %d Y: %d"), CurrentCoord.Key, CurrentCoord.Value);
					hasFoundblockType = true;
					break;
				}
				else {
					//UE_LOG(LogTemp, Warning, TEXT("Searching: X: %d Y: %d"), CurrentCoord.Key, CurrentCoord.Value);
				}

			}
		}

		if (hasFoundblockType == true)
			break;

		for (int j = 1; loopCounter >= j; j++) {
			CurrentCoord = TPairInitializer<int, int>(j, (loopCounter - j));

			CurrentCoord.Key = CurrentCoord.Key + originCoord.Key;
			CurrentCoord.Value = CurrentCoord.Value + originCoord.Value;

			if (WorldBlockData.Contains(CurrentCoord)) {
				if (WorldBlockData.Find(CurrentCoord)->WorldBlockType == targetBlockType) {
					//UE_LOG(LogTemp, Warning, TEXT("CHOOOOOOSE THIS ONE: X: %d Y: %d"), CurrentCoord.Key, CurrentCoord.Value);
					hasFoundblockType = true;
					break;
				}
				else {
					//UE_LOG(LogTemp, Warning, TEXT("Searching: X: %d Y: %d"), CurrentCoord.Key, CurrentCoord.Value);
				}
			}
		}

		if (hasFoundblockType == true)
			break;

		for (int j = 1; loopCounter >= j; j++) {
			CurrentCoord = TPairInitializer<int, int>(-(loopCounter - j), (j - 1));

			CurrentCoord.Key = CurrentCoord.Key + originCoord.Key;
			CurrentCoord.Value = CurrentCoord.Value + originCoord.Value;

			if (WorldBlockData.Contains(CurrentCoord)) {
				if (WorldBlockData.Find(CurrentCoord)->WorldBlockType == targetBlockType) {
					//UE_LOG(LogTemp, Warning, TEXT("CHOOOOOOSE THIS ONE: X: %d Y: %d"), CurrentCoord.Key, CurrentCoord.Value);
					hasFoundblockType = true;
					break;
				}
				else {
					//UE_LOG(LogTemp, Warning, TEXT("Searching: X: %d Y: %d"), CurrentCoord.Key, CurrentCoord.Value);
				}
			}
		}

		if (hasFoundblockType == true)
			break;

		loopCounter++;
	}

	//UE_LOG(LogTemp, Warning, TEXT("SEND IN THIS COORD: X: %d Y: %d"), CurrentCoord.Key, CurrentCoord.Value);

	return hasFoundblockType;
}

//MyAscensionManager

void AMyWorldGenerator::OnAddArtifactToAscensionManager(TEnumAsByte<MyArtifactTypes> TargetArtifactType)
{
	MyAscensionManager->OnAddArtifactOnWorld(TargetArtifactType);
}

void AMyWorldGenerator::OnRemoveArtifactInAscensionManager(TEnumAsByte<MyArtifactTypes> TargetArtifactType)
{
	MyAscensionManager->OnRemoveArtifactOnWorld(TargetArtifactType);
}

void AMyWorldGenerator::onSetCurrentAreaData(const TArray<FMyWorldBlockData> areaData)
{
	CurrentAreaData = areaData;
	
	for (FMyWorldBlockData blockData : CurrentAreaData) {
		TPair<int, int> targetCoord;
		targetCoord = TPairInitializer<int, int>(blockData.CordX, blockData.CordX);

		WorldBlockData.Add(targetCoord, blockData);
	}

	//UE_LOG(LogTemp, Warning, TEXT("MyWorldGenerator - onSetCurrentAreaData(): currentAreaData.Num() %d"), CurrentAreaData.Num());

	return;
}
