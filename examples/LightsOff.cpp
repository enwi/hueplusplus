#include <thread>

#include <hueplusplus/Bridge.h>

#ifdef _MSC_VER
#include <hueplusplus/WinHttpHandler.h>

using SystemHttpHandler = hueplusplus::WinHttpHandler;

#else
#include <hueplusplus/LinHttpHandler.h>

using SystemHttpHandler = hueplusplus::LinHttpHandler;

#endif

namespace hue = hueplusplus;

// Configure existing connections here, or leave empty for new connection
const std::string macAddress = "";
const std::string username = "";

hue::Bridge connectToBridge()
{
    hue::BridgeFinder finder(std::make_shared<SystemHttpHandler>());

    std::vector<hue::BridgeFinder::BridgeIdentification> bridges = finder.findBridges();

    for (const auto& bridge : bridges)
    {
        std::cout << "Bridge: " << bridge.mac << " at " << bridge.ip << '\n';
    }
    if (bridges.empty())
    {
        std::cout << "Found no bridges\n";
        throw std::runtime_error("no bridges found");
    }

    if (macAddress.empty())
    {
        std::cout << "No bridge given, connecting to first one.\n";
        return finder.getBridge(bridges.front());
    }
    if (!username.empty())
    {
        finder.addUsername(macAddress, username);
    }
    auto it = std::find_if(
        bridges.begin(), bridges.end(), [&](const auto& identification) { return identification.mac == macAddress; });
    if (it == bridges.end())
    {
        std::cout << "Given bridge not found\n";
        throw std::runtime_error("bridge not found");
    }
    return finder.getBridge(*it);
}

void lightsOff(hue::Bridge& hue)
{
    std::vector<hue::Light> lights = hue.lights().getAll();

    // Save current on state of the lights
    std::map<int, bool> onMap;
    for (hue::Light& l : lights)
    {
        onMap.emplace(l.getId(), l.isOn());
        l.Off();
    }

    // This would be preferrable, but does not work because it also resets the brightness of all lights
    // Group 0 contains all lights, turn all off with a transition of 1 second
    // hue.groups().get(0).setOn(false, 10);
    // -------------------------------------

    std::cout << "Turned off all lights\n";

    std::this_thread::sleep_for(std::chrono::seconds(20));

    // Restore the original state of the lights
    for (hue::Light& l : lights)
    {
        if (onMap[l.getId()])
        {
            l.on();
        }
    }

    std::cout << "Turned lights back on\n";
}

int main(int argc, char** argv)
{

    try
    {
        hue::Bridge hue = connectToBridge();

        std::cout << "Connected to bridge. IP: " << hue.getBridgeIP() << ", username: " << hue.getUsername() << '\n';

        lightsOff(hue);
    }
    catch (...)
    { }

    std::cout << "Press enter to exit\n";
    std::cin.get();

    return 0;
}