#region License
/*
 * Copyright (c) 2011 Christopher S. Case

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#endregion

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using RtAudioNet;
using System.Runtime.InteropServices;
using System.IO;

namespace RtAudioNet_Test_Suite
{
    public partial class MainForm : Form
    {
        private RtAudio audio = null;
        private bool streamRunning = false;

        private string userData = null;

        public MainForm()
        {
            InitializeComponent();

            // Circular Buffer Tests
            /*
            CircularBuffer<byte> cb = new CircularBuffer<byte>(512, true);
            Console.WriteLine("cb Cap: {0}", cb.Capacity);
            cb.Add((byte)255);
            Console.WriteLine("cb Cap: {0}", cb.Capacity);
            cb.Add((byte)255);
            Console.WriteLine("cb Cap: {0}", cb.Capacity);
            cb.Add((byte)255);
            Console.WriteLine("cb Cap: {0}", cb.Capacity);
            cb.Add((byte)128);
            Console.WriteLine("cb Cap: {0}", cb.Capacity);
            cb.Add((byte)255);
            Console.WriteLine("cb Cap: {0}", cb.Capacity);
            cb.Add((byte)255);
            Console.WriteLine("cb Cap: {0}", cb.Capacity);

            Console.WriteLine("Count: {0}", cb.Count);
            */

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

                Console.WriteLine("ID {1}: {0}:", info.name, idx);
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

        int loopbackCallback(IntPtr outputBufferPtr, IntPtr inputBufferPtr, uint frames, double streamTime, uint status, Object userData)
        {
            // Just testing to make sure we can get the user data.
            String data = userData as String;

            int size = (int) audio.frames * 2 * 4; // Frames per sample * channels * bytes in an int
            byte[] bytes = new byte[size];
            
            // Copy the input to the output
            Marshal.Copy(inputBufferPtr, bytes, 0, size);
            Marshal.Copy(bytes, 0, outputBufferPtr, size);
            return 0;
        }

        private void startLoopback_Click(object sender, EventArgs e)
        {
            if (streamRunning == false)
            {
                uint inputID = 0, outputID = 0;

                string selectedInput = inputCombo.SelectedItem as String;
                string selectedOutput = outputCombo.SelectedItem as String;

                for (uint idx = 0; audio.getDeviceCount() > idx; idx++)
                {
                    RtAudio.DeviceInfo info = audio.getDeviceInfo(idx);

                    if (info.name.Contains(selectedInput))
                    {
                        inputID = idx;
                    } // end if

                    if (info.name.Contains(selectedOutput))
                    {
                        outputID = idx;
                    } // end if
                } // end for

                RtAudio.StreamParameters inputParams = new RtAudio.StreamParameters();
                inputParams.deviceId = inputID;
                inputParams.nChannels = 2;

                RtAudio.StreamParameters outputParams = new RtAudio.StreamParameters();
                outputParams.deviceId = outputID;
                outputParams.nChannels = 2;

                RtAudio.StreamOptions options = new RtAudio.StreamOptions();
                uint frames = 512;

                userData = "This is my User Data!";

                audio.openStream(outputParams, inputParams, RtAudioFormat.RTAUDIO_SINT32, 44100, frames, loopbackCallback, userData);

                // Change button text
                startLoopback.Text = "Stop Loopback";
                streamRunning = true;

                // Start the stream
                audio.startStream();
            }
            else
            {
                audio.stopStream();
                audio.closeStream();
                streamRunning = false;

                startLoopback.Text = "Start Loopback";
            } // end if
        } // end startLoopback_Click

    }
}
