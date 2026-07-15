/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

/*! @file mcr/template/list.h
 * @brief Defines template list container abstraction for arrays.
 */

#pragma once

#include "mcr/defines.h"
#include "mcr/error.h"

#ifdef __cplusplus

namespace mcr
{

/**
 * @brief Lightweight non-owning view over a C-style array.
 *
 *  @tparam ElementT Element type stored in the array.
 */
template <typename ElementT> struct List {
	/** @brief Pointer to the array data. */
	ElementT *array;
	/** @brief Number of elements in the array. */
	const mcr_index_t count;

	/** @brief Construct a list.
	 *  @param array Pointer to array data (default nullptr).
	 *  @param count Number of elements (default 0).
	 */
	List(ElementT *array = nullptr, const mcr_index_t count = 0)
		: array(array)
		, count(count)
	{
		validate();
	}
	List(const List &) = default;
	virtual ~List() = default;
	List &operator=(const List &) = default;

	/** @brief Access element by index (mutable).
	 *  @param index Element index.
	 *  @return Reference to the element.
	 *  @throws Error(ERANGE) if index >= count.
	 */
	ElementT &operator[](const mcr_index_t index)
	{
		if (index >= count)
			throw Error(ERANGE, "Index out of range");
		return array[index];
	}

	/** @brief Access element by index (const).
	 *  @param index Element index.
	 *  @return Const reference to the element.
	 *  @throws Error(ERANGE) if index >= count.
	 */
	const ElementT &operator[](const mcr_index_t index) const
	{
		if (index >= count)
			throw Error(ERANGE, "Index out of range");
		return array[index];
	}

	/** @brief Pointer arithmetic: add offset to array base.
	 *  @param index Offset from array start.
	 *  @return Pointer to the element at the given offset.
	 *  @throws Error(ERANGE) if index >= count.
	 */
	ElementT *operator+(const mcr_index_t index) const
	{
		if (index >= count)
			throw Error(ERANGE, "Index out of range");
		return array + index;
	}

	/** @brief Stream the list contents for debugging.
	 *  @tparam OutputStream Output stream type.
	 *  @param os Output stream.
	 *  @param list List to output.
	 *  @return Reference to the output stream.
	 */
	template <typename OutputStream>
	friend OutputStream &operator<<(OutputStream &os,
					const List<ElementT> &list)
	{
		os << "[ ";
		for (mcr_index_t i = 0; i < list.count; ++i) {
			os << list[i];
			if (i < list.count - 1) {
				os << ", ";
			}
		}
		os << " ]";
		return os;
	}

	/** @brief Validate list consistency.
	 *  @throws Error(EINVAL) if array and count are inconsistent.
	 */
	void validate() const
	{
		if (!array != !count)
			throw Error(EINVAL, "Invalid list state");
	}

	/** @brief Check if the list is empty.
	 *  @return true if count is zero.
	 */
	bool empty() const
	{
		return !count;
	}
};
template struct MCR_API List<const char *>;
template struct MCR_API List<int>;
template struct MCR_API List<unsigned int>;
/** @brief List of C strings. */
typedef List<const char *> CStringList;
/** @brief List of integers. */
typedef List<int> IntList;
/** @brief List of unsigned integers. */
typedef List<unsigned int> UIntList;
} // namespace mcr
#endif

