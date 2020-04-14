#include "hueplusplus/Group.h"

#include "hueplusplus/HueExceptionMacro.h"

hueplusplus::StateTransaction::StateTransaction(
    const HueCommandAPI& commands, const std::string& path, const nlohmann::json& currentState)
    : commands(commands), path(path), state(state), request(nlohmann::json::object())
{}

bool hueplusplus::StateTransaction::commit() &&
{
    // Empty request or request with only transition makes no sense
    if (!request.empty() || (request.size() == 1 && request.count("transition")))
    {
        if (!request.count("on"))
        {
            if (request.value("bri", 254) == 0)
            {
                // Turn off if brightness is 0
                request["on"] = false;
            }
            else if (request.value("bri", 0) != 0 || request.count("colorloop") || request.count("hue")
                || request.count("sat") || request.count("xy"))
            {
                // Turn on if it was turned off
                request["on"] = true;
            }
        }

        commands.PUTRequest(path, request, CURRENT_FILE_INFO);
        return true;
    }
    return false;
}

hueplusplus::StateTransaction&& hueplusplus::StateTransaction::setOn(bool on) &&
{
    request["on"] = on;
    return std::move(*this);
}

hueplusplus::StateTransaction&& hueplusplus::StateTransaction::setBrightness(uint8_t brightness) &&
{
    request["bri"] = std::min<uint8_t>(brightness, 254);
    return std::move(*this);
}

hueplusplus::StateTransaction&& hueplusplus::StateTransaction::setColorSaturation(uint8_t saturation) &&
{
    request["sat"] = std::min<uint8_t>(saturation, 254);
    return std::move(*this);
}

hueplusplus::StateTransaction&& hueplusplus::StateTransaction::setColorHue(uint16_t hue) &&
{
    request["hue"] = hue;
    return std::move(*this);
}

hueplusplus::StateTransaction&& hueplusplus::StateTransaction::setColorHueSaturation(
    uint16_t hue, uint8_t saturation) &&
{
    request["hue"] = hue;
    request["sat"] = std::min<uint8_t>(saturation, 254);
    return std::move(*this);
}

hueplusplus::StateTransaction&& hueplusplus::StateTransaction::setColorXY(float x, float y) &&
{
    request["xy"] = {x, y};
    return std::move(*this);
}

hueplusplus::StateTransaction&& hueplusplus::StateTransaction::setColorTemperature(unsigned int mired) &&
{
    request["ct"] = mired;
    return std::move(*this);
}

hueplusplus::StateTransaction&& hueplusplus::StateTransaction::setColorLoop(bool on) &&
{
    request["effect"] = on ? "colorloop" : "none";
    return std::move(*this);
}

hueplusplus::StateTransaction&& hueplusplus::StateTransaction::incrementBrightness(int increment) &&
{
    request["bri_inc"] = std::max(-254, std::min(increment, 254));
    return std::move(*this);
}

hueplusplus::StateTransaction&& hueplusplus::StateTransaction::incrementSaturation(int increment) &&
{
    request["sat_inc"] = std::max(-254, std::min(increment, 254));
    return std::move(*this);
}

hueplusplus::StateTransaction&& hueplusplus::StateTransaction::incrementHue(int increment) &&
{
    request["hue_inc"] = std::max(-65534, std::min(increment, 65534));
    return std::move(*this);
}

hueplusplus::StateTransaction&& hueplusplus::StateTransaction::incrementColorTemperature(int increment) &&
{
    request["ct_inc"] = std::max(-65534, std::min(increment, 65534));
    return std::move(*this);
}

hueplusplus::StateTransaction&& hueplusplus::StateTransaction::incrementColorXY(float xInc, float yInc) &&
{
    request["xy_inc"] = {std::min(-0.5f, std::max(xInc, 0.5f)), std::min(-0.5f, std::max(yInc, 0.5f))};
    return std::move(*this);
}

hueplusplus::StateTransaction&& hueplusplus::StateTransaction::setScene(const std::string& scene) &&
{
    request["scene"] = scene;
    return std::move(*this);
}

hueplusplus::StateTransaction&& hueplusplus::StateTransaction::setTransition(uint16_t transition) &&
{
    if (transition != 4)
    {
        request["transitiontime"] = transition;
    }
    return std::move(*this);
}

hueplusplus::Group::Group(int id, const HueCommandAPI& commands, std::chrono::steady_clock::duration refreshDuration)
    : id(id), state("/groups/" + std::to_string(id), commands, refreshDuration), commands(commands)
{
    state.Refresh();
}

void hueplusplus::Group::Refresh()
{
    state.Refresh();
}

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
    std::vector<int> ids;
    ids.reserve(lights.size());
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

bool hueplusplus::Group::getAllOn()
{
    return state.GetValue().at("state").at("all_on").get<bool>();
}
bool hueplusplus::Group::getAllOn() const
{
    return state.GetValue().at("state").at("all_on").get<bool>();
}

bool hueplusplus::Group::getAnyOn()
{
    return state.GetValue().at("state").at("any_on").get<bool>();
}
bool hueplusplus::Group::getAnyOn() const
{
    return state.GetValue().at("state").at("any_on").get<bool>();
}

bool hueplusplus::Group::getActionOn()
{
    return state.GetValue().at("action").at("on").get<bool>();
}
bool hueplusplus::Group::getActionOn() const
{
    return state.GetValue().at("action").at("on").get<bool>();
}

std::pair<uint16_t, uint8_t> hueplusplus::Group::getActionHueSaturation()
{
    const nlohmann::json& action = state.GetValue().at("action");

    return std::make_pair(action.at("hue").get<int>(), action.at("sat").get<int>());
}
std::pair<uint16_t, uint8_t> hueplusplus::Group::getActionHueSaturation() const
{
    const nlohmann::json& action = state.GetValue().at("action");

    return std::make_pair(action.at("hue").get<int>(), action.at("sat").get<int>());
}

unsigned int hueplusplus::Group::getActionBrightness()
{
    return state.GetValue().at("action").at("bri").get<int>();
}
unsigned int hueplusplus::Group::getActionBrightness() const
{
    return state.GetValue().at("action").at("bri").get<int>();
}

unsigned int hueplusplus::Group::getActionColorTemperature()
{
    return state.GetValue().at("action").at("ct").get<int>();
}
unsigned int hueplusplus::Group::getActionColorTemperature() const
{
    return state.GetValue().at("action").at("ct").get<int>();
}

std::pair<float, float> hueplusplus::Group::getActionColorXY()
{
    const nlohmann::json& xy = state.GetValue().at("action").at("xy");
    return std::pair<float, float>(xy[0].get<float>(), xy[1].get<float>());
}
std::pair<float, float> hueplusplus::Group::getActionColorXY() const
{
    const nlohmann::json& xy = state.GetValue().at("action").at("xy");
    return std::pair<float, float>(xy[0].get<float>(), xy[1].get<float>());
}

std::string hueplusplus::Group::getActionColorMode()
{
    return state.GetValue().at("action").at("colormode").get<std::string>();
}
std::string hueplusplus::Group::getActionColorMode() const
{
    return state.GetValue().at("action").at("colormode").get<std::string>();
}

hueplusplus::StateTransaction hueplusplus::Group::transaction()
{
    return StateTransaction(commands, "/groups/" + std::to_string(id) + "/action", state.GetValue());
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
    transaction().setBrightness(brightness).setTransition(transition).commit();
}

void hueplusplus::Group::setColorHueSaturation(uint16_t hue, uint8_t saturation, uint8_t transition)
{
    transaction().setColorHueSaturation(hue, saturation).setTransition(transition).commit();
}

void hueplusplus::Group::setColorXY(float x, float y, uint8_t transition)
{
    transaction().setColorXY(x, y).setTransition(transition).commit();
}

void hueplusplus::Group::setColorTemperature(unsigned int mired, uint8_t transition)
{
    transaction().setColorTemperature(mired).setTransition(transition).commit();
}

void hueplusplus::Group::setColorLoop(bool on, uint8_t transition)
{
    transaction().setColorLoop(on).setTransition(transition);
}

void hueplusplus::Group::incrementBrightness(int increment, uint8_t transition)
{
    transaction().incrementBrightness(increment).setTransition(transition).commit();
}

void hueplusplus::Group::incrementSaturation(int increment, uint8_t transition)
{
    transaction().incrementSaturation(increment).setTransition(transition).commit();
}

void hueplusplus::Group::incrementHue(int increment, uint8_t transition)
{
    transaction().incrementHue(increment).setTransition(transition).commit();
}

void hueplusplus::Group::incrementColorTemperature(int increment, uint8_t transition)
{
    transaction().incrementColorTemperature(increment).setTransition(transition).commit();
}

void hueplusplus::Group::incrementColorXY(float incX, float incY, uint8_t transition)
{
    transaction().incrementColorXY(incX, incY).setTransition(transition).commit();
}

void hueplusplus::Group::setScene(const std::string& scene, uint8_t transition)
{
    transaction().setScene(scene).setTransition(transition).commit();
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

std::string hueplusplus::Group::getModelId() const
{
    return state.GetValue().at("modelid").get<std::string>();
}

std::string hueplusplus::Group::getUniqueId() const
{
    return state.GetValue().at("uniqueid").get<std::string>();
}
