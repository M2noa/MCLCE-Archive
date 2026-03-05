
from os.path import isfile
from shutil import move 

trophynames = [
	"MCTrophy_All.png", # "All_Trophies.png", # Special for ps3/ps4
	"MCTrophy_00.png", # "TakingInventory_icon.png",
	"MCTrophy_01.png", # "GettingWood_icon.png",
	"MCTrophy_02.png", # "Benchmarking_icon.png",
	"MCTrophy_10.png", # "TimeToMine_icon.png",
	"MCTrophy_12.png", # "HotTopic_icon.png",
	"MCTrophy_14.png", # "AcquireHardware_icon.png",
	"MCTrophy_03.png", # "TimeToFarm_icon.png",
	"MCTrophy_04.png", # "BakeBread_icon.png",
	"MCTrophy_05.png", # "TheLie_icon.png",
	"MCTrophy_11.png", # "GettingAnUpgrade_icon.png",
	"MCTrophy_13.png", # "DeliciousFish_icon.png",
	"MCTrophy_15.png", # "OnARail_icon.png",
	"MCTrophy_06.png", # "TimeToStrike_icon.png",
	"MCTrophy_07.png", # "MonsterHunter_icon.png",
	"MCTrophy_08.png", # "CowTipper_icon.png",
	"MCTrophy_09.png", # "WhenPigsFly_icon.png",
	"MCTrophy_16.png", # "LeaderOfThePack_icon.png",
	"MCTrophy_17.png", # "MOARTools_icon.png",
	"MCTrophy_18.png", # "DispenseWithThis_icon.png",
	"MCTrophy_19.png", # "IntoTheNether_icon.png",
	"MCTrophy_20.png", # "SniperDuel_icon.png",
	"MCTrophy_21.png", # "Diamonds_icon.png",
	"MCTrophy_22.png", # "ReturnToSender_icon.png",
	"MCTrophy_23.png", # "IntoFire_icon.png",
	"MCTrophy_24.png", # "LocalBrewery_icon.png",
	"MCTrophy_25.png", # "TheEnd_icon.png",
	"MCTrophy_26.png", # "The_Other_End_icon.png",
	"MCTrophy_27.png", # "Enchanter_icon.png",
	"MCTrophy_28.png", # "Overkill_icon.png",
	"MCTrophy_29.png", # "Librarian_icon.png",
	"MCTrophy_30.png", # "AdventuringTime_icon.png",
	"MCTrophy_31.png", # "Repopulation_icon.png",
	"MCTrophy_32.png", # "DiamondsToYou_icon.png",
	"MCTrophy_33.png", # "PorkChop_icon.png",
	"MCTrophy_34.png", # "PassingTheTime_icon.png",
	"MCTrophy_35.png", # "Archer_icon.png",
	"MCTrophy_36.png", # "TheHaggler_icon.png",
	"MCTrophy_37.png", # "PotPlanter_icon.png",
	"MCTrophy_38.png", # "ItsASign_icon.png",
	"MCTrophy_39.png", # "IronBelly_icon.png",
	"MCTrophy_40.png", # "HaveAShearfulDay_icon.png",
	"MCTrophy_41.png", # "RainbowCollection_icon.png",
	"MCTrophy_42.png", # "StayinFrosty_icon.png",
	"MCTrophy_43.png", # "ChestfulOfCobblestone_icon.png",
	"MCTrophy_44.png", # "RenewableEnergy_icon.png",
	"MCTrophy_45.png", # "MusicToMyEars_icon.png",
	"MCTrophy_46.png", # "BodyGuard_icon.png",
	"MCTrophy_47.png", # "IronMan_icon.png",
	"MCTrophy_48.png", # "ZombieDoctor_icon.png",
	"MCTrophy_49.png", # "LionTamer_icon.png"
	]

def getTargetName(id):
	return 'TROP%03d.PNG' % id
	
if __name__=="__main__":	
	for id, name in enumerate(trophynames):
		if isfile(name):
			print ("Found: " + name)
			move(name, getTargetName(id))
		else:
			print ("Can't find '"+name+"'")
		