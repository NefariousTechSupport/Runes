/*
	File:
		HardwareInterface.cpp

	Description:
		Base class for communicating with a portal peripheral.
*/

#include "hardware/HardwareInterface.hpp"

using namespace Runes::Portal;



//=============================================================================
// Constructor for HardwareInterface
//=============================================================================
HardwareInterface::HardwareInterface()
: _state(kStateUninitialised)
{
}



//=============================================================================
// Destructor for HardwareInterface
//=============================================================================
HardwareInterface::~HardwareInterface()
{
}



//=============================================================================
// connected: Determine whether or not the device is still connected
//=============================================================================
bool HardwareInterface::connected() const
{
	return _state == kStateConnected;
}