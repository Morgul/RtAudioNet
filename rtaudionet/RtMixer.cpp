#include "RtMixer.h"

using namespace RtStream;

//////////////////////////////////////////////////////////////////////////
/// RtDuplexStream Class

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


//////////////////////////////////////////////////////////////////////////
/// RtStreamMixer Class

// Default Constructor
RtStreamMixer::RtStreamMixer()
{
	Format = gcnew RtStreamFormat();
	Format->type = ::RtAudioNet::RtAudioFormat::RTAUDIO_FLOAT32;
	Format->sampleRate = 11025;
	Format->channels = 2;

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
	this->outputStream->callbackFired += gcnew EventHandler(this, &RtStreamMixer::callbackHandler);
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

	internalBuffer->Put(tempBuff, 0, tempBuff->Length);

	// Should we be buffering, or should we be outputting?
	if(running || framesBuffered >= FramesToBuffer)	
	{
		if (!running && framesBuffered >= FramesToBuffer)	
		{
			running = true;
		} // end if

		internalBuffer->Get(tempBuff, 0, tempBuff->Length);
		outputStream->Write(tempBuff);
		//Console::WriteLine("Do Stuff!");
	}
	else
	{
		framesBuffered += 1;
	} // end if
} // end callbackHandler