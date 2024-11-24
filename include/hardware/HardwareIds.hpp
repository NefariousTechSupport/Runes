/*
	File:
		HardwareIds.hpp

	Description:
		portal hardware identifiers.
*/

#ifndef RUNES_HARDWARE_IDS_HPP
#define RUNES_HARDWARE_IDS_HPP

#include <cstdint>

#define kDefaultPortalVID 0x1430
#define kDefaultPortalPID 0x0150

#define kXbox360PortalVID 0x1430
#define kXbox360PortalPID 0x1F17

// Unimplemented
#define kXboxOnePortalVID 0x1430
#define kXboxOnePortalPID 0xFFFF

#define kBluetoothPortalShortServiceUUID "1530"
#define kBluetoothPortalServiveUUID      "533E1530-3ABE-F33F-CD00-594E8B0A8EA3" 
#define kBluetoothPortalWriteUUID        "533E1543-3ABE-F33F-CD00-594E8B0A8EA3" 
#define kBluetoothPortalReadUUID         "533E1542-3ABE-F33F-CD00-594E8B0A8EA3" 

#endif // RUNES_HARDWARE_IDS_HPP