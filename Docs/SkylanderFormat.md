# Skylander Format

## tfbSpyroTag_TagHeader

The header is 0x20 bytes long.

| Offset | Type                   | Description
|--------|------------------------|---------------
|  0000  | `uint32_t`             | Non-Unique Identifier for this toy, internally referred to as the serial number
|  0010  | `kTfbSpyroTag_ToyType` | (24 bit int) The Character ID of this Skylander (see [kTfbSpyroTag_ToyType.hpp](../include/kTfbSpyroTag_ToyType.hpp))
|  0013  | `uint8_t`              | Full purpose unknown. If non-0, the game will refuse the toy completely, prompting that the tag cannot be used in this game (SG, TT, SSC tested)
|  0014  | `uint64_t`             | The Trading Card ID, [Web Code](#web-code) is derived from this, internally this is seperated into 2 `uint32_t`s, presumably to get around alignment issues
|  001C  | `uint16_t`             | The Variant ID of this skylander (see [here](#sub-type) to understand how this works)
|  001E  | `uint16_t`             | The crc16-ccit/false checksum for the first 0x1E bytes of the header

### Variant ID

The Variant ID is a 16 bit long bit field. Note that with the way some tags were programmed, some do not fit with the criteria perfectly. Additionally, the flag that determines if the Skylander is a SuperCharger is never acted upon by the games and is essentially meaningless.

| Shift | Mask | Type                | Description
|-------|------|---------------------|-------------
| 0x00  | 00FF | kTfbSpyroTag_DecoID | The [Deco ID](../include/kTfbSpyroTag_DecoID.hpp)
| 0x08  | 0001 | bool                | Whether or not this Skylander is a SuperCharger
| 0x09  | 0001 | bool                | Whether or not this Skylander possesses LightCore technology; includes LightCores, Giants, Battle Pieces, certain TT expansions, and Creation Crystals
| 0x0A  | 0001 | bool                | Whether or not this Skylander is an in-game variant
| 0x0B  | 0001 | bool                | Whether or not this Skylander is reposed, normally meaning it has a Wow Pow (decided from the year code)
| 0x0C  | 000F | ESkylandersGame     | [Year code](../include/ESkylandersGame.hpp)

### Web Code

The Web Code is derived from the Trading Card ID.
Similar to how we have base-16 or base-2 for hexadecimal and binary respectively, the web code is actually stored as base-29.
By this, I mean that, you can modulo the Trading Card ID by 29, and then put it through a lookup table to get the last digit of the web code,
you can then divide by 29 and repeat to get the second to last digit.
The lookup table consists of the characters `23456789BCDFGHJKLMNPQRSTVWXYZ` in that order, such that the first item, item 0, is `2` and the last character, item 28, is `Z`.

Moreover, you can look at `Runes::PortalTag::StoreHeader()` [here](../source/PortalTag.cpp) for easier to read code.

### Trap

* The 0x40 bytes from offset 0x00 is the Magic Moment Data (encompasses the actual trapped villain)
* The 0x110 bytes from 0x40 is the Remaining Data (contains caches of previously trapped villains)

NOTE: Some of this information may be incorrect and is actively being worked on.

|  St_Off  | Block  | Bl_Off | Type                       | Description
|----------|--------|--------|----------------------------|---------------
|  0x0000  | 08/24  |  0x00  | `uint8_t`                  | Set to 1 if this Trap contains a pre-trapped variant Villain (?)
|  0x0001  | 08/24  | 0x01   | `uint8_t`                  | Number of villains captured (?)
|  0x0007  | 08/24  |  0x07  | `kTfbSpyroTag_VillainType` | Variant Villain ID, used if he primary villain ID is 0 (is 0x00 used to determine if to do?) (locked to element)
|  0x0009  | 08/24  |  0x09  | `uint8_t`                  | [Area Sequence](#area-sequence)
|  0x000A  | 08/24  |  0x0C  | `uint16_t`                 | crc16-ccit/false checksum of 0x110 bytes from 0x40 (so blocks 0D/29 -> 23/3F excluding access control blocks)
|  0x000C  | 08/24  |  0x0A  | `uint16_t`                 | crc16-ccit/false checksum of 0x30 bytes starting from 0x10 (so blocks 09/25 -> 0C/28 excluding access control blocks)
|  0x000E  | 08/24  |  0x0E  | `uint16_t`                 | crc16-ccit/false checksum of the first 14 bytes of this struct + the bytes "05 00" at the end
|  0x0010  | 09/25  |  0x00  | `kTfbSpyroTag_VillainType` | ID of the currently trapped villain, always checked first (not locked to element) (See [kTfbSpyroTag_VillainType.hpp](../include//kTfbSpyroTag_VillainType.hpp))
| 0x0011   | 09/25  |  0x01  | `uint8_t`                  | Whether or not the villain is evolved (set to 1 if so)
| 0x0012   | 09/25  | 0x02   | `uint8_t`                  | Villain Hat value
| 0x0013   | 09/25  | 0x03   | `uint8_t`                  | [Villain Trinket value](#trinket-value)
| 0x0014   | 09/25  | 0x04   | `wchar_t[6]`               | First 12 bytes of villain nickname
|  0x0020  | 0A/26  |  0x00  | `wchar_t[8]`               | Next 16 bytes of villain nickname
|  0x0030  | 0C/28  |  0x00  | `wchar_t[2]`               | Remaining 4 bytes of villain nickname
|  0x0040  | 0D/29  |  0x00  | `kTfbSpyroTag_VillainType` | ID of the second trapped villain (villain data is "cached" after being replaced, this is the first one of these)
| 0x0041   | 0D/29  |  0x01  | `uint8_t`                  | Whether or not the second villain is evolved (set to 1 if so)
| 0x0042   | 0D/29  | 0x02   | `uint8_t`                  | Second Villain Hat value
| 0x0043   | 0D/29  | 0x03   | `uint8_t`                  | [Second Villain Trinket value](#trinket-value)
| 0x0044   | 0D/29  | 0x04   | `wchar_t[6]`               | First 12 bytes of second villain nickname
|  0x0050  | 0E/2A  |  0x00  | `wchar_t[8]`               | Next 16 bytes of second villain nickname
|  0x0060  | 10/2C  |  0x00  | `wchar_t[2]`               | Remaining 4 bytes of second villain nickname
|  0x0070  | 11/2D  |  0x00  | `kTfbSpyroTag_VillainType` | ID of the third trapped villain (villain data is "cached" after being replaced, this is the second one of these)
| 0x0071   | 11/2D  |  0x01  | `uint8_t`                  | Whether or not the third villain is evolved (set to 1 if so)
| 0x0072   | 11/2D  | 0x02   | `uint8_t`                  | Third Villain Hat value
| 0x0073   | 11/2D  | 0x03   | `uint8_t`                  | [Third Villain Trinket value](#trinket-value)
| 0x0074   | 11/2D  | 0x04   | `wchar_t[6]`               | First 12 bytes of third villain nickname
|  0x0080  | 12/2E  |  0x00  | `wchar_t[8]`               | Next 16 bytes of third villain nickname
|  0x0090  | 14/31  |  0x00  | `wchar_t[2]`               | Remaining 4 bytes of third villain nickname
|  0x00A0  | 15/32  |  0x00  | `kTfbSpyroTag_VillainType` | ID of the fourth trapped villain (villain data is "cached" after being replaced, this is the third one of these)
| 0x00A1   | 15/32  |  0x01  | `uint8_t`                  | Whether or not the fourth villain is evolved (set to 1 if so)
| 0x00A2   | 15/32  | 0x02   | `uint8_t`                  | Fourth Villain Hat value
| 0x00A3   | 15/32  | 0x03   | `uint8_t`                  | [Fourth Villain Trinket value](#trinket-value)
| 0x00A4   | 15/32  | 0x04   | `wchar_t[6]`               | First 12 bytes of fourth villain nickname
|  0x00B0  | 16/33  |  0x00  | `wchar_t[8]`               | Next 16 bytes of fourth villain nickname
|  0x00C0  | 18/35  |  0x00  | `wchar_t[2]`               | Remaining 4 bytes of fourth villain nickname
|  0x00D0  | 19/35  |  0x00  | `kTfbSpyroTag_VillainType` | ID of the fifth trapped villain (villain data is "cached" after being replaced, this is the fourth one of these)
| 0x00D1   | 19/35  |  0x01  | `uint8_t`                  | Whether or not the fifth villain is evolved (set to 1 if so)
| 0x00D2   | 19/35  | 0x02   | `uint8_t`                  | Fifth Villain Hat value
| 0x00D3   | 19/35  | 0x03   | `uint8_t`                  | [Fifth Villain Trinket value](#trinket-value)
| 0x00D4   | 19/35  | 0x04   | `wchar_t[6]`               | First 12 bytes of fifth villain nickname
|  0x00E0  | 1A/36  |  0x00  | `wchar_t[8]`               | Next 16 bytes of fifth villain nickname
|  0x00F0  | 1C/38  |  0x00  | `wchar_t[2]`               | Remaining 4 bytes of fifth villain nickname
|  0x0100  | 1D/39  |  0x00  | `kTfbSpyroTag_VillainType` | ID of the sixth trapped villain (villain data is "cached" after being replaced, this is the fifth one of these)
| 0x0101   | 1D/39  |  0x01  | `uint8_t`                  | Whether or not the sixth villain is evolved (set to 1 if so)
| 0x0102   | 1D/39  | 0x02   | `uint8_t`                  | Sixth Villain Hat value
| 0x0103   | 1D/39  | 0x03   | `uint8_t`                  | [Sixth Villain Trinket value](#trinket-value)
| 0x0104   | 1D/39  | 0x04   | `wchar_t[6]`               | First 12 bytes of sixth villain nickname
|  0x0110  | 1E/3A  |  0x00  | `wchar_t[8]`               | Next 16 bytes of sixth villain nickname
|  0x0120  | 1F/3B  |  0x00  | `wchar_t[2]`               | Remaining 4 bytes of sixth villain nickname
|  0x0130  | 21/3D  |  0x00  | `uint8_[9]`                | Something related to time spent on what platforms

### Racing Pack

| St_Off | Block  | Bl_Off | Type                   | Description
|--------|--------|--------|------------------------|---------------
|  0x10  | 09/25  |  0x04  | `uint16_t`             | [Captured Trophy villains](#captured-trophy-villains)

### Vehicle

The full purpose of "Last platform identifier" bytes are quite unknown. When the figure is written to by Skylanders SuperChargers Racing, all of these are set to 00. When the figure is written to by Skylanders SuperChargers (mainline), all of these a`re set to 0F.

| St_Off | Block  | Bl_Off | Type                   | Description
|--------|--------|--------|------------------------|---------------
|  0x00  | 08/24  |  0x00  | `uint16_t`             | SSCR [Vehicle Experience](#experience)
|  0x05  | 08/24  |  0x05  | `uint32_t`             | Cumulative time in seconds
|  0x09  | 08/24  |  0x09  | `uint8_t`              | [Area Sequence](#area-sequence)
|  0x0A  | 08/24  |  0x0A  | `uint16_t`             | crc16-ccit/false checksum of 0x30 bytes starting from 0x40, followed by 0xE0 bytes of 0 (so blocks 0D/29 -> 10/2C excluding access control blocks)
|  0x0C  | 08/24  |  0x0C  | `uint16_t`             | crc16-ccit/false checksum of 0x30 bytes starting from 0x10 (so blocks 09/25 -> 0C/28 excluding access control blocks)
|  0x0E  | 08/24  |  0x0E  | `uint16_t`             | crc16-ccit/false checksum of the first 14 bytes of this struct + the bytes "05 00" at the end
|  0x10  | 09/25  |  0x00  | `uint24_t`             | [Vehicle Flags](#vehicle-flags)
|  0x13  | 09/25  |  0x03  | `uint8_t`              | 2011 [Platform bitfield](#platform-bitfield)
|  0x14  | 09/25  |  0x04  | `uint16_t`             | Last platform identifier
|  0x16  | 09/25  |  0x06  | `uint8_t`              | `(1 << (dataRegionCount - 1)) - 1`. Since `dataRegionCount` is always set to 2 on core figures, this always evaluates to 1
|  0x17  | 09/25  |  0x07  | `uint8_t`              | 2013 [Platform bitfield](#platform-bitfield)
|  0x18  | 09/25  |  0x08  | `uint8_t`              | [Vehicle Decoration](#vehicle-decorationneon)
|  0x19  | 09/25  |  0x09  | `uint8_t`              | [Vehicle Topper](#vehicle-topper)
|  0x1A  | 09/25  |  0x0A  | `uint8_t`              | [Vehicle Neon](#vehicle-decorationneon)
|  0x1B  | 09/25  |  0x0B  | `uint8_t`              | [Vehicle Shout](#vehicle-shout)
|  0x3E  | 0C/28  |  0x0E  | `uint16_t`             | [ModFlags](#vehicle-mod-flags)
|  0x40  | 0D/29  |  0x00  | `uint8_t`              | Minute value of the last time this figure was placed on the portal
|  0x41  | 0D/29  |  0x01  | `uint8_t`              | Hour value of the last time this figure was placed on the portal
|  0x42  | 0D/29  |  0x02  | `uint8_t`              | Day value of the last time this figure was placed on the portal
|  0x43  | 0D/29  |  0x03  | `uint8_t`              | Month value of the last time this figure was placed on the portal
|  0x44  | 0D/29  |  0x04  | `uint16_t`             | Year value of the last time this figure was placed on the portal
|  0x4F  | 0D/29  |  0x0F  | `uint8_t`              | Owner count(?)
|  0x50  | 0E/2A  |  0x00  | `uint8_t`              | Minute value of the last time this figure was reset (if never reset then the first time they were placed on a portal)
|  0x51  | 0E/2A  |  0x01  | `uint8_t`              | Hour value of the last time this figure was reset (if never reset then the first time they were placed on a portal)
|  0x52  | 0E/2A  |  0x02  | `uint8_t`              | Day value of the last time this figure was reset (if never reset then the first time they were placed on a portal)
|  0x53  | 0E/2A  |  0x03  | `uint8_t`              | Month value of the last time this figure was reset (if never reset then the first time they were placed on a portal)
|  0x54  | 0E/2A  |  0x04  | `uint16_t`             | Year value of the last time this figure was reset (if never reset then the first time they were placed on a portal)
|  0x60  | 10/2C  |  0x00  | `uint8_[9]`            | Something related to time spent on what platforms
|  0x70  | 11/2D  |  0x00  | `uint16_t`             | crc16-ccit/false checksum of the bytes "06 01" followed by 0x3E bytes from 0x72
|  0x72  | 11/2D  |  0x02  | `uint8_t`              | Area sequence for this data area
|  0x73  | 11/2D  |  0x03  | `uint8_t`              | Last platform identifer
|  0x76  | 11/2D  |  0x06  | `uint8_t`              | Last platform identifer
|  0x78  | 11/2D  |  0x08  | `uint16_t`             | Gearbits

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
|  0x09  | 08/24  |  0x09  | `uint8_t`              | [Area Sequence](#area-sequence)
|  0x0A  | 08/24  |  0x0A  | `uint16_t`             | crc16-ccit/false checksum of 0x30 bytes starting from 0x40, followed by 0xE0 bytes of 0 (so blocks 0D/29 -> 10/2C excluding access control blocks)
|  0x0C  | 08/24  |  0x0C  | `uint16_t`             | crc16-ccit/false checksum of 0x30 bytes starting from 0x10 (so blocks 09/25 -> 0C/28 excluding access control blocks)
|  0x0E  | 08/24  |  0x0E  | `uint16_t`             | crc16-ccit/false checksum of the first 14 bytes of this struct + the bytes "05 00" at the end
|  0x10  | 09/25  |  0x00  | `uint24_t`             | [Flags1](#flags)
|  0x13  | 09/25  |  0x03  | `uint8_t`              | 2011 [Platform bitfield](#platform-bitfield)
|  0x14  | 09/25  |  0x04  | `uint16_t`             | 2011 [Hat value](#hat-value)
|  0x16  | 09/25  |  0x06  | `uint8_t`              | `(1 << (dataRegionCount - 1)) - 1`. Since `dataRegionCount` is always set to 2 on core figures, this always evaluates to 1
|  0x17  | 09/25  |  0x07  | `uint8_t`              | 2013 [Platform bitfield](#platform-bitfield)
|  0x20  | 0A/26  |  0x00  | `wchar_t[8]`           | First 16 bytes of nickname
|  0x30  | 0C/28  |  0x00  | `wchar_t[8]`           | Last 16 bytes of nickname
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
|  0x60  | 10/2C  |  0x00  | `uint8_[9]`            | [Owner usage info](#owner-usage-info)
|  0x70  | 11/2D  |  0x00  | `uint16_t`             | crc16-ccit/false checksum of the bytes "06 01" followed by 0x3E bytes from 0x72
|  0x72  | 11/2D  |  0x02  | `uint8_t`              | Area sequence for this data area
|  0x73  | 11/2D  |  0x03  | `uint16_t`             | 2012 [Experience](#experience) value (Max is 63500 for ssf characters onwards, for ssa and giants it's 65535)
|  0x75  | 11/2D  |  0x05  | `uint8_t`              | 2012 [Hat value](#hat-value)
|  0x76  | 11/2D  |  0x06  | `uint16_t`             | [Flags2](#flags)
|  0x78  | 11/2D  |  0x08  | `uint32_t`             | 2013 [Experience](#experience) value (Max is 101000)
|  0x7C  | 11/2D  |  0x0C  | `uint8_t`              | 2013/2014 [Hat value](#hat-value)
|  0x7D  | 11/2D  |  0x0D  | `uint8_t`              | [Trinket value](#trinket-value)
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

Skylanders SuperChargers Racing vehicle experience

| Level | Experience |
|-------|------------|
| 1     | 0          |
| 2     | 1000       |
| 3     | 2000 (?)   |

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
|  0007  | Propeller Cap
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

### Trinket Value

| ID | Trinket Name
|----|------------------
| 00 | No Trinket
| 01 | T-Bone's Lucky Tie
| 02 | Batterson's Bubble
| 03 | Dark Water Daisy
| 04 | Vote for Cyclops
| 05 | Ramses' Dragon Horn
| 06 | Iris' Iris
| 07 | Kuckoo Kazoo
| 08 | Ramses' Rune
| 09 | Ullysses Uniclops
| 0A | Billy Bison
| 0B | Stealth Elf's Gift
| 0C | Lizard Lilly
| 0D | Pirate Pinwheel
| 0E | Bubble Blower
| 0F | Medal of Heroism
| 10 | Blobber's Medal of Courage
| 11 | Medal of Valiance
| 12 | Medal of Gallantry
| 13 | Medal of Mettle
| 14 | Winged Medal of Bravery
| 15 | Seadog Seashell
| 16 | Snuckles' Sunflower
| 17 | Teddy Cyclops
| 18 | Goo Factory Gear
| 19 | Elemental Opal
| 1A | Elemental Radiant
| 1B | Elemental Diamond
| 1C | Cyclops Spinner
| 1D | Wilikin Windmill
| 1E | Time Town Ticker
| 1F | Big Bow of Boom
| 20 | Mabu's Medallion
| 21 | Spyro's Shield	

### Platform bitfield

2011 value:

* Bit 0: Wii
* Bit 1: Xbox 360
* Bit 2: PS3
* Bit 3: PC (Maybe MacOS as well)
* Bit 4: Nintendo 3DS

2013 value:

* Bit 0: Android 32 (Maybe Android 64 as well)
* Bit 1: Xbox One
* Bit 2: PS4
* Bit 3: iOS 64 (Maybe iOS 32 as well)

So for example, if the 2011 value is set to 3, then bits 0 and 1 are set, and therefore the figure has been used on Wii and Xbox 360.

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

### Owner usage info

Still quite unknown, but seems to have a pattern. Although I've seen it take up 16 bytes, not just the 9 bytes mentioned beforehand.

How it is laid of as a whole doesn't seem clear, but somewhere it'd presumably store an identifier for a specific instance of a game that has used the toy (doesn't seem to be ownership however, just placed down). Following this is some sort of `uint16_t` (?) value to indicate how much times the figure's data regions have been modified - appearing to to increment whenever the [Area Sequence](#area-sequence) bytes are updated (which dictates that the region was written to/modified). If both data area sequences are updated in one write, it'll increment by two instead. As I said, the layout of this is quite unknown.

### Heroic Challenges

* Heroic Challenges are sprawled out over 2 different areas. One `uint32_t` at block 0x0d/0x29, offset 0x06, and another `uint24_t` at block 0x12/0x2e, offset 0x04.
* The first one of these accords to the Heroic Challenges in Spyro's Adventure, and the second one the Heroic Challenges introduced in Giants.
* Each bit corresponds to a boolean whether or not the heroic challenge has been complete by the Skylander.
* Their offset into the bit field is the same as their internally stored identifier. For Giants Heroics, add 32 to their offset to reveal the true heroic ID. This essentially means that if you append the Giants heroic bytes to the start of the SSA heroic bytes, then using their ID would give you the corresponding bit
* Assume `SSA` refers to the 4 bytes for SSA heroics, and `SG` for the other 3 bytes

| Shift           | ID | Description
|-----------------|----|---------------------------
| `SSA >> 0x00`   | 0  | Chompy Chomp Down
| `SSA >> 0x01`   | 1  | This Bomb's For You
| `SSA >> 0x02`   | 2  | Jump For It!
| `SSA >> 0x03`   | 3  | Where Art Thou, Paintings
| `SSA >> 0x04`   | 4  | Lair of the Giant Spiders
| `SSA >> 0x05`   | 5  | Fight, Teleport, Fight!
| `SSA >> 0x06`   | 6  | The Three Teleporters
| `SSA >> 0x07`   | 7  | Stop, Sheep Thieves!
| `SSA >> 0x08`   | 8  | Mining for Charms
| `SSA >> 0x09`   | 9  | Dungeoness Creeps
| `SSA >> 0x0A`   | 10 | Mining is the Key
| `SSA >> 0x0B`   | 11 | Mission Achomplished
| `SSA >> 0x0C`   | 12 | Pod Gauntlet
| `SSA >> 0x0D`   | 13 | Time's A-Wastin'
| `SSA >> 0x0E`   | 14 | Save the Purple Chompies!
| `SSA >> 0x0F`   | 15 | Spawner Cave
| `SSA >> 0x10`   | 16 | Arachnid Antechamber
| `SSA >> 0x11`   | 17 | Hobson's Choice
| `SSA >> 0x12`   | 18 | Isle of the Automatons
| `SSA >> 0x13`   | 19 | You Break It, You Buy It!
| `SSA >> 0x14`   | 20 | Minefield Mishap
| `SSA >> 0x15`   | 21 | Lobs O' Fun
| `SSA >> 0x16`   | 22 | Spell Punked!
| `SSA >> 0x17`   | 23 | Charm Hunt
| `SSA >> 0x18`   | 24 | Flip the Script
| `SSA >> 0x19`   | 25 | You've Stolen My Hearts!
| `SSA >> 0x1A`   | 26 | Bombs to the Walls
| `SSA >> 0x1B`   | 27 | Operation: Sheep Freedom
| `SSA >> 0x1C`   | 28 | Jailbreak!
| `SSA >> 0x1D`   | 29 | Environmentally Unfriendly
| `SSA >> 0x1E`   | 30 | Chemical Cleanup
| `SSA >> 0x1F`   | 31 | Break the Cats
| `SG >> 0x00`    | 32 | Flame Pirates on Ice
| `SG >> 0x01`    | 33 | Skylands Salute
| `SG >> 0x02`    | 34 | S.A.B.R.I.N.A
| `SG >> 0x03`    | 35 | The Sky is Falling
| `SG >> 0x05`    | 37 | Break the Fakes!
| `SG >> 0x06`    | 38 | Baking with Batterson
| `SG >> 0x07`    | 39 | Blobber's Folly
| `SG >> 0x08`    | 40 | Unused Heroic 40
| `SG >> 0x09`    | 41 | Unused Heroic 41
| `SG >> 0x0A`    | 42 | Unused Heroic 42
| `SG >> 0x0B`    | 43 | Unused Heroic 43
| `SG >> 0x0C`    | 44 | Unused Heroic 44
| `SG >> 0x0D`    | 45 | Unused Heroic 45
| `SG >> 0x0E`    | 46 | Delivery Day
| `SG >> 0x0F`    | 47 | Give a Hoot
| `SG >> 0x10`    | 48 | Zombie Dance Party
| `SG >> 0x11`    | 49 | Shepherd's Pie
| `SG >> 0x12`    | 50 | Watermelon's Eleven
| `SG >> 0x13`    | 51 | A Real Goat-Getter
| `SG >> 0x14`    | 52 | Wooly Bullies
| `SG >> 0x15`    | 53 | The Great Pancake Slalom
| `SG >> 0x16`    | 54 | Shoot First, Shoot Later
| `SG >> 0x17`    | 55 | The King's Breech

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

### Captured Trophy Villains

16 bit long (?) bit field field that represents which villains has been unlocked within the cup. Note that none of this exists for the Kaos Trophy; he is unlocked just by reading the figure on the Portal.

Road Racers (Land villains)
* Bit 0: Glumshanks in Steam Roller
* Bit 1: Dragon Hunter in Scale Biter
* Bit 2: Moneybone in Spirit Dragster
* Bit 3: Chompy Mage in Chompy Buster
* Bit 4: Dr. Krankcase in unnamed vehicle (SSCR only)

Wave Runners (Sea villains)
* Bit 0: Mesmeralda in Wave Singer
* Bit 1: Captain Frightbeard in The Lil' Phantom Tide
* Bit 2: Golden Queen in Glitter Glider
* Bit 3: Spellslamzer in Rune Slider
* Bit 4: The Gulper in unnamed vehicle (SSCR only)

Sky Captains (Sky villains)
* Bit 0: Chef Pepper Jack in Toaster Bomber
* Bit 1: Stratosphere in Storm Striker
* Bit 2: Cluck in Sky Scrambler
* Bit 3: Wolfgang in Sub Woofer
* Bit 4: Pain-Yatta in unnamed vehicle (SSCR only)

### Vehicle Flags

Oddly, each level of the vehicle's shield and weapon occupies it's own bit, even though it's impossible normally to purchase a level and skip one. This spans over a `uint16_t`.

* Bit 0: Shield level 1 purchased
* Bit 1: Shield level 2 purchased
* Bit 2: Shield level 3 purchased
* Bit 3: Shield level 4 purchased
* Bit 4: Shield level 5 purchased
* Bit 5: Weapon level 1 purchased
* Bit 6: Weapon level 2 purchased
* Bit 7: Weapon level 3 purchased
* Byte 1, Bit 0: Weapon level 4 purchased
* Byte 1, Bit 1: Weapon level 5 purchased

### Vehicle Decoration/Neon

| ID | Deco/Neon Name
|----|----------------
| 00 | None
| 01 | Darkness
| 02 | Cap'N Cluck
| 03 | Ancient
| 04 | Cartoon
| 05 | Eon
| 06 | Kaos
| 07 | Police
| 08 | Construction
| 09 | Holiday
| 0A | Ghost
| 0B | Thermal
| 0C | Fire Truck
| 0D | Ninja
| 0E | Royal
| 0F | Robot

### Vehicle Topper

| ID  | Topper Name
|-----|----------------
| 00  | None
| 01  | The Darkness
| 02  | Lucky Coin
| 03  | King-Sized Bucket
| 04  | Popcorn
| 05  | Chicken Leg
| 06  | Pinata
| 07  | Bag of Gold
| 08  | Chompy
| 09  | Balloon
| 0A  | Ripe Banana
| 0B  | Beach Ball
| 0C  | Teddy Hat
| 0D  | Corn on the Cob
| 0E  | Dragonfire Cannon
| 0F  | Eon's Sock
| 10  | Eon's Statue
| 11  | Kaos Statue
| 12  | Spitfire Doll
| 13  | Golden Piggy Bank
| 14  | Raccoon Tail
| 15  | Rasta Hat
| 16  | Party Sheep
| 17  | Snap Shot Doll
| 18  | Space Helmet
| 19  | Squeeks Jr.
| 1A  | Tiki Speaky
| 1B  | Traffic Cone
| 1C  | Tree Rex Doll
| 1D  | Tricorn Hat
| 1E  | Trigger Happy Doll
| 1F  | Wash Buckler Doll
| 20  | Weathervane
| 21  | Eon's Helm
| 22  | Pluck
| 23  | Siren
| 24  | Ghost Topper
| 25  | Cartoon Doll
| 26  | Kaos Punching Bag
| 27  | Cup O' Cocoa
| 28  | Hand of Fate
| 29  | Like Clockwork
| 2A  | Empire of Ice
| 2B  | Pizza
| 2C  | Yeti Doll
| 2D  | Kaos Sigil
| 2E  | Cowboy Hat
| 2F  | Eyeball Ball
| 30  | Asteroid
| 31  | Hook Hand
| 32  | The Mighty Atom
| 33  | Holiday Tree
| 34  | Shuriken
| 35  | Mechanical Gear
| 36  | Royal Crown
| 37  | Fire Hydrant


### Vehicle Shout

| ID  | Name
|-----|-------------------
| 00  | None
| 01  | Sneer: Cali
| 02  | Jeer: Cali
| 03  | Cheer: Cali
| 04  | Back off Bear
| 05  | Breakdown
| 06  | Pull Over!
| 07  | Evil Eye
| 08  | Bird Brain
| 09  | The Ultimate Evil!
| 0A  | Leave Me Alone Lion
| 0B  | Going Nuclear
| 0C  | Sneer: Sharpfin
| 0D  | The Darkness
| 0E  | Why I Oughta
| 0F  | Police Siren
| 10  | Fire It Up
| 11  | Sneer: Buzz
| 12  | Call Me!
| 13  | Car Trouble
| 14  | Sneer: Pomfrey
| 15  | Yield!
| 16  | Hype Train
| 17  | Doggin' After Ya
| 18  | Crash and Burn
| 19  | Earthquake
| 1A  | Flat Tire
| 1B  | Fly Trap
| 1C  | Sneer: Glumshanks
| 1D  | Sneer: Hugo
| 1E  | Sneer: Queen Cumulus
| 1F  | Ninja Stars
| 20  | AAAAAA…
| 21  | Jeer: Sharpfin
| 22  | Red Means Go Right?
| 23  | The Final Countdown
| 24  | Rush Hour
| 25  | Sneer: Tessa
| 26  | Tidal Wave
| 27  | Toasty!
| 28  | All Spun Up
| 29  | Under Construction
| 2A  | Howlin' Good
| 2B  | Cheer: Buzz
| 2C  | Cheer: Pomfrey
| 2D  | Checkered Flag
| 2E  | Eon Impersonator
| 2F  | Cheer: Flynn
| 30  | Cheer: Glumshanks
| 31  | Wink Wink Nudge Nudge
| 32  | Silver Bells
| 33  | Cheer: Queen Cumulus
| 34  | Cheer: Persephone
| 35  | Cheer: Sharpfin
| 36  | Cheer: Hugo
| 37  | :)
| 38  | Cheer: Tessa
| 39  | First Place Trophy
| 3A  | Big Bell
| 3B  | Rude Chompy
| 3C  | Your Robot Son
| 3D  | Cry Baby
| 3E  | The Gulper
| 3F  | Sweet Innocence
| 40  | Diplomacy
| 41  | The Prince of Pontification
| 42  | Scandalous!
| 43  | Like Clockwork
| 44  | Ancient Energy
| 45  | Banana Peel
| 46  | Bashful Face
| 47  | Boo
| 48  | Boo Too
| 49  | Jeer: Buzz
| 4A  | Catchy Jingle
| 4B  | Jeer: Pomfrey
| 4C  | Laugh It Up
| 4D  | Cow Crossing
| 4E  | Cuckoo Cuckoo
| 4F  | Rude Dolphin
| 50  | Jack the Donkey
| 51  | Quack!
| 52  | Trumpet Trunk
| 53  | Blub-Blub
| 54  | Jeer: Flynn
| 55  | Jeer: Glumshanks
| 56  | Indignant Goose
| 57  | Ham!
| 58  | Horsin' Around
| 59  | Jeer: Hugo
| 5A  | Kissy Face
| 5B  | Purrfect
| 5C  | Lockpick Gremlin
| 5D  | Nature Calls
| 5E  | Jeer: Queen Cumulus
| 5F  | Oop Oop Eek
| 60  | Tauntalizing
| 61  | Soda Pop
| 62  | Wow!
| 63  | Baa-Aaa!
| 64  | Squeaky Toy
| 65  | Jeer: Tessa

### Vehicle Mod Flags

For the first byte, the lower nibble (bits 0-3) represent an integer of the currently equipped performance mod. The upper nibble (bits 4-7) represent an integer of the currently equipped specialty mod. For both, 0 is the default, 1 is the first mod, 2 is the second mod, and 3 is the SuperCharged mod.

The second byte is essentially the same thing, but just for the currently equipped horn. 0 for default etc...

* Performance Mod: `ModFlags & 0xF`
* Specialty Mod: `(ModFlags >> 4) & 0xF`
* Horn: `(ModFlags >> 8) & 0xF`

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
  * Figured out trinkets
* Texthead:
  * Variant ID additions/corrections
  * Additional Trap info
  * Figured out Vehicles
  * Racing pack additions/corrections
  * The PC, PC, Nintendo 3DS, Xbox One, and PS4 values for the platform usage
  * Heroic challenges
