#include "Dungeons.h"

#include "world/level/levelgen/structure/StructurePieceFactory.h"
#include "world/level/levelgen/structure/EndCityPieces.h"
#include "world/level/levelgen/structure/MineshaftPieces.h"
#include "world/level/levelgen/structure/NetherBridgePieces.h"
#include "world/level/levelgen/structure/OceanMonumentPieces.h"
#include "world/level/levelgen/structure/ScatteredFeaturePieces.h"
#include "world/level/levelgen/structure/StrongholdPieces.h"
#include "world/level/levelgen/structure/VillagePieces.h"

Unique<StructurePiece> StructurePieceFactory::createStructurePiece(StructurePieceType type, StructureManager& structureManager) {
	MineshaftData mineData(false);	// This shouldn't really matter as the mineshaft should already be generated, and if not the pieces will be initialized correctly

	switch (type) {
	case StructurePieceType::EndCityPiece: return make_unique<EndCityPieces::EndCityPiece>(structureManager);

	case StructurePieceType::MineshaftRoom: return make_unique<MineshaftRoom>(mineData);
	case StructurePieceType::MineshaftCorridor: return make_unique<MineshaftCorridor>(mineData);
	case StructurePieceType::MineshaftCrossing: return make_unique<MineshaftCrossing>(mineData);
	case StructurePieceType::MineshaftStairs: return make_unique<MineshaftStairs>(mineData);
		 		  
	case StructurePieceType::NetherBridgeCrossing: return make_unique<NBBridgeCrossing>();
	case StructurePieceType::NetherBridgeStartPiece: return make_unique<NBStartPiece>();
	case StructurePieceType::NetherBridgeStraight: return make_unique<NBBridgeStraight>();
	case StructurePieceType::NetherBridgeEndFiller: return make_unique<NBBridgeEndFiller>();
	case StructurePieceType::NetherBridgeRoomCrossing: return make_unique<NBRoomCrossing>();
	case StructurePieceType::NetherBridgeStairsRoom: return make_unique<NBStairsRoom>();
	case StructurePieceType::NetherBridgeMonsterThrone: return make_unique<NBMonsterThrone>();
	case StructurePieceType::NetherCastleEntrance: return make_unique<NBCastleEntrance>();
	case StructurePieceType::NetherCastleStalkRoom: return make_unique<NBCastleStalkRoom>();
	case StructurePieceType::NetherCastleSmallCorridor: return make_unique<NBCastleSmallCorridorPiece>();
	case StructurePieceType::NetherCastleSmallCorridorCrossing: return make_unique<NBCastleSmallCorridorCrossingPiece>();
	case StructurePieceType::NetherCastleSmallCorridorRightTurn: return make_unique<NBCastleSmallCorridorRightTurnPiece>();
	case StructurePieceType::NetherCastleSmallCorridorLeftTurn: return make_unique<NBCastleSmallCorridorLeftTurnPiece>();
	case StructurePieceType::NetherCastleCorridorStairs: return make_unique<NBCastleCorridorStairsPiece>();
	case StructurePieceType::NetherCastleCorridorTBalcony: return make_unique<NBCastleCorridorTBalconyPiece>();
		 		  
	case StructurePieceType::OceanMonumentBuilding: return make_unique<MonumentBuilding>();
	case StructurePieceType::OceanMonumentEntryRoom: return make_unique<OceanMonumentEntryRoom>();
	case StructurePieceType::OceanMonumentSimpleRoom: return make_unique<OceanMonumentSimpleRoom>();
	case StructurePieceType::OceanMonumentSimpleTopRoom: return make_unique<OceanMonumentSimpleTopRoom>();
	case StructurePieceType::OceanMonumentDoubleXRoom: return make_unique<OceanMonumentDoubleXRoom>();
	case StructurePieceType::OceanMonumentDoubleYRoom: return make_unique<OceanMonumentDoubleYRoom>();
	case StructurePieceType::OceanMonumentDoubleZRoom: return make_unique<OceanMonumentDoubleZRoom>();
	case StructurePieceType::OceanMonumentDoubleXYRoom: return make_unique<OceanMonumentDoubleXYRoom>();
	case StructurePieceType::OceanMonumentDoubleYZRoom: return make_unique<OceanMonumentDoubleYZRoom>();
	case StructurePieceType::OceanMonumentCoreRoom: return make_unique<OceanMonumentCoreRoom>();
	case StructurePieceType::OceanMonumentWingRoom: return make_unique<OceanMonumentWingRoom>();
	case StructurePieceType::OceanMonumentPenthouse: return make_unique<OceanMonumentPenthouse>();
		 		 
	case StructurePieceType::DesertPyramid: return make_unique<DesertPyramidPiece>(0, 0);
	case StructurePieceType::Igloo: return make_unique<Igloo>(0, 0);
	case StructurePieceType::JunglePyramid: return make_unique<JunglePyramidPiece>(0, 0);
	case StructurePieceType::WitchHut: return make_unique<SwamplandHut>(0, 0);
		 		  
	case StructurePieceType::StrongholdStairsDown: return make_unique<SHStairsDown>();
	case StructurePieceType::StrongholdStartPiece: return make_unique<SHStartPiece>();
	case StructurePieceType::StrongholdChestCorridor: return make_unique<SHChestCorridor>();
	case StructurePieceType::StrongholdFillerCorridor: return make_unique<SHFillerCorridor>();
	case StructurePieceType::StrongholdFiveCrossing: return make_unique<SHFiveCrossing>();
	case StructurePieceType::StrongholdLeftTurn: return make_unique<SHLeftTurn>();
	case StructurePieceType::StrongholdRightTurn: return make_unique<SHRightTurn>();
	case StructurePieceType::StrongholdLibrary: return make_unique<SHLibrary>();
	case StructurePieceType::StrongholdPortalRoom: return make_unique<SHPortalRoom>();
	case StructurePieceType::StrongholdPrisonHall: return make_unique<SHPrisonHall>();
	case StructurePieceType::StrongholdRoomCrossing: return make_unique<SHRoomCrossing>();
	case StructurePieceType::StrongholdStraight: return make_unique<SHStraight>();
	case StructurePieceType::StrongholdStraightStairsDown: return make_unique<SHStraightStairsDown>();
		 		  
	case StructurePieceType::VillageWell: return make_unique<Well>();
	case StructurePieceType::VillageStartPiece: return make_unique<StartPiece>();
	case StructurePieceType::VillageSimpleHouse: return make_unique<SimpleHouse>();
	case StructurePieceType::VillageSmallTemple: return make_unique<SmallTemple>();
	case StructurePieceType::VillageBookHouse: return make_unique<BookHouse>();
	case StructurePieceType::VillageSmallHut: return make_unique<SmallHut>();
	case StructurePieceType::VillagePigHouse: return make_unique<PigHouse>();
	case StructurePieceType::VillageDoubleFarmland: return make_unique<DoubleFarmland>();
	case StructurePieceType::VillageFarmland: return make_unique<Farmland>();
	case StructurePieceType::VillageSmithy: return make_unique<Smithy>();
	case StructurePieceType::VillageTwoRoomHouse: return make_unique<TwoRoomHouse>();
	case StructurePieceType::VillageLightPost: return make_unique<LightPost>();
	case StructurePieceType::VillageStraightRoad: return make_unique<StraightRoad>();
	default:
		DEBUG_ASSERT(false, "You forgot to add your StructurePiece to StructurePieceFactory");
		return nullptr;
	}
}
