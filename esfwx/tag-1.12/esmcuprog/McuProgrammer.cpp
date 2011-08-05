#include "stdafx.h"
#pragma hdrstop

#include "McuProgrammer.h"
#include "McuLpcProgrammer.h"

// common MCU programmer implementation
//
McuProgrammer* McuProgrammer::create(McuProgrammer::Type type)
{
	switch( type )
	{
	case McuProgrammer::LPC2000:
		return new McuLpc2kProgrammer;
	}

	return 0;
}
