# Skylander Format

## tfbSpyroTag_TagHeader

The header is 0x20 bytes long

| Offset | Type                   | Description
|--------|------------------------|---------------
|  0000  | `uint32_t`             | Non-Unique Identifier for this toy, internally referred to as the serial number
|  0010  | `kTfbSpyroTag_ToyType` | (24 bit int) The character id of this skylander (see [kTfbSpyroTag_ToyType.hpp](../include/kTfbSpyroTag_ToyType.hpp))
|  0013  | `uint8_t`              | Unknown, Skylanders Superchargers at least checks to see if this byte is 0, if not, it'll refuse to read the toy
|  0014  | `uint64_t`             | The trading card id, [web-code](#web-code) is derived from this, internally this is seperated into 2 `uint32_t`s, presumably to get around alignment issues
|  001C  | `uint16_t`             | The variant id of this skylander (see [here](#sub-type) to understand how this works)
|  001E  | `uint16_t`             | The crc16-ccit/false checksum for the first 0x1E bytes of the header

### Variant ID

The variant id is a 16 bit long bit field

| Shift | Mask | Type                | Description
|-------|------|---------------------|-------------
| 0x00  | 00FF | kTfbSpyroTag_DecoID | The [deco ID](../include/kTfbSpyroTag_DecoID.hpp)
| 0x08  | 0001 | bool                | Whether or not this skylander is a supercharger
| 0x09  | 0001 | bool                | Whether or not this skylander is a lightcore
| 0x0A  | 0001 | bool                | Whether or not this skylander is a full alt deco
| 0x0B  | 0001 | bool                | Whether or not this skylander has a wow pow (wow pow to use is decided from the year code)
| 0x0C  | 000F | ESkylandersGame     | [Year code](../include/ESkylandersGame.hpp)

### Web Code

The web code is derived from the trading card id.
Similar to how we have base 16 or base 2 for hexadecimal and binary respectively, the web code is actually stored as base-29.
By this, I mean that, you can modulo the trading card id by 29, and then put it through a lookup table to get the last digit of the web code,
you can then divide by 29 and repeat to get the second to last digit.
The lookup table consists of the characters `23456789BCDFGHJKLMNPQRSTVWXYZ` in that order, such that the first item, item 0, is `2` and the last character, item 28, is `Z`.

Moreover, you can look at `Runes::PortalTag::StoreHeader()` for easier to read code.

### Trap

* The 0x40 bytes from offset 0x00 is the magic moment data
* The 0x110 bytes from 0x40 is the remaining data;

NOTE: Some of this information may be incorrect and is actively being worked on.

|  St_Off  | Block  | Bl_Off | Type                       | Description
|----------|--------|--------|----------------------------|---------------
|  0x0007  | 08/24  |  0x07  | `kTfbSpyroTag_VillainType` | ID of currently trapped villain, used if the other one is 0
|  0x0009  | 08/24  |  0x09  | `uint8_t`                  | [area sequence](#area-sequence)
|  0x000A  | 08/24  |  0x0C  | `uint16_t`                 | crc16-ccit/false checksum of 0x110 bytes from 0x40 (so blocks 0D/29 -> 23/3F excluding access control blocks)
|  0x000C  | 08/24  |  0x0A  | `uint16_t`                 | crc16-ccit/false checksum of 0x30 bytes starting from 0x10 (so blocks 09/25 -> 0C/28 excluding access control blocks)
|  0x000E  | 08/24  |  0x0E  | `uint16_t`                 | crc16-ccit/false checksum of the first 14 bytes of this struct + the bytes "05 00" at the end
|  0x0010  | 09/25  |  0x00  | `kTfbSpyroTag_VillainType` | ID of currently trapped villain, always checked first (not locked to element) (See [kTfbSpyroTag_VillainType.hpp](../include//kTfbSpyroTag_VillainType.hpp))
|  0x0130  | 21/3D  |  0x00  | `uint8_[9]`                | Something related to time spent on what platforms

### Racing Pack

| St_Off | Block  | Bl_Off | Type                   | Description
|--------|--------|--------|------------------------|---------------
|  0x10  | 09/25  |  0x04  | `uint16_t`             | Number of captured villains (Superchargers). Something else (Superchargers Racing).

### Vehicle

Placeholder

### Not a Trap or Vehicle

Note that tfbSpyroTag_MagicMomentAll and tfbSpyroTag_RemainingDataAll are used by the game internally
* The 0x40 bytes from offset 0x00 are the first 0x40 bytes of tfbSpyroTag_MagicMomentAll
* The 0x30 bytes from offset 0x40 are the first 0x30 bytes of tfbSpyroTag_RemainingDataAll
* The 0x10 bytes from offset 0x70 are the remaining 0x10 bytes of tfbSpyroTag_MagicMomentAll
* The 0x30 bytes from offset 0x80 are the remaining 0x30 bytes of tfbSpyroTag_RemainingDataAll

| St_Off | Block  | Bl_Off | Type                   | Description
|--------|--------|--------|------------------------|---------------
|  0x00  | 08/24  |  0x00  | `uint24_t`             | 2011 [Experience](#experience) value (Max is 33000)
|  0x03  | 08/24  |  0x03  | `uint16_t`             | Money
|  0x05  | 08/24  |  0x05  | `uint32_t`             | Cumulative time in seconds
|  0x09  | 08/24  |  0x09  | `uint8_t`              | [area sequence](#area-sequence)
|  0x0A  | 08/24  |  0x0C  | `uint16_t`             | crc16-ccit/false checksum of 0x30 bytes starting from 0x40, followed by 0xE0 bytes of 0 (so blocks 0D/29 -> 10/2C excluding access control blocks)
|  0x0C  | 08/24  |  0x0A  | `uint16_t`             | crc16-ccit/false checksum of 0x30 bytes starting from 0x10 (so blocks 09/25 -> 0C/28 excluding access control blocks)
|  0x0E  | 08/24  |  0x0E  | `uint16_t`             | crc16-ccit/false checksum of the first 14 bytes of this struct + the bytes "05 00" at the end
|  0x10  | 09/25  |  0x00  | `uint24_t`             | [Flags1](#flags)
|  0x13  | 09/25  |  0x03  | `uint8_t`              | 2011 [Platform bitfield](#platform-bitfield)
|  0x14  | 09/25  |  0x04  | `uint16_t`             | 2011 [Hat value](#hat-value)
|  0x16  | 09/25  |  0x06  | `uint8_t`              | `(1 << (dataRegionCount - 1)) - 1`. since `dataRegionCount` is always set to 2 on core figures, this always evaluates to 1
|  0x17  | 09/25  |  0x07  | `uint8_t`              | 2013 [Platform bitfield](#platform-bitfield)
|  0x20  | 0A/26  |  0x00  | `wchar_t[8]`           | First 16 bytes of nickname
|  0x30  | 0C/28  |  0x00  | `wchar_t[8]`           | First 16 bytes of nickname
|  0x40  | 0D/29  |  0x00  | `uint8_t`              | Minute value of the last time this figure was placed on the portal
|  0x41  | 0D/29  |  0x01  | `uint8_t`              | Hour value of the last time this figure was placed on the portal
|  0x42  | 0D/29  |  0x02  | `uint8_t`              | Day value of the last time this figure was placed on the portal
|  0x43  | 0D/29  |  0x03  | `uint8_t`              | Month value of the last time this figure was placed on the portal
|  0x44  | 0D/29  |  0x04  | `uint16_t`             | Year value of the last time this figure was placed on the portal
|  0x46  | 0D/29  |  0x06  | `uint32_t`             | Completed ssa heroic challenges
|  0x4A  | 0D/29  |  0x0A  | `uint16_t`             | Hero points (Max is 100)
|  0x4F  | 0D/29  |  0x0F  | `uint8_t`              | Owner count(?)
|  0x50  | 0E/2A  |  0x00  | `uint8_t`              | Minute value of the last time this figure was reset (if never reset then the first time they were placed on a portal)
|  0x51  | 0E/2A  |  0x01  | `uint8_t`              | Hour value of the last time this figure was reset (if never reset then the first time they were placed on a portal)
|  0x52  | 0E/2A  |  0x02  | `uint8_t`              | Day value of the last time this figure was reset (if never reset then the first time they were placed on a portal)
|  0x53  | 0E/2A  |  0x03  | `uint8_t`              | Month value of the last time this figure was reset (if never reset then the first time they were placed on a portal)
|  0x54  | 0E/2A  |  0x04  | `uint16_t`             | Year value of the last time this figure was reset (if never reset then the first time they were placed on a portal)
|  0x60  | 10/2C  |  0x00  | `uint8_[9]`            | Something related to time spent on what platforms
|  0x70  | 11/2D  |  0x00  | `uint16_t`             | crc16-ccit/false checksum of the bytes "06 01" followed by 0x3E bytes from 0x72
|  0x72  | 11/2D  |  0x02  | `uint8_t`              | Area sequence for this data area
|  0x73  | 11/2D  |  0x03  | `uint16_t`             | 2012 [Experience](#experience) value (Max is 63500 for ssf characters onwards, for ssa and giants it's 65535)
|  0x75  | 11/2D  |  0x05  | `uint8_t`              | 2012 [Hat value](#hat-value)
|  0x76  | 11/2D  |  0x06  | `uint16_t`             | [Flags2](#flags)
|  0x78  | 11/2D  |  0x08  | `uint32_t`             | 2013 [Experience](#experience) value (Max is 101000)
|  0x7C  | 11/2D  |  0x0C  | `uint8_t`              | 2013/2014 [Hat value](#hat-value)
|  0x7E  | 11/2D  |  0x0E  | `uint8_t`              | 2015 [Hat value](#hat-value) (add 256 to get the true hat id)
|  0x80  | 12/2E  |  0x00  | `uint32_t`             | [Battlegrounds Flags](#battlegrounds-flags)
|  0x84  | 12/2E  |  0x04  | `uint24_t`             | Completed sg heroic challenges
|  0x87  | 12/2E  |  0x07  | `uint72_t`             | Giants [quests](#quests)
|  0x97  | 14/30  |  0x07  | `uint72_t`             | Swap Force [quests](#quests)

### Experience

Sum of all experience values

| Level | Experience |
|-------|------------|
| 1     | 0          |
| 2     | 1000       |
| 3     | 2200       |
| 4     | 3800       |
| 5     | 6000       |
| 6     | 9000       |
| 7     | 13000      |
| 8     | 18200      |
| 9     | 24800      |
| 10    | 33000      |
| 11    | 42700      |
| 12    | 53900      |
| 13    | 66600      |
| 14    | 80800      |
| 15    | 96500      |
| 16    | 113700     |
| 17    | 132400     |
| 18    | 152600     |
| 19    | 174300     |
| 20    | 197500     |

### Area sequence

* The core skylanders at least store the tfbSpyroTagData struct twice, once from block 0x08/0x24 and again from block 0x11/0x2D, these are called data regions or data areas.
* The recently written to area is the one with the area sequence value that is one higher than the other, remember to take overflows into accounts (e.g., an area sequence of 0 is considered higher than one of 255).
* When writing to a figure, the game will switch to the lower area sequence value and increment it by 2 (so that it becomes higher than the other one).
* Note that there are two area sequences, one for blocks 0x08/0x24->0x10/0x2C and another for blocks 0x11/0x2D->0x15/0x31 (inclusive)

### Hat value

* Check the most newest hat value, if it's not 0, return that, otherwise check the next oldest hat value and repeat
* [hat enum](../include/kTfbSpyroTag_HatType.hpp)
* Note that the following ids are identical to the ids used in the file names of Skylanders Spyro's Adventure, Skylanders Giants, and Skylanders Trap Team minus 1. For example, The straw hat has id 9 on figures but has id 8 in the files. 
* The unused hat ids are unused.
* The padding hat ids were never meant to be used in the first place.

| Hat ID |  Hat Name
|--------|-------------------------------
|  0000  | None
|  0001  | Combat Hat
|  0002  | Napoleon Hat
|  0003  | Spy Gear
|  0004  | Miner Hat
|  0005  | General's Hat
|  0006  | Pirate Hat
|  0007  | Propellor Cap
|  0008  | Coonskin Cap
|  0009  | Straw Hat
|  0010  | Fancy Hat
|  0011  | Top Hat
|  0012  | Viking Helmet
|  0013  | Spiked Hat
|  0014  | Anvil Hat
|  0015  | Beret
|  0016  | Birthday Hat
|  0017  | Bone Head
|  0018  | Bowler Hat
|  0019  | Wabbit Ears
|  0020  | Tropical Turban
|  0021  | Chef Hat
|  0022  | Cowboy Hat
|  0023  | Rocker Hair
|  0024  | Royal Crown
|  0025  | Lil Devil
|  0026  | Eye Hat
|  0027  | Fez
|  0028  | Crown of Light
|  0029  | Jester Hat
|  0030  | Winged Hat
|  0031  | Moose Hat
|  0032  | Plunger Head
|  0033  | Pan Hat
|  0034  | Rocket Hat
|  0035  | Santa Hat
|  0036  | Tiki Hat
|  0037  | Trojan Helmet
|  0038  | Unicorn Hat
|  0039  | Wizard Hat (Spyro's Adventure)
|  0040  | Pumpkin Hat
|  0041  | Pirate Doo Rag
|  0042  | Cossack Hat
|  0043  | Flower Hat
|  0044  | Balloon Hat
|  0045  | Happy Birthday!
|  0046  | Vintage Baseball Cap
|  0047  | Unused Hat 47
|  0048  | Bowling Pin Hat
|  0049  | Officer Cap
|  0050  | Firefighter Helmet
|  0051  | Graduation Hat
|  0052  | Lampshade Hat
|  0053  | Mariachi Hat
|  0054  | Unused Hat 54
|  0055  | Paper Fast Food Hat
|  0056  | Pilgrim Hat
|  0057  | Police Siren Hat
|  0058  | Purple Fedora
|  0059  | Archer Hat
|  0060  | Unused Hat 60
|  0061  | Safari Hat
|  0062  | Sailor Hat
|  0063  | Unused Hat 63
|  0064  | Dancer Hat
|  0065  | Traffic Cone Hat
|  0066  | Turban
|  0067  | Battle Helmet
|  0068  | Bottle Cap Hat
|  0069  | Unused Hat 69
|  0070  | Carrot Hat
|  0071  | Unused Hat 71
|  0072  | Elf Hat
|  0073  | Fishing Hat
|  0074  | Future Hat
|  0075  | Nefertiti
|  0076  | Unused Hat 76
|  0077  | Pants Hat
|  0078  | Princess Hat
|  0079  | Toy Soldier Hat
|  0080  | Trucker Hat
|  0081  | Umbrella Hat
|  0082  | Showtime Hat
|  0083  | Ceasar Hat
|  0084  | Flower Fairy Hat
|  0085  | Funnel Hat
|  0086  | Scrumshanks Hat
|  0087  | Biter Hat
|  0088  | Atom Hat
|  0089  | Sombrero
|  0090  | Rasta Hat
|  0091  | Kufi Hat
|  0092  | Knight Helm
|  0093  | Dangling Carrot Hat
|  0094  | Bronze Top Hat
|  0095  | Silver Top Hat
|  0096  | Gold Top Hat
|  0097  | Rain Hat
|  0098  | The Outsider
|  0099  | Greeble Hat
|  0100  | Volcano Hat
|  0101  | Boater Hat
|  0102  | Stone Hat
|  0103  | Stovepipe Hat
|  0104  | Boonie Hat
|  0105  | Sawblade Hat
|  0106  | Zombeanie Hat
|  0107  | Gaucho Hat
|  0108  | Roundlet Hat
|  0109  | Capuchon
|  0110  | Tricorn Hat
|  0111  | Feathered Headdress
|  0112  | Bearskin Cap
|  0113  | Fishbone Hat
|  0114  | Ski Cap
|  0115  | Crown of Frost
|  0116  | Four Winds Hat
|  0117  | Beacon Hat
|  0118  | Flower Garland
|  0119  | Tree Branch
|  0120  | Aviator's Cap
|  0121  | Asteroid Hat
|  0122  | Crystal Hat
|  0123  | Creepy Helm
|  0124  | Fancy Ribbon
|  0125  | Deely Boppers
|  0126  | Beanie
|  0127  | Leprechaun Hat
|  0128  | Shark Hat
|  0129  | Life Preserver Hat
|  0130  | Glittering Tiara
|  0131  | Great Helm
|  0132  | Space Helmet
|  0133  | UFO Hat
|  0134  | Whirlwind Diadem
|  0135  | Obsidian Helm
|  0136  | Lilypad Hat
|  0137  | Crown of Flames
|  0138  | Runic Headband
|  0139  | Clockwork Hat
|  0140  | Cactus Hat
|  0141  | Skullhelm
|  0142  | Gloop Hat
|  0143  | Puma Hat
|  0144  | Elephant Hat
|  0145  | Tiger Skin Cap
|  0146  | Teeth Top Hat
|  0147  | Turkey Hat
|  0148  | Eyefro
|  0149  | Bacon Bandana
|  0150  | Awesome Hat
|  0151  | Card Shark Hat
|  0152  | Springtime Hat
|  0153  | Jolly Hat
|  0154  | Kickoff Hat
|  0155  | Beetle Hat
|  0156  | Brain Hat
|  0157  | Brainiac Hat
|  0158  | Bucket Hat
|  0159  | Desert Crown
|  0160  | Ceiling Fan Hat
|  0161  | Imperial Hat
|  0162  | Clown Classic Hat
|  0163  | Clown Bowler Hat
|  0164  | Colander Hat
|  0165  | Kepi Hat
|  0166  | Cornucopia Hat
|  0167  | Cubano Hat
|  0168  | Cycling Hat
|  0169  | Daisy Crown
|  0170  | Dragon Skull
|  0171  | Outback Hat
|  0172  | Lil' Elf Hat
|  0173  | Generalissimo
|  0174  | Garrison Hat
|  0175  | Gondolier Hat
|  0176  | Hunting Hat
|  0177  | Juicer Hat
|  0178  | Kokoshnik
|  0179  | Medic Hat
|  0180  | Melon Hat
|  0181  | Mountie Hat
|  0182  | Nurse Hat
|  0183  | Palm Hat
|  0184  | Paperboy Hat
|  0185  | Parrot Nest
|  0186  | Old-Time Movie Hat
|  0187  | Classic Pot Hat
|  0188  | Radar Hat
|  0189  | Crazy Light Bulb Hat
|  0190  | Rubber Glove Hat
|  0191  | Rugby Hat
|  0192  | Sharkfin Hat (Shark Hat)
|  0193  | Sleuth Hat
|  0194  | Shower Cap
|  0195  | Bobby
|  0196  | Hedgehog Hat
|  0197  | Steampunk Hat
|  0198  | Flight Attendant Hat
|  0199  | Monday Hat
|  0200  | Sherpa Hat
|  0201  | Trash Lid
|  0202  | Turtle Hat
|  0203  | Extreme Viking Hat
|  0204  | Scooter Hat
|  0205  | Volcano Island Hat
|  0206  | Synchronized Swimming Cap
|  0207  | William Tell Hat
|  0208  | Tribal Hat
|  0209  | Rude Boy Hat
|  0210  | Pork Pie Hat
|  0211  | Alarm Clock Hat
|  0212  | Batter Up Hat
|  0213  | Horns Be With You
|  0214  | Croissant Hat
|  0215  | Weather Vane Hat
|  0216  | Rainbow Hat
|  0217  | Eye of Kaos Hat
|  0218  | Bat Hat
|  0219  | Light Bulb
|  0220  | Firefly Jar
|  0221  | Shadow Ghost Hat
|  0222  | Lighthouse Beacon Hat
|  0223  | Tin Foil Hat
|  0224  | Night Cap
|  0225  | Storm Hat
|  0226  | Gold Arkeyan Helm
|  0227  | Toucan Hat
|  0228  | Pyramid Hat
|  0229  | Miniature Skylands Hat
|  0230  | Wizard Hat (Trap Team)/Sorcerer
|  0231  | Unused Hat 231
|  0232  | Candy Cane Hat
|  0233  | Eggshell Hat
|  0234  | Candle Hat
|  0235  | Dark Helm
|  0236  | Planet Hat
|  0237  | Bellhop Hat
|  0238  | Bronze Arkeyan Helm
|  0239  | Silver Arkeyan Helm
|  0240  | Raver Hat
|  0241  | Shire Hat
|  0242  | Mongol Hat
|  0243  | Skipper Hat
|  0244  | Medieval Bard Hat
|  0245  | Wooden Hat
|  0246  | Carnival Hat
|  0247  | Coconut Hat
|  0248  | Model Home Hat
|  0249  | Ice Cream Hat
|  0250  | Molekin Mountain Hat
|  0251  | Sheepwrecked Hat
|  0252  | Core of Light Hat
|  0253  | Octavius Cloptimus Hat
|  0254  | Padding Hat 254
|  0255  | Padding Hat 255
|  0256  | Padding Hat 256
|  0257  | Padding Hat 257
|  0258  | Padding Hat 258
|  0259  | Padding Hat 259
|  0260  | Dive Bomber Hat
|  0261  | Sea Shadow Hat
|  0262  | Burn-Cycle Header
|  0263  | Reef Ripper Helmet
|  0264  | Jet Stream Helmet
|  0265  | Soda Skimmer Shower Cap
|  0266  | Tomb Buggy Skullcap
|  0267  | Stealth Stinger Beanie
|  0268  | Shark Tank Topper
|  0269  | Gold Rusher Cog Cap
|  0270  | Splatter Splasher Spires
|  0271  | Thump Trucker's Hat
|  0272  | Buzz Wing Hat
|  0273  | Shield Striker Helmet
|  0274  | Sun Runner Spikes
|  0275  | Hot Streak Headpiece
|  0276  | Sky Slicer Hat
|  0277  | Crypt Crusher Cap
|  0278  | Mags Hat
|  0279  | Kaos Krown
|  0280  | Eon's Helm

### Platform bitfield

* 0x0001: Wii
* 0x0002: Xbox 360
* 0x0004: PS3
* 0x0800: iOS 64 (Maybe iOS 32 as well)
* 0x0100: Android 32 (Maybe Android 64 as well)

### Flags

* Upgrage flags: `((Flags2 & 0xF) << 10) | (Flags1 & 0x3FF)`
* Element Collection Count 1: `(Flags1 >> 10) & 3`
* Element Collection Count 2: `(Flags2 >> 6) & 7`
* Element Collection: `((Flags1 >> 10) & 3) + ((Flags2 >> 6) & 7) + ((Flags2 >> 11) & 3)`
* SSF Accolade Rank: `(Flags2 >> 9) & 3`
* SG Accolade Rank: `(Flags2 >> 4) & 3`

### Battlegrounds Flags

* Portal Master Level: `(BGFlags << 0x06) >> 0x1A`
* Ability Slot Count: `((BGFlags << 0x0C) >> 0x1E) + 1`
* Ability Level: `(BGFlags >> ((abilityIndex * 3) & 0xFF)) & 0x7`

### Quests

Think of it as a 72 bit int.

Giants Quests
|  Bits  | Giants Name      
|--------|----------------------
|   0A   | Monster Masher
|   04   | Battle Champ
|   06   | Chow Hound
|   01   | Heroic Challenger
|   01   | Arena Artist
|   0D   | Elementalist
|   05   | Stonesmith (Earth)
|   05   | Wrecker (Earth)
|   01   | Extinguisher (Water)
|   05   | Waterfall (Water)
|   09   | Sky Looter (Air)
|   05   | From Above (Air)
|   05   | Bombardier (Fire)
|   01   | Steamer (Fire)
|   08   | Fully Stocked (Life)
|   08   | Melon Maestro (Life)
|   01   | By a Thread (Undead)
|   02   | Bossed Around (Undead)
|   05   | Puzzle Power (Magic)
|   01   | Warp Womper (Magic)
|   06   | Magic Isn't Might (Tech)
|   05   | Cracker (Tech)
|   10   | Individul Quest

Swap Force Quests
| Shift  | Mask | Swap Force Name      
|--------|------|----------------------
|   00   | 03FF | Badguy Basher
|   0A   | 000F | Fruit Frontiersman
|   0E   | 0001 | Flawless Challenger
|   0F   | 000F | True Gladiator
|   13   | 0001 | Totally Maxed Out
|   14   | 1FFF | Elementalist
|   21   | 00FF | Elemental Quest 1
|   29   | 01FF | Elemental Quest 2
|   32   | FFFF | Individual Quest

Credits:
* Brandon Wilson:
  * The encryption method
  * Checkum types 0->3 and access specifier
  * The Wii and Xbox 360 values for the platform usage
  * Hero points max value
* Mandar1jn:
  * Bytes 0x04->0x10 of the tag header
* Winner Nombre:
  * Figured out Tech element quests
  * Figured out what Accolade Ranks are
  * Corrections with crc checksums
