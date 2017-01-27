# hueplusplus
A simple and easy to use library for Philips Hue Lights

## Features
* finds bridges with SSDP or set ip manually
* function to assign username or set manually
* all common light functions
* extended alert() function, which alerts in a specific color
* documented with doxygen

## How to use
### <a name="findingBridges"></a>Finding Bridges
If you want to find a Hue Bridge make a HueFinder object and call FindBridges()
it will return a vector containing the ip and mac adress of all found Bridges.
If no Bridges were found the vector is empty, so make sure that in that case you provide an ip and mac address.
```C++
HueFinder finder;
std::vector<HueFinder::HueIdentification> bridges = finder.FindBridges();
if (bridges.empty())
{
	bridges.push_back({ "<ip address>", "<mac address>" });
}
```

### Using Bridges
If you have found the Bridge you were looking for, you can then move on with the authentication process.
To get a new username from the Bridge (for now) you simply call GetBridge(bridges[<index>]), 
where index is your preferred Bridge from the part [Finding Bridges](findingBridges).
```C++
Hue bridge = finder.GetBridge(bridges[0]);
```
If you on the other hand already have a username you can add you bridge like so
```C++
finder.AddUsername(bridges[0].mac, "<username>");
Hue bridge = finder.GetBridge(bridges[0]);
```

### Controlling lights
If you have your Bridge all set up now you can get and control its lights.
For that create a pointer of the specific light type you want to use.
You can choose from 
* HueLight
* HueDimmableLight
* HueTemperatureLight
* HueColorLight
* HueExtendedColorLight
Then call getLight(<id>) from you bridge object to get the specific light, where id
is the id of the light set internally by the Hue Bridge.
```C++
HueExtendedColorLight* lamp1 = static_cast<HueExtendedColorLight*>(bridge.getLight(1));
```
If you now want to control the light you just call the specific function of the light.
```C++
lamp1->On();
lamp1->setBrightness(120);
lamp1->alertHueSaturation(25500, 255);
lamp1->setColorLoop(true);
lamp1->setColorRGB(255, 128, 0);
```

## Copyright
Copyright (c) 2017 Jan Rogall & Moritz Wirger. See LICENSE for further details.


```C++

```