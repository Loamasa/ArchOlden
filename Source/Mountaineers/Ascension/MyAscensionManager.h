// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "..\\Artifact\MyArtifactGenerationData.h"
#include "MyAscensionData.h"
#include "MyCompatibilities.h"
#include "MyAscensionManager.generated.h"


UENUM(BlueprintType)
enum MyArtifactTypes
{
	AT_WorldGeneration  UMETA(DisplayName = "WorldGeneration"),
	AT_Building			UMETA(DisplayName = "Building"),
	AT_Modifier			UMETA(DisplayName = "Modifier"),
	AT_Win				UMETA(DisplayName = "Win"),

	AT_None				UMETA(DisplayName = "None"),
};

UCLASS()
class MOUNTAINEERS_API AMyAscensionManager : public AActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
		void OnAddArtifactOnWorld(TEnumAsByte<MyArtifactTypes> TargetArtifactType);
	UFUNCTION(BlueprintCallable)
		void OnRemoveArtifactOnWorld(TEnumAsByte<MyArtifactTypes> TargetArtifactType);

	UFUNCTION(BlueprintCallable)
		FString OnElectCandidate(TArray<FMyCompatibilities> CompatabilitityMatrix, TArray<FString> Evaluators, TArray<FString> Candidates);
public: 

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FMyAscensionData ascensionData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int ArtifactEachGeneration = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<TEnumAsByte<MyArtifactTypes>> ArtifactsOnWorld;

public:
	// Sets default values for this actor's properties
	AMyAscensionManager();

	virtual void Tick(float DeltaTime) override;

	FMyArtifactGenerationData onGetArtifactGenerationData();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TEnumAsByte<MyArtifactGenerationType> OnFetchValidGenerationArtifactFromAscensionData();
	int OnFetchValidModifierArtifactFromAscensionData();
	TEnumAsByte<MyBuildingType> OnFetchValidBuildingArtifactFromAscensionData();

private:

	FMyArtifactGenerationData generateArtifactGenerationData(TArray<TEnumAsByte<MyArtifactTypes>> tempValidArtifactTypes, const int Min, const int Max);
	TPair<int, int> GenerateMinMaxForArtifacts();
	int getWorldGenerationArtifactsCountInWorld();
	bool isWorldGenerationException(); //If their is worldGeneration left and their is only one World Generation Artifact in world 

	TArray<TEnumAsByte<MyArtifactTypes>> getValidArtifactsForSpawning();

};
