// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAscensionManager.h"

// Sets default values
AMyAscensionManager::AMyAscensionManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMyAscensionManager::BeginPlay()
{
	Super::BeginPlay();

}

void AMyAscensionManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyAscensionManager::OnRemoveArtifactOnWorld(TEnumAsByte<MyArtifactTypes> TargetArtifactType)
{
	ArtifactsOnWorld.RemoveSingle(TargetArtifactType);
}


FString AMyAscensionManager::OnElectCandidate(const TArray<FMyCompatibilities> CompatibilityMatrix, const TArray<FString> Evaluators, const TArray<FString> Candidates)
{
	// Find the indices of the candidate names in the compatibility matrix.
	TArray<int32> CandidateIndices;
	for (int32 i = 0; i < Candidates.Num(); i++)
	{
		const FString& Candidate = Candidates[i];
		for (int32 j = 0; j < CompatibilityMatrix.Num(); j++)
		{
			if (Candidate == CompatibilityMatrix[j].Name)
			{
				CandidateIndices.Add(j);
				break;
			}
		}
	}

	// If no candidates were found, return "No Valid Candidates".
	if (CandidateIndices.Num() == 0)
	{
		return FString(TEXT("No Valid Candidates"));
	}

	// Find the indices of the valid candidates based on the compatibility matrix and the evaluators.
	TArray<int32> ValidCandidateIndices = CandidateIndices;
	for (int32 i = 0; i < Evaluators.Num(); i++)
	{
		const FString& Evaluator = Evaluators[i];
		for (int32 j = 0; j < CompatibilityMatrix.Num(); j++)
		{
			if (Evaluator == CompatibilityMatrix[j].Name)
			{
				for (int32 k = ValidCandidateIndices.Num() - 1; k >= 0; k--)
				{
					if (!CompatibilityMatrix[j].Compatibilities[ValidCandidateIndices[k]])
					{
						ValidCandidateIndices.RemoveAt(k);
					}
				}
				break;
			}
		}
	}

	// If no valid candidates were found, return "No Valid Candidates".
	if (ValidCandidateIndices.Num() == 0)
	{
		return FString(TEXT("No Valid Candidates"));
	}

	// Select a random valid candidate and return its name.
	const int32 RandomIndex = FMath::RandRange(0, ValidCandidateIndices.Num() - 1);
	const FString& RandomCandidateName = CompatibilityMatrix[ValidCandidateIndices[RandomIndex]].Name;
	UE_LOG(LogTemp, Warning, TEXT("AMyAscensionManager::OnElectCandidate - Output: %s"), *RandomCandidateName);
	return RandomCandidateName;
}

void AMyAscensionManager::OnAddArtifactOnWorld(TEnumAsByte<MyArtifactTypes> TargetArtifactType)
{
	ArtifactsOnWorld.Add(TargetArtifactType);
}

FMyArtifactGenerationData AMyAscensionManager::onGetArtifactGenerationData()
{	
	TArray<TEnumAsByte<MyArtifactTypes>> tempValidArtifactTypes = getValidArtifactsForSpawning();
	
	TPair<int, int> MinMax = GenerateMinMaxForArtifacts();

	//UE_LOG(LogTemp, Warning, TEXT("AMyAscensionManager::FMyArtifactGenerationData - Min: %d Max: %d"), MinMax.Key, MinMax.Value);

	return generateArtifactGenerationData(tempValidArtifactTypes, MinMax.Key, MinMax.Value);
}

FMyArtifactGenerationData AMyAscensionManager::generateArtifactGenerationData(TArray<TEnumAsByte<MyArtifactTypes>> tempValidArtifactTypes, const int Min, const int Max)
{
	FMyArtifactGenerationData targetArtifactGenerationData;
	targetArtifactGenerationData.ModifierArtifacts = 0;
	int numberOfArtifacts = FMath::RandRange(Min, Max);

	// Check if there is a world generation exception
	if (isWorldGenerationException()) {
		numberOfArtifacts--;

		// Remove the WorldGeneration type from valid artifact types
		int worldGenerationIndex = tempValidArtifactTypes.Find(MyArtifactTypes::AT_WorldGeneration);
		targetArtifactGenerationData.WorldGenerationArtifacts.Add(OnFetchValidGenerationArtifactFromAscensionData());
		tempValidArtifactTypes.RemoveAt(worldGenerationIndex);
	}

	// Check if it is necessary to find a win artifact
	if (ascensionData.isFindWinArtifact && ArtifactsOnWorld.Num() - 1 <= 0 && ascensionData.SpawningArtifactData.WorldGenerationArtifacts.Num() <= 0) {
		targetArtifactGenerationData.isFindWinArtifact = true;
	}

	// Generate artifacts until the desired number is reached or there are no more valid types
	while (numberOfArtifacts > 0 && tempValidArtifactTypes.Num() > 0) {
		int RandomIndex = FMath::RandRange(0, tempValidArtifactTypes.Num() - 1);
		TEnumAsByte<MyArtifactTypes> randomArtifactValidType = tempValidArtifactTypes[RandomIndex];
		tempValidArtifactTypes.RemoveAt(RandomIndex);

		// Generate artifact based on the randomly selected type
		switch (randomArtifactValidType) {
		case MyArtifactTypes::AT_Building:
			targetArtifactGenerationData.BuildingArtifacts.Add(OnFetchValidBuildingArtifactFromAscensionData());
			break;

		case MyArtifactTypes::AT_Modifier:
			targetArtifactGenerationData.ModifierArtifacts = OnFetchValidModifierArtifactFromAscensionData();
			break;

		case MyArtifactTypes::AT_WorldGeneration:
			targetArtifactGenerationData.WorldGenerationArtifacts.Add(OnFetchValidGenerationArtifactFromAscensionData());
			break;

		default:
			break;
		}

		numberOfArtifacts--;
	}

	return targetArtifactGenerationData;
}

TPair<int, int> AMyAscensionManager::GenerateMinMaxForArtifacts()
{
	TPair<int, int> targetMinMax;
	float allSpawningArtifacts = 0;
	float allGenerationArtifact = 0;

	// Calculate the counts of spawning artifacts and generation artifacts
	if (ascensionData.SpawningArtifactData.BuildingArtifacts.Num() > 0 ||
		ascensionData.SpawningArtifactData.ModifierArtifacts > 0 ||
		ascensionData.SpawningArtifactData.WorldGenerationArtifacts.Num() > 0)
	{
		allSpawningArtifacts = ascensionData.SpawningArtifactData.BuildingArtifacts.Num() +
			ascensionData.SpawningArtifactData.ModifierArtifacts +
			ascensionData.SpawningArtifactData.WorldGenerationArtifacts.Num();

		allGenerationArtifact = getWorldGenerationArtifactsCountInWorld() +
			ascensionData.SpawningArtifactData.WorldGenerationArtifacts.Num();
	}

	// Calculate the minimum and maximum values for artifacts
	targetMinMax.Key = FMath::FloorToInt(allSpawningArtifacts / allGenerationArtifact);
	targetMinMax.Value = FMath::CeilToInt(allSpawningArtifacts / allGenerationArtifact);

	// Log the calculated values (optional)
	//UE_LOG(LogTemp, Warning, TEXT("AMyAscensionManager::GenerateMinMaxForArtifacts - allSpawningArtifacts: %f allGenerationArtifact: %f"), allSpawningArtifacts, allGenerationArtifact);

	return targetMinMax;
}

//If their is only one World Generation Artifact on World, then force a World Generation Artifact to Spawn
bool AMyAscensionManager::isWorldGenerationException()
{
	int WorldGenerationOnWorldCount = 1; // Includes the WorldGenerationArtifact that is removed when expanding the world

	// Count the number of WorldGeneration artifacts on the world
	for (const TEnumAsByte<MyArtifactTypes>& Artifact : ArtifactsOnWorld)
	{
		if (Artifact == MyArtifactTypes::AT_WorldGeneration)
			WorldGenerationOnWorldCount++;
	}

	// Check if there are WorldGeneration artifacts available and if there is only one on the world
	if (ascensionData.SpawningArtifactData.WorldGenerationArtifacts.Num() > 0 &&
		WorldGenerationOnWorldCount == 1)
	{
		return true;
	}

	return false;
}

int AMyAscensionManager::getWorldGenerationArtifactsCountInWorld()
{
	int WorldGenerationArtifactOnWorld = 0;

	// Count the number of WorldGeneration artifacts on the world in reverse order
	for (int i = ArtifactsOnWorld.Num() - 1; i >= 0; i--)
	{
		if (ArtifactsOnWorld[i] == MyArtifactTypes::AT_WorldGeneration)
			WorldGenerationArtifactOnWorld++;
	}

	return WorldGenerationArtifactOnWorld;
}

TArray<TEnumAsByte<MyArtifactTypes>> AMyAscensionManager::getValidArtifactsForSpawning()
{
	TArray<TEnumAsByte<MyArtifactTypes>> tempValidArtifactTypes;

	// Add Building artifacts to the valid artifact types
	for (size_t i = 0; i < ascensionData.SpawningArtifactData.BuildingArtifacts.Num(); i++){
		tempValidArtifactTypes.Add(MyArtifactTypes::AT_Building);
	}

	// Add Modifier artifacts to the valid artifact types
	for (size_t i = 0; i < ascensionData.SpawningArtifactData.ModifierArtifacts; i++){
		tempValidArtifactTypes.Add(MyArtifactTypes::AT_Modifier);
	}

	// Add WorldGeneration artifacts to the valid artifact types
	for (size_t i = 0; i < ascensionData.SpawningArtifactData.WorldGenerationArtifacts.Num(); i++){
		tempValidArtifactTypes.Add(MyArtifactTypes::AT_WorldGeneration);
	}

	// Empty the valid artifact types array if it is empty or has no elements
	if (0 >= tempValidArtifactTypes.Num())
		tempValidArtifactTypes.Empty();

	return tempValidArtifactTypes;
}

TEnumAsByte<MyArtifactGenerationType> AMyAscensionManager::OnFetchValidGenerationArtifactFromAscensionData()
{
	// Check if there are any WorldGeneration artifacts available
	if (ascensionData.SpawningArtifactData.WorldGenerationArtifacts.Num() <= 0)
	{
		return TEnumAsByte<MyArtifactGenerationType>(MyArtifactGenerationType::AGT_None);
	}

	// Generate a random index within the valid range of artifacts
	int tempIndex = FMath::RandRange(0, ascensionData.SpawningArtifactData.WorldGenerationArtifacts.Num() - 1);

	// Retrieve the artifact at the random index
	TEnumAsByte<MyArtifactGenerationType> tempGenerationType = ascensionData.SpawningArtifactData.WorldGenerationArtifacts[tempIndex];

	// Remove the retrieved artifact from the available artifacts
	ascensionData.SpawningArtifactData.WorldGenerationArtifacts.RemoveAt(tempIndex);

	return tempGenerationType;
}

int AMyAscensionManager::OnFetchValidModifierArtifactFromAscensionData()
{
	// Check if there are any Modifier artifacts available
	if (ascensionData.SpawningArtifactData.ModifierArtifacts <= 0)
	{
		return 0;
	}

	// Decrement the count of available Modifier artifacts
	ascensionData.SpawningArtifactData.ModifierArtifacts--;

	return 1;
}

TEnumAsByte<MyBuildingType> AMyAscensionManager::OnFetchValidBuildingArtifactFromAscensionData()
{
	// Check if there are any Building artifacts available
	if (ascensionData.SpawningArtifactData.BuildingArtifacts.Num() <= 0)
	{
		return TEnumAsByte<MyBuildingType>(MyBuildingType::None);
	}

	// Generate a random index within the valid range of artifacts
	int tempIndex = FMath::RandRange(0, ascensionData.SpawningArtifactData.BuildingArtifacts.Num() - 1);

	// Retrieve the artifact at the random index
	TEnumAsByte<MyBuildingType> tempBuildingType = ascensionData.SpawningArtifactData.BuildingArtifacts[tempIndex];

	// Remove the retrieved artifact from the available artifacts
	ascensionData.SpawningArtifactData.BuildingArtifacts.RemoveAt(tempIndex);

	return tempBuildingType;
}