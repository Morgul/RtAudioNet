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
        private RtAudioManager manager = null;

        public MainWindow()
        {
            InitializeComponent();

            manager = RtAudioManager.GetInstance();

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

            apiBox.DataSource = Enum.GetValues(typeof(RtAudio.Api));
            apiBox.SelectedIndex = apiBox.FindString("WINDOWS_ASIO");

            enumerateDevices();
        } // end MainWindow

        private void enumerateDevices()
        {
            // Enumerate Devices
            foreach (KeyValuePair<String, int> kvp in manager.InputDevices)
            {
                inputsBox.Items.Add(kvp.Key);
            } // end foreach

            foreach (KeyValuePair<String, int> kvp in manager.OutputDevices)
            {
                outputBox.Items.Add(kvp.Key);
            } // end foreach

            outputBox.SelectedIndex = 0;
        } // end enumerateDevices 

        private void startButton_Click(object sender, EventArgs e)
        {
            List<string> inputStrings = inputsBox.CheckedItems.OfType<String>().ToList<String>();

            int sampleRate = Convert.ToInt32(sampleBox.SelectedItem as String);

            string selectedOutput = outputBox.SelectedItem as String;

            RtAudio.Api api = (RtAudio.Api) apiBox.SelectedIndex;

            Console.WriteLine("Selected API: {0}", api.ToString());
            // Try to get the global instance. If we fail, make our own.
            try
            {
                manager = RtAudioManager.GetInstance();
            }
            catch (RtAudioManagerApiException)
            {
                manager = new RtAudioManager();
            } // end try/catch

            mixer = manager.CreateMixer(inputStrings, selectedOutput, sampleRate);
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
