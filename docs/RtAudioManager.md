# RtAudioManager

{{RtAudioManager}} is designed to make enumerating devices and creating mixers simple. It wraps an internal {{RtAudio}} instance that it uses to enumerate devices, and it provides access to a very easy to use API for discovering device ids, or creating a mixer. It also has an event that will fire to let you know when a device has been added or removed from the system.

## Creating a RtAudioManager

{{RtAudioManager}} provides a static {{GetInstance()}} method to allow for the creation of a global instance. If you use {{GetInstance()}}, you will be guaranteed to always get the same instance anywhere in your program. You may also specify the Api you would like to use when calling {{GetInstance()}}, however, it will throw a {{RtAudioManagerApiException}} exception if the global instance has already been created with a different Api.

**Note**: You are not _required_ to use {{GetInstance()}}. Standard class instantiation is also supported.

Example:
{code:c#}
using System;

using RtAudioNet;
using RtStream;

class Program
{
    private static RtAudioManager manager = null;

    [STAThread](STAThread)
    static void Main(string[]() args)
    {
        // Get global instance
        manager = RtAudioManager.GetInstance();

        // Get global instance with API specification.
        manager = RtAudioManager.GetInstance(RtAudio.Api.WINDOWS_DS); // Throws RtAudioManagerApiException

        // Standard instantiation
        manager = new RtAudioManager();
    } // end Main
} // end Program
{code:c#}

## Enumerating Devices

{{RtAudioManager}} makes accessing devices very simple. It provides the following properties: {{DeviceCount}}, {{InputDevices}} and {{OutputDevices}}. These properties are automatically updated when new devices are connected. There is also an event, {{DeviceEnumerationChanged}}, which is fired whenever devices on the system are added or removed.

{{InputDevices}} and {{OutputDevices}} are simply dictionaries of String to Int, corresponding to a device name to device id mapping. This makes working with devices very simple.

Example:
{code:c#}
using System;
using System.Collections.Generic;

using RtAudioNet;
using RtStream;

class Program
{
    private static RtAudioManager manager = null;

    [STAThread](STAThread)
    static void Main(string[]() args)
    {
        // Get global instance
        manager = RtAudioManager.GetInstance();

        Console.WriteLine("Total Numer of Devices: {0}\n", manager.DeviceCount);

        // Print a list of all input devices currently on the system
        foreach (KeyValuePair<String, int> kvp in manager.InputDevices)
        {
            RtAudio.DeviceInfo info = manager.GetDeviceInfo(kvp.Value);

            Console.WriteLine("Input Device: \"{0}\"", kvp.Key);
            Console.WriteLine("        ID: {0} ", kvp.Value);
            Console.WriteLine("        Channels: {0} ", info.inputChannels);
            Console.WriteLine("        IsDefaultInput: {0} ", info.isDefaultInput);
            Console.WriteLine("        SampleRates: {0} ", string.Join(", ", info.sampleRates));
            Console.WriteLine("");
        } // end foreach

        // Print a list of all output devices currently on the system
        foreach (KeyValuePair<String, int> kvp in manager.OutputDevices)
        {
            RtAudio.DeviceInfo info = manager.GetDeviceInfo(kvp.Value);

            Console.WriteLine("Input Device: \"{0}\"", kvp.Key);
            Console.WriteLine("        ID: {0} ", kvp.Value);
            Console.WriteLine("        Channels: {0} ", info.outputChannels);
            Console.WriteLine("        IsDefaultOutput: {0} ", info.isDefaultOutput);
            Console.WriteLine("        SampleRates: {0} ", string.Join(", ", info.sampleRates));
            Console.WriteLine("");
        } // end foreach
    } // end Main
} // end Program
{code:c#}

Output:
{{
Total Numer of Devices: 5

Input Device: "Default Device"
        ID: 0
        Channels: 2
        IsDefaultInput: True
        SampleRates: 4000, 5512, 8000, 9600, 11025, 16000, 22050, 32000, 44100, 48000, 88200, 96000, 176400, 192000

Input Device: "Microphone (VXI USB 7.05 NO ST)"
        ID: 3
        Channels: 2
        IsDefaultInput: False
        SampleRates: 11025, 22050, 44100, 96000

Input Device: "Microphone (High Definition Audio Device)"
        ID: 4
        Channels: 2
        IsDefaultInput: False
        SampleRates: 11025, 22050, 44100, 96000

Input Device: "Default Device"
        ID: 0
        Channels: 2
        IsDefaultOutput: True
        SampleRates: 4000, 5512, 8000, 9600, 11025, 16000, 22050, 32000, 44100, 48000, 88200, 96000, 176400, 192000

Input Device: "Speakers (VXI USB 7.05 NO ST)"
        ID: 1
        Channels: 2
        IsDefaultOutput: False
        SampleRates: 4000, 5512, 8000, 9600, 11025, 16000, 22050, 32000, 44100, 48000, 88200, 96000, 176400, 192000

Input Device: "Speakers (High Definition Audio Device)"
        ID: 2
        Channels: 2
        IsDefaultOutput: False
        SampleRates: 4000, 5512, 8000, 9600, 11025, 16000, 22050, 32000, 44100, 48000, 88200, 96000, 176400, 192000
}}
## Creating a Mixer

{{RtAudioManager}} also makes it very easy to create a {{RtStreamMixer}} instance. You can pass either a list of device ids, or a list of strings representing the device names. (These strings can be partial names, such as "GN", which would match the device named "Jabra GN2000")

Example:
{code:c#}
using System;
using System.Collections.Generic;
using System.Threading;

using RtStream;

class Program
{
    private static RtAudioManager manager = null;

    [STAThread](STAThread)
    static void Main(string[]() args)
    {
        // Get global instance
        manager = RtAudioManager.GetInstance();

        // Create a simple mixer, mixing the default input device to the default output device
        RtStreamMixer mixer = manager.CreateMixer(new List<string>() {"Default"}, "Default");

        // Start the mixer
        mixer.Start();

       // Infinite Loop
       while (true)
       {
           Thread.Sleep(50);
       } // end while
    } // end Main
} // end Program
{code:c#}