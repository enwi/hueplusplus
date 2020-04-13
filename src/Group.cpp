#include "hueplusplus/Group.h"

#include "hueplusplus/HueExceptionMacro.h"

hueplusplus::Group::Group(int id, const HueCommandAPI& commands)
    : id(id), state("/groups/" + std::to_string(id), commands, std::chrono::seconds(10)), commands(commands)
{}

int hueplusplus::Group::getId() const
{
    return id;
}

std::string hueplusplus::Group::getName() const
{
    return state.GetValue().at("name").get<std::string>();
}

std::string hueplusplus::Group::getType() const
{
    return state.GetValue().at("type").get<std::string>();
}

std::vector<int> hueplusplus::Group::getLightIds() const
{
    const nlohmann::json& lights = state.GetValue().at("lights");
    std::vector<int> ids(lights.size());
    for (const nlohmann::json& id : lights)
    {
        ids.push_back(std::stoi(id.get<std::string>()));
    }
    return ids;
}

void hueplusplus::Group::setName(const std::string& name)
{
    nlohmann::json request = {{"name", name}};
    SendPutRequest(request, "", CURRENT_FILE_INFO);
}

void hueplusplus::Group::setLights(const std::vector<int>& ids)
{
    nlohmann::json lights = nlohmann::json::array();
    for (int id : ids)
    {
        lights.push_back(std::to_string(id));
    }
    SendPutRequest({{"lights", lights}}, "", CURRENT_FILE_INFO);
}

bool hueplusplus::Group::getAllOn() const
{
    return state.GetValue().at("state").at("all_on").get<bool>();
}

bool hueplusplus::Group::getAnyOn() const
{
    return state.GetValue().at("state").at("any_on").get<bool>();
}

bool hueplusplus::Group::getActionOn() const
{
    return state.GetValue().at("action").at("on").get<bool>();
}

std::pair<uint16_t, uint8_t> hueplusplus::Group::getActionHueSaturation() const
{
    const nlohmann::json& action = state.GetValue().at("action");

    return std::make_pair(action.at("hue").get<int>(), action.at("sat").get<int>());
}

unsigned int hueplusplus::Group::getActionBrightness() const
{
    return state.GetValue().at("action").at("bri").get<int>();
}

unsigned int hueplusplus::Group::getActionColorTemperature() const
{
    return state.GetValue().at("action").at("ct").get<int>();
}

std::pair<float, float> hueplusplus::Group::getActionColorXY() const
{
    const nlohmann::json& xy = state.GetValue().at("action").at("xy");
    return std::pair<float, float>(xy[0].get<float>(), xy[1].get<float>());
}

std::string hueplusplus::Group::getActionColorMode() const
{
    return state.GetValue().at("action").at("colormode").get<std::string>();
}

void hueplusplus::Group::setOn(bool on, uint8_t transition)
{
    nlohmann::json request = {{"on", on}};
    if (transition != 4)
    {
        request["transition"] = transition;
    }
    SendPutRequest(request, "/action", CURRENT_FILE_INFO);
}

void hueplusplus::Group::setBrightness(uint8_t brightness, uint8_t transition)
{
    // TODO: turn on/off
    nlohmann::json request = {{"bri", brightness}};
    if (transition != 4)
    {
        request["transition"] = transition;
    }
    SendPutRequest(request, "/action", CURRENT_FILE_INFO);
}

void hueplusplus::Group::setColorHueSaturation(uint16_t hue, uint8_t saturation, uint8_t transition)
{
    nlohmann::json request = {{"hue", hue}, {"sat", saturation}};
    if (transition != 4)
    {
        request["transition"] = transition;
    }
    SendPutRequest(request, "/action", CURRENT_FILE_INFO);
}

void hueplusplus::Group::setColorXY(float x, float y, uint8_t transition)
{
    nlohmann::json request = {{"xy", {x, y}}};
    if (transition != 4)
    {
        request["transition"] = transition;
    }
    SendPutRequest(request, "/action", CURRENT_FILE_INFO);
}

void hueplusplus::Group::setColorTemperature(unsigned int mired, uint8_t transition)
{
    nlohmann::json request = {{"ct", mired}};
    if (transition != 4)
    {
        request["transition"] = transition;
    }
    SendPutRequest(request, "/action", CURRENT_FILE_INFO);
}

void hueplusplus::Group::setColorLoop(bool on, uint8_t transition)
{
    nlohmann::json request = {{"effect", on ? "colorloop" : "none"}};
    if (transition != 4)
    {
        request["transition"] = transition;
    }
    SendPutRequest(request, "/action", CURRENT_FILE_INFO);
}

void hueplusplus::Group::incrementBrightness(int increment, uint8_t transition)
{
    nlohmann::json request = {{"bri_inc", increment}};
    if (transition != 4)
    {
        request["transition"] = transition;
    }
    SendPutRequest(request, "/action", CURRENT_FILE_INFO);
}

void hueplusplus::Group::incrementSaturation(int increment, uint8_t transition)
{
    nlohmann::json request = {{"sat_inc", increment}};
    if (transition != 4)
    {
        request["transition"] = transition;
    }
    SendPutRequest(request, "/action", CURRENT_FILE_INFO);
}

void hueplusplus::Group::incrementHue(int increment, uint8_t transition)
{
    nlohmann::json request = {{"hue_inc", increment}};
    if (transition != 4)
    {
        request["transition"] = transition;
    }
    SendPutRequest(request, "/action", CURRENT_FILE_INFO);
}

void hueplusplus::Group::incrementColorTemperature(int increment, uint8_t transition)
{
    nlohmann::json request = {{"ct_inc", increment}};
    if (transition != 4)
    {
        request["transition"] = transition;
    }
    SendPutRequest(request, "/action", CURRENT_FILE_INFO);
}

void hueplusplus::Group::incrementColorXY(float increment, uint8_t transition)
{
    nlohmann::json request = {{"xy_inc", increment}};
    if (transition != 4)
    {
        request["transition"] = transition;
    }
    SendPutRequest(request, "/action", CURRENT_FILE_INFO);
}

void hueplusplus::Group::setScene(const std::string& scene, uint8_t transition)
{
    nlohmann::json request = {{"scene", scene}};
    if (transition != 4)
    {
        request["transition"] = transition;
    }
    SendPutRequest(request, "/action", CURRENT_FILE_INFO);
}

nlohmann::json hueplusplus::Group::SendPutRequest(
    const nlohmann::json& request, const std::string& subPath, FileInfo fileInfo)
{
    return commands.PUTRequest("/groups/" + std::to_string(id) + subPath, request, std::move(fileInfo));
}

std::string hueplusplus::Group::getRoomType() const
{
    return state.GetValue().at("class").get<std::string>();
}

void hueplusplus::Group::setRoomType(const std::string& type)
{
    SendPutRequest({{"class", type}}, "", CURRENT_FILE_INFO);
}
