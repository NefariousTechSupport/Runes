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

## tfbSpyroTagData

0xB0 byte long struct

### Trap

* The 0x40 bytes from offset 0x00 is the magic moment data
* The 0x110 bytes from 0x40 is the remaining data;

### Not a trap

Note that tfbSpyroTag_MagicMomentAll and tfbSpyroTag_RemainingDataAll are used by the game internally
* The 0x40 bytes from offset 0x00 are the first 0x40 bytes of tfbSpyroTag_MagicMomentAll
* The 0x30 bytes from offset 0x40 are the first 0x30 bytes of tfbSpyroTag_RemainingDataAll
* The 0x10 bytes from offset 0x70 are the remaining 0x10 bytes of tfbSpyroTag_MagicMomentAll
* The 0x30 bytes from offset 0x80 are the remaining 0x30 bytes of tfbSpyroTag_RemainingDataAll

| Offset | MM_Off | RD_Off | Type                   | Description
|--------|--------|--------|------------------------|---------------
|  0x00  |  0x00  |  N/A   | `uint24_t`             | 2011 [Experience](#experience) value (Max is 33000)
|  0x03  |  0x03  |  N/A   | `uint16_t`             | Money
|  0x05  |  0x05  |  N/A   | `uint32_t`             | Cumulative time in seconds
|  0x09  |  0x09  |  N/A   | `uint8_t`              | [area sequence](#area-sequence)
|  0x0A  |  0x0A  |  N/A   | `uint16_t`             | crc16-ccit/false checksum of 0x30 bytes starting from 0x10
|  0x0E  |  0x0E  |  N/A   | `uint16_t`             | crc16-ccit/false checksum of 0x30 bytes starting from 0x40, followed by 0xE0 bytes of 0
|  0x0E  |  0x0E  |  N/A   | `uint16_t`             | crc16-ccit/false checksum of the first 14 bytes of this struct + the bytes "05 00" at the end
|  0x10  |  0x10  |  N/A   | `uint24_t`             | [Flags1](#flags)
|  0x13  |  0x13  |  N/A   | `uint8_t`              | 2011 [Platform bitfield](#platform-bitfield)
|  0x14  |  0x14  |  N/A   | `uint16_t`             | 2011 [Hat value](#hat-value)
|  0x16  |  0x16  |  N/A   | `uint8_t`              | `(1 << (dataRegionCount - 1)) - 1`. since `dataRegionCount` is always set to 2 on core figures, this always evaluates to 1
|  0x17  |  0x17  |  N/A   | `uint8_t`              | 2013 [Platform bitfield](#platform-bitfield)
|  0x20  |  0x20  |  N/A   | `wchar_t[16]`          | Nickname
|  0x40  |  N/A   |  0x00  | `uint8_t`              | Minute value of the last time this figure was placed on the portal
|  0x41  |  N/A   |  0x01  | `uint8_t`              | Hour value of the last time this figure was placed on the portal
|  0x42  |  N/A   |  0x02  | `uint8_t`              | Day value of the last time this figure was placed on the portal
|  0x43  |  N/A   |  0x03  | `uint8_t`              | Month value of the last time this figure was placed on the portal
|  0x44  |  N/A   |  0x04  | `uint16_t`             | Year value of the last time this figure was placed on the portal
|  0x46  |  N/A   |  0x06  | `uint32_t`             | Completed ssa heroic challenges
|  0x4A  |  N/A   |  0x0A  | `uint16_t`             | Hero points (Max is 100)
|  0x4F  |  N/A   |  0x0F  | `uint8_t`              | Owner count(?)
|  0x50  |  N/A   |  0x10  | `uint8_t`              | Minute value of the last time this figure was reset (if never reset then the first time they were placed on a portal)
|  0x51  |  N/A   |  0x11  | `uint8_t`              | Hour value of the last time this figure was reset (if never reset then the first time they were placed on a portal)
|  0x52  |  N/A   |  0x12  | `uint8_t`              | Day value of the last time this figure was reset (if never reset then the first time they were placed on a portal)
|  0x53  |  N/A   |  0x13  | `uint8_t`              | Month value of the last time this figure was reset (if never reset then the first time they were placed on a portal)
|  0x54  |  N/A   |  0x14  | `uint16_t`             | Year value of the last time this figure was reset (if never reset then the first time they were placed on a portal)
|  0x60  |  N/A   |  0x20  | `uint8_[9]`            | Something related to time spent on what platforms
|  0x6C  |  N/A   |  0x2C  | `uint32_t`             | Challenge level
|  0x73  |  0x43  |  N/A   | `uint16_t`             | 2012 [Experience](#experience) value (Max is 63500)
|  0x75  |  0x45  |  N/A   | `uint8_t`              | 2012 [Hat value](#hat-value)
|  0x76  |  0x10  |  N/A   | `uint16_t`             | [Flags2](#flags)
|  0x78  |  0x48  |  N/A   | `uint32_t`             | 2013 [Experience](#experience) value (Max is 101000)
|  0x7C  |  0x4C  |  N/A   | `uint8_t`              | 2013/2014 [Hat value](#hat-value)
|  0x7E  |  0x4E  |  N/A   | `uint8_t`              | 2015 [Hat value](#hat-value) (add 256 to get the true hat id)
|  0x84  |  N/A   |  0x34  | `uint24_t`             | Completed sg heroic challenges
|  0x87  |  N/A   |  0x37  | `uint72_t`             | Giants [quests](#quests)
|  0x97  |  N/A   |  0x47  | `uint72_t`             | Swap Force [quests](#quests)

### Experience

Sum of all experience values

### Area sequence

* The core skylanders at least store the tfbSpyroTagData struct twice, once from block 0x08 and again from block 0x24, these are called data regions or data areas.
* The recently written to area is the one with the higher area sequence value.
* When writing to a figure, the game will switch to the lower area sequence value and increment it by 2 (so that it becomes higher than the other one).

### Hat value

* Check the most newest hat value, if it's not 0, return that, otherwise check the next oldest hat value and repeat
* [hat enum](../include/kTfbSpyroTag_HatType.hpp)

### Platform bitfield

* 0x0001: Wii
* 0x0002: Xbox 360
* 0x0004: PS3
* 0x8000: iOS 64 (Maybe iOS 32 as well)

### Flags

* Upgrage flags: `((Flags2 & 0xF) << 10) | (Flags1 & 0x3FF)`
* Element Collection Count 1: `(Flags1 >> 10) & 3`
* Element Collection Count 2: `(Flags1 >> 14) & 7`
* Element Collection: `((Flags1 >> 10) & 3) + ((Flags1 >> 14) & 7) + ((Flags2 >> 11) & 7)`
* Accolade Rank 1: `(Flags2 >> 9) & 3`
* Accolade Rank 2: `(Flags2 >> 4) & 3`

### Quests

Think of it as a 72 bit int.
| Shift  | Mask | Swap Force Name      | Giants Name
|--------|------|----------------------|-----------------
|   00   | 03FF | Badguy Basher        | Monster Masher
|   0A   | 000F | Fruit Frontiersman   | Chow Hound
|   0E   | 0001 | Flawless Challenger  | Arena Artist
|   0F   | 000F | True Gladiator       | Battle Champ
|   13   | 0001 | Totally Maxed Out    | Heroic Challenger
|   14   | 1FFF | Elementalist         | Elementalist
|   21   | 00FF | Elemental Quest 1    | Elemental Quest 1
|   29   | 01FF | Elemental Quest 2    | Elemental Quest 2
|   32   | FFFF | Individual Quest     | Individual Quest


Credits:
* Brandon Wilson:
  * The encryption method
  * Checkums and access specifier
  * The Wii and Xbox 360 values for the platform usage
  * Hero points max value
* Mandar1jn:
  * Bytes 0x04->0x10 of the tag header