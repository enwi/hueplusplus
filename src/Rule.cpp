/**
    \file Rule.cpp
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

#include <hueplusplus/HueExceptionMacro.h>
#include <hueplusplus/Rule.h>

namespace hueplusplus
{
Condition::Condition(const std::string& address, Operator op, const std::string& value)
    : address(address), op(op), value(value)
{ }
std::string Condition::getAddress() const
{
    return address;
}
Condition::Operator Condition::getOperator() const
{
    return op;
}
std::string Condition::getValue() const
{
    return value;
}

nlohmann::json Condition::toJson() const
{
    std::string opStr;
    switch (op)
    {
    case Operator::eq:
        opStr = "eq";
        break;
    case Operator::gt:
        opStr = "gt";
        break;
    case Operator::lt:
        opStr = "lt";
        break;
    case Operator::dx:
        opStr = "dx";
        break;
    case Operator::ddx:
        opStr = "ddx";
        break;
    case Operator::stable:
        opStr = "stable";
        break;
    case Operator::notStable:
        opStr = "not stable";
        break;
    case Operator::in:
        opStr = "in";
        break;
    case Operator::notIn:
        opStr = "not in";
        break;
    }

    nlohmann::json result = {{"address", address}, {"operator", opStr}};
    if (!value.empty())
    {
        result["value"] = value;
    }
    return result;
}

Condition Condition::parse(const nlohmann::json& json)
{
    std::string address = json.at("address").get<std::string>();
    std::string value = json.value("value", "");
    std::string opStr = json.at("operator").get<std::string>();
    Operator op = Operator::eq;
    if (opStr == "gt")
    {
        op = Operator::gt;
    }
    else if (opStr == "lt")
    {
        op = Operator::lt;
    }
    else if (opStr == "dx")
    {
        op = Operator::dx;
    }
    else if (opStr == "ddx")
    {
        op = Operator::ddx;
    }
    else if (opStr == "stable")
    {
        op = Operator::stable;
    }
    else if (opStr == "not stable")
    {
        op = Operator::notStable;
    }
    else if (opStr == "in")
    {
        op = Operator::in;
    }
    else if (opStr == "not in")
    {
        op = Operator::notIn;
    }
    else
    {
        throw HueException("Unknown condition operator: " + opStr, CURRENT_FILE_INFO);
    }

    return Condition(address, op, value);
}

Rule::Rule(int id, const HueCommandAPI& commands, std::chrono::steady_clock::duration refreshDuration)
    : id(id), state("/rules/" + id, commands, refreshDuration)
{ }

void Rule::refresh(bool force)
{
    if (force)
    {
        state.refresh();
    }
    else
    {
        state.getValue();
    }
}

int Rule::getId() const
{
    return id;
}

std::string Rule::getName() const
{
    return state.getValue().at("name").get<std::string>();
}

time::AbsoluteTime Rule::getCreated() const
{
    return time::AbsoluteTime::parseUTC(state.getValue().at("creationtime").get<std::string>());
}

time::AbsoluteTime Rule::getLastTriggered() const
{
    return time::AbsoluteTime::parseUTC(state.getValue().at("lasttriggered").get<std::string>());
}

int Rule::getTimesTriggered() const
{
    return state.getValue().at("timestriggered").get<int>();
}

bool Rule::isEnabled() const
{
    return state.getValue().at("status").get<std::string>() == "enabled";
}

std::string Rule::getOwner() const
{
    return state.getValue().at("owner").get<std::string>();
}

std::vector<Condition> Rule::getConditions() const
{
    std::vector<Condition> result;
    const nlohmann::json& conditions = state.getValue().at("conditions");
    for (const nlohmann::json& c : conditions)
    {
        result.emplace_back(Condition::parse(c));
    }
    return result;
}

std::vector<Action> Rule::getActions() const
{
    std::vector<Action> result;
    const nlohmann::json& actions = state.getValue().at("actions");
    for (const nlohmann::json& a : actions)
    {
        result.emplace_back(a);
    }
    return result;
}

} // namespace hueplusplus