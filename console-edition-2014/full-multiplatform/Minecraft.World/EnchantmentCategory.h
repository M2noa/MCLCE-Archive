#pragma once

class Item;

class EnchantmentCategory
{
public:
	static const EnchantmentCategory *all;
	static const EnchantmentCategory *armor;
	static const EnchantmentCategory *armor_feet;
	static const EnchantmentCategory *armor_legs;
	static const EnchantmentCategory *armor_torso;
	static const EnchantmentCategory *armor_head;
	static const EnchantmentCategory *weapon;
	static const EnchantmentCategory *digger;
	static const EnchantmentCategory *bow;

	bool canEnchant(Item *item) const;
};