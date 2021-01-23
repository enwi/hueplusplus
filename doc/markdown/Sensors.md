# Sensors {#sensors}

## Sensor support
The library supports the sensor types listed on the Hue developer documentation.
Other sensors can be used with the generic [Sensor](@ref hueplusplus::Sensor) class.

### Working with a known sensor
In most cases, the type of the sensors is known in advance, such as a switch.
The classes in the [sensors](@ref hueplusplus::sensors) namespace provide the documented
functionality. The type can be specified when accessing the sensor. When it does not match,
an exception is thrown.
```{.cpp}
hueplusplus::sensors::ZLLSwitch switchSensor = bridge.sensors().getAsType<hueplusplus::sensors::ZLLSwitch>(2);
```

You can also get all sensors of a specified type by using [getAllByType<T>()](@ref hueplusplus::SensorList::getAllByType).
```{.cpp}
std::vector<hueplusplus::sensors::ZLLSwitch> allSwitches = bridge.sensors().getAllByType<hueplusplus::sensors::ZLLSwitch>();
```

### Working with an unknown sensor
When the sensor type is not known, use the generic sensor class. In this case, some attributes might not
exist, so they have to be checked first. This applies to all attributes that have a `hasXXX` method.

```{.cpp}
hueplusplus::Sensor genericSensor = bridge.sensors().get(1);
if(genericSensor.hasOn())
{
	// Now can check whether it is on
	if(genericSensor.isOn())
	{
		// ...
	}
}
```

It is easiest to compare the sensor type to the existing ones (`typeStr` on the specific sensor classes)
and then convert the sensor to that type.
```{.cpp}
hueplusplus::Sensor genericSensor = bridge.sensors().get(1);
if(genericSensor.getType() == hueplusplus::sensors::ZLLSwitch::typeStr)
{
	hueplusplus::sensors::ZLLSwitch switchSensor = genericSensor.asSensorType<hueplusplus::sensors::ZLLSwitch>();
	// ...
}
```

## ZLL sensors vs. CLIP sensors
ZLL sensors (defined in `ZLLSensors.h`) are physical device sensors which send their data
to the bridge using ZigBee. They are added in the same way as lights are, using [search()](@ref hueplusplus::SearchableResourceList::search).

CLIP sensors (in `CLIPSensors.h`) are added using [create()](@ref hueplusplus::CreateableResourceList::create) with [CreateSensor](@ref hueplusplus::CreateSensor)
for parameters. In general, which config and state attributes exist is specified when the sensor is created.
The values of CLIP sensors can be changed using requests, unlike ZLL sensors. They can also have a URL to query from.

## Creating conditions
The most important use for sensors is in [Rules](@ref hueplusplus::Rule), to trigger changes.
Conditions can be created from the specific sensor types using `makeCondition()`.

These functions return a helper class with methods for the [possible operators](@ref hueplusplus::Condition::Operator) valid for the state.

For some sensors, which have multiple possible states, there exist multiple variations of makeCondition.

```{.cpp}
hueplusplus::sensors::ZLLSwitch switchSensor = ...;
// ZLLSwitch conditions operate on `buttonEvent`, use makeConditionLastUpdate()
// to trigger on the last update time.

// Some examples:
hueplusplus::Condition upPressed = makeCondition(switchSensor).eq(hueplusplus::sensors::ZLLSwitch::c_UP_INITIAL_PRESS);
hueplusplus::Condition buttonChanged = makeCondition(switchSensor).dx();

hueplusplus::time::TimeInterval interval(std::chrono::hours(12), std::chrono::hours(13));
hueplusplus::Condition updatedAtNoon = makeConditionLastUpdate(switchSensor).in(interval);
```

For generic sensors, the conditions must be created manually using the [Condition](@ref hueplusplus::Condition::Condition)
constructor with a proper address to the sensor state.