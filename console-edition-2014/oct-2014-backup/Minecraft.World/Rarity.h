#pragma once

class Rarity
{
public:
	static const Rarity *common;
	static const Rarity *uncommon;
	static const Rarity *rare;
	static const Rarity *epic;

	const eMinecraftColour color;
	const wstring name;

	Rarity(eMinecraftColour color, const wstring &name);
};