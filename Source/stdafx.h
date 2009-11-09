// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <string>
#include <map>
#include <vector>
#include <OgreSharedPtr.h>
#include <OgreAny.h>
#include <OgreVector3.h>

namespace Bismuth {
	using Ogre::SharedPtr;
	using Ogre::Any;
	using Ogre::any_cast;
}

// TODO: reference additional headers your program requires here
