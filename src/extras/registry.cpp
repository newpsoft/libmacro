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

#include "mcr/libmacro.h"

#include <map>
#include <vector>

#include "mcr/extras/util/string_less_ci.h"

#define priv _private

namespace mcr
{
class RegistryPrivate
{
	friend struct Registry;
public:
	std::vector<mcr_Interface *> interfaces;
	std::map<mcr_Interface *, std::string> interfaceMap;
	std::map<std::string, mcr_Interface *, string_less_ci> nameMap;
};

Registry::Registry()
	: priv(new RegistryPrivate)
{
	mcr_IRegistry *iregPt = &self;
	iregPt->deinit = &Registry::deinit;
	iregPt->id = &Registry::id;
	iregPt->from_id = &Registry::from_id;
	iregPt->name = &Registry::name;
	iregPt->from_name = &Registry::from_name;
	iregPt->set_name = &Registry::set_name;
	iregPt->add_names = &Registry::add_names;
	iregPt->remove = &Registry::remove;
	iregPt->remove_name = &Registry::remove_name;
	iregPt->count = &Registry::count;
	iregPt->trim = &Registry::trim;
	iregPt->clear = &Registry::clear;
}

Registry::~Registry()
{
	unregister();
	delete priv;
}

size_t Registry::id(struct mcr_Interface *ifacePt) const
{
	size_t id = ifacePt->id;
	if (id < priv->interfaces.size() && priv->interfaces[ifacePt->id] == ifacePt)
		return ifacePt->id;
	for (id = 0; id < priv->interfaces.size(); id++) {
		if (priv->interfaces[id] == ifacePt) {
			ifacePt->id = id;
			return id;
		}
	}
	return (size_t)~0;
}

struct mcr_Interface *Registry::interface(size_t id) const
{
	if (id < priv->interfaces.size())
		return priv->interfaces[id];
	return nullptr;
}

const char *Registry::name(struct mcr_Interface *ifacePt) const
{
	auto found = priv->interfaceMap.find(ifacePt);
	return found == priv->interfaceMap.end() ? nullptr : found->second.c_str();
	return nullptr;
}

struct mcr_Interface *Registry::interface(const char *name) const
{
	if (!name)
		return nullptr;
	auto found = priv->nameMap.find(name);
	return found == priv->nameMap.end() ? nullptr : found->second;
}

size_t Registry::setName(struct mcr_Interface *ifacePt, const char *name)
{
	mcr_throwif(!ifacePt, EFAULT);
	mcr_throwif(!name, EFAULT);
	mcr_throwif(!name[0], EFAULT);
	if (id(ifacePt) == (size_t)~0) {
		ifacePt->id = priv->interfaces.size();
		priv->interfaces.push_back(ifacePt);
	}
	ifacePt->registry = &self;
	priv->interfaceMap[ifacePt] = name;
	priv->nameMap[name] = ifacePt;
	return ifacePt->id;
}

void Registry::addName(struct mcr_Interface *ifacePt, const char *name)
{
	mcr_throwif(!name, EFAULT);
	mcr_throwif(!name[0], EFAULT);
	priv->nameMap[name] = ifacePt;
}

void Registry::addNames(struct mcr_Interface *ifacePt, const char * const*addNames, size_t addNamesCount)
{
	mcr_throwif(addNamesCount && !addNames, EINVAL);
	for (size_t i = 0; i < addNamesCount; i++) {
		if (addNames[i] && addNames[i][0])
			priv->nameMap[addNames[i]] = ifacePt;
	}
}

void Registry::remove(struct mcr_Interface *ifacePt)
{
	mcr_throwif(!ifacePt, EFAULT);
	size_t idFound = id(ifacePt);
	auto mapFound = priv->interfaceMap.find(ifacePt);
	if (mapFound != priv->interfaceMap.end())
		priv->interfaceMap.erase(mapFound);
	for (auto rit = priv->nameMap.rbegin(); rit != priv->nameMap.rend(); rit++) {
		if (rit->second == ifacePt)
			priv->nameMap.erase(rit.base());
	}
	if (idFound != (size_t)~0) {
		priv->interfaces.erase(priv->interfaces.begin() + idFound);
		/* Anything after removed interface is down-shifted. */
		while (idFound < priv->interfaces.size()) {
			--priv->interfaces[idFound]->id;
			++idFound;
		}
	}
	ifacePt->id = (size_t)~0;
	ifacePt->registry = nullptr;
}

void Registry::removeName(const char *removeName)
{
	struct mcr_Interface *ifacePt = interface(removeName);
	if (ifacePt)
		remove(ifacePt);
}

size_t Registry::count() const
{
	return priv->interfaces.size();
}

void Registry::trim()
{
	priv->interfaces.shrink_to_fit();
}

void Registry::clear()
{
	priv->interfaceMap.clear();
	priv->nameMap.clear();
	unregister();
}

void Registry::unregister()
{
	/* Deregister, so they do not try to remove */
	for (auto &iterPt : priv->interfaces) {
		iterPt->registry = nullptr;
		iterPt->id = (size_t)~0;
	}
	priv->interfaces.clear();
}
}
