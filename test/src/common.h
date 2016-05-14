/*
 * common.h:
 *
 * Author(s): objectx
 *
 * $Id$
 */
#ifndef common_h__ec772635_200f_4375_a22b_263bda92f515
#define common_h__ec772635_200f_4375_a22b_263bda92f515  1

#pragma once

#ifndef _WIN32_WINNT            // Allow use of features specific to Windows XP or later.
#define _WIN32_WINNT 0x0501     // Change this to the appropriate value to target other versions of Windows.
#endif

#if defined (_WIN32) || defined (_WIN64)
#define WIN32_LEAN_AND_MEAN     1
#include <windows.h>
#endif

#include <sys/types.h>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

#endif  /* common_h__ec772635_200f_4375_a22b_263bda92f515 */
/*
 * $LastChangedRevision$
 * $LastChangedBy$
 * $HeadURL$
 */
