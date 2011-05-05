#pragma region License
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
#pragma endregion

#include "RtMixer.h"

using namespace RtStream;

//////////////////////////////////////////////////////////////////////////
/// RtMixerInput Class
#pragma region RtMixerInput

// Constructor	
RtMixerInput::RtMixerInput()
{
	Gain = 0.0;
	Pan = 0.0;
	InputStream = nullptr;
} // end RtMixerInput

// Constructor	
RtMixerInput::RtMixerInput(RtInputStream^ stream)
{
	Gain = 1.0;
	Pan = 0.0;
	InputStream = stream;
} // end RtMixerInput

// Constructor	
RtMixerInput::RtMixerInput(RtInputStream^ stream, float gain, float pan)
{
	Gain = gain;
	Pan = pan;
	InputStream = stream;
} // end RtMixerInput
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// RtStreamMixer Class
#pragma region RtStreamMixer

// Default Constructor
RtStreamMixer::RtStreamMixer()
{
	Format = gcnew RtStreamFormat();
	Format->type = ::RtAudioNet::RtAudioFormat::RTAUDIO_FLOAT32;
	Format->sampleRate = 11025;
	Format->channels = 2;
	Format->bitsPerSample = 16;

	FramesToBuffer = 1;

	// Internal buffer
	internalBuffer = gcnew CircularBuffer<float>(1024, true);

	running = false;

	// All our internal inputStreams
	inputs = gcnew Dictionary<String^, RtMixerInput^>();
} // end RtStreamMixer

// Default Destructor
RtStreamMixer::~RtStreamMixer()
{
	Stop();

	for each(KeyValuePair<String^, RtMixerInput^>^ kvp in inputs)
	{
		kvp->Value->InputStream->Finish();
	} // end for

	outputStream->Finish();

} // end ~RtStreamMixer

// Add an input stream to the mixer
void RtStreamMixer::AddInputStream(RtInputStream^ inputStream)
{
	if (inputStream->Name == "")
	{
		// FIXME: Throw an exception here
		Console::WriteLine("[Error]: Adding InputStreams without names is unsupported! Ignoring stream.");
		return;
	} // end if

	// Close the stream if it's open
	if (inputStream->IsStreamOpen())
	{
		inputStream->Close();
	} // end if

	// Reset the format to our required format and then reopen the stream
	inputStream->Format->type = Format->type;
	inputStream->Format->sampleRate = Format->sampleRate;
	inputStream->Open();

	// Add the string to our internal list
	inputs[inputStream->Name] = gcnew RtMixerInput(inputStream);
} // end AddInputStream

void RtStreamMixer::AddInputStream(RtInputStream^ inputStream, float gain, float pan)
{
	if (inputStream->Name == "")
	{
		// FIXME: Throw an exception here
		Console::WriteLine("[Error]: Adding InputStreams without names is unsupported! Ignoring stream.");
		return;
	} // end if

	// Close the stream if it's open
	if (inputStream->IsStreamOpen())
	{
		inputStream->Close();
	} // end if

	// Reset the format to our required format and then reopen the stream
	inputStream->Format->type = Format->type;
	inputStream->Format->sampleRate = Format->sampleRate;
	inputStream->Open();

	// Add the string to our internal list
	inputs[inputStream->Name] = gcnew RtMixerInput(inputStream, gain, pan);
} // end AddInputStream

// Add an input stream to the mixer
void RtStreamMixer::AddInputStream(RtMixerInput^ input)
{
	if (input->InputStream->Name == "")
	{
		// FIXME: Throw an exception here
		Console::WriteLine("[Error]: Adding InputStreams without names is unsupported! Ignoring stream.");
		return;
	} // end if

	// Close the stream if it's open
	if (input->InputStream->IsStreamOpen())
	{
		input->InputStream->Close();
	} // end if

	// Reset the format to our required format and then reopen the stream
	input->InputStream->Format->type = Format->type;
	input->InputStream->Format->sampleRate = Format->sampleRate;
	input->InputStream->Open();

	// Add the string to our internal list
	inputs[input->InputStream->Name] = input;
} // end AddInputStream

// Add an outputstream to the mixer
void RtStreamMixer::SetOutputStream(RtOutputStream^ outputStream)
{
	// Close the stream if it's open
	if (outputStream->IsStreamOpen())
	{
		outputStream->Close();
	} // end if

	// Reset the format to our required format and then reopen the stream
	outputStream->Format->type = Format->type;
	outputStream->Format->sampleRate = Format->sampleRate;
	outputStream->Open();

	// Set out outputStream
	this->outputStream = outputStream;

	// Set our eventhandler
	this->outputStream->CallbackFired += gcnew EventHandler(this, &RtStreamMixer::callbackHandler);
} // end SetOutputStream

// Adjust the gain on the selected input
void RtStreamMixer::AdjustGain(String^ inputName, float gain)
{
} // end AdjustGaim

// Adjust the pan on the selected input
void RtStreamMixer::AdjustPan(String^ inputName, float pan)
{
} // end AdjustPan

// Start the mixer
void RtStreamMixer::Start()
{
	for each(KeyValuePair<String^, RtMixerInput^>^ kvp in inputs)
	{
		kvp->Value->InputStream->Start();
	} // end for

	outputStream->Start();
} // end Start

// Stop the mixer
void RtStreamMixer::Stop()
{
	for each(KeyValuePair<String^, RtMixerInput^>^ kvp in inputs)
	{
		kvp->Value->InputStream->Stop();
	} // end for

	outputStream->Stop();
} // end Stop

bool RtStreamMixer::IsRunning()
{
	return running;
} // end IsRunning

// Callback Event Handler
void RtStreamMixer::callbackHandler(Object^ sender, EventArgs^ e)
{
	array<float>^ tempBuff = gcnew array<float>(outputStream->Frames * outputStream->Format->channels);
	array<float>^ inputBuff = gcnew array<float>(outputStream->Frames * outputStream->Format->channels);

	for each(KeyValuePair<String^, RtMixerInput^>^ kvp in inputs)
	{
		RtInputStream^ inputStream = kvp->Value->InputStream;
		int floatsRead = inputStream->Read(inputBuff);
		float gain = kvp->Value->Gain;
		float leftGain = gain;
		float rightGain = gain;

		// Currently, we only support pan on 2 channel inputs
		if (inputStream->Format->channels == 2)
		{
			leftGain = Convert::ToSingle(Math::Cos((kvp->Value->Pan + 1) * Math::PI / 4) * kvp->Value->Gain);
			rightGain = Convert::ToSingle(Math::Cos((kvp->Value->Pan - 1) * Math::PI / 4) * kvp->Value->Gain);
			//Console::WriteLine("Values are: {0}, {1}, {2}, {3}", leftGain, rightGain, kvp->Value->Pan, kvp->Value->Gain);
		} // end if

		for(int idx = 0; idx < floatsRead; idx++)
		{
			// This code assumes interleaved audio! 
			if (idx % 2 == 0)
			{
				gain = leftGain;
			}
			else
			{
				gain = rightGain;
			} // end if
			tempBuff[idx] += (inputBuff[idx] * gain);
		} // end for
	} // end for

	outputStream->Write(tempBuff);
	//internalBuffer->Put(tempBuff, 0, tempBuff->Length);

	// Should we be buffering, or should we be outputting?
	//if(running || framesBuffered >= FramesToBuffer)	
	//{
	//	if (!running && framesBuffered >= FramesToBuffer)	
	//	{
	//		running = true;
	//	} // end if

	//	internalBuffer->Get(tempBuff, 0, tempBuff->Length);
	//	outputStream->Write(tempBuff);
		//Console::WriteLine("Do Stuff!");
	//}
	//else
	//{
	//	framesBuffered += 1;
	//} // end if
} // end callbackHandler
#pragma endregion


//////////////////////////////////////////////////////////////////////////
/// RtDuplexMixer Class
#pragma region RtDuplexMixer
// Default Constructor
RtDuplexMixer::RtDuplexMixer()
{
	Format = gcnew RtStreamFormat();
	Format->type = ::RtAudioNet::RtAudioFormat::RTAUDIO_FLOAT32;
	Format->sampleRate = 11025;
	Format->channels = 2;
	Format->bitsPerSample = 32;

	FramesToBuffer = 1;

	running = false;

	duplexStream = gcnew RtDuplexStream(512);
} // end RtDuplexMixer

// Default Destructor
RtDuplexMixer::~RtDuplexMixer()
{
	Stop();

	duplexStream->Finish();

} // end ~RtDuplexMixer

// Add an input stream to the mixer
void RtDuplexMixer::AddInputStream(RtInputStream^ inputStream)
{
	duplexStream->selectInputDevice(inputStream->DeviceID);
	duplexStream->Format = Format;
} // end AddInputStream

// Add an input stream to the mixer
void RtDuplexMixer::AddInputStream(RtInputStream^ inputStream, float gain, float pan)
{
	duplexStream->selectInputDevice(inputStream->DeviceID);
	duplexStream->Format = Format;
} // end AddInputStream

// Add an input stream to the mixer
void RtDuplexMixer::AddInputStream(RtMixerInput^ input)
{
	duplexStream->selectInputDevice(input->InputStream->DeviceID);
	duplexStream->Format = Format;
} // AddInputStream

// Add an outputstream to the mixer
void RtDuplexMixer::SetOutputStream(RtOutputStream^ outputStream)
{
	duplexStream->selectOutputDevice(outputStream->DeviceID);
	duplexStream->Format = Format;
} // SetOutputStream

// Start the mixer
void RtDuplexMixer::Start()
{
	duplexStream->Open();
	duplexStream->Start();
} // end Start

// Stop the mixer
void RtDuplexMixer::Stop()
{
	duplexStream->Stop();
	System::Threading::Thread::Sleep(50);
	duplexStream->Finish();
} // end Stop

#pragma endregion