// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "lovika/io/ObjectGroupFile.h"
#include "CommonTypes.h"
#include "LovikaLevelActor.generated.h"

struct BlockInfo {
	BlockInfo(FullBlock fullBlock, BlockPos pos)
		: fullBlock(fullBlock), pos(pos) {}

	FullBlock fullBlock;
	BlockPos pos;
};

UCLASS()
class DUNGEONS_API ALovikaLevelActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALovikaLevelActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual void PostInitializeComponents() override;

#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
#endif   // WITH_EDITOR

	/** Dynamically created texture */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LovikaLevel)
		UTexture2D* DebugAtlasTexture;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LovikaLevel)
		UTexture2D* DebugAtlasTextureEmissive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LovikaLevel)
		bool useAmbientOcclusion = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LovikaLevel)
		bool doRefresh = true;

private:

	void generateObjectGroupLevel();

	void generateLovikaSourceLevel();

	void generateMinecraftDBLevel();

	UMaterialInterface* materialOpaque;
	UMaterialInterface* materialAlphaTest;
	UMaterialInterface* materialEmissive;
	UMaterialInterface* materialWater;

	UMaterialInstanceDynamic* atlasMaterialInstanceOpaque;
	UMaterialInstanceDynamic* atlasMaterialInstanceAlpha;

	std::unique_ptr<io::ObjectGroup> objectGroup;
	
};
