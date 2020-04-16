#include "hueplusplus/Group.h"

#include "hueplusplus/HueExceptionMacro.h"

namespace hueplusplus
{
Group::Group(int id, const HueCommandAPI& commands, std::chrono::steady_clock::duration refreshDuration)
    : id(id), state("/groups/" + std::to_string(id), commands, refreshDuration), commands(commands)
{
    state.Refresh();
}

void Group::Refresh()
{
    state.Refresh();
}

int Group::getId() const
{
    return id;
}

std::string Group::getName() const
{
    return state.GetValue().at("name").get<std::string>();
}

std::string Group::getType() const
{
    return state.GetValue().at("type").get<std::string>();
}

std::vector<int> Group::getLightIds() const
{
    const nlohmann::json& lights = state.GetValue().at("lights");
    std::vector<int> ids;
    ids.reserve(lights.size());
    for (const nlohmann::json& id : lights)
    {
        ids.push_back(std::stoi(id.get<std::string>()));
    }
    return ids;
}

void Group::setName(const std::string& name)
{
    nlohmann::json request = {{"name", name}};
    SendPutRequest(request, "", CURRENT_FILE_INFO);
}

void Group::setLights(const std::vector<int>& ids)
{
    nlohmann::json lights = nlohmann::json::array();
    for (int id : ids)
    {
        lights.push_back(std::to_string(id));
    }
    SendPutRequest({{"lights", lights}}, "", CURRENT_FILE_INFO);
}

bool Group::getAllOn()
{
    return state.GetValue().at("state").at("all_on").get<bool>();
}
bool Group::getAllOn() const
{
    return state.GetValue().at("state").at("all_on").get<bool>();
}

bool Group::getAnyOn()
{
    return state.GetValue().at("state").at("any_on").get<bool>();
}
bool Group::getAnyOn() const
{
    return state.GetValue().at("state").at("any_on").get<bool>();
}

bool Group::getActionOn()
{
    return state.GetValue().at("action").at("on").get<bool>();
}
bool Group::getActionOn() const
{
    return state.GetValue().at("action").at("on").get<bool>();
}

std::pair<uint16_t, uint8_t> Group::getActionHueSaturation()
{
    const nlohmann::json& action = state.GetValue().at("action");

    return std::make_pair(action.at("hue").get<int>(), action.at("sat").get<int>());
}
std::pair<uint16_t, uint8_t> Group::getActionHueSaturation() const
{
    const nlohmann::json& action = state.GetValue().at("action");

    return std::make_pair(action.at("hue").get<int>(), action.at("sat").get<int>());
}

unsigned int Group::getActionBrightness()
{
    return state.GetValue().at("action").at("bri").get<int>();
}
unsigned int Group::getActionBrightness() const
{
    return state.GetValue().at("action").at("bri").get<int>();
}

unsigned int Group::getActionColorTemperature()
{
    return state.GetValue().at("action").at("ct").get<int>();
}
unsigned int Group::getActionColorTemperature() const
{
    return state.GetValue().at("action").at("ct").get<int>();
}

std::pair<float, float> Group::getActionColorXY()
{
    const nlohmann::json& xy = state.GetValue().at("action").at("xy");
    return std::pair<float, float>(xy[0].get<float>(), xy[1].get<float>());
}
std::pair<float, float> Group::getActionColorXY() const
{
    const nlohmann::json& xy = state.GetValue().at("action").at("xy");
    return std::pair<float, float>(xy[0].get<float>(), xy[1].get<float>());
}

std::string Group::getActionColorMode()
{
    return state.GetValue().at("action").at("colormode").get<std::string>();
}
std::string Group::getActionColorMode() const
{
    return state.GetValue().at("action").at("colormode").get<std::string>();
}

StateTransaction Group::transaction()
{
    // Do not pass state, because it is not the state of ALL lights in the group
    return StateTransaction(commands, "/groups/" + std::to_string(id) + "/action", nlohmann::json::object());
}

void Group::setOn(bool on, uint8_t transition)
{
    nlohmann::json request = {{"on", on}};
    if (transition != 4)
    {
        request["transition"] = transition;
    }
    SendPutRequest(request, "/action", CURRENT_FILE_INFO);
}

void Group::setBrightness(uint8_t brightness, uint8_t transition)
{
    transaction().setBrightness(brightness).setTransition(transition).commit();
}

void Group::setColorHueSaturation(uint16_t hue, uint8_t saturation, uint8_t transition)
{
    transaction().setColorHue(hue).setColorSaturation(saturation).setTransition(transition).commit();
}

void Group::setColorXY(float x, float y, uint8_t transition)
{
    transaction().setColorXY(x, y).setTransition(transition).commit();
}

void Group::setColorTemperature(unsigned int mired, uint8_t transition)
{
    transaction().setColorTemperature(mired).setTransition(transition).commit();
}

void Group::setColorLoop(bool on, uint8_t transition)
{
    transaction().setColorLoop(on).setTransition(transition);
}

void Group::incrementBrightness(int increment, uint8_t transition)
{
    transaction().incrementBrightness(increment).setTransition(transition).commit();
}

void Group::incrementSaturation(int increment, uint8_t transition)
{
    transaction().incrementSaturation(increment).setTransition(transition).commit();
}

void Group::incrementHue(int increment, uint8_t transition)
{
    transaction().incrementHue(increment).setTransition(transition).commit();
}

void Group::incrementColorTemperature(int increment, uint8_t transition)
{
    transaction().incrementColorTemperature(increment).setTransition(transition).commit();
}

void Group::incrementColorXY(float incX, float incY, uint8_t transition)
{
    transaction().incrementColorXY(incX, incY).setTransition(transition).commit();
}

void Group::setScene(const std::string& scene, uint8_t transition)
{
    SendPutRequest({{"scene", scene}}, "/action", CURRENT_FILE_INFO);
}

nlohmann::json Group::SendPutRequest(const nlohmann::json& request, const std::string& subPath, FileInfo fileInfo)
{
    return commands.PUTRequest("/groups/" + std::to_string(id) + subPath, request, std::move(fileInfo));
}

std::string Group::getRoomType() const
{
    return state.GetValue().at("class").get<std::string>();
}

void Group::setRoomType(const std::string& type)
{
    SendPutRequest({{"class", type}}, "", CURRENT_FILE_INFO);
}

std::string Group::getModelId() const
{
    return state.GetValue().at("modelid").get<std::string>();
}

std::string Group::getUniqueId() const
{
    return state.GetValue().at("uniqueid").get<std::string>();
}
} // namespace hueplusplus
