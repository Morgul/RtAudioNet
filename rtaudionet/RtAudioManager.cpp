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

#include "RtAudioManager.h"


namespace RtStream
{
	//////////////////////////////////////////////////////////////////////////
	/// RtAudioManager

	// Default Constructor
	RtAudioManager::RtAudioManager()
	{
		_api = ::RtAudioNet::RtAudio::Api::UNSPECIFIED;

		// Finish initialization
		initialize();
	} // end RtAudioManager

	// Default Constructor
	RtAudioManager::RtAudioManager(::RtAudioNet::RtAudio::Api api)
	{
		_api = api;

		// Finish initialization
		initialize();
	} // end RtAudioManager

	// Default Constructor
	RtAudioManager::~RtAudioManager()	
	{
		if (enumerateTimer->Enabled)
		{
			enumerateTimer->Stop();
		} // end if
	} // end ~RtAudioManager

	void RtAudioManager::initialize()
	{
		// Initialize RtAudio
		rtaudio = gcnew ::RtAudioNet::RtAudio(_api);

		// Initialize device dictionaries
		InputDevices = gcnew Dictionary<String^, int>();
		OutputDevices = gcnew Dictionary<String^, int>();

		// Populate device dictionaries
		EnumerateDevices();

		// Reenumerate the devices every half second
		enumerateTimer = gcnew System::Timers::Timer(500);
		enumerateTimer->Elapsed += gcnew ElapsedEventHandler(this, &RtAudioManager::OnEnumerateTimer);
		//enumerateTimer->Start();
	} // end initialize

	// Finds the input device id by name
	int RtAudioManager::FindInputDeviceByName(String^ name)
	{
		int devID = -1;	
		for each(KeyValuePair<String^, int>^ kvp in InputDevices)
		{
			if (kvp->Key->Contains(name))	
			{
				devID = kvp->Value;
			} // end if
		} // end for each

		return devID;
	} // end FindInputDeviceByName

	// Finds the output device id by name
	int RtAudioManager::FindOutputDeviceByName(String^ name)
	{
		int devID = -1;	
		for each(KeyValuePair<String^, int>^ kvp in OutputDevices)
		{
			if (kvp->Key->Contains(name))	
			{
				devID = kvp->Value;
			} // end if
		} // end for each

		return devID;
	} // end FindOutputDeviceByName

	// Gets the device information for the given device id
	::RtAudioNet::RtAudio::DeviceInfo^ RtAudioManager::GetDeviceInfo(int devID)	
	{
		return rtaudio->getDeviceInfo((unsigned int) devID);
	} // end GetDeviceInfo

	// Creates and returns a mixer based on the inputs and outputs
	RtStreamMixer^ RtAudioManager::CreateMixer(List<int>^ inputs, int output)
	{
		// See how many inputs we have. If we only have a single input (or no inputs), then it's much more performant to use
		// a RtDuplexMixer. This greatly improves performance over using the standard RtStreamMixer class.
		RtStreamMixer^ mixer = nullptr;
		if (inputs->Count < 2)
		{
			mixer = (RtStreamMixer^) gcnew RtDuplexMixer();
		}
		else
		{
			mixer = gcnew RtStreamMixer();
		} // end if

		return _createMixer(inputs, output, mixer);
	} // end CreateMixer

	// Creates and returns a mixer based on the inputs and outputs
	RtStreamMixer^ RtAudioManager::CreateMixer(List<String^>^ inputs, String^ output)
	{
		// See how many inputs we have. If we only have a single input (or no inputs), then it's much more performant to use
		// a RtDuplexMixer. This greatly improves performance over using the standard RtStreamMixer class.
		RtStreamMixer^ mixer = nullptr;
		if (inputs->Count < 2)
		{
			mixer = (RtStreamMixer^) gcnew RtDuplexMixer();
		}
		else
		{
			mixer = gcnew RtStreamMixer();
		} // end if

		return _createMixer(inputs, output, mixer);
	} // end CreateMixer

	// Creates and returns a mixer based on the inputs and outputs
	RtStreamMixer^ RtAudioManager::CreateMixer(List<Dictionary<String^, String^>^>^ inputs, int output)
	{
		// See how many inputs we have. If we only have a single input (or no inputs), then it's much more performant to use
		// a RtDuplexMixer. This greatly improves performance over using the standard RtStreamMixer class.
		RtStreamMixer^ mixer = nullptr;
		if (inputs->Count < 2)
		{
			mixer = (RtStreamMixer^) gcnew RtDuplexMixer();
		}
		else
		{
			mixer = gcnew RtStreamMixer();
		} // end if

		return _createMixer(inputs, output, mixer);
	} // end CreateMixer

	// Creates and returns a mixer based on the inputs and outputs
	RtStreamMixer^ RtAudioManager::CreateMixer(List<int>^ inputs, int output, int sampleRate)
	{
		// See how many inputs we have. If we only have a single input (or no inputs), then it's much more performant to use
		// a RtDuplexMixer. This greatly improves performance over using the standard RtStreamMixer class.
		RtStreamMixer^ mixer = nullptr;
		if (inputs->Count < 2)
		{
			mixer = (RtStreamMixer^) gcnew RtDuplexMixer();
		}
		else
		{
			mixer = gcnew RtStreamMixer();
		} // end if
		mixer->Format->sampleRate = sampleRate;

		return _createMixer(inputs, output, mixer);
	} // end CreateMixer

	// Creates and returns a mixer based on the inputs and outputs
	RtStreamMixer^ RtAudioManager::CreateMixer(List<String^>^ inputs, String^ output, int sampleRate)
	{
		// See how many inputs we have. If we only have a single input (or no inputs), then it's much more performant to use
		// a RtDuplexMixer. This greatly improves performance over using the standard RtStreamMixer class.
		RtStreamMixer^ mixer = nullptr;
		if (inputs->Count < 2)
		{
			mixer = (RtStreamMixer^) gcnew RtDuplexMixer();
		}
		else
		{
			mixer = gcnew RtStreamMixer();
		} // end if
		mixer->Format->sampleRate = sampleRate;

		return _createMixer(inputs, output, mixer);
	} // end CreateMixer

	// Returns true if the list of devices changed.
	bool RtAudioManager::EnumerateDevices()
	{
		// Update DeviceCount	
		DeviceCount = rtaudio->getDeviceCount();

		Dictionary<String^, int>^ inputDevices = gcnew Dictionary<String^, int>();
		Dictionary<String^, int>^ outputDevices = gcnew Dictionary<String^, int>();

		// Update Device Dictionaries
		for (int idx = 0; DeviceCount > idx; idx++)
		{
			::RtAudioNet::RtAudio::DeviceInfo^ info = rtaudio->getDeviceInfo(idx);

			if (info->inputChannels > 0)
			{
				inputDevices[info->name] = idx;
			} // end if

			if (info->outputChannels > 0)
			{
				outputDevices[info->name] = idx;
			} // end if
		} // end for

		InputDevices = inputDevices;

		OutputDevices = outputDevices;

		// TODO: Actually look for equality, return true or false if changed.
		return false;
	} // end EnumerateDevices

	void RtAudioManager::OnEnumerateTimer(Object^ sender, ElapsedEventArgs^ args)
	{
		int devCount = DeviceCount;

		bool changed = EnumerateDevices();

		if ((devCount != DeviceCount) || changed)
		{
			DeviceEnumerationChanged(this, gcnew EventArgs());
		} // end if
	} // end OnEnumerateTimer

	// Creates and returns a mixer based on the inputs and outputs
	RtStreamMixer^ RtAudioManager::_createMixer(List<int>^ inputs, int output, RtStreamMixer^ mixer)
	{
		for each(int inputID in inputs)
		{
			RtInputStream^ inputStream = gcnew RtInputStream(512);
			inputStream->Name = String::Format("Input {0}", inputID);
			inputStream->selectInputDevice(inputID);

			try
			{
				mixer->AddInputStream(inputStream);
			}
			catch(System::Exception^) { /* TODO: Report this error. */ }
		} // end for

		RtOutputStream^ outputStream = gcnew RtOutputStream(512);
		outputStream->selectOutputDevice(output);

		mixer->SetOutputStream(outputStream);

		return mixer;
	} // end CreateMixer

	// Creates and returns a mixer based on the inputs and outputs
	RtStreamMixer^ RtAudioManager::_createMixer(List<String^>^ inputs, String^ output, RtStreamMixer^ mixer)
	{
		for each(String^ inputString in inputs)
		{
			int inputID = FindInputDeviceByName(inputString);
			RtInputStream^ inputStream = gcnew RtInputStream(512);
			inputStream->Name = inputString;
			inputStream->selectInputDevice(inputID);

			try
			{
				mixer->AddInputStream(inputStream);
			}
			catch(System::Exception^)
			{
				RtStream::RtAudioErrorEventArgs^ eventArgs = gcnew RtStream::RtAudioErrorEventArgs();
				eventArgs->errorString->Format("Failed to add inputID: {0} to mixer.", inputID);
				ErrorOccured(this, eventArgs);
			}
		} // end for

		RtOutputStream^ outputStream = gcnew RtOutputStream(512);
		outputStream->selectOutputDevice(output);

		mixer->SetOutputStream(outputStream);

		return mixer;
	} // end CreateMixer

	// Creates and returns a mixer based on the inputs and outputs
	RtStreamMixer^ RtAudioManager::_createMixer(List<Dictionary<String^, String^>^>^ inputs, int output, RtStreamMixer^ mixer)
	{
		for each(Dictionary<String^, String^>^ input in inputs)
		{
			RtInputStream^ inputStream = gcnew RtInputStream(512);
			inputStream->Name = String::Format("Input {0}", input["id"]);
			inputStream->selectInputDevice(System::Convert::ToInt32(input["id"]));

			try
			{
				mixer->AddInputStream(inputStream, System::Convert::ToSingle(input["gain"]), System::Convert::ToSingle(input["pan"]));
			}
			catch(System::Exception^) { /* TODO: Report this error. */ }
		} // end for

		RtOutputStream^ outputStream = gcnew RtOutputStream(512);
		outputStream->selectOutputDevice(output);

		mixer->SetOutputStream(outputStream);

		return mixer;
	} // end CreateMixer

	//////////////////////////////////////////////////////////////////////////
	/// Static Methods

	// Simply returns the static instance, or creates a new one if it's null
	RtAudioManager^ RtAudioManager::GetInstance()
	{
		if (_manager == nullptr)
		{
			_manager = gcnew RtAudioManager();
		} // end if

		return _manager;
	} // end GetInstance

	// Attempts to create a new instance with the given api, or throws an exception if the static instance is not null
	RtAudioManager^ RtAudioManager::GetInstance(::RtAudioNet::RtAudio::Api api)
	{
		if (_manager == nullptr)
		{
			_manager = gcnew RtAudioManager(api);
		}
		else
		{
			if (_manager->GetCurrentApi() != api)
			{
				throw gcnew RtAudioManagerApiException("Tried to instantiate RtAudioManager with a different API than the current global instance!");
			} // end if
		} // end if

		return _manager;
	} // end GetInstance
} // end namespace