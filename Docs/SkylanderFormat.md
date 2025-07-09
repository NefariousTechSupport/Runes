# Skylander Format

## tfbSpyroTag_TagHeader

The header is 0x20 bytes long.

| Offset | Type                   | Description
|--------|------------------------|---------------
|  0000  | `uint32_t`             | Non-Unique Identifier for this toy, internally referred to as the serial number
|  0010  | `kTfbSpyroTag_ToyType` | (24 bit int) The Character ID of this Skylander (see [kTfbSpyroTag_ToyType.hpp](../source/kTfbSpyroTag_ToyType.hpp))
|  0013  | `uint8_t`              | [Error byte](#error-byte)
|  0014  | `uint64_t`             | The Trading Card ID, [Web Code](#web-code) is derived from this, internally this is separated into 2 `uint32_t`s, presumably to get around alignment issues
|  001C  | `uint16_t`             | The Variant ID of this Skylander (see [here](#variant-id) to understand how this works)
|  001E  | `uint16_t`             | The crc16-ccitt/false checksum for the first 0x1E bytes of the header

### Variant ID

The Variant ID is a 16 bit long bit field. Note that with the way some tags were programmed, some do not fit with the criteria perfectly. Additionally, the flag that determines if the Skylander is a SuperCharger is never acted upon by the games and is essentially meaningless.

| Shift | Mask | Type                  | Description
|-------|------|-----------------------|-------------
| 0x00  | 00FF | `kTfbSpyroTag_DecoID` | The [Deco ID](../source/kTfbSpyroTag_DecoID.hpp)
| 0x08  | 0001 | `bool`                | Whether or not this Skylander is a SuperCharger
| 0x09  | 0001 | `bool`                | Whether or not this Skylander possesses LightCore technology; includes LightCores, Giants, Battle Pieces, certain TT expansions, and Creation Crystals
| 0x0A  | 0001 | `bool`                | Whether or not this Skylander is an in-game variant
| 0x0B  | 0001 | `bool`                | Whether or not this Skylander is reposed, normally meaning it has a Wow Pow (decided from the year code)
| 0x0C  | 000F | `ESkylandersGame`     | [Year code](../source/ESkylandersGame.hpp)

### Web Code

The Web Code is derived from the Trading Card ID.
Similar to how we have base-16 or base-2 for hexadecimal and binary respectively, the web code is actually stored as base-29.
By this, I mean that, you can modulo the Trading Card ID by 29, and then put it through a lookup table to get the last digit of the web code,
you can then divide by 29 and repeat to get the second to last digit.
The lookup table consists of the characters `23456789BCDFGHJKLMNPQRSTVWXYZ` in that order, such that the first item, item 0, is `2` and the last character, item 28, is `Z`.

Moreover, you can look at `Runes::PortalTag::StoreHeader()` [here](../source/PortalTag.cpp) for easier to read code.

### Trap

* The 0x40 bytes from offset 0x00 is the Magic Moment Data (encompasses the actual trapped villain).
* The 0x110 bytes from 0x40 is the Remaining Data (contains caches of previously trapped villains).

|  St_Off  | Block  | Bl_Off | Type                       | Description
|----------|--------|--------|----------------------------|---------------
|  0x0000  | 08/24  |  0x00  | `uint8_t`                  | Set to 1 if this Trap contains a pre-trapped variant villain, but not required for it to load
|  0x0001  | 08/24  |  0x01  | `uint8_t`                  | Number of unique villains captured/stored
|  0x0007  | 08/24  |  0x07  | `kTfbSpyroTag_VillainType` | Which villain to load as its variant. In SSC, used if the primary villain is 0x00, and only in SSC must be element locked
|  0x0009  | 08/24  |  0x09  | `uint8_t`                  | [Area Sequence](#area-sequence)
|  0x000A  | 08/24  |  0x0C  | `uint16_t`                 | crc16-ccitt/false checksum of 0x110 bytes from 0x40 (so blocks 0D/29 -> 23/3F excluding access control blocks)
|  0x000C  | 08/24  |  0x0A  | `uint16_t`                 | crc16-ccitt/false checksum of 0x30 bytes starting from 0x10 (so blocks 09/25 -> 0C/28 excluding access control blocks)
|  0x000E  | 08/24  |  0x0E  | `uint16_t`                 | crc16-ccitt/false checksum of the first 14 bytes of this struct + the bytes "05 00" at the end
|  0x0010  | 09/25  |  0x00  | `kTfbSpyroTag_VillainType` | ID of the currently trapped villain (See [kTfbSpyroTag_VillainType.hpp](../source/kTfbSpyroTag_VillainType.hpp)). Note that any villain can be put in any trap
|  0x0011  | 09/25  |  0x01  | `uint8_t`                  | Whether or not the villain is evolved (set to 1 if so)
|  0x0012  | 09/25  |  0x02  | `uint8_t`                  | Villain Hat value
|  0x0013  | 09/25  |  0x03  | `uint8_t`                  | [Villain Trinket value](#trinket-value)
|  0x0014  | 09/25  |  0x04  | `wchar_t[6]`               | First 12 bytes of villain nickname
|  0x0020  | 0A/26  |  0x00  | `wchar_t[8]`               | Next 16 bytes of villain nickname
|  0x0030  | 0C/28  |  0x00  | `wchar_t[2]`               | Remaining 4 bytes of villain nickname
|  0x0040  | 0D/29  |  0x00  | `kTfbSpyroTag_VillainType` | ID of the second trapped villain (villain data is "cached" after being replaced, this is the first one of these)
|  0x0041  | 0D/29  |  0x01  | `uint8_t`                  | Whether or not the second villain is evolved (set to 1 if so)
|  0x0042  | 0D/29  |  0x02  | `uint8_t`                  | Second villain Hat value
|  0x0043  | 0D/29  |  0x03  | `uint8_t`                  | [Second villain Trinket value](#trinket-value)
|  0x0044  | 0D/29  |  0x04  | `wchar_t[6]`               | First 12 bytes of second villain nickname
|  0x0050  | 0E/2A  |  0x00  | `wchar_t[8]`               | Next 16 bytes of second villain nickname
|  0x0060  | 10/2C  |  0x00  | `wchar_t[2]`               | Remaining 4 bytes of second villain nickname
|  0x0070  | 11/2D  |  0x00  | `kTfbSpyroTag_VillainType` | ID of the third trapped villain (villain data is "cached" after being replaced, this is the second one of these)
|  0x0071  | 11/2D  |  0x01  | `uint8_t`                  | Whether or not the third villain is evolved (set to 1 if so)
|  0x0072  | 11/2D  |  0x02  | `uint8_t`                  | Third villain Hat value
|  0x0073  | 11/2D  |  0x03  | `uint8_t`                  | [Third villain Trinket value](#trinket-value)
|  0x0074  | 11/2D  |  0x04  | `wchar_t[6]`               | First 12 bytes of third villain nickname
|  0x0080  | 12/2E  |  0x00  | `wchar_t[8]`               | Next 16 bytes of third villain nickname
|  0x0090  | 14/31  |  0x00  | `wchar_t[2]`               | Remaining 4 bytes of third villain nickname
|  0x00A0  | 15/32  |  0x00  | `kTfbSpyroTag_VillainType` | ID of the fourth trapped villain (villain data is "cached" after being replaced, this is the third one of these)
|  0x00A1  | 15/32  |  0x01  | `uint8_t`                  | Whether or not the fourth villain is evolved (set to 1 if so)
|  0x00A2  | 15/32  |  0x02  | `uint8_t`                  | Fourth villain Hat value
|  0x00A3  | 15/32  |  0x03  | `uint8_t`                  | [Fourth villain Trinket value](#trinket-value)
|  0x00A4  | 15/32  |  0x04  | `wchar_t[6]`               | First 12 bytes of fourth villain nickname
|  0x00B0  | 16/33  |  0x00  | `wchar_t[8]`               | Next 16 bytes of fourth villain nickname
|  0x00C0  | 18/35  |  0x00  | `wchar_t[2]`               | Remaining 4 bytes of fourth villain nickname
|  0x00D0  | 19/35  |  0x00  | `kTfbSpyroTag_VillainType` | ID of the fifth trapped villain (villain data is "cached" after being replaced, this is the fourth one of these)
|  0x00D1  | 19/35  |  0x01  | `uint8_t`                  | Whether or not the fifth villain is evolved (set to 1 if so)
|  0x00D2  | 19/35  |  0x02  | `uint8_t`                  | Fifth villain Hat value
|  0x00D3  | 19/35  |  0x03  | `uint8_t`                  | [Fifth villain Trinket value](#trinket-value)
|  0x00D4  | 19/35  |  0x04  | `wchar_t[6]`               | First 12 bytes of fifth villain nickname
|  0x00E0  | 1A/36  |  0x00  | `wchar_t[8]`               | Next 16 bytes of fifth villain nickname
|  0x00F0  | 1C/38  |  0x00  | `wchar_t[2]`               | Remaining 4 bytes of fifth villain nickname
|  0x0100  | 1D/39  |  0x00  | `kTfbSpyroTag_VillainType` | ID of the sixth trapped villain (villain data is "cached" after being replaced, this is the fifth one of these)
|  0x0101  | 1D/39  |  0x01  | `uint8_t`                  | Whether or not the sixth villain is evolved (set to 1 if so)
|  0x0102  | 1D/39  |  0x02  | `uint8_t`                  | Sixth villain Hat value
|  0x0103  | 1D/39  |  0x03  | `uint8_t`                  | [Sixth villain Trinket value](#trinket-value)
|  0x0104  | 1D/39  |  0x04  | `wchar_t[6]`               | First 12 bytes of sixth villain nickname
|  0x0110  | 1E/3A  |  0x00  | `wchar_t[8]`               | Next 16 bytes of sixth villain nickname
|  0x0120  | 1F/3B  |  0x00  | `wchar_t[2]`               | Remaining 4 bytes of sixth villain nickname
|  0x0130  | 21/3D  |  0x00  | `uint8_t[15]`              | [Usage/owner info](#usage-info)

### Racing Pack

| St_Off | Block  | Bl_Off | Type                   | Description
|--------|--------|--------|------------------------|---------------
|  0x0A  | 08/24  |  0x0A  | `uint16_t`             | crc16-ccitt/false checksum of 0x30 bytes starting from 0x40, followed by 0xE0 bytes of 0 (so blocks 0D/29 -> 10/2C excluding access control blocks)
|  0x0C  | 08/24  |  0x0C  | `uint16_t`             | crc16-ccitt/false checksum of 0x30 bytes starting from 0x10 (so blocks 09/25 -> 0C/28 excluding access control blocks)
|  0x0E  | 08/24  |  0x0E  | `uint16_t`             | crc16-ccitt/false checksum of the first 14 bytes of this struct + the bytes "05 00" at the end
|  0x10  | 09/25  |  0x04  | `uint16_t`             | [Captured Trophy villains](#captured-trophy-villains)
|  0x70  | 11/2D  |  0x00  | `uint16_t`             | crc16-ccitt/false checksum of the bytes "06 01" followed by 0x3E bytes from 0x72

### Vehicle

| St_Off | Block  | Bl_Off | Type                   | Description
|--------|--------|--------|------------------------|---------------
|  0x00  | 08/24  |  0x00  | `uint24_t`             | SSCR [Vehicle Experience](#experience)
|  0x05  | 08/24  |  0x05  | `uint32_t`             | Cumulative time in seconds
|  0x09  | 08/24  |  0x09  | `uint8_t`              | [Area Sequence](#area-sequence)
|  0x0A  | 08/24  |  0x0A  | `uint16_t`             | crc16-ccitt/false checksum of 0x30 bytes starting from 0x40, followed by 0xE0 bytes of 0 (so blocks 0D/29 -> 10/2C excluding access control blocks)
|  0x0C  | 08/24  |  0x0C  | `uint16_t`             | crc16-ccitt/false checksum of 0x30 bytes starting from 0x10 (so blocks 09/25 -> 0C/28 excluding access control blocks)
|  0x0E  | 08/24  |  0x0E  | `uint16_t`             | crc16-ccitt/false checksum of the first 14 bytes of this struct + the bytes "05 00" at the end
|  0x10  | 09/25  |  0x00  | `uint24_t`             | [Vehicle Flags](#vehicle-flags)
|  0x13  | 09/25  |  0x03  | `uint8_t`              | 2011 [Platform bitfield](#platform-bitfield)
|  0x16  | 09/25  |  0x06  | `uint8_t`              | [Region Count Identifier](#region-count-identifier)
|  0x17  | 09/25  |  0x07  | `uint8_t`              | 2013 [Platform bitfield](#platform-bitfield)
|  0x18  | 09/25  |  0x08  | `uint8_t`              | [Vehicle Decoration](#vehicle-decoration)
|  0x19  | 09/25  |  0x09  | `uint8_t`              | [Vehicle Topper](#vehicle-topper)
|  0x1A  | 09/25  |  0x0A  | `uint8_t`              | [Vehicle Neon](#vehicle-neon)
|  0x1B  | 09/25  |  0x0B  | `uint8_t`              | [Vehicle Shout](#vehicle-shout)
|  0x3E  | 0C/28  |  0x0E  | `uint16_t`             | [ModFlags](#vehicle-mod-flags)
|  0x40  | 0D/29  |  0x00  | `uint8_t`              | Minute value of the last time this figure was placed on the portal
|  0x41  | 0D/29  |  0x01  | `uint8_t`              | Hour value of the last time this figure was placed on the portal
|  0x42  | 0D/29  |  0x02  | `uint8_t`              | Day value of the last time this figure was placed on the portal
|  0x43  | 0D/29  |  0x03  | `uint8_t`              | Month value of the last time this figure was placed on the portal
|  0x44  | 0D/29  |  0x04  | `uint16_t`             | Year value of the last time this figure was placed on the portal
|  0x4C  | 0D/29  |  0x0C  | `uint8_t[3]`           | Last game build diagnostics. First byte is the build year (from 2000); second byte is build month; third byte is build day
|  0x4F  | 0D/29  |  0x0F  | `uint8_t`              | No. times owner changed. Normally increments by 1 when ownership is changed (capped at 255)
|  0x50  | 0E/2A  |  0x00  | `uint8_t`              | Minute value of the last time this figure was reset (if never reset then the first time they were placed on a portal)
|  0x51  | 0E/2A  |  0x01  | `uint8_t`              | Hour value of the last time this figure was reset (if never reset then the first time they were placed on a portal)
|  0x52  | 0E/2A  |  0x02  | `uint8_t`              | Day value of the last time this figure was reset (if never reset then the first time they were placed on a portal)
|  0x53  | 0E/2A  |  0x03  | `uint8_t`              | Month value of the last time this figure was reset (if never reset then the first time they were placed on a portal)
|  0x54  | 0E/2A  |  0x04  | `uint16_t`             | Year value of the last time this figure was reset (if never reset then the first time they were placed on a portal)
|  0x56  | 0E/2A  |  0x06  | `uint16_t`             | PR event data
|  0x58  | 0E/2A  |  0x08  | `uint32_t`             | "Wii" data
|  0x5C  | 0E/2A  |  0x0C  | `uint32_t`             | "Xbox 360" data
|  0x60  | 10/2C  |  0x00  | `uint8_t[15]`          | [Usage/owner info](#usage-info)
|  0x70  | 11/2D  |  0x00  | `uint16_t`             | crc16-ccitt/false checksum of the bytes "06 01" followed by 0x3E bytes from 0x72
|  0x72  | 11/2D  |  0x02  | `uint8_t`              | [Area Sequence](#area-sequence) for this data area
|  0x78  | 11/2D  |  0x08  | `uint16_t`             | Gearbits (max is 33000). SuperChargers will refuse to update the Gearbits if it exceeds 33000 in game, even though the counter can go beyond

### CYOS (Creation Crystals/Imaginators)

CYOS stands for "Create Your Own Skylander", the internal name given to Imaginators, which are stored on Creation Crystals, 3D printed figures, and cards.

NOTE: Some of this information may be incorrect and is actively being worked on.

| St_Off | Block  | Bl_Off | Type                   | Description
|--------|--------|--------|------------------------|---------------
|  0x03  | 08/24  |  0x03  | `uint16_t`             | Money
|  0x05  | 08/24  |  0x05  | `uint32_t`             | Cumulative time in seconds
|  0x09  | 08/24  |  0x09  | `uint8_t`              | [Area Sequence](#area-sequence)
|  0x0A  | 08/24  |  0x0A  | `uint16_t`             | crc16-ccitt/false checksum of 0x100 bytes starting from 0x40 (so blocks 0D/29 -> 22/3E excluding access control blocks)
|  0x0C  | 08/24  |  0x0C  | `uint16_t`             | crc16-ccitt/false checksum of 0x30 bytes starting from 0x10 (so blocks 09/25 -> 0C/28 excluding access control blocks)
|  0x0E  | 08/24  |  0x0E  | `uint16_t`             | crc16-ccitt/false checksum of the first 14 bytes of this struct + the bytes "05 00" at the end
|  0x13  | 09/25  |  0x03  | `uint8_t`              | 2011 [Platform bitfield](#platform-bitfield)
|  0x17  | 09/25  |  0x07  | `uint8_t`              | 2013 [Platform bitfield](#platform-bitfield)
|  0x19  | 09/25  |  0x09  | `uint8_t`              | [Element](../source/EElementType.hpp) of the CYOS figure (non-Creation Crystal only)
|  0x20  | 0A/26  |  0x00  | `uint8_t[16]`          | First 16 bytes of nickname
|  0x30  | 0C/28  |  0x00  | `uint8_t[16]`          | Last 16 bytes of nickname
|  0x40  | 0D/29  |  0x00  | `uint8_t`              | Minute value of the last time this figure was placed on the portal
|  0x41  | 0D/29  |  0x01  | `uint8_t`              | Hour value of the last time this figure was placed on the portal
|  0x42  | 0D/29  |  0x02  | `uint8_t`              | Day value of the last time this figure was placed on the portal
|  0x43  | 0D/29  |  0x03  | `uint8_t`              | Month value of the last time this figure was placed on the portal
|  0x44  | 0D/29  |  0x04  | `uint16_t`             | Year value of the last time this figure was placed on the portal
|  0x4F  | 0D/29  |  0x0F  | `uint8_t`              | No. times owner changed. Normally increments by 1 when ownership is changed (capped at 255)
|  0x50  | 0E/2A  |  0x00  | `uint8_t`              | Minute value of the last time this figure was reset (if never reset then the first time they were placed on a portal)
|  0x51  | 0E/2A  |  0x01  | `uint8_t`              | Hour value of the last time this figure was reset (if never reset then the first time they were placed on a portal)
|  0x52  | 0E/2A  |  0x02  | `uint8_t`              | Day value of the last time this figure was reset (if never reset then the first time they were placed on a portal)
|  0x53  | 0E/2A  |  0x03  | `uint8_t`              | Month value of the last time this figure was reset (if never reset then the first time they were placed on a portal)
|  0x54  | 0E/2A  |  0x04  | `uint16_t`             | Year value of the last time this figure was reset (if never reset then the first time they were placed on a portal)
|  0x60  | 10/2C  |  0x00  | `uint8_t[15]`          | [Usage/owner info](#usage-info)
|  0x7C  | 11/2D  |  0x0C  | `uint8_t[4]`           | First 4 bytes of [CYOS data](#cyos-data)
|  0x80  | 12/2E  |  0x00  | `uint8_t[16]`          | Next 16 bytes of [CYOS data](#cyos-data)
|  0x90  | 14/2E  |  0x00  | `uint8_t[16]`          | Next 16 bytes of [CYOS data](#cyos-data)
|  0xA0  | 15/2E  |  0x00  | `uint8_t[16]`          | Next 16 bytes of [CYOS data](#cyos-data)
|  0xB0  | 16/2E  |  0x00  | `uint8_t[16]`          | Next 16 bytes of [CYOS data](#cyos-data)
|  0xC0  | 18/2E  |  0x00  | `uint8_t`              | Final byte of [CYOS data](#cyos-data)

### None of the above data structures

Note that tfbSpyroTag_MagicMomentAll and tfbSpyroTag_RemainingDataAll are used by the game internally.
* The 0x40 bytes from offset 0x00 are the first 0x40 bytes of tfbSpyroTag_MagicMomentAll.
* The 0x30 bytes from offset 0x40 are the first 0x30 bytes of tfbSpyroTag_RemainingDataAll.
* The 0x10 bytes from offset 0x70 are the remaining 0x10 bytes of tfbSpyroTag_MagicMomentAll.
* The 0x30 bytes from offset 0x80 are the remaining 0x30 bytes of tfbSpyroTag_RemainingDataAll.

| St_Off | Block  | Bl_Off | Type                   | Description
|--------|--------|--------|------------------------|---------------
|  0x00  | 08/24  |  0x00  | `uint24_t`             | 2011 [Experience](#experience) value (max is 33000)
|  0x03  | 08/24  |  0x03  | `uint16_t`             | Money
|  0x05  | 08/24  |  0x05  | `uint32_t`             | Cumulative time in seconds
|  0x09  | 08/24  |  0x09  | `uint8_t`              | [Area Sequence](#area-sequence)
|  0x0A  | 08/24  |  0x0A  | `uint16_t`             | crc16-ccitt/false checksum of 0x30 bytes starting from 0x40, followed by 0xE0 bytes of 0 (so blocks 0D/29 -> 10/2C excluding access control blocks)
|  0x0C  | 08/24  |  0x0C  | `uint16_t`             | crc16-ccitt/false checksum of 0x30 bytes starting from 0x10 (so blocks 09/25 -> 0C/28 excluding access control blocks)
|  0x0E  | 08/24  |  0x0E  | `uint16_t`             | crc16-ccitt/false checksum of the first 14 bytes of this struct + the bytes "05 00" at the end
|  0x10  | 09/25  |  0x00  | `uint24_t`             | [Flags1](#flags)
|  0x13  | 09/25  |  0x03  | `uint8_t`              | 2011 [Platform bitfield](#platform-bitfield)
|  0x14  | 09/25  |  0x04  | `uint16_t`             | 2011 [Hat value](#hat-value)
|  0x16  | 09/25  |  0x06  | `uint8_t`              | [Region Count Identifier](#region-count-identifier)
|  0x17  | 09/25  |  0x07  | `uint8_t`              | 2013 [Platform bitfield](#platform-bitfield)
|  0x18  | 09/25  |  0x08  | `uint64_t`             | Owner ID (used by SSA/Giants. Future games store what figures they own, instead of the figure storing who their owner is)
|  0x20  | 0A/26  |  0x00  | `wchar_t[8]`           | First 16 bytes of nickname
|  0x30  | 0C/28  |  0x00  | `wchar_t[8]`           | Last 16 bytes of nickname
|  0x40  | 0D/29  |  0x00  | `uint8_t`              | Minute value of the last time this figure was placed on the portal
|  0x41  | 0D/29  |  0x01  | `uint8_t`              | Hour value of the last time this figure was placed on the portal
|  0x42  | 0D/29  |  0x02  | `uint8_t`              | Day value of the last time this figure was placed on the portal
|  0x43  | 0D/29  |  0x03  | `uint8_t`              | Month value of the last time this figure was placed on the portal
|  0x44  | 0D/29  |  0x04  | `uint16_t`             | Year value of the last time this figure was placed on the portal
|  0x46  | 0D/29  |  0x06  | `uint32_t`             | Completed SSA Heroic Challenges
|  0x4A  | 0D/29  |  0x0A  | `uint16_t`             | Hero points (max is 999 in SSA, 100 in Universe)
|  0x4C  | 0D/29  |  0x0C  | `uint24_t`             | Last game build diagnostics. First byte is the build year (from 2000); second byte is build month; third byte is build day
|  0x4F  | 0D/29  |  0x0F  | `uint8_t`              | No. times owner changed. Normally increments by 1 when ownership is changed (capped at 255)
|  0x50  | 0E/2A  |  0x00  | `uint8_t`              | Minute value of the last time this figure was reset (if never reset then the first time they were placed on a portal)
|  0x51  | 0E/2A  |  0x01  | `uint8_t`              | Hour value of the last time this figure was reset (if never reset then the first time they were placed on a portal)
|  0x52  | 0E/2A  |  0x02  | `uint8_t`              | Day value of the last time this figure was reset (if never reset then the first time they were placed on a portal)
|  0x53  | 0E/2A  |  0x03  | `uint8_t`              | Month value of the last time this figure was reset (if never reset then the first time they were placed on a portal)
|  0x54  | 0E/2A  |  0x04  | `uint16_t`             | Year value of the last time this figure was reset (if never reset then the first time they were placed on a portal)
|  0x56  | 0E/2A  |  0x06  | `uint16_t`             | PR Event data
|  0x58  | 0E/2A  |  0x08  | `uint32_t`             | "Wii" data
|  0x5C  | 0E/2A  |  0x0C  | `uint32_t`             | "Xbox 360" data
|  0x60  | 10/2C  |  0x00  | `uint8_t[15]`          | [Usage/owner info](#usage-info)
|  0x70  | 11/2D  |  0x00  | `uint16_t`             | crc16-ccitt/false checksum of the bytes "06 01" followed by 0x3E bytes from 0x72
|  0x72  | 11/2D  |  0x02  | `uint8_t`              | [Area Sequence](#area-sequence) for this data area
|  0x73  | 11/2D  |  0x03  | `uint16_t`             | 2012 [Experience](#experience) value (max is 63500 for SSF characters onwards, for SSA and Giants it's 65535)
|  0x75  | 11/2D  |  0x05  | `uint8_t`              | 2012 [Hat value](#hat-value)
|  0x76  | 11/2D  |  0x06  | `uint16_t`             | [Flags2](#flags)
|  0x78  | 11/2D  |  0x08  | `uint32_t`             | 2013 [Experience](#experience) value (max is 101000)
|  0x7C  | 11/2D  |  0x0C  | `uint8_t`              | 2013/2014 [Hat value](#hat-value)
|  0x7D  | 11/2D  |  0x0D  | `uint8_t`              | [Trinket value](#trinket-value)
|  0x7E  | 11/2D  |  0x0E  | `uint8_t`              | 2015 [Hat value](#hat-value) (add 256 to get the true hat ID)
|  0x80  | 12/2E  |  0x00  | `uint32_t`             | [Battlegrounds Flags](#battlegrounds-flags)
|  0x84  | 12/2E  |  0x04  | `uint24_t`             | Completed SG Heroic Challenges
|  0x87  | 12/2E  |  0x07  | `uint72_t`             | Giants [Quests](#quests)
|  0x90  | 14/30  |  0x07  | `uint48_t`             | Next 6 bytes of Giants [Quests](#quests). Note that no data ever gets populated to these bytes, so are effectively unused
|  0x97  | 14/30  |  0x07  | `uint72_t`             | SWAP Force [Quests](#quests)

### Experience

Sum of all experience values. In SSCR instead, the experience value for each year must be more than or equal to the max to then read the next year's experience.

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

Note that vehicle experience in SuperChargers Racing uses the exact same experience amounts for levels, but instead of having different bytes for the 2012/13 experience etc... it is all self contained within the initial "2011" `uint24_t`.

### Area sequence

* The core skylanders at least store the tfbSpyroTagData struct twice, once from block 0x08/0x24 and again from block 0x11/0x2D, these are called data regions or data areas.
* The recently written to area is the one with the area sequence value that is one higher than the other, remember to take overflows into account (e.g., an area sequence of 0 is considered higher than one of 255).
* When writing to a figure, the game will switch to the lower area sequence value and set it to be the higher value incremented by 1.
* Certain games, like Trap Team, will throw an error if the two sequences become out of sync with another, whereas other games, like SSCR, do not mind.
* Note that there are two area sequences, one for blocks 0x08/0x24->0x10/0x2C and another for blocks 0x11/0x2D->0x15/0x31 (inclusive).

### Error byte

Used to reject tags for whatever reason; seems mostly tied to other franchises using the same RFID protocol to make sure they don't get interpreted as Skylander data.

By default, for all Skylander figures, this byte is set to 0. If a toy is read and this byte is not equal to zero, the game will in some way refuse the toy, however the way the game behaves upon doing so varies and can sometimes act in an unintended manner.
* In Skylanders Spyro's Adventure, Skylanders Giants, and Skylanders Trap Team, the toy will be considered unsupported and cannot be used in the game.
* In Skylanders SWAP Force, Skylanders SuperChargers, and Skylanders Imaginators, if no other Skylanders have been placed on the portal prior, the game will consider the toy unsupported. If a Skylander has been placed prior, the game will remember that character info from that figure index on the portal. If this toy is then placed on the portal with the exact same figure index (which can be done by unplugging and plugging back in the portal, or loading the same slot on emulated portals) that character will incidently be loaded instead - including halves of SWAP Force Skylanders and Senseis - regardless of the actual character on the tag or data on the previous tag. The new character will then act somewhat similar to Template Template, where changing Ownership and writing to the tag does not function.
* In Skylanders SuperChargers Racing, the game will constantly bring up the corrupted toy prompt before immediately closing the prompt, and then reopening, halting any further progress.

### Hat value

* [Hat enum](../source/kTfbSpyroTag_HatType.hpp).
* Note that the following ids are identical to the ids used in the file names of Skylanders Spyro's Adventure, Skylanders Giants, and Skylanders Trap Team minus 1. For example, The straw hat has id 9 on figures but has id 8 in the files. 
* The unused hat ids are not used.
* The padding hat ids were never intended to be used.

#### Lookup Algorithm
* In Skylanders Giants and Trap Team, check the oldest hat value, if it's not 0, return that, otherwise check the second oldest hat value, repeat, and so on.
* In Skylanders SWAP Force, SuperChargers, and Imaginators, the order is reversed: check the newest hat value, if it's not 0, return that, otherwise check the second newest hat value, repeat, and so on.
* When returning the SuperChargers hat value, add 256 to get the true hat ID.
* Note that SWAP Force, SuperChargers, and Imaginators also attempt to wipe all unread hat areas on every write.

| Hat ID | Hat Name
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
|  0010  | Fancy Hat (Monday)
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
|  0025  | Lil' Devil
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
|  0083  | Caesar Hat
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
|  0111  | Feathered Headdress (Peacock Hat)
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
|  0128  | Shark Hat (Sharkfin Hat)
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
|  0141  | Skullhelm (Skyll)
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
|  0192  | Metal Fin Hat
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

#### 2011 value:
* Bit 0: Wii
* Bit 1: Xbox 360
* Bit 2: PS3
* Bit 3: PC (maybe macOS as well)
* Bit 4: Nintendo 3DS

#### 2013 value:
* Bit 0: Android 32-bit (maybe Android 64-bit as well). Seems to also correspond to Wii U
* Bit 1: Xbox One
* Bit 2: PS4
* Bit 3: iOS 64-bit (maybe iOS 32-bit as well)
* Bit 6: Nintendo Switch

So for example, if the 2011 value is set to 3, then bits 0 and 1 are set, and therefore the figure has been used on Wii and Xbox 360.

Note that SSA JP for the Wii U incidently sets the Wii platform usage flag instead of the Wii U one.

### Region Count Identifier

Used as a reset indicator to make sure all data regions are correctly wiped. Skylanders Giants added 0x10 bytes of Magic Moment info and 0x30 bytes of Remaining Data info, these 2 extension structs form the "second data region", with the original info from SSA being considered the "first data region".

This amount is known as the `dataRegionCount`; 1 for SSA, 2 for all other games. Note that the Wii and Wii U versions of SSA JP act like all games after SSA, with 2 data regions.

The `regionCountID` is encoded with the formula `(1 << (dataRegionCount - 1)) - 1` which returns a bit index for `dataRegionCount`s above 1 - this is stored at blocks 09/25, byte 0x06. All games after SSA will set this value every write.

The game does a comparision with a bitwise OR if the bit is set to determine if all data regions have valid data.

* For Spyro's Adventure, the `regionCountID` is unused, and will be set to 0 when the figure is reset. As SSA's reset routine doesn't wipe the second data region information from the figure, the second data region info will still persist on reset. Later games can verify that the data in the second data region is invalid by reading that the byte is 0, the bitwise OR check fails, and the second data region info is treated as reset/empty.
* For Skylanders Giants and future games, the reset figure routine from these games correctly wipe the second data region information and set the `regionCountID` to the correct value to have it succeed on future checks. The check succeeds if ever written to by these games, and the second data region is always read correctly.

### Flags

* [Upgrade](#upgrades) flags: `((Flags2 & 0xF) << 10) | (Flags1 & 0x3FF)`
* Element Collection Count 1: `(Flags1 >> 10) & 3`
* Element Collection Count 2: `(Flags2 >> 6) & 7`
* Element Collection: `((Flags1 >> 10) & 3) + ((Flags2 >> 6) & 7) + ((Flags2 >> 11) & 3)`
* SSF Accolade Rank: `(Flags2 >> 9) & 3`
* SG Accolade Rank: `(Flags2 >> 4) & 3`

### Battlegrounds Flags

* Portal Master Level: `(BGFlags << 0x06) >> 0x1A`
* Ability Slot Count: `((BGFlags << 0x0C) >> 0x1E) + 1`
* Ability Level: `(BGFlags >> ((abilityIndex * 3) & 0xFF)) & 0x7`

### Vehicle Flags

Oddly, each level of the vehicle's shield and weapon occupies its own bit, even though it's impossible normally to purchase a level and skip one. This spans over a `uint16_t`.

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

### Vehicle Mod Flags

* Performance Mod: `ModFlags & 0xF`
* Specialty Mod: `(ModFlags >> 4) & 0xF`
* Horn: `(ModFlags >> 8) & 0xF`

### Upgrades

* Bit 0: Whether or not the Skylander is on a path
* Bit 1: Path (0 = top, 1 = bottom)
* Bit 2: Upgrade 1 purchased
* Bit 3: Upgrade 2 purchased
* Bit 4: Upgrade 3 purchased
* Bit 5: Upgrade 4 purchased
* Bit 6: Path upgrade 1 purchased
* Bit 7: Path upgrade 2 purchased
* Bit 8: Path upgrade 3 purchased
* Bit 9: Soul Gem purchased
* Bit 10: Wow Pow purchased (or Sky-Chi for Senseis)
* Bit 11: Alternate path upgrade 1 purchased (used to retain details of upgrades on the other path when switching path as a repose or in SuperChargers)
* Bit 12: Alternate path upgrade 2 purchased
* Bit 13: Alternate path upgrade 3 purchased

### Usage info

Unknown. TODO in future

### Heroic Challenges

* Heroic Challenges are sprawled out over 2 different areas. One `uint32_t` at block 0x0D/0x29, offset 0x06, and another `uint24_t` at block 0x12/0x2E, offset 0x04.
* The first one of these accords to the Heroic Challenges in Spyro's Adventure, and the second one the Heroic Challenges introduced in Giants.
* Each bit corresponds to a boolean indicating whether or not the heroic challenge has been complete by the Skylander.
* Their offset into the bit field is the same as their internally stored identifier. For Giants Heroics, add 32 to their offset to reveal the true heroic ID. This essentially means that if you append the Giants heroic bytes to the start of the SSA heroic bytes, then using their ID would give you the corresponding bit.
* Assume `SSA` refers to the 4 bytes for SSA heroics, and `SG` for the other 3 bytes.

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
| `SG >> 0x04`    | 36 | Nort's Winter Classic
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

#### Giants Quests
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
|   10   | Individual Quest

#### SWAP Force Quests
| Shift  | Mask | SWAP Force Name
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

16 bit long bit field that represents which villains have been unlocked within the cup. Note that none of this exists for the Kaos Trophy; he is unlocked just by reading the figure on the Portal.

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

### Vehicle Decoration

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

### Vehicle Neon

| ID | Deco/Neon Name
|----|----------------
| 00 | None
| 01 | Darkness
| 02 | Eon
| 03 | Ancient
| 04 | Cap'N Cluck
| 05 | Cartoon
| 06 | Kaos
| 07 | Police
| 08 | Construction
| 09 | Holiday
| 0A | Ghost
| 0B | Royal
| 0C | Ninja
| 0D | Thermal
| 0E | Robot
| 0F | Fire Truck

### Vehicle Topper

| ID | Topper Name
|----|----------------
| 00 | None
| 01 | The Darkness
| 02 | Lucky Coin
| 03 | King-Sized Bucket
| 04 | Popcorn
| 05 | Chicken Leg
| 06 | Pinata
| 07 | Bag of Gold
| 08 | Chompy
| 09 | Balloon
| 0A | Ripe Banana
| 0B | Beach Ball
| 0C | Teddy Hat
| 0D | Corn on the Cob
| 0E | Dragonfire Cannon
| 0F | Eon's Sock
| 10 | Eon's Statue
| 11 | Kaos Statue
| 12 | Spitfire Doll
| 13 | Golden Piggy Bank
| 14 | Raccoon Tail
| 15 | Rasta Hat
| 16 | Party Sheep
| 17 | Snap Shot Doll
| 18 | Space Helmet
| 19 | Squeeks Jr.
| 1A | Tiki Speaky
| 1B | Traffic Cone
| 1C | Tree Rex Doll
| 1D | Tricorn Hat
| 1E | Trigger Happy Doll
| 1F | Wash Buckler Doll
| 20 | Weathervane
| 21 | Eon's Helm
| 22 | Pluck
| 23 | Siren
| 24 | Ghost Topper
| 25 | Cartoon Doll
| 26 | Kaos Punching Bag
| 27 | Cup O' Cocoa
| 28 | Hand of Fate
| 29 | Like Clockwork
| 2A | Empire of Ice
| 2B | Pizza
| 2C | Yeti Doll
| 2D | Kaos Sigil
| 2E | Cowboy Hat
| 2F | Eyeball Ball
| 30 | Asteroid
| 31 | Hook Hand
| 32 | The Mighty Atom
| 33 | Holiday Tree
| 34 | Shuriken
| 35 | Mechanical Gear
| 36 | Royal Crown
| 37 | Fire Hydrant

### Vehicle Shout

| ID | Name
|----|-------------------
| 00 | None
| 01 | Sneer: Cali
| 02 | Jeer: Cali
| 03 | Cheer: Cali
| 04 | Back off Bear
| 05 | Breakdown
| 06 | Pull Over!
| 07 | Evil Eye
| 08 | Bird Brain
| 09 | The Ultimate Evil!
| 0A | Leave Me Alone Lion
| 0B | Going Nuclear
| 0C | Sneer: Sharpfin
| 0D | The Darkness
| 0E | Why I Oughta
| 0F | Police Siren
| 10 | Fire It Up
| 11 | Sneer: Buzz
| 12 | Call Me!
| 13 | Car Trouble
| 14 | Sneer: Pomfrey
| 15 | Yield!
| 16 | Hype Train
| 17 | Doggin' After Ya
| 18 | Crash and Burn
| 19 | Earthquake
| 1A | Flat Tire
| 1B | Fly Trap
| 1C | Sneer: Glumshanks
| 1D | Sneer: Hugo
| 1E | Sneer: Queen Cumulus
| 1F | Ninja Stars
| 20 | AAAAAA…
| 21 | Jeer: Sharpfin
| 22 | Red Means Go Right?
| 23 | The Final Countdown
| 24 | Rush Hour
| 25 | Sneer: Tessa
| 26 | Tidal Wave
| 27 | Toasty!
| 28 | All Spun Up
| 29 | Under Construction
| 2A | Howlin' Good
| 2B | Cheer: Buzz
| 2C | Cheer: Pomfrey
| 2D | Checkered Flag
| 2E | Eon Impersonator
| 2F | Cheer: Flynn
| 30 | Cheer: Glumshanks
| 31 | Wink Wink Nudge Nudge
| 32 | Silver Bells
| 33 | Cheer: Queen Cumulus
| 34 | Cheer: Persephone
| 35 | Cheer: Sharpfin
| 36 | Cheer: Hugo
| 37 | :)
| 38 | Cheer: Tessa
| 39 | First Place Trophy
| 3A | Big Bell
| 3B | Rude Chompy
| 3C | Your Robot Son
| 3D | Cry Baby
| 3E | The Gulper
| 3F | Sweet Innocence
| 40 | Diplomacy
| 41 | The Prince of Pontification
| 42 | Scandalous!
| 43 | Like Clockwork
| 44 | Ancient Energy
| 45 | Banana Peel
| 46 | Bashful Face
| 47 | Boo
| 48 | Boo Too
| 49 | Jeer: Buzz
| 4A | Catchy Jingle
| 4B | Jeer: Pomfrey
| 4C | Laugh It Up
| 4D | Cow Crossing
| 4E | Cuckoo Cuckoo
| 4F | Rude Dolphin
| 50 | Jack the Donkey
| 51 | Quack!
| 52 | Trumpet Trunk
| 53 | Blub-Blub
| 54 | Jeer: Flynn
| 55 | Jeer: Glumshanks
| 56 | Indignant Goose
| 57 | Ham!
| 58 | Horsin' Around
| 59 | Jeer: Hugo
| 5A | Kissy Face
| 5B | Purrfect
| 5C | Lockpick Gremlin
| 5D | Nature Calls
| 5E | Jeer: Queen Cumulus
| 5F | Oop Oop Eek
| 60 | Tauntalizing
| 61 | Soda Pop
| 62 | Wow!
| 63 | Baa-Aaa!
| 64 | Squeaky Toy
| 65 | Jeer: Tessa

### CYOS data

0x45 byte long struct encompassing all of the CYOS data stored on the figure.

The information stored by CYOS figures is not byte aligned, so each "part" has a given bit size in the struct; each "part" is stored sequentially.

#### Storage Info

| Shift | Bits | CYOS part
|-------|------|---------------
| 0000  |  10  | Primary Weapon
| 000A  |  10  | Secondary Weapon (unused?)
| 0014  |  10  | Backpack
| 001E  |  10  | Headgear
| 0028  |  10  | Leg Guards
| 0032  |  10  | Arm Guards
| 003C  |  10  | Shoulder Guards
| 0046  |  10  | Ears
| 0050  |  4   | Lower Body Scale
| 0054  |  4   | Upper Body Scale
| 0058  |  4   | Height
| 005C  |  4   | Muscle Scale
| 0060  |  4   | Head Scale
| 0064  |  4   | Tail Width
| 0068  |  10  | Head
| 0072  |  10  | Torso
| 007C  |  10  | Arms
| 0086  |  10  | Legs/Tasset
| 0090  |  7   | Tail
| 0097  |  7   | Head Color 1
| 009E  |  7   | Head Color 2
| 00A5  |  7   | Head Color 3
| 00AC  |  7   | Head Color 4
| 00B3  |  7   | Head Color 5
| 00BA  |  7   | "Ear" Color (unused?)
| 00C1  |  7   | Arms Color 1
| 00C8  |  7   | Arms Color 2
| 00CF  |  7   | Arms Color 3
| 00D6  |  7   | Arms Color 4
| 00DD  |  7   | Arms Color 5
| 00E4  |  7   | Torso Color 1
| 00EB  |  7   | Torso Color 2
| 00F2  |  7   | Torso Color 3
| 00F9  |  7   | Torso Color 4
| 0100  |  7   | Torso Color 5
| 0107  |  7   | Legs/Tasset Color 1
| 010E  |  7   | Legs/Tasset Color 2
| 0115  |  7   | Legs/Tasset Color 3
| 011C  |  7   | Legs/Tasset Color 4
| 0123  |  7   | Legs/Tasset Color 5
| 012A  |  7   | Eye Color 1 (Pupil)
| 0131  |  7   | Eye Color 2 (Sclera)
| 0138  |  7   | Tail Color 1
| 013F  |  7   | Tail Color 2
| 0146  |  7   | Ears Color 1
| 014D  |  7   | Ears Color 2
| 0154  |  7   | Ears Color 3
| 015B  |  7   | Headgear Color 1
| 0162  |  7   | Headgear Color 2
| 0169  |  7   | Headgear Color 3
| 0170  |  7   | Arm Guards Color 1
| 0177  |  7   | Arm Guards Color 2
| 017E  |  7   | Arm Guards Color 3
| 0185  |  7   | Shoulder Guards Color 1
| 018C  |  7   | Shoulder Guards Color 2
| 0193  |  7   | Shoulder Guards Color 3
| 019A  |  7   | Backpack Color 1
| 01A1  |  7   | Backpack Color 2
| 01A8  |  7   | Backpack Color 3
| 01AF  |  7   | Leg Guards Color 1
| 01B6  |  7   | Leg Guards Color 2
| 01BD  |  7   | Leg Guards Color 3
| 01C4  |  10  | Second Power Flags
| 01CE  |  10  | Tertiary Power Flags
| 01D8  |  7   | [Battle Class](#battle-classes)
| 01DF  |  6   | Aura
| 01E5  |  8   | Sound Effects
| 01ED  |  6   | Eyes
| 01F3  |  8   | Catchphrase 1
| 01FB  |  8   | Catchphrase 2
| 0203  |  8   | Music
| 020B  |  8   | Voice
| 0213  |  7   | Voice Filter
| 021A  |  9   | Primary Power Flags

#### Battle Classes

| ID | Battle Class
|----|--------------
| 0  | None
| 1  | Knight
| 2  | Bowslinger
| 3  | Quickshot
| 4  | Ninja
| 5  | Brawler
| 6  | Smasher
| 7  | Sorcerer
| 8  | Swashbuckler
| 9  | Sentinel
| 10 | Bazooker
| 11 | Kaos

Certain CYOS pieces on the figure have IDs respective to the Battle Class; this is done using the formula `ID + ((battleClass - 1) * 100)`. Note that blank/not set pieces that abide by this will have the IDs above 1000.

### Credits:
* Brandon Wilson:
  * The encryption method
  * Checksum types 0->3 and area sequences
  * The Wii and Xbox 360 values for the platform usage
  * Hero points max value
* Mandar1jn:
  * Bytes 0x04->0x10 of the tag header
* Winner Nombre:
  * Figured out Tech element quests
  * Figured out what Accolade Ranks are
  * Corrections with crc checksums
  * Figured out trinkets
  * The PC, Nintendo 3DS, Xbox One, PS4, and Nintendo Switch values for the platform usage
* Texthead:
  * Variant ID additions/corrections
  * Racing Pack additions/corrections
  * Figured out Vehicles
  * Figured out CYOS figures
  * Additional Trap info
  * Heroic Challenges
  * Hat fixes and info
  * Region Count ID info
* Maff:
  * Help with CYOS pieces
  * Discovered PR event, "Wii", and "Xbox 360" miscellaneous data
  * Help with build diagnostic data