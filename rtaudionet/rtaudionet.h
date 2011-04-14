// rtaudionet.h

#pragma once

#include "RtErrorNet.h"
#include "RtAudio\RtAudio.h"

using namespace System;
using namespace System::Collections::Generic;

namespace RtAudioNet {

    // RtAudio data format type.
	typedef unsigned long RtAudioFormat;
	static const RtAudioFormat RTAUDIO_SINT8 = 0x1;    // 8-bit signed integer.
	static const RtAudioFormat RTAUDIO_SINT16 = 0x2;   // 16-bit signed integer.
	static const RtAudioFormat RTAUDIO_SINT24 = 0x4;   // Lower 3 bytes of 32-bit signed integer.
	static const RtAudioFormat RTAUDIO_SINT32 = 0x8;   // 32-bit signed integer.
	static const RtAudioFormat RTAUDIO_FLOAT32 = 0x10; // Normalized between plus/minus 1.0.
	static const RtAudioFormat RTAUDIO_FLOAT64 = 0x20; // Normalized between plus/minus 1.0.

    // RtAudio stream option flags.
	typedef unsigned int RtAudioStreamFlags;
	static const RtAudioStreamFlags RTAUDIO_NONINTERLEAVED = 0x1;    // Use non-interleaved buffers (default = interleaved).
	static const RtAudioStreamFlags RTAUDIO_MINIMIZE_LATENCY = 0x2;  // Attempt to set stream parameters for lowest possible latency.
	static const RtAudioStreamFlags RTAUDIO_HOG_DEVICE = 0x4;        // Attempt grab device and prevent use by others.
	static const RtAudioStreamFlags RTAUDIO_SCHEDULE_REALTIME = 0x8; // Try to select realtime scheduling for callback thread.
	static const RtAudioStreamFlags RTAUDIO_ALSA_USE_DEFAULT = 0x10; // Use the "default" PCM device (ALSA only).
	
	// RtAudio stream status (over- or underflow) flags.
	typedef unsigned int RtAudioStreamStatus;
	static const RtAudioStreamStatus RTAUDIO_INPUT_OVERFLOW = 0x1;    // Input data was discarded because of an overflow condition at the driver.
	static const RtAudioStreamStatus RTAUDIO_OUTPUT_UNDERFLOW = 0x2;  // The output buffer ran low, likely causing a gap in the output sound.
	
	// RtAudio callback function prototype.
	/*typedef int (*RtAudioCallback)( void *outputBuffer, void *inputBuffer,
									unsigned int nFrames,
									double streamTime,
									RtAudioStreamStatus status,
									void *userData );*/

	typedef Func<IntPtr, IntPtr, unsigned int, double, RtAudioStreamStatus, IntPtr, int> RtAudioCallback;

	public ref class RtAudio
	{
	public:

		// Audio API specifier arguments.
		enum class Api
		{
			UNSPECIFIED,    // Search for a working compiled API.
			LINUX_ALSA,     // The Advanced Linux Sound Architecture API.
			LINUX_OSS,      // The Linux Open Sound System API.
			UNIX_JACK,      // The Jack Low-Latency Audio Server API.
			MACOSX_CORE,    // Macintosh OS-X Core Audio API.
			WINDOWS_ASIO,   // The Steinberg Audio Stream I/O API.
			WINDOWS_DS,     // The Microsoft Direct Sound API.
			RTAUDIO_DUMMY   // A compilable but non-functional API.
		}; // end Api

		// The public device information structure for returning queried values.
		ref struct DeviceInfo 
		{
			bool probed;					// true if the device capabilities were successfully probed.
			String^ name;			// Character string device identifier.
			unsigned int outputChannels;	// Maximum output channels supported by device.
			unsigned int inputChannels;		// Maximum input channels supported by device.
			unsigned int duplexChannels;	// Maximum simultaneous input/output channels supported by device.
			bool isDefaultOutput;			// true if this is the default output device.
			bool isDefaultInput;			// true if this is the default input device.
			List<unsigned int> sampleRates;	// Supported sample rates (queried from list of standard rates).
			RtAudioFormat nativeFormats;	// Bit mask of supported data formats.
			
			// Default constructor.
			DeviceInfo() : probed(false), outputChannels(0), inputChannels(0), duplexChannels(0),
				isDefaultOutput(false), isDefaultInput(false), nativeFormats(0) {}

			DeviceInfo(::RtAudio::DeviceInfo _info)
			{
				// Translate from the unmanaged instance to the managed instance
				probed = _info.probed;
				name = gcnew String(_info.name.c_str());;
				outputChannels = _info.outputChannels;
				inputChannels = _info.inputChannels;
				duplexChannels = _info.duplexChannels;
				isDefaultOutput = _info.isDefaultOutput;
				isDefaultInput = _info.isDefaultInput;
				nativeFormats = _info.nativeFormats;
				
				List<unsigned int>^ sampleRateList = gcnew List<unsigned int>();
		
				// Get the sample rates
				for(std::vector<unsigned int>::iterator it = _info.sampleRates.begin(); it != _info.sampleRates.end(); ++it) 
				{
					sampleRateList->Add(*it);
				} // end for
				
				// Set the sample rate to our converted list.
				sampleRates.AddRange(sampleRateList);
			} // end DefaultAudio
		}; // end DeviceInfo
		
		// The structure for specifying input or ouput stream parameters.
		ref struct StreamParameters 
		{
			unsigned int deviceId;     // Device index (0 to getDeviceCount() - 1).
			unsigned int nChannels;    // Number of channels.
			unsigned int firstChannel; // First channel index on device (default = 0).
			
			// Default constructor.
			StreamParameters() : deviceId(0), nChannels(0), firstChannel(0) {}
		}; // end StreamParameters

		// The structure for specifying stream options.
		ref struct StreamOptions 
		{
			RtAudioStreamFlags flags;		// A bit-mask of stream flags (RTAUDIO_NONINTERLEAVED, RTAUDIO_MINIMIZE_LATENCY, RTAUDIO_HOG_DEVICE, RTAUDIO_ALSA_USE_DEFAULT).
			unsigned int numberOfBuffers;	// Number of stream buffers.
			String^ streamName;				// A stream name (currently used only in Jack).
			int priority;					// Scheduling priority of callback thread (only used with flag RTAUDIO_SCHEDULE_REALTIME).
			
			// Default constructor.
			StreamOptions() : flags(0), numberOfBuffers(0), priority(0) {}
		};
		
		// A static function to determine the available compiled audio APIs.
		static List<RtAudio::Api>^ getCompiledApi();
		
		// The class constructor.
		RtAudio();

		// The class constructor.
		RtAudio(RtAudio::Api api);

		// The destructor.
		~RtAudio();
		
		// Returns the audio API specifier for the current instance of RtAudio.
		RtAudio::Api getCurrentApi();
		
		// A public function that queries for the number of audio devices available.
		unsigned int getDeviceCount();
		
		// Return an RtAudio::DeviceInfo structure for a specified device number.
		RtAudio::DeviceInfo^ getDeviceInfo(unsigned int device);
		
		// A function that returns the index of the default output device.
		unsigned int getDefaultOutputDevice();
		
		// A function that returns the index of the default input device.
		unsigned int getDefaultInputDevice();
		
		// A public function for opening a stream with the specified parameters.
		void openStream( RtAudio::StreamParameters^ outputParameters, RtAudio::StreamParameters^ inputParameters, RtAudioFormat format, unsigned int sampleRate, 
			array<unsigned int>^ bufferFrames, RtAudioCallback^ callback) {openStream(outputParameters, inputParameters, format, sampleRate, bufferFrames, callback, nullptr, gcnew RtAudio::StreamOptions());};
		
		// A public function for opening a stream with the specified parameters.
		void openStream( RtAudio::StreamParameters^ outputParameters, RtAudio::StreamParameters^ inputParameters, RtAudioFormat format, unsigned int sampleRate, 
			array<unsigned int>^ bufferFrames, RtAudioCallback^ callback, Object^ userData) {openStream(outputParameters, inputParameters, format, sampleRate, bufferFrames, callback, userData, gcnew RtAudio::StreamOptions());};
		
		// A public function for opening a stream with the specified parameters.
		void openStream( RtAudio::StreamParameters^ outputParameters, RtAudio::StreamParameters^ inputParameters, RtAudioFormat format, unsigned int sampleRate, 
			array<unsigned int>^ bufferFrames, RtAudioCallback^ callback, RtAudio::StreamOptions^ options) {openStream(outputParameters, inputParameters, format, sampleRate, bufferFrames, callback, nullptr, options);};
		
		// A public function for opening a stream with the specified parameters.
		void openStream( RtAudio::StreamParameters^ outputParameters, RtAudio::StreamParameters^ inputParameters, RtAudioFormat format, unsigned int sampleRate, 
			array<unsigned int>^ bufferFrames, RtAudioCallback^ callback, Object^ userData, RtAudio::StreamOptions^ options);
		
		// A function that closes a stream and frees any associated stream memory.
		void closeStream();
		
		// A function that starts a stream.
		void startStream();
		
		// Stop a stream, allowing any samples remaining in the output queue to be played.
		void stopStream();
		
		// Stop a stream, discarding any samples remaining in the input/output queue.
		void abortStream();
		
		// Returns true if a stream is open and false if not.
		bool isStreamOpen();
		
		// Returns true if the stream is running and false if it is stopped or not open.
		bool isStreamRunning();
		
		// Returns the number of elapsed seconds since the stream was started.
		double getStreamTime();
		
		// Returns the internal stream latency in sample frames.
		long getStreamLatency();
		
		// Returns actual sample rate in use by the stream.
		unsigned int getStreamSampleRate();
		
		// Specify whether warning messages should be printed to stderr.
		void showWarnings() {showWarnings(true);};

		// Specify whether warning messages should be printed to stderr.
		void showWarnings(bool value);
		
	protected:
		
		// Actual constructor. This is a work around for issues using default parameters in managed C++.
		void initialize(RtAudio::Api api);

		// Converstion of StreamParameters^ to StreamParameters*
		::RtAudio::StreamParameters* convertManagedToUnmanaged(RtAudio::StreamParameters^ _params);

		// Converstion of StreamOptions^ to StreamOptions*
		::RtAudio::StreamOptions* convertManagedToUnmanaged(RtAudio::StreamOptions^ _options);

		// Actual RtAudio instance
		::RtAudio* _rtaudio;

	};

	// This global structure type is used to pass callback information
	// between the private RtAudio stream structure and global callback
	// handling functions.
	ref struct CallbackInfo {
		void *object;			// Used as a "this" pointer.
		ThreadHandle thread;
		void *callback;
		void *userData;
		void *apiInfo;			// void pointer for API specific callback information
		bool isRunning;
		
		// Default constructor.
		CallbackInfo() : object(0), callback(0), userData(0), apiInfo(0), isRunning(false) {}
	};
} // end namespace
