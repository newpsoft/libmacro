/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*! \file
 *  \brief String contract registry for a primitive type.
 */

#ifndef __cplusplus
	#pragma message "C++ support is required for extras module"
	#include "mcr/err.h"
#endif

#ifndef MCR_EXTRAS_REGISTRY_H_
#define MCR_EXTRAS_REGISTRY_H_

#include "mcr/extras/base_cpp.h"

namespace mcr
{
/*! Non-exportable members */
class RegistryPrivate;
struct MCR_API Registry
{
	friend class RegistryPrivate;
	mcr_IRegistry self;

	Registry();
	Registry(const Registry &copytron) = delete;
	virtual ~Registry();
	Registry &operator =(const Registry &copytron) = delete;

	inline mcr_IRegistry &operator *()
	{
		return self;
	}
	inline const mcr_IRegistry &operator *() const
	{
		return self;
	}
	static inline Registry *offset(mcr_IRegistry *originPt)
	{
		return mcr::offset<Registry>(originPt);
	}
	static inline const Registry *offset(const mcr_IRegistry *originPt)
	{
		return mcr::offset<Registry>(originPt);
	}
//	static inline const Registry *self(const mcr_IRegistry *originPt)
//	{
//		return (const Registry *)originPt;
//	}

	virtual size_t id(struct mcr_Interface *ifacePt) const;
	virtual struct mcr_Interface *interface(size_t id) const;
	virtual const char *name(struct mcr_Interface *ifacePt) const;
	virtual struct mcr_Interface *interface(const char *name) const;
	virtual size_t setName(struct mcr_Interface *ifacePt, const char *name);
	virtual void addName(struct mcr_Interface *ifacePt, const char *name);
	virtual void addNames(struct mcr_Interface *ifacePt, const char * const *addNames, size_t addNamesCount);
	virtual void remove(struct mcr_Interface *ifacePt);
	virtual void removeName(const char *removeName);
	virtual size_t count() const;
	virtual void trim();
	virtual void clear();
	/* Remove all interfaces registerd, but do not modify name mapping. */
	virtual void unregister();

private:
	/* non-export */
	RegistryPrivate *_private;

	static void deinit(struct mcr_IRegistry *iregPt)
	{
		if (iregPt)
			offset(iregPt)->unregister();
	}
	static size_t id(const struct mcr_IRegistry *iregPt, struct mcr_Interface *ifacePt)
	{
		return iregPt ? offset(iregPt)->id(ifacePt) : (size_t)~0;
	}
	static struct mcr_Interface *from_id(const struct mcr_IRegistry *iregPt, size_t id)
	{
		return iregPt ? offset(iregPt)->interface(id) : nullptr;
	}
	static const char *name(const struct mcr_IRegistry *iregPt, struct mcr_Interface *ifacePt)
	{
		return iregPt ? offset(iregPt)->name(ifacePt) : nullptr;
	}
	static struct mcr_Interface *from_name(const struct mcr_IRegistry *iregPt, const char *name)
	{
		return iregPt ? offset(iregPt)->interface(name) : nullptr;
	}
	static size_t set_name(struct mcr_IRegistry *iregPt, struct mcr_Interface *ifacePt, const char *name)
	{
		mcr_dassert(iregPt);
		if (!iregPt) {
			mcr_err = EFAULT;
			return (size_t)~0;
		}
		try {
			return offset(iregPt)->setName(ifacePt, name);
		} catch (int e) {
			mcr_err = e;
			return (size_t)~0;
		}
		return (size_t)~0;
	}
	static int add_names(struct mcr_IRegistry *iregPt, struct mcr_Interface *ifacePt, const char * const*addNames, size_t addNamesCount)
	{
		mcr_dassert(iregPt);
		if (!iregPt)
			return EFAULT;
		try {
			offset(iregPt)->addNames(ifacePt, addNames, addNamesCount);
		} catch (int e) {
			return e;
		}
		return 0;
	}
	static void remove(struct mcr_IRegistry *iregPt, struct mcr_Interface *ifacePt)
	{
		if (iregPt)
			offset(iregPt)->remove(ifacePt);
	}
	static void remove_name(struct mcr_IRegistry *iregPt, const char *removeName)
	{
		if (iregPt)
			offset(iregPt)->removeName(removeName);
	}
	static size_t count(const struct mcr_IRegistry *iregPt)
	{
		return iregPt ? offset(iregPt)->count() : 0;
	}
	static void trim(struct mcr_IRegistry *iregPt)
	{
		if (iregPt)
			offset(iregPt)->trim();
	}
	static void clear(struct mcr_IRegistry *iregPt)
	{
		if (iregPt)
			offset(iregPt)->clear();
	}
};
}

#endif
