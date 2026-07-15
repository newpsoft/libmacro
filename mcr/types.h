/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

/**
 * @file mcr_types.h
 * @brief Defines core type definitions for the macro and hotkey library.
 *
 * This header provides Plain Old Data (POD) types, typedefs to simplify
 * common pointer or handle usage, and enumerations for defining standard
 * states within hotkey logic systems. Structures are deliberately designed
 * without function encapsulation to support zero-copy operations and efficient
 * memory layout required in real-time macro processing.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Defines usable spatial dimensions for event coordinates.
 *
 * This enumeration supports defining multi-dimensional spaces (e.g., X, Y, Z).
 * The values are kept sequential for array indexing efficiency, although aliases
 * like MCR_Y1/MCR_Y2 ensure backwards compatibility when dimensions are added.
 */
enum mcr_Dimension {
	MCR_X = 0, /**< First spatial dimension (e.g., X-axis). */
	MCR_Y, /**< Second spatial dimension (e.g., Y-axis). */
	MCR_Z, /**< Third spatial dimension (e.g., Z-axis). */
	//! @ref mcr_future
	MCR_W, /**< Fourth potential dimension (for future use). */
	MCR_Y1 = MCR_X, /**< Alias for X coordinate/dimension 1. */
	MCR_Y2 = MCR_Y, /**< Alias for Y coordinate/dimension 2. */
	MCR_Y3 = MCR_Z, /**< Alias for Z coordinate/dimension 3. */
	//! @ref mcr_future
	MCR_Y4 = MCR_W, /**< Alias for W coordinate/dimension 4. */
	/** @brief Maximum usable dimension index (Exclusive count of dimensions). */
	MCR_DIMENSION_MAX = MCR_Z,
	/** @brief Total count of defined dimensions (MCR_DIMENSION_MAX + 1). */
	MCR_DIMENSION_COUNT = MCR_DIMENSION_MAX + 1
};

/**
 * @brief Describes the application state of a key or value.
 *
 * This enumeration defines how a reported signal should be interpreted:
 * setting, releasing, both sequentially, toggling, etc.
 */
typedef enum mcr_ApplyValue {
	/*! The value is actively set (Key Down event). */
	MCR_SET = 0,
	/*! The value is released or not applied (Key Up event). */
	MCR_UNSET,
	/*! Signal indicates both a set followed by an unset state. */
	MCR_BOTH,
	/*! Toggles the current state (e.g., if pressed while up, it sets; if pressed while down, it unsets). Other values are ignored. */
	MCR_TOGGLE,
	/*! Alias for MCR_SET, used for compatibility with key press semantics. */
	MCR_PRESS = MCR_SET,
	/*! Alias for MCR_UNSET, used for compatibility with key release semantics. */
	MCR_UNPRESS = MCR_UNSET
} mcr_Press_t;

/**
 * @brief Flags representing logical keyboard modifiers (Ctrl, Alt, Shift, etc.).
 *
 * This set of flags maps hardware/OS modifier keys to bitmasks. Some values
 * might share the same mask due to operating system conventions (e.g., Alt and Option).
 * Note: These constants are subject to change based on target OS or keyboard layout.
 */
enum mcr_ModFlags {
	/*! No modifiers are active. */
	MCR_MF_NONE = 0,
	/* Platform-specific aliases for common modifiers (Alt/Option). */
	MCR_ALT = 1,
	MCR_OPTION = MCR_ALT,
	/* Bitmask definitions: */
	MCR_ALTGR = 1 << 1, /**< AltGr modifier. */
	MCR_AMIGA = 1 << 2, /**< Amiga system key. */
	MCR_CMD = 1 << 3, /**< Command/Meta key (e.g., Cmd on macOS). */
	MCR_COMMAND = MCR_CMD, /**< Alias for command modifier. */
	MCR_CODE = 1 << 4, /**< Code key. */
	MCR_COMPOSE = 1 << 5, /**< Compose key. */
	MCR_CTRL = 1 << 6, /**< Control key (Ctrl). */
	MCR_CONTROL = MCR_CTRL, /**< Alias for control modifier. */
	MCR_FN = 1 << 7, /**< Function key (Fn). */
	MCR_FRONT = 1 << 8, /**< Front/Navigation key. */
	MCR_GRAPH = 1 << 9, /**< Graphics mode indicator. */
	MCR_HYPER = 1 << 10, /**< Hyper key. */
	MCR_META = 1 << 11, /**< Meta/Alt key (often used generically). */
	MCR_SHIFT = 1 << 12, /**< Shift key. */
	MCR_SUPER = 1 << 13, /**< Super/Windows key (Win). */
	MCR_SYMBOL = 1 << 14, /**< Symbol key indicator. */
	MCR_TOP = 1 << 15, /**< Top-level modifier status flag. */
	MCR_OS = 1 << 16, /**< Operating System/Win key state (generic). */
	MCR_WIN = MCR_OS, /**< Alias for OS/Windows key. */
	/** @brief User-defined mask starting after standard system keys. */
	MCR_MF_USER = MCR_OS << 1,
	/** @brief Mask representing all possible active flags (all bits set). */
	MCR_MF_ANY = ~0u
};

/**
 * @brief Defines logical triggering behaviors for flag sets (modifiers).
 *
 * This dictates how a set of incoming modifiers is checked against an expected
 * combination. Used primarily by action triggers to determine if an event should
 * be processed by receivers.
 */
enum mcr_TriggerMode {
	/*! Trigger only if all provided flags exactly match the input flags. */
	MCR_TM_EQUAL,
	/*! Trigger if all required flags are present in the input set (can have extra flags). */
	MCR_TM_ALL,
	/*! Trigger if absolutely none of the specified flags are present in the input set. */
	MCR_TM_NONE,
	/*! Trigger only if ALL specified flags are present AND no other unexpected flags are present. Mathematically: (lhs | rhs) == lhs && ((lhs | rhs) & ~input) == 0 */
	MCR_TM_EXCLUSIVE,
	/*! Trigger if the input set is not identical to the expected set in any way. */
	MCR_TM_INEQUAL,
	// MCR_TM_EQUAL_OR_NONE,   //! <todo> To be implemented: Matches exactly OR matches nothing.
	// MCR_TM_SOME,             //! <todo> To be implemented: Matches some flags but not all (more complex logic).
	/*! Trigger if at least one specified flag is present in the input set. */
	MCR_TM_MATCH, // Replaced 'MCR_TM_MATCH' with a potential clearer alias/placeholder for context
	/*! Always trigger regardless of incoming modifiers. */
	MCR_TM_ANY,
	/** @brief Starting index for user-defined, extension logic masks. */
	MCR_TM_USER,
	/*! Maximum value used for triggering modes. */
	MCR_TM_MAX = MCR_TM_ANY,
	/*! Total count of defined trigger mode constants. */
	MCR_TM_COUNT = MCR_TM_USER
};

/**
 * @brief Represents a spatial coordinate point in an N-dimensional space.
 *
 * This union provides memory layout flexibility by allowing access as either:
 * 1. A fixed-size array of `long long` for dimension-agnostic indexing (POD guarantee).
 * 2. Explicit named members (x, y, z, w) for common 3D/4D usage patterns.
 * The dimensions are indexed according to mcr_Dimension.
 */
typedef union mcr_SpacePosition {
	long long array
		[MCR_DIMENSION_COUNT]; /**< Array access for any dimension index. */
	struct {
		long long x; /**< X coordinate component. */
		long long y; /**< Y coordinate component. */
		long long z; /**< Z coordinate component. */
		//! @ref mcr_future
		long long w; /**< W (or 4th) coordinate component. */
	} point;
} mcr_Dimensions_t;

/** @brief Macro to initialize a space position to all zeros. */
#define NEW_mcr_SpacePosition { 0 }
/** @brief Alias macro for initializing the main dimension structure. */
#define NEW_mcr_Dimensions NEW_mcr_SpacePosition

#ifdef __cplusplus
}

namespace mcr
{
using Dimension = mcr_Dimension;
using ApplyValue = mcr_ApplyValue;
using Press_t = mcr_Press_t;
using ModFlags = mcr_ModFlags;
using SpacePosition = mcr_SpacePosition;
using TriggerMode = mcr_TriggerMode;
}

#endif

