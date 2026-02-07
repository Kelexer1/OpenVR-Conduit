## Brief Overview
OpenVR Conduit is a custom OpenVR driver and client library combo that intercepts tracked device pose and input states in real time. It exposes these states for reading and writing through an easy-to-use client API using a complex, high-performance shared memory buffer that uses no mutex locking mechanisms. This allows a theoretical latency of just 100ns, ensuring your devices feel as smooth as always while unlocking full control for your C++ apps. The client API allows apps to modify input states, and device poses in real time through an intuitive event receiver interface and command sender, which can toggle between using a modified state or the original (natural) state of the input/pose. Conduit enables the creation of custom apps in numerous impactful fields, such as accessibility tools, community software, and even machine learning.

## Features
###### Conduit is a powerful tool that allows client apps to...
- Intercept and override OpenVR tracked device pose and input states in real time
- Choose whether the OpenVR runtime should use the unmodified input/pose state, or the modified one on demand
###### Conduit makes developing client apps easy by...
- Exposing a minimalistic yet powerful feedback loop, where updates are received by an event receiver interface, and commands are dispatched by the client using a command sender interface
- Providing extensive inline documentation for all classes and interfaces
- Including numerous diverse sample applications that explain how to use all the main features

## Limitations
- Conduit is not designed to run multiple client applications at once, and instability may occur

## Prerequisites
1. [Microsoft Visual Studio](https://visualstudio.microsoft.com/downloads/) (Preferably 2022+) with the "Desktop development with C++" workload
2. The latest version of the [OpenVR SDK](https://github.com/ValveSoftware/openvr) is highly recommended for creating client apps
3. [SteamVR](https://store.steampowered.com/app/250820/SteamVR/) is highly recommended, as installation and functionality is built on SteamVR

## Installation
1.  Clone the repository
	1. Either download the code as a `.zip`, or clone the repository by running `git clone https://github.com/kelexer1/Conduit.git`
2. Build the project
	1. Open the project in Microsoft Visual Studio and build the solution, which will generate the driver and client build files in `<Installation Path>\Build\`. You can use either the 'Debug' or 'Release' configuration, but 'Release' is recommended
3. Register the Conduit driver (2 methods)
	1. Register the build directory **(Recommended)**
		1. In a command terminal, navigate to your SteamVR `bin` directory, usually found at `C:\Program Files (x86)\Steam\Steamapps\common\SteamVR\bin\win64\`
		2. Run the following command to register the driver: `./vrpathreg adddriver <Installation Path>\Build\ConduitDriver\<Build Configuration>`
	2. Copy the driver to the SteamVR drivers folder
		1. In a command terminal, navigate to your SteamVR `drivers` directory, usually found at `C:\Program Files (x86)\Steam\steamapps\common\SteamVR\drivers`
		2. Make a new folder called `conduit`, and paste the contents of `<Installation Path>\Build\ConduitDriver\<Build Configuration>`
		3. Navigate to your SteamVR `bin` directory, usually found at `C:\Program Files (x86)\Steam\Steamapps\common\SteamVR\bin\win64\`
		4. Run the following command to register the driver: `./vrpathreg adddriver <Path To conduit Folder>`
4. You can verify that the Conduit driver is correctly installed by navigating to the SteamVR `bin` folder and running `./vrpathreg show`

## Driver Logs
Driver logs are written to `C:\OpenVRConduit\log_OpenVRConduit.log`, logging by client applications may vary, and will not be at the same directory

## Using the Client API
- Ensure your project has all the headers found at `\Lib\include`
- Ensure your project is linked against the Conduit Lib, which can be found at `\Build\ConduitLib\<Build Configuration>`
- It is highly recommended that client apps are also initialized as OpenVR apps, which required `openvr.h` and linking against `openvr_api.lib` and `openvr_api.dll`. For detailed steps on doing this, consult the [OpenVR SDK](https://github.com/ValveSoftware/openvr)
- Consult the numerous sample applications, and inline documentation in the header files, to see how to initialize Conduit, and interface with the API
- In general, you should inherit and implement the `IDeviceStateEventReciever` class with your logic that you want to run when the Conduit driver sends updates, which will be directed to the correct event callback automatically by the Conduit lib. You should directly instantiate and call methods on a `DeviceStateCommandSender` object to send information back to the Conduit driver, you should not inherit or override the methods of this class

## Sample Applications
- Sample applications can be found at `\Samples` in the repository directory
- `MyFirstConduitApplication`: Demonstrates basic initialization and event receiver logic
- `LiveDeviceModification`: Demonstrates basic real-time manipulation of devices (controllers specifically for this demo) by overriding their position to the origin, while leaving their rotation untouched to demonstrate the lack of latency even while modifying data on demand
	- Note: The origin will vary between users. For users with base station tracked headsets, the origin is likely at one of the base stations. For users with in house tracking solutions, the origin may be on the floor at the center of your play space
	- Note: Notice that when you close this application, your controllers will appear to stop responding to movement, but still allow inputs. This followed from the app enabling overridden pose usage through a command. However, it does not deactivate this when closing, so the driver continues to use the last modification of the overridden pose until it is either deactivated or modified again. All inputs have their own toggle which is never activated by this demo, so they remain functional
- `DeviceTracker`: Demonstrates more complex event receiver logic using a model pattern to keep track of the state of all poses and inputs, and constantly pretty prints them to the console for easy viewing

## Technical Implementation Details
### Shared Memory
Conduit uses a complex shared memory protocol that uses zero mutex locks to ensure lightning fast data transfers with near-zero packet drop rates. The shared memory region is divided as follows:

`[ Shared Memory Header ][ Path Table ][ Driver Client Lane ][ Client Driver Lane ]`

`Shared Memory Header`: This region is small compared to all other layers, and contains important metadata that is required for both the driver and lib to communicate live values. Specifically, it contains parameters for the lanes and the path table (driver-client and client-driver). For the two lanes, it encodes their size, start offset, reader offsets, writer offsets, and write counts. For the path table, it encodes the size, start offset, and current write offset. The shared memory header is able to exclusively rely on atomic values for data that is regularly changing.

`Path Table`: The path table is a single chunk of memory that serves as a giant cache for input paths, such as `input/trigger/value`, which are used along with device indices to uniquely identify inputs. The path table is a giant null terminated string, and input paths are appended after the end of the last string. Offsets in this path table are used in place of input strings for packets in the two lanes, saving many write operations and space demanded by copying the string potentially hundreds of times per second. Moreover, the writing offset serves as a commit counter, it is zero when the driver is not writing a path, and equal to the offset being written to when it is writing. By doing this, the client has no risk of reading partially written paths, since it can identify in-progress writes and simply wait until the flag returns to zero before reading.

`Lanes`: Conduit takes advantage of a single writer single consumer (SWSC) pattern for blazing fast concurrency and cross-process communication. Each lane has exactly one entity writing to it, and one reading from it, using the respective offsets and counts (only writes have counts stored in memory). Lanes are implemented as ring buffers, constantly writing new packets and looping around once they reach a padding region at the end to signal end-of-lane. This allows large amount of unique data to be written, since old data is no longer needed once its been parsed and interpreted.

For the driver-client lane, the driver writes update snapshots of poses and inputs as it intercepts them, and writes them in a serialized binary format to the shared memory, which the lib reads and parses, before calling event receivers for client apps. The driver writes constant sized object entry headers that contain common metadata, such as the type of state being serialized (important for parsing), the version of the packet (useful for ordering and only reading new packets), the input path offset, and a valid flag that is true if the object is currently active, and false if it is been deactivated (ex. the device that has the pose disconnected). After the object entry, variable size serialized data is written with no separator, ranging from boolean inputs (~18 bytes), up to skeleton inputs (~4kb).

The client-driver lane does the opposite, the client writes command packets and parameters to the lane when they are called from the command sender, which the driver will read and parse, then update its internal model that connects directly to the internal OpenVR runtime. Command packets are written as a command header, which serves a similar purpose to object entries in the driver-client lane, except more suited for client commands. Command headers are immediately followed by variable size command params, which encode additional command specific parameters, such as a serialized state, or a flag to use the overridden state for a specific input or pose.

It is no coincidence that the implementation of both lanes are closely related. They are both identical in size, padding, and extremely similar in implementation. Both lanes take advantage of multiple integrity checks and safety features to ensure packets are not overwritten early, read before being fully written, and are exactly aligned as the reader expects. If the writer writes data faster than the reader and laps it, it would leave the reader unaligned from whichever packet it was in the process of reading. To combat this, writers take into account the read offset and do not lap it, instead waiting directly behind it and dropping packets are required. This is of course a worst-case scenario which is unlikely to occur, both the reader and writer use a single polling rate of 512Hz to check for updates, though once a single packet is identified, the reader will continue to read trailing packets without any delay until no more valid packets are available to read. In terms of preventing the reader from reading garbage or partially written data, Conduit implements many checks to identify and correct packets for extremely high stability. First, object entries and command headers encode a common alignment constant, which is a constant bit pattern known by both the writer and reader that is unlikely to occur randomly in garbage data. If a packet being read has an alignment constant that isn't exactly equal to the defined constant, we can immediately conclude that packet is either misaligned, or improperly written. Second, object entries and command headers both have an atomic boolean 'committed' flag, which is written to shared memory as false, and only atomically set to true by having the writer modify the object directly in shared memory, ensuring it has completely written. Lastly, readers are able to intelligently identify potential bad packets based on the values of their parameters. For example, device indices can only range from 0 to 64 by the OpenVR SDK, so a packet read with device index 168 must be invalid. Similar logic is used for most parameters in object entries and command headers. These three integrity features together are able to reduce the rate of misaligned packets and garbage reads to almost perfect levels, but occasionally, bad reads are bound to occur.

When a bad read is identified, a forward search algorithm is implemented to advance a test read header forwards in memory until a packet that is safe to read is identified. This works more often than not, and does not require dropping many (if any at all) packets. If all else fails, we need to realign the reader by any means necessary, which is accomplished by resetting the read header to the current write offset, dropping and packets that haven't yet been read but allowing the writer to begin rewriting aligned data while guaranteeing that the client is now aligned with the first of the new packets.

By using these clever implementations and protocols, the shared memory used by Conduit is able to completely avoid using named mutexes to allow safe cross-process communication. This methodology offers hundreds, or potentially thousands of times better performance in theory when comparing raw memory read times to named mutex lock times.

### Intercepting Data From OpenVR
Conduit uses MinHook to hook onto the internal values of a large number of critical methods and functions in the OpenVR runtime, ranging from input creation and updating, to pose updates. These hooks allow the conduit driver to model the current state of the entire device space with minimal overhead by simply reading the parameters the internal methods are called with. These methods are central and are therefore used by every single OpenVR driver, allowing for infinite extensibility to new controllers without changing a single line of code. Moreover, this enables mutating or entirely replacing original parameters. For example, if the Conduit driver has received a command that enables the overridden pose for device index 1, when the OpenVR method responsible for device pose updates is called, Conduit records the pose as the natural pose, and will then replace the parameter with the overridden pose it has on record, before calling the original internal function with the new parameters. This tricks the OpenVR runtime into using these values as if they were the intended values, enabling infinite possibilities for client apps to directly interface with devices in ways never seen before.

## License
This project is licensed under the MIT license. See the [LICENSE](https://github.com/Kelexer1/OpenVR-ControllerHooker/blob/main/LICENSE) file for details