// This is the main DLL file.

#include "rtaudionet.h"

namespace RtAudioNet
{
	
	//////////////////////////////////////////////////////////////////////////
	/// RtAudio Class

	// A static function to determine the available compiled audio APIs.
	void RtAudio::getCompiledApi(List<RtAudio::Api> ^apis)
	{
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
	} // end ~RtAudio
	
	// Returns the audio API specifier for the current instance of RtAudio.
	RtAudio::Api RtAudio::getCurrentApi()
	{
		return Api::UNSPECIFIED;
	} // end getCurrentApi
	
	// A public function that queries for the number of audio devices available.
	unsigned int RtAudio::getDeviceCount()
	{
		return 0;
	} // end getDeviceCount
	
	// Return an RtAudio::DeviceInfo structure for a specified device number.
	RtAudio::DeviceInfo^ RtAudio::getDeviceInfo(unsigned int device)
	{
		return gcnew DeviceInfo();
	} // end getDeviceInfo
	
	// A function that returns the index of the default output device.
	unsigned int RtAudio::getDefaultOutputDevice()
	{
		return 0;
	} // end getDefaultOutputDevice
	
	// A function that returns the index of the default input device.
	unsigned int RtAudio::getDefaultInputDevice()
	{
		return 0;
	} // end getDefaultInputDevice
	
	// A public function for opening a stream with the specified parameters.
	void RtAudio::openStream( RtAudio::StreamParameters^ outputParameters, RtAudio::StreamParameters^ inputParameters, RtAudioFormat^ format, 
		unsigned int sampleRate, unsigned int *bufferFrames, RtAudioCallback callback, void *userData, RtAudio::StreamOptions^ options)
	{
	} // end openStream
	
	// A function that closes a stream and frees any associated stream memory.
	void RtAudio::closeStream()
	{
	} // end closeStream()
	
	// A function that starts a stream.
	void RtAudio::startStream()
	{
	} // end startStream
	
	// Stop a stream, allowing any samples remaining in the output queue to be played.
	void RtAudio::stopStream()
	{
	} // end stopStream
	
	// Stop a stream, discarding any samples remaining in the input/output queue.
	void RtAudio::abortStream()
	{
	} // end abortStream
	
	// Returns true if a stream is open and false if not.
	bool RtAudio::isStreamOpen()
	{
		return false;
	} // end isStreamOpen()
	
	// Returns true if the stream is running and false if it is stopped or not open.
	bool RtAudio::isStreamRunning()
	{
		return false;
	} // end isStreamRunning
	
	// Returns the number of elapsed seconds since the stream was started.
	double RtAudio::getStreamTime()
	{
		return 0.0;
	} // end getStreamTime
	
	// Returns the internal stream latency in sample frames.
	long RtAudio::getStreamLatency()
	{
		return 0;
	} // end getStreamLatency
	
	// Returns actual sample rate in use by the stream.
	unsigned int RtAudio::getStreamSampleRate()
	{
		return 0;
	} // end getStreamSampleRate
	
	// Specify whether warning messages should be printed to stderr.
	void RtAudio::showWarnings(bool value)
	{
	} // end showWarnings
	
	/// Protected \\\
	
	// Actual constructor. This is a work around for issues using default parameters in managed C++.
	void RtAudio::initialize(RtAudio::Api api)
	{
	} // end initialize

	void RtAudio::openRtApi(RtAudio::Api api)
	{
	} // end openRtApit
} // end namespace