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
#include "Utils.h"

namespace hueplusplus
{
//! \brief Handles a list of a certain API resource
//! \tparam Resource Resource type that is in the list
//! \tparam IdType Type of the resource id. int or std::string
//!
//! The resources are assumed to be in an object with ids as keys.
//! The Resource class needs a constructor that accepts \c id, HueCommandAPI and \c refreshDuration;
//! otherwise a factory function needs to be provided that takes \c id and the JSON state.
template <typename Resource, typename IdType>
class ResourceList
{
public:
    static_assert(std::is_integral<IdType>::value || std::is_same<std::string, IdType>::value,
        "IdType must be integral or string");

    //! \brief Construct ResourceList using a base cache and optional factory function
    //! \param baseCache Base cache which holds the parent state, not nullptr
    //! \param cacheEntry Entry name of the list state in the base cache
    //! \param refreshDuration Interval between refreshing the cache
    //! \param factory Optional factory function to create Resources.
    //! Necessary if Resource is not constructible as described above.
    ResourceList(std::shared_ptr<APICache> baseCache, const std::string& cacheEntry,
        std::chrono::steady_clock::duration refreshDuration,
        const std::function<Resource(IdType, const nlohmann::json&)>& factory = nullptr)
        : stateCache(baseCache, cacheEntry, refreshDuration), factory(factory), path(stateCache.getRequestPath() + '/')
    {}
    //! \brief Construct ResourceList with a separate cache and optional factory function
    //! \param commands HueCommandAPI for requests
    //! \param path Path of the resource list
    //! \param refreshDuration Interval between refreshing the cache
    //! \param factory Optional factory function to create Resources.
    //! Necessary if Resource is not constructible as described above.
    ResourceList(const HueCommandAPI& commands, const std::string& path,
        std::chrono::steady_clock::duration refreshDuration,
        const std::function<Resource(IdType, const nlohmann::json&)>& factory = nullptr)
        : stateCache(path, commands, refreshDuration), factory(factory), path(path + '/')
    {}

    //! \brief Deleted copy constructor
    ResourceList(const ResourceList&) = delete;
    //! \brief Defaulted move constructor
    ResourceList(ResourceList&&) noexcept = default;
    //! \brief Deleted copy assignment
    ResourceList& operator=(const ResourceList&) = delete;
    //! \brief Defaulted move assignment
    ResourceList& operator=(ResourceList&&) = default;

    //! \brief Refreshes internal state now
    void refresh() { stateCache.refresh(); }

    //! \brief Get all resources that exist
    //! \returns A vector of references to every Resource
    //! \throws std::system_error when system or socket operations fail
    //! \throws HueException when response contains no body
    //! \throws HueAPIResponseException when response contains an error
    //! \throws nlohmann::json::parse_error when response could not be parsed
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

    //! \brief Get resource specified by id
    //! \param id Identifier of the resource
    //! \returns The resource matching the id
    //! \throws std::system_error when system or socket operations fail
    //! \throws HueException when id does not exist
    //! \throws HueAPIResponseException when response contains an error
    //! \throws nlohmann::json::parse_error when response could not be parsed
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

    //! \brief Checks whether resource with id exists
    //! \param id Identifier of the resource to check
    //! \returns true when the resource with given id exists
    //! \throws std::system_error when system or socket operations fail
    //! \throws HueException when response contains no body
    //! \throws HueAPIResponseException when response contains an error
    //! \throws nlohmann::json::parse_error when response could not be parsed
    bool exists(const IdType& id)
    {
        auto pos = resources.find(id);
        if (pos != resources.end())
        {
            return true;
        }
        return stateCache.getValue().count(maybeToString(id)) != 0;
    }

    //! \brief Checks whether resource with id exists
    //! \param id Identifier of the resource to check
    //! \returns true when the resource with given id exists
    //! \note This will not update the cache
    //! \throws HueException when the cache is empty
    bool exists(const IdType& id) const
    {
        auto pos = resources.find(id);
        if (pos != resources.end())
        {
            return true;
        }
        return stateCache.getValue().count(maybeToString(id)) != 0;
    }

    //! \brief Removes the resource
    //! \param id Identifier of the resource to remove
    //! \returns true on success
    //! \throws std::system_error when system or socket operations fail
    //! \throws HueException when response contains no body
    //! \throws HueAPIResponseException when response contains an error
    //! \throws nlohmann::json::parse_error when response could not be parsed
    //!
    //! If successful, invalidates references to the Resource removed.
    bool remove(const IdType& id)
    {
        std::string requestPath = path + maybeToString(id);
        nlohmann::json result = stateCache.getCommandAPI().DELETERequest(
            requestPath, nlohmann::json::object(), FileInfo {__FILE__, __LINE__, __func__});
        bool success = utils::safeGetMember(result, 0, "success") == requestPath + " deleted";
        auto it = resources.find(id);
        if (success && it != resources.end())
        {
            resources.erase(it);
        }
        return success;
    }

protected:
    //! \brief Calls std::stoi if IdType is int
    static IdType maybeStoi(const std::string& key) { return maybeStoi(key, std::is_integral<IdType> {}); }

    //! \brief Calls std::to_string if IdType is int
    static std::string maybeToString(const IdType& id) { return maybeToString(id, std::is_integral<IdType> {}); }

    //! \brief Constructs resource using factory or constructor, if available
    //! \throws HueException when factory is nullptr and Resource cannot be constructed as specified above.
    Resource construct(const IdType& id, const nlohmann::json& state)
    {
        return construct(
            id, state, std::is_constructible<Resource, IdType, HueCommandAPI, std::chrono::steady_clock::duration> {});
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
            return Resource(id, stateCache.getCommandAPI(), stateCache.getRefreshDuration());
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
    APICache stateCache;
    std::function<Resource(IdType, const nlohmann::json&)> factory;
    std::string path;
    std::map<IdType, Resource> resources;
};

//! \brief Handles a ResourceList where Resources can be added by the user
//! \tparam Resource Resource type that is in the list
//! \tparam IdType Type of the resource id. int or std::string
//! \tparam CreateType Type that provides parameters for creation.
//! Must have a const getRequest() function returning the JSON for the POST request.
template <typename Resource, typename IdType, typename CreateType>
class CreateableResourceList : public ResourceList<Resource, IdType>
{
public:
    using ResourceList<Resource, IdType>::ResourceList;

    //! \brief Create a new resource
    //! \param params Parameters for the new resource
    //! \returns The id of the created resource or 0/an empty string if failed.
    //! \throws std::system_error when system or socket operations fail
    //! \throws HueException when response contains no body
    //! \throws HueAPIResponseException when response contains an error
    //! \throws nlohmann::json::parse_error when response could not be parsed
    //! \throws std::invalid_argument when IdType is int and std::stoi fails
    IdType create(const CreateType& params)
    {
        std::string requestPath = this->path;
        // Remove slash
        requestPath.pop_back();
        nlohmann::json response = this->stateCache.getCommandAPI().POSTRequest(
            requestPath, params.getRequest(), FileInfo {__FILE__, __LINE__, __func__});
        nlohmann::json id = utils::safeGetMember(response, 0, "success", "id");
        if (id.is_string())
        {
            std::string idStr = id.get<std::string>();
            if (idStr.find(this->path) == 0)
            {
                idStr.erase(0, this->path.size());
            }
            this->stateCache.refresh();
            return this->maybeStoi(idStr);
        }
        return IdType {};
    }
};

template <typename Resource, typename CreateType>
class GroupResourceList : public CreateableResourceList<Resource, int, CreateType>
{
public:
    using CreateableResourceList<Resource, int, CreateType>::CreateableResourceList;
    //! \brief Get group, specially handles group 0
    //! \see ResourceList::get
    Resource& get(const int& id)
    {
        auto pos = this->resources.find(id);
        if (pos != this->resources.end())
        {
            pos->second.refresh();
            return pos->second;
        }
        const nlohmann::json& state = this->stateCache.getValue();
        std::string key = this->maybeToString(id);
        if (!state.count(key) && id != 0)
        {
            throw HueException(FileInfo {__FILE__, __LINE__, __func__}, "Resource id is not valid");
        }
        return this->resources.emplace(id, this->construct(id, state[key])).first->second;
    }
    //! \brief Get group, specially handles group 0
    //! \see ResourceList::exists
    bool exists(int id) const { return id == 0 || CreateableResourceList<Resource, int, CreateType>::exists(id); }
};
} // namespace hueplusplus

#endif
