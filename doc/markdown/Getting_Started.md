# Getting started {#getting-started}

## Creating the Hue bridge
To start searching for a Hue Bridge you will need to choose an IHttpHandler and create one.
The options are a [WinHttpHandler](@ref hueplusplus::WinHttpHandler) (for windows) or a [LinHttpHandler](@ref hueplusplus::LinHttpHandler) (for linux or linux-like).

Then create a [BridgeFinder](@ref hueplusplus::BridgeFinder) object with the handler.
The handler is needed, because it tells the finder which functions to use to communicate with a bridge or your local network.
After that you can call [findBridges()](@ref hueplusplus::HueFinder::findBridges), which will return a vector containing the ip and mac address of all found Bridges.
```{.cpp}
// For windows use std::make_shared<hueplusplus::WinHttpHandler>();
auto handler = std::make_shared<hueplusplus::LinHttpHandler>();
hueplusplus::BridgeFinder finder(handler);
std::vector<hueplusplus::BridgeFinder::BridgeIdentification> bridges = finder.findBridges();
if (bridges.empty())
{
	std::cerr << "No bridges found\n";
	return;
}
```

## Authenticate Bridges
If you have found the Bridge you were looking for, you can then move on with the authentication process.
To get a new username from the Bridge (for now) you simply call [getBridge(bridges[\<index\>])](@ref hueplusplus::BridgeFinder::getBridge),
where index is your preferred Bridge from the part [Searching for Bridges](#searchingBridges). This requires the user to press the link button.
```{.cpp}
hueplusplus::Bridge bridge = finder.getBridge(bridges[0]);
```
If you on the other hand already have a username you can add your bridge like so
```{.cpp}
finder.addUsername(bridges[0].mac, "<username>");
hueplusplus::Bridge bridge = finder.getBridge(bridges[0]);
```
If you do not want to use the HueFinder or you already know the ip and username of your bridge you have the option to create your own Hue object.
Here you will need to provide the ip address, the port number, a username and an HttpHandler
```{.cpp}
// For windows use std::make_shared<hueplusplus::WinHttpHandler>();
auto handler = std::make_shared<hueplusplus::LinHttpHandler>();
hueplusplus::Bridge bridge("192.168.2.102", 80, "<username>", handler);
```

### Controlling lights

```{.cpp}
hueplusplus::Light light = bridge.getLight(1);
light1.On();
light1.Off();
```

### Controlling groups

```{.cpp}
hueplusplus::Group group = bridge.getGroup(1);
group.setOn(true);
```

## More information
- [Transactions](@ref transactions)