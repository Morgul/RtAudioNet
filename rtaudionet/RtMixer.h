#include "RtStream.h"

using namespace System::Collections::Generic;

namespace RtStream
{
	// Wraps an RtInputStream, and provides Pan/Gain support.
	public ref class RtMixerInput
	{
	public:
		// Constructor	
		RtMixerInput();

		// Constructor	
		RtMixerInput(RtInputStream^ stream);

		// Constructor	
		RtMixerInput(RtInputStream^ stream, float gain, float pan);

		// Stream Gain
		property float Gain;

		// Stream Pan
		property float Pan;

		// Stream
		property RtInputStream^ InputStream;
	};

	public ref class RtStreamMixer
	{
	public:
		// Default Constructor
		RtStreamMixer();

		// Default Destructor
		~RtStreamMixer();

		// Add an input stream to the mixer
		void AddInputStream(RtInputStream^ inputStream);

		// Add an input stream to the mixer
		void AddInputStream(RtInputStream^ inputStream, float gain, float pan);

		// Add an input stream to the mixer
		void AddInputStream(RtMixerInput^ input);

		// Add an outputstream to the mixer
		void SetOutputStream(RtOutputStream^ outputStream);

		// Adjust the gain on the selected input
		void AdjustGain(String^ inputName, float gain);

		// Adjust the pan on the selected input
		void AdjustPan(String^ inputName, float pan);

		// Start the mixer
		void Start();

		// Stop the mixer
		void Stop();

		// Is the mixer currently running?
		bool IsRunning();

		// Output Format
		RtStreamFormat^ Format;

		// Frames of audio to buffer
		unsigned int FramesToBuffer;

	private:
		// Callback Event Handler
		void callbackHandler(Object^ sender, EventArgs^ e);

		// Internal buffer
		CircularBuffer<float>^ internalBuffer;

		// Is the mixer running
		bool running;

		// Frames we've buffered
		unsigned int framesBuffered;

		// All our internal inputStreams
		Dictionary<String^, RtMixerInput^>^ inputs;

		// Our outputStream
		RtOutputStream^ outputStream;
	}; // end RtStreamMixer
} // end namespace