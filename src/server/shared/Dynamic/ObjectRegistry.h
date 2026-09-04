/*
 * This file is part of the Legends of Azeroth Pandaria Project. See THANKS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TRINITY_OBJECTREGISTRY_H
#define TRINITY_OBJECTREGISTRY_H

#include "Define.h"

#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

/** ObjectRegistry holds all registry item of the same type
 */
template<class T, class Key = std::string>
class ObjectRegistry final
{
    public:
        typedef std::map<Key, std::unique_ptr<T>> RegistryMapType;

        static ObjectRegistry<T, Key>* instance()
        {
            static ObjectRegistry<T, Key> instance;
            return &instance;
        }

        /// Returns a registry item
        const T* GetRegistryItem(Key key) const
        {
            typename RegistryMapType::const_iterator iter = _registeredObjects.find(key);
            return( iter == _registeredObjects.end() ? NULL : iter->second.get() );
        }

        /// Inserts a registry item
        bool InsertItem(T *obj, Key key, bool override = false)
        {
            typename RegistryMapType::iterator iter = _registeredObjects.find(key);
            if ( iter != _registeredObjects.end() )
            {
                if ( !override )
                    return false;
                _registeredObjects.erase(iter);
            }

            _registeredObjects.emplace(std::piecewise_construct, std::forward_as_tuple(key), std::forward_as_tuple(obj));
            return true;
        }

        /// Returns true if registry contains an item
        bool HasItem(Key key) const
        {
            return (_registeredObjects.find(key) != _registeredObjects.end());
        }

        /// Inefficiently return a vector of registered items
        unsigned int GetRegisteredItems(std::vector<Key> &l) const
        {
            unsigned int sz = l.size();
            l.resize(sz + _registeredObjects.size());
            for (typename RegistryMapType::const_iterator iter = _registeredObjects.begin(); iter != _registeredObjects.end(); ++iter)
                l[sz++] = iter->first;
            return _registeredObjects.size();
        }

        /// Return the map of registered items
        RegistryMapType const &GetRegisteredItems() const
        {
            return _registeredObjects;
        }

        ObjectRegistry() { }
        ~ObjectRegistry() { }
        ObjectRegistry(ObjectRegistry const&) = delete;
        ObjectRegistry& operator=(ObjectRegistry const&) = delete;
    private:
        RegistryMapType _registeredObjects;
};

#endif
