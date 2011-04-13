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
        public MainForm()
        {
            InitializeComponent();

            RtAudio rtaudio = new RtAudio();
            RtAudio.DeviceInfo info = new RtAudio.DeviceInfo();
        }
    }
}
