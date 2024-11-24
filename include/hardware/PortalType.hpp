/*
	File:
		PortalType.hpp

	Description:
		portal hardware identifiers.
*/

#ifndef RUNES_HARDWARE_PORTAL_TYPE_HPP
#define RUNES_HARDWARE_PORTAL_TYPE_HPP

enum PortalType
{
	PORTAL_TYPE_DEFAULT, // Non-xbox
	PORTAL_TYPE_XBOX360, // Xbox 360
	PORTAL_TYPE_XBOXONE, // Xbox One
	PORTAL_TYPE_BT,      // Bluetooth
	PORTAL_TYPE_NUM      // Number of types
};

#endif // RUNES_HARDWARE_PORTAL_TYPE_HPP
