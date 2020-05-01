/**
    \file ResourceList.h
    Copyright Notice\n
    Copyright (C) 2020  Jan Rogall		- developer\n

    This file is part of hueplusplus.

    hueplusplus is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    hueplusplus is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with hueplusplus.  If not, see <http://www.gnu.org/licenses/>.
**/

#ifndef INCLUDE_RESOURCE_LIST_H
#define INCLUDE_RESOURCE_LIST_H

#include <functional>
#include <map>
#include <string>
#include <vector>

#include "APICache.h"
#include "HueException.h"

namespace hueplusplus
{
template <typename Resource, typename IdType>
class ResourceList
{
public:
    ResourceList(const HueCommandAPI& commands, const std::string& path, std::shared_ptr<APICache> baseCache,
        const std::string& cacheEntry, const std::function<Resource(int, const nlohmann::json&)>& factory = nullptr)
        : commands(commands), stateCache(baseCache, cacheEntry), path(path + '/'), factory(factory)
    {}
    ResourceList(const HueCommandAPI& commands, const std::string& path,
        std::chrono::steady_clock::duration refreshDuration,
        const std::function<Resource(int, const nlohmann::json&)>& factory = nullptr)
        : commands(commands), stateCache(path, commands, refreshDuration), path(path + '/'), factory(factory)
    {}

    ResourceList(const ResourceList&) = delete;
    ResourceList(ResourceList&&) = default;
    ResourceList& operator=(const ResourceList&) = delete;
    ResourceList& operator=(ResourceList&&) = default;

    void refresh() { stateCache.refresh(); }

    std::vector<std::reference_wrapper<Resource>> getAll()
    {
        nlohmann::json state = stateCache.getValue();
        for (auto it = state.begin(); it != state.end(); ++it)
        {
            get(maybeStoi(it.key()));
        }
        std::vector<std::reference_wrapper<Resource>> result;
        result.reserve(state.size());
        for (auto& entry : resources)
        {
            result.emplace_back(entry.second);
        }
        return result;
    }

    Resource& get(const IdType& id)
    {
        auto pos = resources.find(id);
        if (pos != resources.end())
        {
            pos->second.refresh();
            return pos->second;
        }
        const nlohmann::json& state = stateCache.getValue();
        std::string key = maybeToString(id);
        if (!state.count(key))
        {
            throw HueException(FileInfo {__FILE__, __LINE__, __func__}, "Resource id is not valid");
        }
        return resources.emplace(id, construct(id, state[key])).first->second;
    }

    bool exists(const IdType& id) const
    {
        auto pos = resources.find(id);
        if (pos != resources.end())
        {
            return true;
        }
        return stateCache.getValue().count(maybeToString(id)) != 0;
    }

    bool remove(const IdType& id)
    {
        std::string requestPath = path + maybeToString(id);
        nlohmann::json result
            = commands.DELETERequest(requestPath, nlohmann::json::object(), FileInfo {__FILE__, __LINE__, __func__});
        bool success = utils::safeGetMember(result, 0, "success") == requestPath + " deleted";
        auto it = resources.find(id);
        if (success && it != resources.end())
        {
            resources.erase(it);
        }
        return success;
    }

protected:
    static IdType maybeStoi(const std::string& key) { return maybeStoi(key, std::is_integral<IdType> {}); }

    static std::string maybeToString(const IdType& id) { return maybeToString(id, std::is_integral<IdType> {}); }

    Resource construct(const IdType& id, const nlohmann::json& state)
    {
        return construct(id, state, std::is_constructible<Resource, IdType, HueCommandAPI, std::chrono::steady_clock::duration> {});
    }

private:
    // Resource is constructable
    Resource construct(const IdType& id, const nlohmann::json& state, std::true_type)
    {
        if (factory)
        {
            return factory(id, state);
        }
        else
        {
            return Resource(id, commands, stateCache.getRefreshDuration());
        }
    }
    // Resource is not constructable
    Resource construct(const IdType& id, const nlohmann::json& state, std::false_type)
    {
        if (!factory)
        {
            throw HueException(FileInfo {__FILE__, __LINE__, __func__},
                "Resource is not constructable with default parameters, but no factory given");
        }
        return factory(id, state);
    }

private:
    static IdType maybeStoi(const std::string& key, std::true_type) { return std::stoi(key); }
    static IdType maybeStoi(const std::string& key, std::false_type) { return key; }
    static std::string maybeToString(IdType id, std::true_type) { return std::to_string(id); }
    static std::string maybeToString(const IdType& id, std::false_type) { return id; }

protected:
    std::function<Resource(int, const nlohmann::json&)> factory;

    HueCommandAPI commands;
    APICache stateCache;
    std::string path;

    std::map<IdType, Resource> resources;
};

template <typename Resource, typename IdType, typename CreateType>
class CreateableResourceList : public ResourceList<Resource, IdType>
{
public:
    using ResourceList::ResourceList;

    IdType create(const CreateType& params)
    {
        std::string requestPath = path;
        // Remove leading slash
        requestPath.pop_back();
        nlohmann::json response
            = commands.POSTRequest(requestPath, params.getRequest(), FileInfo {__FILE__, __LINE__, __func__});
        nlohmann::json id = utils::safeGetMember(response, 0, "success", "id");
        if (id.is_string())
        {
            std::string idStr = id.get<std::string>();
            if (idStr.find(path) == 0)
            {
                idStr.erase(0, path.size());
            }
            stateCache.refresh();
            return maybeStoi(idStr);
        }
        return IdType {};
    }
};
} // namespace hueplusplus

#endif
