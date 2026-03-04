#include "stdafx.h"

#include "Rarity.h"

const Rarity *Rarity::common = new Rarity(eHTMLColor_f, L"Common");
const Rarity *Rarity::uncommon = new Rarity(eHTMLColor_e, L"Uncommon");
const Rarity *Rarity::rare = new Rarity(eHTMLColor_b, L"Rare");
const Rarity *Rarity::epic = new Rarity(eHTMLColor_d, L"Epic");

Rarity::Rarity(eMinecraftColour color, const wstring &name) : color(color), name(name)
{
}