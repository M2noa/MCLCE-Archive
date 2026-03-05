
from os.path import isfile
from shutil import move 

trophynames = [
	"All_Trophies.png", # Special for ps3/ps4
	"TakingInventory_icon.png",
	"GettingWood_icon.png",
	"Benchmarking_icon.png",
	"TimeToMine_icon.png",
	"HotTopic_icon.png",
	"AcquireHardware_icon.png",
	"TimeToFarm_icon.png",
	"BakeBread_icon.png",
	"TheLie_icon.png",
	"GettingAnUpgrade_icon.png",
	"DeliciousFish_icon.png",
	"OnARail_icon.png",
	"TimeToStrike_icon.png",
	"MonsterHunter_icon.png",
	"CowTipper_icon.png",
	"WhenPigsFly_icon.png",
	"LeaderOfThePack_icon.png",
	"MOARTools_icon.png",
	"DispenseWithThis_icon.png",
	"IntoTheNether_icon.png",
	"SniperDuel_icon.png",
	"Diamonds_icon.png",
	"ReturnToSender_icon.png",
	"IntoFire_icon.png",
	"LocalBrewery_icon.png",
	"TheEnd_icon.png",
	"The_Other_End_icon.png",
	"Enchanter_icon.png",
	"Overkill_icon.png",
	"Librarian_icon.png",
	"AdventuringTime_icon.png",
	"Repopulation_icon.png",
	"DiamondsToYou_icon.png",
	"PorkChop_icon.png",
	"PassingTheTime_icon.png",
	"Archer_icon.png",
	"TheHaggler_icon.png",
	"PotPlanter_icon.png",
	"ItsASign_icon.png",
	"IronBelly_icon.png",
	"HaveAShearfulDay_icon.png",
	"RainbowCollection_icon.png",
	"StayinFrosty_icon.png",
	"ChestfulOfCobblestone_icon.png",
	"RenewableEnergy_icon.png",
	"MusicToMyEars_icon.png",
	"BodyGuard_icon.png",
	"IronMan_icon.png",
	"ZombieDoctor_icon.png",
	"LionTamer_icon.png" ]

def getTargetName(id):
	return 'TROP%03d.PNG' % id
	
if __name__=="__main__":	
	for id, name in enumerate(trophynames):
		if isfile(name):
			print ("Found: " + name)
			move(name, getTargetName(id))
		else:
			print ("Can't find '"+name+"'")
		