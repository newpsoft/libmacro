/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

#include "mcr/signal/noop.h"

#include <chrono>
#include <thread>

namespace mcr
{
void NoOp::send()
{
	if (seconds)
		std::this_thread::sleep_for(std::chrono::milliseconds(seconds));
	if (milliseconds)
		std::this_thread::sleep_for(
			std::chrono::milliseconds(milliseconds));
}
}
