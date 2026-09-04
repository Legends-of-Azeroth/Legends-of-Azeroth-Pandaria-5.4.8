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
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TRINITY_CONTAINERS_H
#define TRINITY_CONTAINERS_H

#include "Define.h"
#include "MapUtils.h"
#include <list>
#include <random>
#include <unordered_map>
#include "Util.h"
#include "Random.h"

//! Because circular includes are bad
extern uint32 urand(uint32 min, uint32 max);

namespace Trinity
{
    namespace Containers
    {
        template<class T>
        void RandomResizeList(std::list<T> &list, uint32 size)
        {
            size_t list_size = list.size();

            while (list_size > size)
            {
                typename std::list<T>::iterator itr = list.begin();
                std::advance(itr, urand(0, list_size - 1));
                list.erase(itr);
                --list_size;
            }
        }

        template<class T, class Predicate>
        void RandomResizeList(std::list<T> &list, Predicate& predicate, uint32 size)
        {
            //! First use predicate filter
            std::list<T> listCopy;
            for (typename std::list<T>::iterator itr = list.begin(); itr != list.end(); ++itr)
                if (predicate(*itr))
                    listCopy.push_back(*itr);

            if (size)
                RandomResizeList(listCopy, size);

            list = listCopy;
        }

        /* Select a random element from a container. Note: make sure you explicitly empty check the container */
        template <class C> typename C::value_type const& SelectRandomContainerElement(C const& container)
        {
            typename C::const_iterator it = container.begin();
            std::advance(it, urand(0, container.size() - 1));
            return *it;
        }

        template<class Iterator1, class Iterator2>
        bool Intersects(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2)
        {
            while (first1 != last1 && first2 != last2)
            {
                if (*first1 < *first2)
                    ++first1;
                else if (*first2 < *first1)
                    ++first2;
                else
                    return true;
            }

            return false;
        }

        /*
         * Select a random element from a container where each element has a different chance to be selected.
         *
         * @param container Container to select an element from
         * @param weights Chances of each element to be selected, must be in the same order as elements in container
         *
         * Note: container cannot be empty
         */
        template <class C>
        typename C::const_iterator SelectRandomWeightedContainerElement(C const& container, std::vector<double> const& weights)
        {
            std::discrete_distribution<uint32>::param_type ddParam(weights.begin(), weights.end());
            std::discrete_distribution<uint32> dd(ddParam);
            typename C::const_iterator it = container.begin();
            std::advance(it, dd(RandomEngine::Instance()));
            return it;
        }

        /*
         * Select a random element from a container where each element has a different chance to be selected.
         *
         * @param container Container to select an element from
         * @param weights Chances of each element to be selected, must be in the same order as elements in container.
         *                Caller is responsible for checking that sum of all weights is greater than 0.
         *
         * Note: container cannot be empty
         */
        template <class C>
        typename C::const_iterator SelectRandomWeightedContainerElement(C const& container, std::function<double(typename C::value_type const&)> const& weightExtractor)
        {
            std::vector<double> weights;
            weights.reserve(container.size());
            double weightSum = 0.0;
            for (auto itr = container.begin(); itr != container.end(); ++itr)
            {
                double weight = weightExtractor(*itr);
                weights.push_back(weight);
                weightSum += weight;
            }
            if (weightSum <= 0.0)
                weights.assign(container.size(), 1.0);

            return SelectRandomWeightedContainerElement(container, weights);
        }

        // Helper to look up a value in a map by key, returning a pointer (or nullptr)
        template<class K, class V>
        V* MapGetValuePtr(std::unordered_map<K, V>& map, K const& key)
        {
            auto it = map.find(key);
            return it != map.end() ? &it->second : nullptr;
        }

        template<class K, class V>
        V const* MapGetValuePtr(std::unordered_map<K, V> const& map, K const& key)
        {
            auto it = map.find(key);
            return it != map.end() ? &it->second : nullptr;
        }
    }
    //! namespace Containers
}
//! namespace Trinity

#endif //! #ifdef TRINITY_CONTAINERS_H
