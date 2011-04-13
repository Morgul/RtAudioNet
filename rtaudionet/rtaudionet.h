// rtaudionet.h

#pragma once

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
	typedef int (*RtAudioCallback)( void *outputBuffer, void *inputBuffer,
									unsigned int nFrames,
									double streamTime,
									RtAudioStreamStatus status,
									void *userData );

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
			System::String^ name;			// Character string device identifier.
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

	};
}
