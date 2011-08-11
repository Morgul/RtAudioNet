using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Windows.Threading;

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

        private Dispatcher dispatcher = null;

        private delegate void enumerateDevicesDelegate();

        public MainWindow()
        {
            InitializeComponent();

            dispatcher = Dispatcher.CurrentDispatcher;

            manager = RtAudioManager.GetInstance();
            manager.DeviceEnumerationChanged += new EventHandler(manager_DeviceEnumerationChanged);

            // Connect Logging Events
            EventLoggerManager.TraceLoggingEvent += new LoggingEventHandler(EventLoggerManager_TraceLoggingEvent);
            EventLoggerManager.DebugLoggingEvent += new LoggingEventHandler(EventLoggerManager_DebugLoggingEvent);
            EventLoggerManager.InfoLoggingEvent += new LoggingEventHandler(EventLoggerManager_InfoLoggingEvent);
            EventLoggerManager.WarnLoggingEvent += new LoggingEventHandler(EventLoggerManager_WarnLoggingEvent);
            EventLoggerManager.ErrorLoggingEvent += new LoggingEventHandler(EventLoggerManager_ErrorLoggingEvent);
            EventLoggerManager.CriticalLoggingEvent += new LoggingEventHandler(EventLoggerManager_CriticalLoggingEvent);

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

            // Check for changes in the list of active devices every 500ms.
            manager.enableDeviceWatcher(500);
        }

        void manager_DeviceEnumerationChanged(object sender, EventArgs e)
        {
            Console.WriteLine("Device enumeration changed! Updating list.");

            dispatcher.BeginInvoke(DispatcherPriority.Normal,
                    new enumerateDevicesDelegate(enumerateDevices));
        } // end MainWindow

        private void enumerateDevices()
        {
            inputsBox.Items.Clear();
            outputBox.Items.Clear();

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

        void LogEvent(string level, LoggingEventArgs e)
        {
            if (e.ex == null)
            {
                Console.WriteLine("[{0}] ({1}) {2}", level, e.logger, e.message);
            }
            else
            {
                Console.WriteLine("[{0}] ({1}) {2}, Exception: {3}", level, e.logger, e.message, e.ex.Message);
            } // end if
        } // end LogEvent

        void EventLoggerManager_TraceLoggingEvent(object sender, LoggingEventArgs e)
        {
            LogEvent("TRACE", e);
        } // end EventLoggerManager_TraceLoggingEvent

        void EventLoggerManager_DebugLoggingEvent(object sender, LoggingEventArgs e)
        {
            LogEvent("DEBUG", e);
        } // end EventLoggerManager_DebugLoggingEvent

        void EventLoggerManager_InfoLoggingEvent(object sender, LoggingEventArgs e)
        {
            LogEvent("INFO", e);
        } // end EventLoggerManager_InfoLoggingEvent

        void EventLoggerManager_WarnLoggingEvent(object sender, LoggingEventArgs e)
        {
            LogEvent("WARN", e);
        } // end EventLoggerManager_WarnLoggingEvent

        void EventLoggerManager_ErrorLoggingEvent(object sender, LoggingEventArgs e)
        {
            LogEvent("ERROR", e);
        } // end EventLoggerManager_ErrorLoggingEvent

        void EventLoggerManager_CriticalLoggingEvent(object sender, LoggingEventArgs e)
        {
            LogEvent("CRITICAL", e);
        } // end EventLoggerManager_CriticalLoggingEvent
    } // end MainWindow
} // end namespace
