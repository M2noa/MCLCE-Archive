// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "LovikaLevelActor.h"

#include "RuntimeMeshComponent.h"

#include "client/renderer/block/BlockGraphics.h"
#include "client/renderer/renderer/Tessellator.h"
#include "client/renderer/block/BlockTessellator.h"
#include "client/renderer/block/BlockTextureTessellator.h"
#include "client/renderer/texture/TextureAtlas.h"
#include "world/level/BlockSource.h"
#include "world/level/LevelConstants.h"
#include "world/level/material/Material.h"
#include "world/level/dimension/NormalDimension.h"
#include "LovikaLevelSource.h"
#include "world/level/Level.h"
#include "world/level/biome/Biome.h"
#include "world/level/block/entity/BlockEntity.h"
#include "world/level/FoliageColor.h"
#include "client/renderer/texture/TextureData.h"
#include "IImageWrapper.h"
#include "io/TextureFile.h"
#include "world/level/chunk/EmptyChunkSource.h"
#include "world/level/storage/DBChunkStorage.h"
#include "world/level/storage/DBStorage.h"
#include "world/level/storage/ExternalFileLevelStorageSource.h"
#include "world/level/chunk/ChunkViewSource.h"
#include "world/level/levelgen/flat/FlatLevelSource.h"
#include "util/GridArea.h"
#include "world/level/chunk/LevelChunk.h"
#include "LovikaBlockSource.h"

// Sets default values
ALovikaLevelActor::ALovikaLevelActor()
{
	UE_LOG(LogTemp, Warning, TEXT("Running Constructor! AVoxel::AVoxel"));

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	//SphereComponent->SetMobility(EComponentMobility::Static);
	RootComponent = SphereComponent;

	static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/Materials/VoxelMat.VoxelMat'"));
	if (Material.Object != NULL) {
		materialOpaque = (UMaterial*)Material.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialAlpha(TEXT("Material'/Game/Materials/VoxelMatAlpha.VoxelMatAlpha'"));
	if (MaterialAlpha.Object != NULL) {
		materialAlphaTest = (UMaterial*)MaterialAlpha.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialEmissive(TEXT("Material'/Game/Materials/VoxelMatEmissive.VoxelMatEmissive'"));
	//static ConstructorHelpers::FObjectFinder<UMaterial> MaterialEmissive(TEXT("Material'/Game/Materials/LavaTest.LavaTest'"));
	if (MaterialEmissive.Object != NULL) {
		materialEmissive = (UMaterial*)MaterialEmissive.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialWater(TEXT("Material'/Game/Materials/VoxelMatWater.VoxelMatWater'"));
	if (MaterialWater.Object != NULL) {
		materialWater = (UMaterial*)MaterialWater.Object;
	}

	// TODO: Refactor all this out

	Material::initMaterials();
	Block::initBlocks();
// 	Item::addBlockItems();
// 	Item::registerItems();
	Biome::initBiomes();
	BlockEntity::initBlockEntities();

	// Major hack
	bool isValid;
	uint32_t width;
	uint32_t height;
	TextureData foliageColorData(io::loadTexture(FPaths::GameDir() + "Content/data/resourcepacks/vanilla/images/colormap/foliage.png", EImageFormat::PNG, isValid, width, height));
	FoliageColor::setFoliageColorPalette(foliageColorData);
	TextureData foliageEvergreenColorData(io::loadTexture(FPaths::GameDir() + "Content/data/resourcepacks/vanilla/images/colormap/evergreen.png", EImageFormat::PNG, isValid, width, height));
 	FoliageColor::setFoliageEvergreenColorPalette(foliageEvergreenColorData);
	TextureData foliageBirchColorData(io::loadTexture(FPaths::GameDir() + "Content/data/resourcepacks/vanilla/images/colormap/birch.png", EImageFormat::PNG, isValid, width, height));
 	FoliageColor::setFoliageBirchColorPalette(foliageBirchColorData);
	TextureData foliageGrassColorData(io::loadTexture(FPaths::GameDir() + "Content/data/resourcepacks/vanilla/images/colormap/grass.png", EImageFormat::PNG, isValid, width, height));
 	FoliageColor::setGrassColorPalette(foliageGrassColorData);

	std::shared_ptr<TextureAtlas> mTextureAtlas = make_shared<TextureAtlas>();
	BlockGraphics::setTextureAtlas(mTextureAtlas);
	mTextureAtlas->loadMetaFile();

	BlockGraphics::initBlocks();

	FString path = FPaths::GameDir() + "Content/data/lovika/objectgroups/objectgroup.json";
 	//FString path = FPaths::GameDir() + "Content/data/lovika/objectgroups/ny_hel_bana2.json";
	std::ifstream in(*path);
	objectGroup = io::loadObjectGroup(in);
}

void ALovikaLevelActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	doRefresh = true;

	UE_LOG(LogTemp, Warning, TEXT("Running PostInitializeComponents! AVoxel::PostInitializeComponents"));
}

#if WITH_EDITOR
void ALovikaLevelActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!doRefresh) {
		return;
	}
	doRefresh = false;

	UE_LOG(LogTemp, Warning, TEXT("Running OnConstruction! AVoxel::OnConstruction"));

	std::shared_ptr<TextureAtlas> atlas = make_shared<TextureAtlas>();
	BlockGraphics::setTextureAtlas(atlas);
	atlas->loadMetaFile();
	atlas->redrawAtlas();
	
	UTexture2D* atlasTex = atlas->atlasTexture;
	UTexture2D* atlasTexEmissive = atlas->atlasTextureEmissive;

	DebugAtlasTexture = atlasTex;
	DebugAtlasTextureEmissive = atlasTexEmissive;

	atlasMaterialInstanceOpaque = UMaterialInstanceDynamic::Create(materialOpaque, this);
	atlasMaterialInstanceOpaque->SetTextureParameterValue(FName("T2DParam"), atlasTex);
	atlasMaterialInstanceOpaque->SetTextureParameterValue(FName("T2DParamEmissive"), atlasTexEmissive);
	atlasMaterialInstanceAlpha = UMaterialInstanceDynamic::Create(materialAlphaTest, this);
	atlasMaterialInstanceAlpha->SetTextureParameterValue(FName("T2DParam"), atlasTex);
	atlasMaterialInstanceAlpha->SetTextureParameterValue(FName("T2DParamEmissive"), atlasTexEmissive);

	BlockTextureTessellator::clearRegisteredUVs();
	for (auto& pair : atlas->mTextureGroup) {
		BlockTextureTessellator::generateUV(pair.first, pair.second);
	}

	generateObjectGroupLevel();
// 	generateLovikaSourceLevel();
// 	generateMinecraftDBLevel();
}

void ALovikaLevelActor::generateObjectGroupLevel()
{
	std::string fullPath = TCHAR_TO_UTF8(*(FPaths::GameDir() + "Content/data/maps"));
	std::string levelId = "testmap";
	Unique<DBStorage> dbStorage = make_unique<DBStorage>(levelId, fullPath + "/" + levelId);
	Level level(std::move(dbStorage));
	NormalDimension dimension(level);

	Tessellator tesselator;
	BlockSource* region = new LovikaBlockSource(level, dimension, objectGroup.get());
	BlockTessellator renderer(tesselator, region);
	renderer.setUnrealActor(this);
	renderer.setSmoothLightingEnabled(useAmbientOcclusion);

	int sectionIndex = 0;
	for (auto& object : objectGroup->objects) {
		auto& bounds = object.bounds;
		BlockPos& min = bounds.minInclusive;
		BlockPos& max = bounds.maxExclusive;

		BlockPos diff = max - min;
		int chunkSizeX = FMath::CeilToInt((float)diff.x / CHUNK_WIDTH);
		int chunkSizeZ = FMath::CeilToInt((float)diff.z / CHUNK_WIDTH);

		for (int chunkX = 0; chunkX < chunkSizeX; chunkX++) {
			for (int chunkZ = 0; chunkZ < chunkSizeZ; chunkZ++) {

				TMap<BlockRenderLayer, TArray<BlockInfo>> infos;

				int maxX = FMath::Min(diff.x - chunkX * CHUNK_WIDTH, CHUNK_WIDTH);
				int maxZ = FMath::Min(diff.z - chunkZ * CHUNK_WIDTH, CHUNK_WIDTH);
				int maxY = diff.y;

				BlockPos currentMin(min.x + chunkX * CHUNK_WIDTH, min.y, min.z + chunkZ * CHUNK_WIDTH);

				for (int x = 0; x < maxX; x++) {
					for (int z = 0; z < maxZ; z++) {
						for (int y = 0; y < maxY; y++) {
							BlockPos pos(x + chunkX * CHUNK_WIDTH, y, z + chunkZ * CHUNK_WIDTH);

							FullBlock fullBlock = object.blocks->getBlock(pos);
							const Block* block = Block::mBlocks[(int)fullBlock.id];
							if (block != nullptr && !block->isType(Block::mAir)) {
								BlockPos realPos(x + currentMin.x, y + currentMin.y, z + currentMin.z);

								if (infos.Contains(block->getRenderLayer())) {
									infos[block->getRenderLayer()].Add(BlockInfo(fullBlock, realPos));
								}
								else {
									TArray<BlockInfo> infoList;
									infoList.Add(BlockInfo(fullBlock, realPos));
									infos.Add(block->getRenderLayer(), infoList);
								}
							}
						}
					}
				}

				URuntimeMeshComponent* mesh = nullptr;
				//mesh->ClearAllMeshSections();

				for (auto& blockInfo : infos) {
					auto l = blockInfo.Key;
					TArray<BlockInfo>& blocks = blockInfo.Value;

					renderer.setRenderLayer(l);
					renderer.setForceOpaque(l == BlockRenderLayer::RENDERLAYER_DOUBLE_SIDED);
					renderer.resetCache(currentMin, *region);
					renderer.clear();

					for (auto& bb : blocks) {
						const Block* block = Block::mBlocks[(int)bb.fullBlock.id];
						renderer.tessellateInWorld(*block, bb.pos, bb.fullBlock.data);
					}

					if (tesselator.vertices.Num() > 0) {
						UE_LOG(LogTemp, Warning, TEXT("Building chunk: %d"), sectionIndex);

						bool generateCollision = true;
						UMaterialInterface* currentMaterial = atlasMaterialInstanceOpaque;
						if (l == BlockRenderLayer::RENDERLAYER_WATER) {
							currentMaterial = materialWater;
							generateCollision = false;
						}
						else if (l == BlockRenderLayer::RENDERLAYER_LAVA) {
							currentMaterial = materialEmissive;
							generateCollision = false;
						}
						else if (
							l == BlockRenderLayer::RENDERLAYER_ALPHATEST ||
							l == BlockRenderLayer::RENDERLAYER_ALPHATEST_SINGLE_SIDE ||
							l == BlockRenderLayer::RENDERLAYER_OPTIONAL_ALPHATEST ||
							l == BlockRenderLayer::RENDERLAYER_DOUBLE_SIDED ||
							l == BlockRenderLayer::RENDERLAYER_BLEND ||
							l == BlockRenderLayer::RENDERLAYER_SEASONS_OPTIONAL_ALPHATEST
							) {
							currentMaterial = atlasMaterialInstanceAlpha;
							generateCollision = false;
						}
						else if (
							l == BlockRenderLayer::RENDERLAYER_SEASONS_OPAQUE
							) {
							generateCollision = false;
						}
						else if (l != BlockRenderLayer::RENDERLAYER_OPAQUE) {
							generateCollision = false;
						}

						if (mesh == nullptr) {
							mesh = NewObject<URuntimeMeshComponent>(this, FName(("Mesh_" + currentMin.toString()).c_str()));
							mesh->RegisterComponent();
							mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
							//mesh->SetMobility(EComponentMobility::Static);
							const int scale = 75;
							mesh->SetRelativeScale3D(FVector(scale, scale, scale));
							sectionIndex = 0;
						}

						mesh->CreateMeshSection(sectionIndex, tesselator.vertices, tesselator.indices, tesselator.normals, tesselator.uv1, TArray<FVector2D>(), tesselator.colors, TArray<FRuntimeMeshTangent>(), generateCollision, EUpdateFrequency::Infrequent);
						mesh->SetMaterial(sectionIndex, currentMaterial);
						sectionIndex++;
					}
				}
			}
		}
	}
}

void ALovikaLevelActor::generateLovikaSourceLevel()
{
	//std::string fullPath = "C:/code/Lovika/handheld/project/VS2015/Minecraft/Minecraft.Win32.OGL/games/com.mojang/minecraftWorlds";
	std::string fullPath = TCHAR_TO_UTF8(*(FPaths::GameDir() + "Content/data/maps"));
	std::string levelId = "testmap";

	Unique<DBStorage> dbStorage = make_unique<DBStorage>(levelId, fullPath + "/" + levelId);

	Level level(std::move(dbStorage));
	LevelData levelData;
	level.getLevelStorage().loadLevelData(levelData);
	NormalDimension dimension(level);
	LovikaLevelSource chunkSource(&level, &dimension, objectGroup.get());

// 	Unique<ChunkSource> emptyChunkSource = make_unique<EmptyChunkSource>(&level, &dimension);
// 	Unique<ChunkSource> flatChunkSource = make_unique<FlatLevelSource>(&level, &dimension, FlatLevelSource::DEFAULT_LAYERS);
// 
// 	Unique<ChunkSource> chunkSource = level.getLevelStorage().createChunkStorage(std::move(flatChunkSource), StorageVersion::LevelDB1);

	Unique<ChunkViewSource> localSource = make_unique<ChunkViewSource>(chunkSource, ChunkSource::LoadMode::Deferred);
	localSource->move(BlockPos(0, 0, 0), 32);
	BlockSource region(localSource->getLevel(), localSource->getDimension(), *localSource, false);

	Tessellator tesselator;
	BlockTessellator renderer(tesselator, &region);
	renderer.setSmoothLightingEnabled(useAmbientOcclusion);
	renderer.setRegion(region);

	int sectionIndex = 0;
	for (auto& object : objectGroup->objects) {
		auto& bounds = object.bounds;
		BlockPos& min = bounds.minInclusive;
		BlockPos& max = bounds.maxExclusive;

		BlockPos diff = max - min;
		int chunkSizeX = FMath::CeilToInt((float)diff.x / CHUNK_WIDTH);
		int chunkSizeZ = FMath::CeilToInt((float)diff.z / CHUNK_WIDTH);

		//UE_LOG(LogTemp, Warning, TEXT("Group size [%s]: %s"), UTF8_TO_TCHAR(object.id.c_str()), UTF8_TO_TCHAR((max - min).toString().c_str()));

		for (int chunkX = 0; chunkX < chunkSizeX; chunkX++) {
			for (int chunkZ = 0; chunkZ < chunkSizeZ; chunkZ++) {

				TMap<BlockRenderLayer, TArray<BlockInfo>> infos;

				int maxX = FMath::Min(diff.x - chunkX * CHUNK_WIDTH, CHUNK_WIDTH);
				int maxZ = FMath::Min(diff.z - chunkZ * CHUNK_WIDTH, CHUNK_WIDTH);
				int maxY = diff.y;

				BlockPos currentMin(min.x + chunkX * CHUNK_WIDTH, min.y, min.z + chunkZ * CHUNK_WIDTH);

				for (int x = 0; x < maxX; x++) {
					for (int z = 0; z < maxZ; z++) {
						for (int y = 0; y < maxY; y++) {
							// 							BlockPos pos(x + chunkX * CHUNK_WIDTH, y, z + chunkZ * CHUNK_WIDTH);
							BlockPos realPos(x + currentMin.x, y + currentMin.y, z + currentMin.z);

							FullBlock fullBlock = region.getBlockAndData(realPos);
							// 							FullBlock fullBlock = object.blocks->getBlock(pos);
							const Block* block = Block::mBlocks[(int)fullBlock.id];
							if (block != nullptr && !block->isType(Block::mAir)) {

								if (infos.Contains(block->getRenderLayer())) {
									infos[block->getRenderLayer()].Add(BlockInfo(fullBlock, realPos));
								}
								else {
									TArray<BlockInfo> infoList;
									infoList.Add(BlockInfo(fullBlock, realPos));
									infos.Add(block->getRenderLayer(), infoList);
								}
							}
						}
					}
				}

				URuntimeMeshComponent* mesh = nullptr;
				//mesh->ClearAllMeshSections();

				for (auto& blockInfo : infos) {
					auto l = blockInfo.Key;
					TArray<BlockInfo>& blocks = blockInfo.Value;

					renderer.setRenderLayer(l);
					renderer.setForceOpaque(l == BlockRenderLayer::RENDERLAYER_DOUBLE_SIDED);
					renderer.resetCache(currentMin, region);
					renderer.clear();

					for (auto& bb : blocks) {
						const Block* block = Block::mBlocks[(int)bb.fullBlock.id];
						renderer.tessellateInWorld(*block, bb.pos, bb.fullBlock.data);
					}

					if (tesselator.vertices.Num() > 0) {
						UE_LOG(LogTemp, Warning, TEXT("Building chunk: %d"), sectionIndex);

						bool generateCollision = true;
						UMaterialInterface* currentMaterial = atlasMaterialInstanceOpaque;
						if (l == BlockRenderLayer::RENDERLAYER_WATER) {
							currentMaterial = materialWater;
							generateCollision = false;
						}
						else if (l == BlockRenderLayer::RENDERLAYER_LAVA) {
							currentMaterial = materialEmissive;
							generateCollision = false;
						}
						else if (
							l == BlockRenderLayer::RENDERLAYER_ALPHATEST ||
							l == BlockRenderLayer::RENDERLAYER_ALPHATEST_SINGLE_SIDE ||
							l == BlockRenderLayer::RENDERLAYER_OPTIONAL_ALPHATEST ||
							l == BlockRenderLayer::RENDERLAYER_DOUBLE_SIDED ||
							l == BlockRenderLayer::RENDERLAYER_BLEND ||
							l == BlockRenderLayer::RENDERLAYER_SEASONS_OPTIONAL_ALPHATEST
							) {
							currentMaterial = atlasMaterialInstanceAlpha;
							generateCollision = false;
						}
						else if (
							l == BlockRenderLayer::RENDERLAYER_SEASONS_OPAQUE
							) {
							generateCollision = false;
						}
						else if (l != BlockRenderLayer::RENDERLAYER_OPAQUE) {
							generateCollision = false;
						}

						if (mesh == nullptr) {
							mesh = NewObject<URuntimeMeshComponent>(this, FName(("Mesh_" + currentMin.toString()).c_str()));
							mesh->RegisterComponent();
							mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
							//mesh->SetMobility(EComponentMobility::Static);
							const int scale = 75;
							mesh->SetRelativeScale3D(FVector(scale, scale, scale));
							sectionIndex = 0;
						}

						mesh->CreateMeshSection(sectionIndex, tesselator.vertices, tesselator.indices, tesselator.normals, tesselator.uv1, TArray<FVector2D>(), tesselator.colors, TArray<FRuntimeMeshTangent>(), generateCollision, EUpdateFrequency::Infrequent);
						mesh->SetMaterial(sectionIndex, currentMaterial);
						sectionIndex++;
					}
				}
			}
		}
	}
}

void ALovikaLevelActor::generateMinecraftDBLevel()
{
	std::string fullPath = TCHAR_TO_UTF8(*(FPaths::GameDir() + "Content/data/maps"));
	std::string levelId = "testmap";

	Unique<DBStorage> dbStorage = make_unique<DBStorage>(levelId, fullPath + "/" + levelId);

	Level level(std::move(dbStorage));
	LevelData levelData;
	level.getLevelStorage().loadLevelData(levelData);
	NormalDimension dimension(level);

	Unique<ChunkSource> emptyChunkSource = make_unique<EmptyChunkSource>(&level, &dimension);
	Unique<ChunkSource> flatChunkSource = make_unique<FlatLevelSource>(&level, &dimension, FlatLevelSource::DEFAULT_LAYERS);

	Unique<ChunkSource> chunkSource = level.getLevelStorage().createChunkStorage(std::move(flatChunkSource), StorageVersion::LevelDB1);

	Unique<ChunkViewSource> localSource = make_unique<ChunkViewSource>(*chunkSource, ChunkSource::LoadMode::Deferred);
	localSource->move(BlockPos(0, 0, 0), 32);
	BlockSource region(localSource->getLevel(), localSource->getDimension(), *localSource, false);

	Tessellator tesselator;
	BlockTessellator renderer(tesselator, &region);
	renderer.setSmoothLightingEnabled(useAmbientOcclusion);
	renderer.setRegion(region);

	for (auto&& c : localSource->getArea()) {
		ChunkPos pos = c->getPosition();
		UE_LOG(LogTemp, Warning, TEXT("Building chunk: %d, %d"), pos.x, pos.z);

		BlockPos min = BlockPos(pos.x * CHUNK_WIDTH, 0, pos.z * CHUNK_WIDTH);
		BlockPos max = min + RENDERCHUNK_SIDE;
		max.y = 255;

		BlockRenderLayer l = BlockRenderLayer::RENDERLAYER_OPAQUE;
		renderer.setRenderLayer(l);
		renderer.setForceOpaque(l == BlockRenderLayer::RENDERLAYER_DOUBLE_SIDED);
		renderer.resetCache(min, region);
		renderer.clear();

		for (BlockPos p = min; p.x < max.x; p.x++) {
			for (p.z = min.z; p.z < max.z; p.z++) {
				for (p.y = min.y; p.y < max.y; p.y++) {
					auto fullBlock = region.getBlockAndData(p);
					auto& block = fullBlock.getBlock();
					if (!block.isType(Block::mAir)) {
						renderer.tessellateInWorld(block, p, fullBlock.data);
					}
				}
			}
		}

		if (tesselator.vertices.Num() > 0) {
			URuntimeMeshComponent* mesh = NewObject<URuntimeMeshComponent>(this, FName(("Mesh_" + min.toString()).c_str()));
			mesh->RegisterComponent();
			mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
			//mesh->SetMobility(EComponentMobility::Static);
			const int scale = 75;
			mesh->SetRelativeScale3D(FVector(scale, scale, scale));

			int sectionIndex = 0;
			bool generateCollision = false;
			UMaterialInterface* currentMaterial = atlasMaterialInstanceOpaque;
			mesh->CreateMeshSection(sectionIndex, tesselator.vertices, tesselator.indices, tesselator.normals, tesselator.uv1, TArray<FVector2D>(), tesselator.colors, TArray<FRuntimeMeshTangent>(), generateCollision, EUpdateFrequency::Infrequent);
			mesh->SetMaterial(sectionIndex, currentMaterial);
		}
	}
}

#endif // WITH_EDITOR

// Called when the game starts or when spawned
void ALovikaLevelActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALovikaLevelActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

