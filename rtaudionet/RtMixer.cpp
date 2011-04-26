#include "RtMixer.h"

using namespace RtStream;

// Default Constructor
RtStreamMixer::RtStreamMixer()
{
	if (Format == nullptr)
	{
		Format = gcnew RtStreamFormat();
		Format->type = ::RtAudioNet::RtAudioFormat::RTAUDIO_FLOAT32;
		Format->sampleRate = 11025;
		Format->channels = 2;
	} // end if

	FramesToBuffer = 1;

	// Internal buffer
	internalBuffer = gcnew CircularBuffer<float>(1024, true);

	running = false;

	// All our internal inputStreams
	inputStreams = gcnew Dictionary<String^, RtInputStream^>();
} // end RtStreamMixer

// Default Destructor
RtStreamMixer::~RtStreamMixer()
{
	Stop();
} // end ~RtStreamMixer

// Add an input stream to the mixer
void RtStreamMixer::AddInputStream(RtInputStream^ inputStream, String^ name)
{
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
	inputStreams[name] = inputStream;
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
	for each(KeyValuePair<String^, RtInputStream^>^ kvp in inputStreams)
	{
		kvp->Value->Start();
	} // end for

	outputStream->Start();
} // end Start

// Stop the mixer
void RtStreamMixer::Stop()
{
	for each(KeyValuePair<String^, RtInputStream^>^ kvp in inputStreams)
	{
		kvp->Value->Stop();
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
	// FIXME: What should this really be set to for size?!
	array<float>^ tempBuff = gcnew array<float>(outputStream->Frames * outputStream->Format->channels);
	array<float>^ inputBuff = gcnew array<float>(outputStream->Frames * outputStream->Format->channels);

	for each(KeyValuePair<String^, RtInputStream^>^ kvp in inputStreams)
	{
		RtInputStream^ inputStream = kvp->Value;
		int floatsRead = inputStream->Read(inputBuff);

		for(int idx = 0; idx < floatsRead; idx++)
		{
			tempBuff[idx] += inputBuff[idx];
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