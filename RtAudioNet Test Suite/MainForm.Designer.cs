namespace RtAudioNet_Test_Suite
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.inputCombo = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.outputCombo = new System.Windows.Forms.ComboBox();
            this.startLoopback = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // inputCombo
            // 
            this.inputCombo.FormattingEnabled = true;
            this.inputCombo.Location = new System.Drawing.Point(12, 25);
            this.inputCombo.Name = "inputCombo";
            this.inputCombo.Size = new System.Drawing.Size(260, 21);
            this.inputCombo.TabIndex = 0;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(73, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "InputDevices:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 54);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(84, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Output Devices:";
            // 
            // outputCombo
            // 
            this.outputCombo.FormattingEnabled = true;
            this.outputCombo.Location = new System.Drawing.Point(12, 70);
            this.outputCombo.Name = "outputCombo";
            this.outputCombo.Size = new System.Drawing.Size(260, 21);
            this.outputCombo.TabIndex = 3;
            // 
            // startLoopback
            // 
            this.startLoopback.Location = new System.Drawing.Point(12, 97);
            this.startLoopback.Name = "startLoopback";
            this.startLoopback.Size = new System.Drawing.Size(260, 23);
            this.startLoopback.TabIndex = 5;
            this.startLoopback.Text = "Start Loopback";
            this.startLoopback.UseVisualStyleBackColor = true;
            this.startLoopback.Click += new System.EventHandler(this.startLoopback_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(284, 140);
            this.Controls.Add(this.startLoopback);
            this.Controls.Add(this.outputCombo);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.inputCombo);
            this.Name = "MainForm";
            this.Text = "RtAudioTest";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ComboBox inputCombo;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox outputCombo;
        private System.Windows.Forms.Button startLoopback;
    }
}

