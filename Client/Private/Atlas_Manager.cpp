#include "stdafx.h"
#include "..\Public\Atlas_Manager.h"

const CAtlas_Manager::RAWDATA	CAtlas_Manager::m_AtlasRawData[] = {
	{ TEXT("CORE"), TEXT("UI_CIRCLE"), 0.f, 740.f, 1005.f, 1740.f },
	{ TEXT("CORE"), TEXT("UI_LINE"), 0.f, 0.f, 0.f, 0.f },
	{ TEXT("CORE"), TEXT("DECOLINE"), 0.f, 1602.f, 556.f, 601.f },
	{ TEXT("CORE"), TEXT("BUTTON_MAP"), 1116.f, 1176.f, 1920.f, 1980.f },
	{ TEXT("CORE"), TEXT("BUTTON_QUEST"), 1196.f, 1256.f, 1841.f, 1901.f },
	{ TEXT("CORE"), TEXT("BUTTON_ITEM"), 1606.f, 1666.f, 1440.f, 1500.f },
	{ TEXT("CORE"), TEXT("BUTTON_WEAPON"), 1276.f, 1336.f, 1712.f, 1772.f },
	{ TEXT("CORE"), TEXT("BUTTON_EQUIPMENT"), 1352.f, 1412.f, 1633.f, 1693.f },
	{ TEXT("CORE"), TEXT("BUTTON_DATA"), 1404.f, 1464.f, 1550.f, 1610.f },
	{ TEXT("CORE"), TEXT("BUTTON_SYSTEM"), 1484.f, 1544.f, 1474.f, 1534.f },
	{ TEXT("CORE"), TEXT("LIST_CURSOR"), 1162.f, 1227.f, 1491.f, 1521.f },
	{ TEXT("CORE"), TEXT("CIRCLE_LINE"), 13.f, 267.f, 1775.f, 1793.f },
	{ TEXT("CORE"), TEXT("BUTTON_ACTIVATE"), 1638.f, 1737.f, 1144.f, 1148.f },

	{ TEXT("CORE"), TEXT("DOT_BACK_HALF"), 0.f, 800.f, 620.f, 850.f },
	{ TEXT("CORE"), TEXT("DOT_BACK_FULL"), 0.f, 800.f, 620.f, 1000.f },
	
	{ TEXT("HUD"), TEXT("HPBAR_LOWER_FRAME"), 1062.f, 1377.f, 222.f, 235.f },
	{ TEXT("HUD"), TEXT("HPBAR_LOWER_BIG_CIRCLE"), 425.f, 450.f, 555.f, 580.f },
	{ TEXT("HUD"), TEXT("HPBAR_LOWER_SMALL_CIRCLE"), 675.f, 685.f, 562.f, 575.f },
	{ TEXT("HUD"), TEXT("HPBAR_UPPER_FRAME"), 1062.f, 1377.f, 242.f, 252.f },
	{ TEXT("HUD"), TEXT("HPBAR_CONTENTS"), 14.f, 314.f, 655.f, 675.f },
	{ TEXT("HUD"), TEXT("SYSMSG_ACTIVE"), 1610.f, 1630.f, 380.f, 420.f },
	{ TEXT("HUD"), TEXT("SYSMSG_DEACTIVE"), 1762.f, 1782.f, 228.f, 268.f },
	{ TEXT("HUD"), TEXT("POINTER"), 212.f, 301.f, 725.f, 795.f },
	{ TEXT("HUD"), TEXT("WARNING_LINE"), 29.f, 1856.f, 24.f, 40.f },
	{ TEXT("HUD"), TEXT("SIMPLE_LINE"), 1098.f, 1416.f, 175.f, 182.f },
	{ TEXT("HUD"), TEXT("COOLTIME_OUTER"), 345.f, 505.f, 654.f, 814.f },	// 쓸 수 없음
	{ TEXT("HUD"), TEXT("COOLTIME_INNER"), 1635.f, 1755.f, 495.f, 615.f },	// 쓸 수 없음

	{ TEXT("BACKGROUND"), TEXT("CINEMATIC"), 0.f, 3200.f, 0.f, 1800.f },
	{ TEXT("BACKGROUND"), TEXT("RECTGRID"), 0.f, 1605.f, 3010.f, 3910.f },
	{ TEXT("TRIGRID"), TEXT("TRIGRID_A"), 0.f, 3200.f, 0.f, 1800.f },
	{ TEXT("TRIGRID"), TEXT("TRIGRID_B"), 0.f, 3200.f, 1805.f, 3605.f },

	// For Cursor
	{ TEXT("CURSOR"), TEXT("IDLE"), 0.f, 250.f, 0.f, 300.f },
	{ TEXT("CURSOR"), TEXT("PICK"), 250.f, 500.f, 0.f, 300.f },

	// For Target
	{ TEXT("TARGET"), TEXT("BASIC"), 0.f, 128.f, 0.f, 128.f },	// 회전을 위해서 별개의 이미지로 사용

	// For Damage
	{ TEXT("DAMAGE_A"), TEXT("0"), 52.f, 75.f, 0.f, 33.f },
	{ TEXT("DAMAGE_A"), TEXT("1"), 22.f, 38.f, 70.f, 102.f },
	{ TEXT("DAMAGE_A"), TEXT("2"), 64.f, 88.f, 35.f, 68.f },
	{ TEXT("DAMAGE_A"), TEXT("3"), 77.f, 100.f, 0.f, 33.f },
	{ TEXT("DAMAGE_A"), TEXT("4"), 90.f, 113.f, 35.f, 68.f },
	{ TEXT("DAMAGE_A"), TEXT("5"), 102.f, 125.f, 0.f, 33.f },
	{ TEXT("DAMAGE_A"), TEXT("6"), 0.f, 24.f, 0.f, 33.f },
	{ TEXT("DAMAGE_A"), TEXT("7"), 0.f, 20.f, 70.f, 102.f },
	{ TEXT("DAMAGE_A"), TEXT("8"), 0.f, 23.f, 35.f, 68.f },
	{ TEXT("DAMAGE_A"), TEXT("9"), 26.f, 50.f, 0.f, 33.f },
	{ TEXT("DAMAGE_B"), TEXT("0"), 52.f, 75.f, 0.f, 33.f },
	{ TEXT("DAMAGE_B"), TEXT("1"), 22.f, 38.f, 70.f, 102.f },
	{ TEXT("DAMAGE_B"), TEXT("2"), 64.f, 88.f, 35.f, 68.f },
	{ TEXT("DAMAGE_B"), TEXT("3"), 77.f, 100.f, 0.f, 33.f },
	{ TEXT("DAMAGE_B"), TEXT("4"), 90.f, 113.f, 35.f, 68.f },
	{ TEXT("DAMAGE_B"), TEXT("5"), 102.f, 125.f, 0.f, 33.f },
	{ TEXT("DAMAGE_B"), TEXT("6"), 0.f, 24.f, 0.f, 33.f },
	{ TEXT("DAMAGE_B"), TEXT("7"), 0.f, 20.f, 70.f, 102.f },
	{ TEXT("DAMAGE_B"), TEXT("8"), 0.f, 23.f, 35.f, 68.f },
	{ TEXT("DAMAGE_B"), TEXT("9"), 26.f, 50.f, 0.f, 33.f },

	// For Body
	{ TEXT("FONT"), TEXT("COUNT"), 0.f, 19.f, 517.f, 537.f },
	{ TEXT("FONT"), TEXT("GOLD"), 0.f, 40.f, 254.f, 274.f },
	
	// For CoolTime
	{ TEXT("CIRCLE"), TEXT("COOLTIME_OUTER"), 0.f, 255.f, 0.f, 255.f },
	{ TEXT("CIRCLE"), TEXT("COOLTIME_INNER"), 256.f, 512.f, 0.f, 256.f },
	{ TEXT("FONT"), TEXT("SKL"), 607.f, 644.f, 0.f, 19.f },
	{ TEXT("FONT"), TEXT("1"), 0.f, 12.f, 279.f, 294.f },
	{ TEXT("FONT"), TEXT("2"), 0.f, 12.f, 303.f, 318.f },
	{ TEXT("FONT"), TEXT("3"), 0.f, 12.f, 327.f, 342.f },
	{ TEXT("FONT"), TEXT("4"), 0.f, 12.f, 351.f, 366.f },
	{ TEXT("FONT"), TEXT("5"), 0.f, 12.f, 375.f, 390.f },
	{ TEXT("FONT"), TEXT("6"), 0.f, 12.f, 399.f, 414.f },
	{ TEXT("FONT"), TEXT("7"), 0.f, 12.f, 423.f, 438.f },
	{ TEXT("FONT"), TEXT("8"), 0.f, 12.f, 447.f, 462.f },
	{ TEXT("FONT"), TEXT("9"), 0.f, 12.f, 471.f, 486.f },
	{ TEXT("FONT"), TEXT("0"), 0.f, 12.f, 495.f, 510.f },		

	// For Status
	{ TEXT("FONT"), TEXT("STATUS"), 0.f, 90.f, 542.f, 561.f },
	{ TEXT("FONT"), TEXT("2B"), 0.f, 24.f, 565.f, 583.f },
	{ TEXT("FONT"), TEXT("9S"), 0.f, 24.f, 589.f, 607.f },
	{ TEXT("FONT"), TEXT("SLASH"), 0.f, 10.f, 613.f, 633.f },
	{ TEXT("FONT"), TEXT("("), 14.f, 24.f, 613.f, 633.f },
	{ TEXT("FONT"), TEXT(")"), 30.f, 40.f, 613.f, 633.f },
	{ TEXT("FONT"), TEXT("["), 47.f, 55.f, 613.f, 633.f },
	{ TEXT("FONT"), TEXT("]"), 57.f, 65.f, 613.f, 633.f },
	{ TEXT("FONT"), TEXT("+"), 0.f, 13.f, 641.f, 651.f },
	//{ TEXT("FONT"), TEXT("-"), 16.f, 28.f, 641.f, 651.f },
	{ TEXT("FONT"), TEXT("~"), 33.f, 45.f, 641.f, 651.f },

	{ TEXT("FONT"), TEXT("MONEY"), 0.f, 67.f, 62.f, 82.f },
	{ TEXT("FONT"), TEXT("EXP"), 0.f, 67.f, 86.f, 106.f },
	{ TEXT("FONT"), TEXT("HP"), 0.f, 49.f, 109.f, 129.f },
	{ TEXT("FONT"), TEXT("ATT_HEAVY"), 0.f, 108.f, 134.f, 154.f },
	{ TEXT("FONT"), TEXT("ATT_LIGHT"), 0.f, 108.f, 158.f, 178.f },
	{ TEXT("FONT"), TEXT("ATT_SHOOT"), 0.f, 108.f, 182.f, 203.f },
	{ TEXT("FONT"), TEXT("DEF"), 0.f, 67.f, 206.f, 227.f },
	{ TEXT("FONT"), TEXT("LEVEL"), 0.f, 34.f, 230.f, 248.f },

	// For System Message
	{ TEXT("FONT"), TEXT("ACHIEVED"), 0.f, 184.f, 20.f, 39.f },
	{ TEXT("FONT"), TEXT("USED"), 215.f, 400.f, 20.f, 39.f },
	{ TEXT("FONT"), TEXT("EQUIPED"), 432.f, 616.f, 20.f, 39.f },
	{ TEXT("FONT"), TEXT("ON_QUEST"), 0.f, 333.f, 949.f, 970.f },
	{ TEXT("FONT"), TEXT("NEED_CHIP"), 124.f, 350.f, 781.f, 801.f },

	// For HeadButton & Title
	{ TEXT("FONT"), TEXT("MAP"), 0.f, 35.f,	40.f, 60.f },
	{ TEXT("FONT"), TEXT("QUEST"), 39.f, 92.f, 40.f, 60.f },
	{ TEXT("FONT"), TEXT("ITEM"), 98.f, 150.f, 40.f, 60.f },
	{ TEXT("FONT"), TEXT("WEAPON"), 155.f, 189.f, 40.f, 60.f },
	{ TEXT("FONT"), TEXT("EQUIPMENT"), 196.f, 230.f, 40.f, 60.f },
	{ TEXT("FONT"), TEXT("DATA"), 237.f, 288.f, 40.f, 60.f },
	{ TEXT("FONT"), TEXT("SYSTEM"), 293.f, 346.f, 40.f, 60.f },
	{ TEXT("FONT"), TEXT("INVENTORY"), 352.f, 421.f, 40.f, 60.f },
	{ TEXT("FONT"), TEXT("SHOP"), 427.f, 462.f,	40.f, 60.f },
	{ TEXT("FONT"), TEXT("STORAGE"), 468.f, 597.f, 40.f, 60.f },

	// For ListLine Icon
	{ TEXT("CORE"), TEXT("ICON_EXPENDABLES"), 1478.f, 1517.f, 1080.f, 1134.f },
	{ TEXT("CORE"), TEXT("ICON_RESOURCES"), 166.f, 206.f, 1896.f, 1940.f },
	{ TEXT("CORE"), TEXT("ICON_SPECIAL"), 1259.f, 1298.f, 1842.f, 1881.f },
	{ TEXT("CORE"), TEXT("ICON_SHORTSWORD"), 1843.f, 1890.f, 1377.f, 1424.f },
	{ TEXT("CORE"), TEXT("ICON_LONGSWORD"), 1780.f, 1829.f, 1439.f, 1487.f },
	{ TEXT("CORE"), TEXT("ICON_SPEAR"), 1923.f, 1974.f, 1294.f, 1345.f },
	{ TEXT("CORE"), TEXT("ICON_GAUNTLET"), 1120.f, 1167.f, 1987.f, 2036.f },
	{ TEXT("CORE"), TEXT("ICON_CHIP_ON"), 698.f, 745.f, 1974.f, 2021.f },
	{ TEXT("CORE"), TEXT("ICON_CHIP_OFF"), 954.f, 1001.f, 1994.f, 2041.f },

	// For ListLine Text
	{ TEXT("FONT"), TEXT("POTION_SMALL"), 658.f, 746.f, 0.f, 22.f },
	{ TEXT("FONT"), TEXT("POTION_MIDDLE"), 658.f, 746.f, 23.f, 45.f },
	{ TEXT("FONT"), TEXT("POTION_LARGE"), 658.f, 746.f, 46.f, 69.f },
	{ TEXT("FONT"), TEXT("POTION_EXLARGE"), 658.f, 746.f, 70.f, 93.f },

	{ TEXT("FONT"), TEXT("CHUNK"), 658.f, 712.f, 94.f, 118.f },
	{ TEXT("FONT"), TEXT("COPPER"), 658.f, 695.f, 119.f, 141.f },
	{ TEXT("FONT"), TEXT("STEEL"), 658.f, 695.f, 142.f, 166.f },
	{ TEXT("FONT"), TEXT("AMBER"), 658.f, 695.f, 167.f, 191.f },
	{ TEXT("FONT"), TEXT("SILVER"), 658.f, 695.f, 192.f, 216.f },
	{ TEXT("FONT"), TEXT("MOLDABYTE"), 658.f, 749.f, 216.f, 240.f },
	{ TEXT("FONT"), TEXT("METEORITE"), 658.f, 767.f, 240.f, 260.f },
	{ TEXT("FONT"), TEXT("PEARL"), 658.f, 695.f, 262.f, 286.f },
	{ TEXT("FONT"), TEXT("TITAN"), 658.f, 737.f, 287.f, 311.f },
	{ TEXT("FONT"), TEXT("SM_ALLOY"), 658.f, 768.f, 311.f, 335.f },

	{ TEXT("FONT"), TEXT("WHITE_COVENANT"), 115.f, 194.f, 134.f, 152.f },
	{ TEXT("FONT"), TEXT("WHITE_CONTRACT"), 224.f, 303.f, 134.f, 152.f },
	{ TEXT("FONT"), TEXT("40TH_SPEAR"), 332.f, 433.f, 134.f, 152.f },
	{ TEXT("FONT"), TEXT("40TH_KNUCKLE"), 475.f, 614.f, 134.f, 152.f },

	{ TEXT("FONT"), TEXT("LASTATTACK"), 115.f, 212.f, 109.f, 129.f },

	{ TEXT("FONT"), TEXT("ATTACKUP"), 115.f, 220.f, 157.f, 177.f },
	{ TEXT("FONT"), TEXT("HEALTHUP"), 115.f, 202.f, 181.f, 202.f },
	{ TEXT("FONT"), TEXT("SKILL_OS"), 658.f, 731.f, 391.f, 412.f },
	{ TEXT("FONT"), TEXT("SKILL_MACHINEGUN"), 658.f, 766.f, 415.f, 435.f },
	{ TEXT("FONT"), TEXT("SKILL_HANG"), 658.f, 782.f, 439.f, 460.f },
	{ TEXT("FONT"), TEXT("SKILL_LASER"), 658.f, 766.f, 463.f, 483.f },
	{ TEXT("FONT"), TEXT("SKILL_LIGHT"), 658.f, 766.f, 487.f, 507.f },
	{ TEXT("FONT"), TEXT("MASTERY_SHORTSWORD"), 115.f, 238.f, 253.f, 273.f },
	{ TEXT("FONT"), TEXT("MASTERY_LONGSWORD"), 115.f, 238.f, 277.f, 297.f },
	{ TEXT("FONT"), TEXT("MASTERY_SPEAR"), 115.f, 222.f, 301.f, 322.f },
	{ TEXT("FONT"), TEXT("MASTERY_GAUNTLET"), 115.f, 257.f, 325.f, 346.f },

	// For Detail Text
	{ TEXT("FONT"), TEXT("POTION_SMALL_DETAIL"), 802.f, 969.f, 0.f, 21.f },
	{ TEXT("FONT"), TEXT("POTION_MIDDLE_DETAIL"), 802.f, 969.f, 23.f, 44.f },
	{ TEXT("FONT"), TEXT("POTION_LARGE_DETAIL"), 802.f, 969.f, 48.f, 69.f },
	{ TEXT("FONT"), TEXT("POTION_EXLARGE_DETAIL"), 802.f, 964.f, 72.f, 93.f },
	{ TEXT("FONT"), TEXT("CHUNK_DETAIL"), 802.f, 992.f, 96.f, 116.f },
	{ TEXT("FONT"), TEXT("COPPER_DETAIL"), 802.f, 936.f, 119.f, 141.f },
	{ TEXT("FONT"), TEXT("STEEL_DETAIL"), 802.f, 936.f, 142.f, 166.f },
	{ TEXT("FONT"), TEXT("AMBER_DETAIL"), 802.f, 972.f, 167.f, 191.f },
	{ TEXT("FONT"), TEXT("SILVER_DETAIL"), 802.f, 936.f, 192.f, 216.f },
	{ TEXT("FONT"), TEXT("MOLDABYTE_DETAIL"), 802.f, 972.f, 216.f, 240.f },
	{ TEXT("FONT"), TEXT("METEORITE_DETAIL"), 802.f, 934.f, 240.f, 260.f },
	{ TEXT("FONT"), TEXT("PEARL_DETAIL"), 802.f, 974.f, 262.f, 286.f },
	{ TEXT("FONT"), TEXT("TITAN_DETAIL"), 802.f, 956.f, 287.f, 311.f },
	{ TEXT("FONT"), TEXT("SM_ALLOY_DETAIL"), 802.f, 956.f, 287.f, 311.f },
	{ TEXT("FONT"), TEXT("SKILL_OS_DETAIL"), 658.f, 913.f, 511.f, 531.f },
	{ TEXT("FONT"), TEXT("SKILL_MACHINEGUN_DETAIL"), 658.f, 796.f, 535.f, 556.f },
	{ TEXT("FONT"), TEXT("SKILL_HANG_DETAIL"), 658.f, 926.f, 559.f, 580.f },
	{ TEXT("FONT"), TEXT("SKILL_LASER_DETAIL"), 658.f, 921.f, 583.f, 603.f },
	{ TEXT("FONT"), TEXT("SKILL_LIGHT_DETAIL"), 658.f, 921.f, 607.f, 628.f },

	// For Engage Message
	{ TEXT("FONT"), TEXT("W"), 116.f, 136.f, 60.f, 80.f },
	{ TEXT("FONT"), TEXT("A"), 139.f, 154.f, 60.f, 80.f },
	{ TEXT("FONT"), TEXT("R"), 157.f, 171.f, 60.f, 80.f },
	{ TEXT("FONT"), TEXT("N"), 174.f, 188.f, 60.f, 80.f },
	{ TEXT("FONT"), TEXT("I"), 192.f, 200.f, 60.f, 80.f },
	{ TEXT("FONT"), TEXT("N"), 202.f, 217.f, 60.f, 80.f },
	{ TEXT("FONT"), TEXT("G"), 220.f, 236.f, 60.f, 80.f },
	{ TEXT("FONT"), TEXT("-"), 332.f, 348.f, 60.f, 80.f },
	{ TEXT("FONT"), TEXT("ZHUANGZI"), 116.f, 173.f, 85.f, 106.f },
	{ TEXT("FONT"), TEXT("ENGELS"), 179.f, 252.f, 85.f, 106.f },
	{ TEXT("FONT"), TEXT("BEAUVOIR"), 253.f, 355.f, 85.f, 106.f },
	{ TEXT("FONT"), TEXT("CHARIOT"), 357.f, 537.f, 85.f, 106.f },

	// For Hacking Message
	{ TEXT("FONT"), TEXT("HACKINGONE"), 376.f, 402.f, 176.f, 232.f },
	{ TEXT("FONT"), TEXT("HACKINGTWO"), 420.f, 461.f, 176.f, 232.f },
	{ TEXT("FONT"), TEXT("HACKINGTHREE"), 469.f, 510.f, 176.f, 232.f },
	{ TEXT("FONT"), TEXT("HACKING_H"), 404.f, 429.f, 677.f, 709.f },
	{ TEXT("FONT"), TEXT("HACKING_A"), 437.f, 462.f, 677.f, 709.f },
	{ TEXT("FONT"), TEXT("HACKING_C"), 471.f, 496.f, 677.f, 709.f },
	{ TEXT("FONT"), TEXT("HACKING_K"), 504.f, 529.f, 677.f, 709.f },
	{ TEXT("FONT"), TEXT("HACKING_I"), 536.f, 549.f, 677.f, 709.f },
	{ TEXT("FONT"), TEXT("HACKING_N"), 558.f, 583.f, 677.f, 709.f },
	{ TEXT("FONT"), TEXT("HACKING_G"), 591.f, 619.f, 677.f, 709.f },
	{ TEXT("FONT"), TEXT("HACKING_S"), 626.f, 649.f, 677.f, 709.f },
	{ TEXT("FONT"), TEXT("HACKING_T"), 656.f, 683.f, 677.f, 709.f },
	//{ TEXT("FONT"), TEXT("Hacking_A"), 139.f, 154.f, 677.f, 709.f },
	{ TEXT("FONT"), TEXT("HACKING_R"), 723.f, 746.f, 677.f, 709.f },
	//{ TEXT("FONT"), TEXT("Hacking_T"), 220.f, 236.f, 677.f, 709.f },
	{ TEXT("FONT"), TEXT("HACKING_-"), 787.f, 807.f, 677.f, 709.f },
	{ TEXT("FONT"), TEXT("HACKING_U"), 818.f, 843.f, 677.f, 709.f },
	{ TEXT("FONT"), TEXT("HACKING_E"), 852.f, 873.f, 677.f, 709.f },
	{ TEXT("FONT"), TEXT("HACKING_9"), 883.f, 906.f, 677.f, 709.f },
	{ TEXT("FONT"), TEXT("HACKING_M"), 915.f, 944.f, 677.f, 709.f },
	{ TEXT("FONT"), TEXT("HACKING_L"), 954.f, 975.f, 677.f, 709.f },
	{ TEXT("FONT"), TEXT("HACKING_SPACE"), 980.f, 999.f, 677.f, 709.f },

	// For DecoLine
	{ TEXT("DECO"), TEXT("RACE_TOP"), 0.f, 3200.f, 0.f, 50.f },
	{ TEXT("DECO"), TEXT("RACE_BOTTOM"), 0.f, 3200.f, 50.f, 100.f },
	{ TEXT("DECO"), TEXT("CIRCLE"), 0.f, 720.f, 160.f, 880.f },
	{ TEXT("DECO"), TEXT("DIAGONAL"), 720.f, 1240.f, 160.f, 680.f },

	// For StageObjective
	{ TEXT("FONT"), TEXT("QUEST1"), 0.f, 306.f, 661.f, 681.f },
	{ TEXT("FONT"), TEXT("QUEST2"), 0.f, 175.f, 685.f, 705.f },
	{ TEXT("FONT"), TEXT("QUEST3"), 0.f, 193.f, 709.f, 729.f },
	{ TEXT("FONT"), TEXT("QUEST4"), 0.f, 286.f, 733.f, 753.f },
	{ TEXT("FONT"), TEXT("QUEST5"), 0.f, 210.f, 757.f, 777.f },

	// For SelectLineRole
	{ TEXT("FONT"), TEXT("ROLE_USE"), 0.f, 72.f, 781.f, 801.f },
	{ TEXT("FONT"), TEXT("ROLE_DISPOSE"), 0.f, 53.f, 805.f, 825.f },
	{ TEXT("FONT"), TEXT("ROLE_EQUIP_MAIN"), 0.f, 53.f, 829.f, 849.f },
	{ TEXT("FONT"), TEXT("ROLE_EQUIP_SUB"), 0.f, 72.f, 853.f, 873.f },
	{ TEXT("FONT"), TEXT("ROLE_RELEASE"), 0.f, 72.f, 877.f, 897.f },
	{ TEXT("FONT"), TEXT("ROLE_INSTALL"), 0.f, 53.f, 901.f, 921.f },
	{ TEXT("FONT"), TEXT("ROLE_UNINSTALL"), 0.f, 72.f, 925.f, 945.f },

	// For General Background
	{ TEXT("DECO"), TEXT("BACKGROUND"), 1500.f, 1820.f, 160.f, 480.f },

	// For Indicator
	{ TEXT("INDICATOR_B"), TEXT("INDICATOR_B"), 0.f, 84.f, 0.f, 184.f },
	{ TEXT("INDICATOR_R"), TEXT("INDICATOR_R"), 0.f, 84.f, 0.f, 184.f },

	// For Loading
	{ TEXT("LOADING"), TEXT("YORHA_LOGO"), 0.f, 1160.f, 0.f, 512.f },
	{ TEXT("LOADING"), TEXT("YORHA_MARK"), 1200.f, 1617.f, 0.f, 512.f },
	{ TEXT("LOADING"), TEXT("YORHA_ALL"), 0.f, 1617.f, 15.f, 485.f },

	// For Weapon
	{ TEXT("WEAPON"), TEXT("KATANA"), 0.f, 512.f, 0.f, 127.f },
	{ TEXT("WEAPON"), TEXT("LONGKATANA"), 0.f, 512.f, 128.f, 255.f },
	{ TEXT("WEAPON"), TEXT("SPEAR"), 0.f, 512.f, 256.f, 383.f },
	{ TEXT("WEAPON"), TEXT("GAUNTLET"), 0.f, 512.f, 384.f, 512.f },
	{ TEXT("FONT"), TEXT("MAIN_WEAPON"), 658.f, 759.f, 343.f, 363.f },
	{ TEXT("FONT"), TEXT("SUB_WEAPON"), 658.f, 778.f, 367.f, 387.f },

	// For Program
	{ TEXT("FONT"), TEXT("OS"), 658.f, 732.f, 391.f, 412.f },
	{ TEXT("FONT"), TEXT("POD_GUN"), 658.f, 766.f, 416.f, 436.f },
	{ TEXT("FONT"), TEXT("POD_HANG"), 658.f, 781.f, 440.f, 460.f },
	{ TEXT("FONT"), TEXT("POD_LASER"), 658.f, 766.f, 464.f, 484.f },
	{ TEXT("FONT"), TEXT("POD_LIGHT"), 658.f, 766.f, 488.f, 508.f },
	{ TEXT("FONT"), TEXT("OS_DETAIL"), 658.f, 912.f, 512.f, 532.f },
	{ TEXT("FONT"), TEXT("POD_GUN_DETAIL"), 658.f, 542.f, 536.f, 556.f },
	{ TEXT("FONT"), TEXT("POD_HANG_DETAIL"), 658.f, 926.f, 560.f, 580.f },
	{ TEXT("FONT"), TEXT("POD_LASER_DETAIL"), 658.f, 920.f, 584.f, 604.f },
	{ TEXT("FONT"), TEXT("POD_LIGHT_DETAIL"), 658.f, 920.f, 608.f, 628.f },

	// For HighlightMessage
	{ TEXT("H_MSG"), TEXT("BG"), 0.f, 2560.f, 1140.f, 1440.f },
	{ TEXT("FONT"), TEXT("HMSG_BANNED"), 70.f, 359.f, 805.f, 825.f },
	{ TEXT("FONT"), TEXT("HMSG_MISSION_CLEAR"), 70.f, 301.f, 829.f, 850.f },
	{ TEXT("FONT"), TEXT("HMSG_MISSION_END"), 107.f, 293.f, 853.f, 874.f },
	{ TEXT("FONT"), TEXT("HMSG_THANKS"), 106.f, 311.f, 877.f, 898.f },

	// For Iron
	{ TEXT("IRON"), TEXT("IRON_BG"), 0.f, 1000.f, 0.f, 562.f },
	{ TEXT("IRON"), TEXT("IRON_HS"), 0.f, 390.f, 613.f, 1000.f },
	{ TEXT("IRON"), TEXT("IRON_9S"), 400.f, 1000.f, 600.f, 1000.f },

	{ TEXT("IRON2"), TEXT("IRON2_BG"), 404.f, 606.f, 0.f, 108.f },
	{ TEXT("IRON2"), TEXT("IRON2_HS"), 0.f, 200.f, 0.f, 220.f },
	{ TEXT("IRON2"), TEXT("IRON2_9S"), 800.f, 1000.f, 0.f, 220.f },

	// For Canvas
	{ TEXT("CANVAS"), TEXT("SIMPLE_RECT"), 0.f, 100.f, 0.f, 100.f }
};

const int	CAtlas_Manager::m_AtlasRawDataCount = sizeof(CAtlas_Manager::m_AtlasRawData) / sizeof(RAWDATA);

IMPLEMENT_SINGLETON(CAtlas_Manager)

CAtlas_Manager::CAtlas_Manager()
{
}

void CAtlas_Manager::Free()
{
	for (_uint i = 0; i < m_iNumCategories; ++i)
	{
		for (auto& pair : m_pAtlases[i])
		{
			Safe_Delete(pair.second);
		}
		m_pAtlases[i].clear();
	}
	Safe_Delete_Array(m_pAtlases);
}

HRESULT CAtlas_Manager::NativeConstruct()
{
	if (FAILED(Reserve_Container((_uint)CAtlas_Manager::CATEGORY::COUNT)))
	{
		return E_FAIL;
	}
	if (FAILED(Parse_DataFromHeader()))
	{
		return E_FAIL;
	}
	//if (FAILED(Parse_DataFromDat()))
	//{
	//	return E_FAIL;
	//}
	return S_OK;
}

HRESULT CAtlas_Manager::Reserve_Container(_uint iNumCategories)
{
	if (nullptr != m_pAtlases)
		return E_FAIL;

	m_pAtlases = new ATLASMAP[iNumCategories];

	m_iNumCategories = iNumCategories;

	return S_OK;
}

HRESULT CAtlas_Manager::Parse_DataFromDat()
{
	//HANDLE	hFile = nullptr;

	//DWORD	dwByte = 0;

	//const _tchar*	pFilePaths[] = {
	//	TEXT("../bin/csv/Atlas_UI_Core.csv"),
	//	TEXT("../bin/csv/Atlas_UI_HUD.csv"),
	//	TEXT("../bin/csv/Atlas_UI_Shop.csv")
	//};

	//_uint iFileCount = sizeof(pFilePaths);

	//for (_uint i = 0; i < iFileCount; ++i)
	//{
	//	hFile = CreateFile(pFilePaths[i], GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	//	//hFile = CreateFile(TEXT("AtlasDesc.csv"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	//	if (INVALID_HANDLE_VALUE == hFile)
	//	{
	//		return E_FAIL;
	//	}

	//	char									AtlasTexName[MAX_PATH];
	//	char									AtlasElemType[MAX_PATH];
	//	ATLASDESC								AtlasCoordDesc;
	//	unordered_map<const char*, ATLASDESC>	AtlasElemMap;

	//	ZeroMemory(AtlasTexName, sizeof(char) * MAX_PATH);
	//	ZeroMemory(&AtlasElemType, sizeof(char) * MAX_PATH);
	//	ZeroMemory(&AtlasCoordDesc, sizeof(ATLASDESC));

	//	ReadFile(hFile, AtlasTexName, sizeof(char) * MAX_PATH, &dwByte, nullptr);

	//	while (true)
	//	{
	//		ReadFile(hFile, AtlasElemType, sizeof(char) * MAX_PATH, &dwByte, nullptr);
	//		ReadFile(hFile, &AtlasCoordDesc, sizeof(ATLASDESC), &dwByte, nullptr);

	//		AtlasElemMap.emplace(AtlasElemType, AtlasCoordDesc);

	//		if (0 == dwByte)
	//			break;
	//	}
	//	m_AtlasMap.emplace(AtlasTexName, AtlasElemMap);

	//}

	return S_OK;
}

HRESULT CAtlas_Manager::Parse_DataFromHeader()
{
	for (_uint i = 0; i < m_AtlasRawDataCount; ++i)
	{
		_uint iCategory = 0;

		if (!lstrcmp(TEXT("CORE"), m_AtlasRawData[i].pCategory))
		{
			iCategory = (_uint)CAtlas_Manager::CATEGORY::CORE;
		}
		else if (!lstrcmp(TEXT("HUD"), m_AtlasRawData[i].pCategory))
		{
			iCategory = (_uint)CAtlas_Manager::CATEGORY::HUD;
		}
		else if (!lstrcmp(TEXT("BACKGROUND"), m_AtlasRawData[i].pCategory))
		{
			iCategory = (_uint)CAtlas_Manager::CATEGORY::BACKGROUND;
		}
		else if (!lstrcmp(TEXT("TRIGRID"), m_AtlasRawData[i].pCategory))
		{
			iCategory = (_uint)CAtlas_Manager::CATEGORY::TRIGRID;
		}
		else if (!lstrcmp(TEXT("CURSOR"), m_AtlasRawData[i].pCategory))
		{
			iCategory = (_uint)CAtlas_Manager::CATEGORY::CURSOR;
		}
		else if (!lstrcmp(TEXT("DAMAGE_A"), m_AtlasRawData[i].pCategory))
		{
			iCategory = (_uint)CAtlas_Manager::CATEGORY::DAMAGE_A;
		}
		else if (!lstrcmp(TEXT("DAMAGE_B"), m_AtlasRawData[i].pCategory))
		{
			iCategory = (_uint)CAtlas_Manager::CATEGORY::DAMAGE_B;
		}
		else if (!lstrcmp(TEXT("TARGET"), m_AtlasRawData[i].pCategory))
		{
			iCategory = (_uint)CAtlas_Manager::CATEGORY::TARGET;
		}
		else if (!lstrcmp(TEXT("FONT"), m_AtlasRawData[i].pCategory))
		{
			iCategory = (_uint)CAtlas_Manager::CATEGORY::FONT;
		}
		else if (!lstrcmp(TEXT("CIRCLE"), m_AtlasRawData[i].pCategory))
		{
			iCategory = (_uint)CAtlas_Manager::CATEGORY::CIRCLE;
		}
		else if (!lstrcmp(TEXT("DECO"), m_AtlasRawData[i].pCategory))
		{
			iCategory = (_uint)CAtlas_Manager::CATEGORY::DECO;
		}
		else if (!lstrcmp(TEXT("INDICATOR_B"), m_AtlasRawData[i].pCategory))
		{
			iCategory = (_uint)CAtlas_Manager::CATEGORY::INDICATOR_B;
		}
		else if (!lstrcmp(TEXT("INDICATOR_R"), m_AtlasRawData[i].pCategory))
		{
			iCategory = (_uint)CAtlas_Manager::CATEGORY::INDICATOR_R;
		}
		else if (!lstrcmp(TEXT("CANVAS"), m_AtlasRawData[i].pCategory))
		{
			iCategory = (_uint)CAtlas_Manager::CATEGORY::CANVAS;
		}
		else if (!lstrcmp(TEXT("LOADING"), m_AtlasRawData[i].pCategory))
		{
			iCategory = (_uint)CAtlas_Manager::CATEGORY::LOADING;
		}
		else if (!lstrcmp(TEXT("WEAPON"), m_AtlasRawData[i].pCategory))
		{
			iCategory = (_uint)CAtlas_Manager::CATEGORY::WEAPON;
		}
		else if (!lstrcmp(TEXT("H_MSG"), m_AtlasRawData[i].pCategory))
		{
			iCategory = (_uint)CAtlas_Manager::CATEGORY::H_MSG;
		}
		else if (!lstrcmp(TEXT("IRON"), m_AtlasRawData[i].pCategory))
		{
			iCategory = (_uint)CAtlas_Manager::CATEGORY::IRON;
		}
		else if (!lstrcmp(TEXT("IRON2"), m_AtlasRawData[i].pCategory))
		{
			iCategory = (_uint)CAtlas_Manager::CATEGORY::IRON2;
		}
		

		ATLASDESC	atlasDesc;

		atlasDesc.fLeft = m_AtlasRawData[i].fLeft;
		atlasDesc.fRight = m_AtlasRawData[i].fRight;
		atlasDesc.fTop = m_AtlasRawData[i].fTop;
		atlasDesc.fBottom = m_AtlasRawData[i].fBottom;
	
		Add_AtlasDesc(iCategory, m_AtlasRawData[i].pElement, &atlasDesc);
	}

	// For Debug
	//m_pAtlases[0];
	//m_pAtlases[1];
	//m_pAtlases[2];
	//m_pAtlases[3];

	return S_OK;
}

HRESULT CAtlas_Manager::Add_AtlasDesc(_uint iCategoryIndex, const _tchar * pElemTag, ATLASDESC * pAtlasDesc)
{
	if (iCategoryIndex >= m_iNumCategories || nullptr == pAtlasDesc)
	{
		return E_FAIL;
	}
	
	auto iterAtlasMap = find_if(m_pAtlases[iCategoryIndex].begin(), m_pAtlases[iCategoryIndex].end(), CTagFinder(pElemTag));

	if (iterAtlasMap == m_pAtlases[iCategoryIndex].end())
	{
		ATLASDESC*	pDesc = new ATLASDESC;

		if (nullptr == pAtlasDesc)
		{
			return E_FAIL;
		}
		memcpy(pDesc, pAtlasDesc, sizeof(ATLASDESC));

		m_pAtlases[iCategoryIndex].emplace(pElemTag, pDesc);
	}
	return S_OK;
}

ATLASDESC * CAtlas_Manager::Get_AtlasDesc(_uint iCategoryIndex, const _tchar * pElemTag)
{
	if (iCategoryIndex >= m_iNumCategories || nullptr == pElemTag)
	{
		return nullptr;
	}

	auto iterAtlasDesc = find_if(m_pAtlases[iCategoryIndex].begin(), m_pAtlases[iCategoryIndex].end(), CTagFinder(pElemTag));
	
	if (iterAtlasDesc == m_pAtlases[iCategoryIndex].end())
	{
		return nullptr;
	}
	return iterAtlasDesc->second;
}
