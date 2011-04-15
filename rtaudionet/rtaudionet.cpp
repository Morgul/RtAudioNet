// This is the main DLL file.

#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>

#include "rtaudionet.h"

using namespace msclr::interop;
using namespace System::Runtime::InteropServices;

namespace RtAudioNet
{
	
	//////////////////////////////////////////////////////////////////////////
	/// RtAudio Class

	// A static function to determine the available compiled audio APIs.
	List<RtAudio::Api>^ RtAudio::getCompiledApi()
	{
		List<RtAudio::Api>^ apiList = gcnew List<RtAudio::Api>();
		::std::vector<::RtAudio::Api> _apiList;

		// Call the unmanaged function
		::RtAudio::getCompiledApi(_apiList);
		
		// Iterate over the vector and populate apiList
		for(std::vector<::RtAudio::Api>::iterator it = _apiList.begin(); it != _apiList.end(); ++it) 
		{
			apiList->Add(static_cast<RtAudio::Api>(*it));
		} // end for

		return apiList;
	} // end getCompiledApi
		
	// The class constructor.
	RtAudio::RtAudio()
	{
		// This should be the only line in this function. All other code should be in initialize.
		initialize(Api::UNSPECIFIED);
	} // end RtAudio

	// The class constructor.
	RtAudio::RtAudio(RtAudio::Api api)
	{
		// This should be the only line in this function. All other code should be in initialize.
		initialize(api);
	} // end RtAudio
	
	// The destructor.
	RtAudio::~RtAudio()
	{
		delete _rtaudio;
		// Cleanup GCHandle
		//handle.Free();
	} // end ~RtAudio
	
	// Returns the audio API specifier for the current instance of RtAudio.
	RtAudio::Api RtAudio::getCurrentApi()
	{
		return static_cast<RtAudio::Api>(_rtaudio->getCurrentApi());
	} // end getCurrentApi
	
	// A public function that queries for the number of audio devices available.
	unsigned int RtAudio::getDeviceCount()
	{
		return _rtaudio->getDeviceCount();
	} // end getDeviceCount
	
	// Return an RtAudio::DeviceInfo structure for a specified device number.
	RtAudio::DeviceInfo^ RtAudio::getDeviceInfo(unsigned int device)
	{
		// Get the unmanaged DeviceInfo class
		return gcnew RtAudio::DeviceInfo(_rtaudio->getDeviceInfo(device));
	} // end getDeviceInfo
	
	// A function that returns the index of the default output device.
	unsigned int RtAudio::getDefaultOutputDevice()
	{
		return _rtaudio->getDefaultOutputDevice();
	} // end getDefaultOutputDevice
	
	// A function that returns the index of the default input device.
	unsigned int RtAudio::getDefaultInputDevice()
	{
		return _rtaudio->getDefaultInputDevice();
	} // end getDefaultInputDevice
	
	// A public function for opening a stream with the specified parameters.
	void RtAudio::openStream(RtAudio::StreamParameters^ outputParameters, RtAudio::StreamParameters^ inputParameters, RtAudioFormat format, 
		unsigned int sampleRate, unsigned int bufferFrames, RtAudioNetCallback^ callback, Object^ userData, RtAudio::StreamOptions^ options)
	{
		// Painful conversion code
		GCHandle handle = GCHandle::Alloc(this, GCHandleType::Normal);

		try
		{
    		// Call the unmanaged function
    		_rtaudio->openStream(convertManagedToUnmanaged(outputParameters), convertManagedToUnmanaged(inputParameters), format, sampleRate, &bufferFrames, &audioCallback, static_cast<IntPtr>(handle).ToPointer(), convertManagedToUnmanaged(options));
		}
        catch (::RtError &exception)
		{
			throwError(&exception);
		} // end try/catch

		// Set frames --This is to work around inability to pass by reference to C#.
		frames = bufferFrames;

		// Set our callback to the delegate we got from the caller.
		_callback = callback;

		//Set userData
		this->userData = userData;
	} // end openStream
	
	// A function that closes a stream and frees any associated stream memory.
	void RtAudio::closeStream()
	{
		_rtaudio->closeStream();
	} // end closeStream()
	
	// A function that starts a stream.
	void RtAudio::startStream()
	{
		_rtaudio->startStream();
	} // end startStream
	
	// Stop a stream, allowing any samples remaining in the output queue to be played.
	void RtAudio::stopStream()
	{
		_rtaudio->stopStream();
	} // end stopStream
	
	// Stop a stream, discarding any samples remaining in the input/output queue.
	void RtAudio::abortStream()
	{
		_rtaudio->abortStream();
	} // end abortStream
	
	// Returns true if a stream is open and false if not.
	bool RtAudio::isStreamOpen()
	{
		return _rtaudio->isStreamOpen();
	} // end isStreamOpen()
	
	// Returns true if the stream is running and false if it is stopped or not open.
	bool RtAudio::isStreamRunning()
	{
		return _rtaudio->isStreamRunning();
	} // end isStreamRunning
	
	// Returns the number of elapsed seconds since the stream was started.
	double RtAudio::getStreamTime()
	{
		return _rtaudio->getStreamTime();
	} // end getStreamTime
	
	// Returns the internal stream latency in sample frames.
	long RtAudio::getStreamLatency()
	{
		return _rtaudio->getStreamLatency();
	} // end getStreamLatency
	
	// Returns actual sample rate in use by the stream.
	unsigned int RtAudio::getStreamSampleRate()
	{
		return _rtaudio->getStreamSampleRate();
	} // end getStreamSampleRate
	
	// Specify whether warning messages should be printed to stderr.
	void RtAudio::showWarnings(bool value)
	{
		_rtaudio->showWarnings(value);
	} // end showWarnings
	
	/// Protected \\\
	
	// Actual constructor. This is a work around for issues using default parameters in managed C++.
	void RtAudio::initialize(RtAudio::Api api)
	{
		_rtaudio = new ::RtAudio(::RtAudio::Api(int(api)));
		frames = 0;
	} // end initialize

	// Converstion of StreamParameters^ to StreamParameters*
	::RtAudio::StreamParameters* RtAudio::convertManagedToUnmanaged(RtAudio::StreamParameters^ _params)
	{
		::RtAudio::StreamParameters* params = new ::RtAudio::StreamParameters();

		params->deviceId = _params->deviceId;
		params->nChannels = _params->nChannels;
		params->firstChannel = _params->firstChannel;
		
		return params;
	} // end convertManagedToUnmanaged

	// Converstion of StreamOptions^ to StreamOptions*
	::RtAudio::StreamOptions* RtAudio::convertManagedToUnmanaged(RtAudio::StreamOptions^ _options)
	{
		::RtAudio::StreamOptions* options = new ::RtAudio::StreamOptions();

		options->numberOfBuffers = _options->numberOfBuffers;

		// Convert streamName
		std::string streamName;
		
		if(_options->streamName != nullptr)
		{
			String^ name = gcnew String(_options->streamName);
			streamName = marshal_as<std::string>(name);
		} // end if

		options->streamName = streamName;
		
		options->priority = _options->priority;

		return options;
	} // end convertManagedToUnmanaged

	//////////////////////////////////////////////////////////////////////////
	/// Audio Callback

	int audioCallback(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *userData)
	{
		// We've stored a pointer to the instance of our managed RtAudio class in the userData.
		// This will retrieve it, and cast it appropriately.
		GCHandle instHandle = GCHandle::FromIntPtr(IntPtr(userData));
		RtAudio^ instance = (RtAudio^) instHandle.Target;

		// Call the callback registered with the instance.
		instance->_callback(IntPtr(outputBuffer), IntPtr(inputBuffer), nBufferFrames, streamTime, status, instance->userData);

		return 0;
	} // end audioCallback
} // end namespace