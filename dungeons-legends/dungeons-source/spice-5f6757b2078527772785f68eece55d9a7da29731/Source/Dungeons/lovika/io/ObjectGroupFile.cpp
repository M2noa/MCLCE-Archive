#include "Dungeons.h"

//#include <leveldb/zlib_compressor.h>
#include "ObjectGroupFile.h"
#include "util/Base64.h"
#include "world/level/block/Block.h"
#include "util/ZLibUtil.h"

using namespace io;
using namespace std;

static Unique<ZLibUtil> mCompressor = make_unique<ZLibUtil>(7);

/// SAVE ///

static Json::Value toJson(const BlockPos& bp) {
    Json::Value node;
    node.append(bp.x);
    node.append(bp.y);
    node.append(bp.z);
    return node;
}

static Json::Value toJson(const Region& region) {
    Json::Value node;
    node["name"] = region.name;
    node["pos"] = toJson(region.bounds.minInclusive);
    node["size"] = toJson(region.bounds.size());
	node["type"] = regionTypeToString(region.type);
    return node;
}

static Json::Value toJson(const Door& door) {
    Json::Value node;
    node["name"] = door.name;
    node["pos"] = toJson(door.bounds.minInclusive);
    node["size"] = toJson(door.bounds.size());
    return node;
}

static Json::Value toJson(const BlockRegion& blocks, int specialAirLevel) {
	BlockID BLOCK_ID_END_STONE = BlockID(121); // Block::mEndStone->getId();
    BlockRegion::RegionSize size = blocks.size();
    size_t nBlocks = (size_t) size.product();
    size_t nBytes = (nBlocks * 12 + 4) / 8;

    string rawData(nBytes, 0);

    //TODO: DF convert to iterator?
    for (int y = 0, i = 0; y < size.y; y++) {
        bool useSpecialAir = y < specialAirLevel;
        for (int z = 0; z < size.z; z++) {
            for (int x = 0; x < size.x; x++, i++) {
                FullBlock block = blocks.getBlock(x, y, z);
                auto blockId = block.id;
                auto blockData = block.data;

                if (useSpecialAir && blockId == BlockID::AIR) {
                    blockData = 0xF;
                }
                if (blockId == BLOCK_ID_END_STONE) {
                    blockId = BlockID::AIR;
                }
                // Set the blockId and blockData
                rawData[i] = blockId;
                rawData[nBlocks + i / 2] |= (i & 1) ? blockData : blockData << 4;
            }
        }
    }

    // compress and base64 encode
    string compressed;
	mCompressor->compress(rawData, compressed);
    return Util::base64_encode(compressed);
}

static Json::Value toJson(const Object& object) {
    Json::Value node;
    node["id"] = object.id;
	node["pos"] = toJson(object.bounds.minInclusive);
	node["size"] = toJson(object.bounds.size());
	node["y"] = object.y;
	node["tags"] = object.tags;

    if (!object.doors.empty()) {
        Json::Value doorNodes;
        for (auto& door : object.doors) {
            doorNodes.append(toJson(door));
        }
        node["doors"] = doorNodes;
    }

    if (!object.regions.empty()) {
        Json::Value regionNodes;
        for (auto& region : object.regions) {
            regionNodes.append(toJson(region));
        }
        node["regions"] = regionNodes;
    }

	if (object.blocks) {
		bool isTile = !object.doors.empty();
		node["blocks"] = toJson(*object.blocks, isTile ? numeric_limits<int>::min() : object.y);
	}
    return node;
}

static Json::Value toJson(const ObjectGroup& objectGroup) {
    Json::Value node, objectNodes(Json::arrayValue);
    for (auto& object : objectGroup.objects) {
        objectNodes.append(toJson(object));
    }
    node["objects"] = objectNodes;
    return node;
}

void io::saveObjectGroup(const ObjectGroup& objectGroup, ostream& out) {
    Json::StyledStreamWriter writer(" ");
    writer.write(out, toJson(objectGroup));
}

/// LOAD ///

static BlockPos parseBlockPos(const Json::Value& node) {
    return BlockPos(node[0].asInt(), node[1].asInt(), node[2].asInt());
}

static BlockCuboid parseBounds(const Json::Value& posNode, const Json::Value& sizeNode) {
    return BlockCuboid::fromPositionAndSize(parseBlockPos(posNode), parseBlockPos(sizeNode));
}

static Region parseRegion(const Json::Value& node) {
    Region region;

	auto parsedType = regionTypeFromString(node["type"].asString());
	region.name = node["name"].asString();
    region.bounds = parseBounds(node["pos"], node["size"]);
	region.type = parsedType ? *parsedType : RegionType::Spawn;
	return region;
}

static Door parseDoor(const Json::Value& node) {
    Door door;
    door.name = node["name"].asString();
    door.bounds = parseBounds(node["pos"], node["size"]);
    return door;
}

static void parseRegions(const Json::Value& node, vector<Region>& regions) {
    for (const Json::Value& regionNode : node) {
        regions.push_back(parseRegion(regionNode));
    }
}

static void parseDoors(const Json::Value& node, vector<Door>& doors) {
    for (const Json::Value& doorNode : node) {
        doors.push_back(parseDoor(doorNode));
    }
}

static Unique<BlockRegion> parseBlocks(const string& encodedBlocks, const BlockPos& size) {
    string compressedBlocks = Util::base64_decode(encodedBlocks);
    string rawBlocks;
	mCompressor->decompress(compressedBlocks, rawBlocks);

    auto blocks = make_unique<BlockRegion> (size);
    auto nBlocks = static_cast<unsigned int> (size.product());

    //TODO: DF convert to iterator?
    for (int y = 0, i = 0; y < size.y; y++) {
        for (int z = 0; z < size.z; z++) {
            for (int x = 0; x < size.x; x++, i++) {
                BlockID blockId = static_cast<BlockID> (rawBlocks[i]);

                auto dataByte = rawBlocks[nBlocks + i / 2];
                dataByte = (i & 1) ? dataByte : dataByte >> 4;
                DataID blockData = static_cast<DataID> (dataByte & 0xF);

                blocks->setBlock(x, y, z, FullBlock(blockId, blockData));
            }
        }
    }

    return move(blocks);
}

static Object parseObject(const Json::Value& node) {
    Object object;
    object.id = node["id"].asString();
	object.bounds = parseBounds(node["pos"], node["size"]);
    object.y = node["y"].asInt();
	object.tags = node["tags"].asString();

	parseDoors(node["doors"], object.doors);
    parseRegions(node["regions"], object.regions);

	if (node.isMember("blocks")) {
		object.blocks = parseBlocks(node["blocks"].asString(), object.bounds.size());
	}
    return object;
}

static Unique<ObjectGroup> parseObjectGroup(const Json::Value& node) {
    auto objectGroup = make_unique<ObjectGroup> ();

    for (const Json::Value& objectNode : node["objects"]) {
        objectGroup->objects.push_back(parseObject(objectNode));
    }

    return move(objectGroup);
}

Unique<ObjectGroup> io::loadObjectGroup(istream& in) {

	//TSharedPtr<FJsonObject> JsonParsed;
	//TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(in);
	//if (FJsonSerializer::Deserialize(JsonReader, JsonParsed)) {
	//	FString ExampleString = JsonParsed->GetStringField("exampleString");
	//}

    Json::Value root;
    Json::Reader reader;

    if (!reader.parse(in, root)) {
        cout << "Failed to parse objectgroup\n" << reader.getFormattedErrorMessages();
        return nullptr;
    }

    return parseObjectGroup(root);
}
