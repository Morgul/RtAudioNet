using System;
using System.Collections.Generic;
using System.Threading;

using RtStream;

class Program
{
    private static RtAudioManager manager = null;

    [STAThread]
    static void Main(string[] args)
    {
        // Get global instance
        manager = RtAudioManager.GetInstance();

        // Create a simple mixer, mixing the default input device to the default output device
        RtStreamMixer mixer = manager.CreateMixer(new List<string>() { "Default" }, "Default");

        // Start the mixer
        mixer.Start();

        // Infinite Loop
        while (true)
        {
            Thread.Sleep(50);
        } // end while
    } // end Main
} // end Program