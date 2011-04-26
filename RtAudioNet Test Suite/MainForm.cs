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
using RtStream;

using System.Runtime.InteropServices;
using System.IO;
using System.Threading;

namespace RtAudioNet_Test_Suite
{
    public partial class MainForm : Form
    {
        private RtAudio audio = null;
        //private RtAudio audio1 = null;
        //private RtDuplexStream duplexStream = null;
        private RtInputStream inputStream = null;
        private RtOutputStream outputStream = null;
        private RtStreamMixer mixer = null;
        private bool streamRunning = false;

        private Byte[] buffer;

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

            buffer = new Byte[512];

            audio = new RtAudio();
            /*
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
            */

            //audio1 = new RtAudio();
            mixer = new RtStreamMixer();

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

        int loopbackCallback1(IntPtr outputBufferPtr, IntPtr inputBufferPtr, uint frames, double streamTime, uint status, Object userData)
        {
            int size = (int) frames * 2; // Frames per sample * channels * bytes in an int
            //byte[] bytes = new byte[size];
            
            // Copy the input to the output
            //Marshal.Copy(inputBufferPtr, bytes, 0, size);
            lock (buffer)
            {
                Marshal.Copy(buffer, 0, outputBufferPtr, size);
            }
            return 0;
        }

        int loopbackCallback(IntPtr outputBufferPtr, IntPtr inputBufferPtr, uint frames, double streamTime, uint status, Object userData)
        {
            lock (buffer)
            {
                Marshal.Copy(inputBufferPtr, buffer, 0, buffer.Length);
            }
            return 0;
        }

        private void startLoopback_Click(object sender, EventArgs e)
        {
            if (streamRunning == false)
            {
                int inputID = 0, outputID = 0;

                string selectedInput = inputCombo.SelectedItem as String;
                string selectedOutput = outputCombo.SelectedItem as String;

                for (int idx = 0; audio.getDeviceCount() > idx; idx++)
                {
                    RtAudio.DeviceInfo info = audio.getDeviceInfo((uint)idx);

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
                inputParams.deviceId = (uint)inputID;
                inputParams.nChannels = 2;

                RtAudio.StreamParameters outputParams = new RtAudio.StreamParameters();
                outputParams.deviceId = (uint)outputID;
                outputParams.nChannels = 2;

                RtAudio.StreamOptions options = new RtAudio.StreamOptions();
                options.priority = (int)RtAudioStreamFlags.RTAUDIO_SCHEDULE_REALTIME;
                options.flags = (int)RtAudioStreamFlags.RTAUDIO_MINIMIZE_LATENCY;


                //uint frames = 512;

                //audio.openStream(null, inputParams, RtAudioFormat.RTAUDIO_SINT8, 44100, 256, loopbackCallback, null, options);
                //audio1.openStream(outputParams, null, RtAudioFormat.RTAUDIO_SINT8, 44100, 256, loopbackCallback1, null, options);


                /*duplexStream = new RtDuplexStream(RtAudioFormat.RTAUDIO_SINT32, 2, 44100, 32, 256);
                duplexStream.selectInputDevice((int)inputID);
                duplexStream.selectOutputDevice((int)outputID);
                duplexStream.Open();
                duplexStream.Start();*/

                inputStream = new RtInputStream(RtAudioFormat.RTAUDIO_FLOAT32, 2, 44100, 32, 512);
                inputStream.Format.options = options;
                inputStream.selectInputDevice(inputID);
                //inputStream.callbackFired +=new EventHandler(inputStream_callbackFired);

                outputStream = new RtOutputStream(RtAudioFormat.RTAUDIO_FLOAT32, 2, 44100, 32, 512);
                outputStream.Format.options = options;
                outputStream.selectOutputDevice(outputID);

                mixer.AddInputStream(inputStream, "FooBar");
                mixer.SetOutputStream(outputStream);

                mixer.Start();

               //outputStream.callbackFired += new EventHandler(inputStream_callbackFired);

                //inputStream.Open();
                //outputStream.Open();

                //inputStream.Start();
                //outputStream.Start();

                // Change button text
                startLoopback.Text = "Stop Loopback";
                streamRunning = true;

                // Start the stream
                //audio.startStream();
                //audio1.startStream();
            }
            else
            {
                mixer.Stop();
                //audio.stopStream();
                //audio.closeStream();
                streamRunning = false;
                //duplexStream.Stop();
                //duplexStream.Abort();

                startLoopback.Text = "Start Loopback";
            } // end if
        }

        void inputStream_callbackFired(object sender, EventArgs e)
        {
            float[] buff = new float[2048];
            int bytesRead = inputStream.Read(buff);
            outputStream.Write(buff, 0, bytesRead);
        } // end startLoopback_Click

    }
}
