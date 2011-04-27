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
using System.Diagnostics;

namespace RtAudioMixerDemo
{
    public partial class MainWindow : Form
    {
        private Process process = null;
        private RtStreamMixer mixer = null;
        private RtAudio rtaudio = null;

        public MainWindow()
        {
            InitializeComponent();

            mixer = new RtStreamMixer();
            rtaudio = new RtAudio();

            // Process Priority
            process = Process.GetCurrentProcess();
            priorityBox.DataSource = Enum.GetValues(typeof(ProcessPriorityClass));
            priorityBox.SelectedIndex = priorityBox.FindString("AboveNormal");

            // SampleRates
            sampleBox.Items.Add("4000");
            sampleBox.Items.Add("8000");
            sampleBox.Items.Add("11025");
            sampleBox.Items.Add("16000");
            sampleBox.Items.Add("22050");
            sampleBox.Items.Add("44100");
            sampleBox.SelectedIndex = 2;

            enumerateDevices();
        } // end MainWindow

        private void enumerateDevices()
        {
            // Enumerate Devices
            uint deviceCount = rtaudio.getDeviceCount();

            for (uint idx = 0; deviceCount > idx; idx++)
            {
                RtAudio.DeviceInfo info = rtaudio.getDeviceInfo(idx);

                if (info.inputChannels > 0)
                {
                    inputsBox.Items.Add(info.name);
                } // end if

                if (info.outputChannels > 0)
                {
                    outputBox.Items.Add(info.name);
                    outputBox.SelectedIndex = 0;
                } // end if
            } // end for

        } // end enumerateDevices 

        private void startButton_Click(object sender, EventArgs e)
        {
            List<string> inputStrings = inputsBox.CheckedItems.OfType<String>().ToList<String>();

            RtAudio.StreamOptions options = new RtAudio.StreamOptions();
            options.numberOfBuffers = 6;
            options.flags = (int)(RtAudioStreamFlags.RTAUDIO_MINIMIZE_LATENCY | RtAudioStreamFlags.RTAUDIO_SCHEDULE_REALTIME);
            options.priority = 1;

            // Mixer format must be set before inputs are added!
            mixer.Format.sampleRate = Convert.ToUInt32(sampleBox.SelectedItem as String);

            foreach (string selectedInput in inputStrings)
            {
                for (int idx = 0; rtaudio.getDeviceCount() > idx; idx++)
                {
                    RtAudio.DeviceInfo info = rtaudio.getDeviceInfo((uint)idx);

                    if (info.name.Contains(selectedInput))
                    {
                        RtInputStream inputStream = new RtInputStream(RtAudioFormat.RTAUDIO_FLOAT32, 2, 22050, 32, 512);
                        inputStream.selectInputDevice(idx);
                        inputStream.Format.options = options;
                       
                        // Important; RtStreamMixer requires streams to be named uniquely.
                        inputStream.Name = selectedInput;

                        mixer.AddInputStream(inputStream);
                    } // end if
                } // end for
            } // end foreach
 
            string selectedOutput = outputBox.SelectedItem as String;
            for (int idx = 0; rtaudio.getDeviceCount() > idx; idx++)
            {
                RtAudio.DeviceInfo info = rtaudio.getDeviceInfo((uint)idx);

                if (info.name.Contains(selectedOutput))
                {
                    RtOutputStream outputStream = new RtOutputStream(RtAudioFormat.RTAUDIO_FLOAT32, 2, 44100, 32, 512);
                    outputStream.selectOutputDevice(idx);
                    outputStream.Format.options = options;

                    mixer.SetOutputStream(outputStream);
                } // end if
            } // end for


            mixer.Start();

            stopButton.Enabled = true;
            startButton.Enabled = false;
        }

        private void stopButton_Click(object sender, EventArgs e)
        {
            mixer.Stop();

            stopButton.Enabled = false;
            startButton.Enabled = true;
        }

        private void priorityBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            // IDDQD (aka: "God Mode")
            process.PriorityClass = (ProcessPriorityClass)priorityBox.SelectedValue;
        } // end startButton_Click
    } // end MainWindow
} // end namespace
