using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using RtAudioNet;

namespace RtAudioNet_Test_Suite
{
    public partial class MainForm : Form
    {
        private RtAudio audio = null;
        private bool streamRunning = false;

        public MainForm()
        {
            InitializeComponent();

            audio = new RtAudio();
            audio.rtErrorDebugWarning += new EventHandler<RtErrorEventArgs>(handleRtError);
            audio.rtErrorDriverError += new EventHandler<RtErrorEventArgs>(handleRtError); 
            audio.rtErrorInvalidDevice += new EventHandler<RtErrorEventArgs>(handleRtError);
            audio.rtErrorInvalidParameter += new EventHandler<RtErrorEventArgs>(handleRtError); 
            audio.rtErrorInvalidUse += new EventHandler<RtErrorEventArgs>(handleRtError); 
            audio.rtErrorMemoryError += new EventHandler<RtErrorEventArgs>(handleRtError); 
            audio.rtErrorNoDevicesFound += new EventHandler<RtErrorEventArgs>(handleRtError); 
            audio.rtErrorSystemError += new EventHandler<RtErrorEventArgs>(handleRtError); 
            audio.rtErrorThreadError += new EventHandler<RtErrorEventArgs>(handleRtError); 
            audio.rtErrorUnspecified += new EventHandler<RtErrorEventArgs>(handleRtError);
            audio.rtErrorWarning += new EventHandler<RtErrorEventArgs>(handleRtError); 

            List<RtAudio.Api> compileApis = RtAudio.getCompiledApi();
            List<RtAudio.DeviceInfo> availableDevices = new List<RtAudio.DeviceInfo>();

            // List the currently compiled APIs.
            Console.WriteLine("Compiled Apis:");
            foreach (RtAudio.Api api in compileApis)
            {
                Console.WriteLine("  {0}", api.ToString());
            } // end foreach

            // Enumerate Devices
            uint deviceCount = audio.getDeviceCount();
            Console.WriteLine("Device Count: {0}", deviceCount);

            Console.WriteLine("Found Devices:");
            for (uint idx = 0; deviceCount > idx; idx++)
            {
                RtAudio.DeviceInfo info = audio.getDeviceInfo(idx);
                availableDevices.Add(info);

                Console.WriteLine("{0}:", info.name);
                Console.WriteLine("    InputChannels: {0}", info.inputChannels);
                Console.WriteLine("    OutputChannels: {0}", info.outputChannels);
                Console.WriteLine("    DuplexChannels: {0}", info.duplexChannels);

                if (info.inputChannels > 0)
                {
                    inputCombo.Items.Add(info.name);
                    inputCombo.SelectedIndex = 0;
                } // end if

                if (info.outputChannels > 0)
                {
                    outputCombo.Items.Add(info.name);
                    outputCombo.SelectedIndex = 0;
                } // end if
            } // end for
        }

        void handleRtError(object sender, RtErrorEventArgs error)
        {
            Console.WriteLine("[RtError] {0}", error.Message);
        } // end handleRtError

        int loopbackCallback(IntPtr outputBufferPtr, IntPtr inputBufferPtr, uint frames, double streamTime, uint status, IntPtr dataPtr)
        {
            Console.WriteLine("Callback Called.");
            return 0;
        }

        private void startLoopback_Click(object sender, EventArgs e)
        {
            if (streamRunning == false)
            {
                RtAudio.StreamParameters inputParams = new RtAudio.StreamParameters();
                inputParams.deviceId = 0;
                inputParams.nChannels = 2;

                RtAudio.StreamParameters outputParams = new RtAudio.StreamParameters();
                outputParams.deviceId = 0;
                outputParams.nChannels = 2;

                RtAudio.StreamOptions options = new RtAudio.StreamOptions();
                uint[] buff = new uint[512];

                audio.openStream(outputParams, inputParams, 0x8, 44100, buff, loopbackCallback);

                // Change button text
                startLoopback.Text = "Stop Loopback";
                streamRunning = true;

                // Start the stream
                audio.startStream();
            }
            else
            {
                audio.stopStream();

                startLoopback.Text = "Start Loopback";
            } // end if
        } // end startLoopback_Click

    }
}
