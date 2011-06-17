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

#include "RtStream.h"
#include <msclr\lock.h>

namespace RtStream
{
	//////////////////////////////////////////////////////////////////////////
	/// RtAudioStream Class

	RtAudioStream::RtAudioStream()
	{
		// Since we don't use it, there's no need to instantiate it.
		//rtaudio = gcnew ::RtAudioNet::RtAudio();
		internalBuffer = gcnew CircularBuffer<float>(4096);

		_canRead = true;
		_canWrite = true;
		_canSeek = false;

		Name = "";
	} // end RtWaveStream
	
	// Read method required by the stream base class.
	int RtAudioStream::Read([InAttribute] [OutAttribute] array<unsigned char>^ buffer, int offset, int count)
	{
		array<float>^ tempBuff = gcnew array<float>(buffer->Length / 4);
		int bytesRead = 4 * internalBuffer->Get(tempBuff, offset, count);

		// Copy to byte array
		Buffer::BlockCopy(tempBuff, 0, buffer, 0, buffer->Length);

		return bytesRead;
	} // end Read

	// Read method required by the stream base class.
	int RtAudioStream::Read([InAttribute] [OutAttribute] array<float>^ buffer, int offset, int count)
	{
		return internalBuffer->Get(buffer, offset, count); 
	} // end Read

	// Read method required by the stream base class.
	int RtAudioStream::Read(float buffer[], int offset, int count)
	{
		array<float>^ tempBuff = gcnew array<float>(count);
		int read = internalBuffer->Get(tempBuff, 0, count); 
		Marshal::Copy(tempBuff, 0, IntPtr(buffer + offset), read);

		if (read != count)
		{
			BufferUnderrun(this, gcnew EventArgs());
		} // end if

		return read;
	} // end Read

	// Read class that's more convienent.
	int RtAudioStream::Read([InAttribute] [OutAttribute] array<float>^ buffer)
	{
		return Read(buffer, 0, buffer->Length);
	} // end Read
	
	// Write method required by the stream base class.
	void RtAudioStream::Write(array<unsigned char>^ buffer, int offset, int count)
	{
		array<float>^ tempBuff = gcnew array<float>(buffer->Length / 4);
		int bytesRead = 4 * internalBuffer->Get(tempBuff, offset, count);

		// Copy to byte array
		Buffer::BlockCopy(buffer, 0, tempBuff, 0, buffer->Length);

		internalBuffer->Put(tempBuff, offset, count);
	} //end Write

	// Write method required by the stream base class.
	void RtAudioStream::Write(array<float>^ buffer, int offset, int count)
	{
		internalBuffer->Put(buffer, offset, count);
	} //end Write

	// Write method required by the stream base class.
	void RtAudioStream::Write(float buffer[], int offset, int count)
	{
		array<float>^ tempBuff = gcnew array<float>(count);
		Marshal::Copy(IntPtr(buffer + offset), tempBuff, 0, count);
		internalBuffer->Put(tempBuff, 0, count);
	} //end Write

	// Write class that's more convienent.
	void RtAudioStream::Write(array<float>^ buffer)
	{
		Write(buffer, 0, buffer->Length);
	} // end Write


	//////////////////////////////////////////////////////////////////////////
	/// RtInputStream Class

	// Finish initializing the stream
	void RtInputStream::initialize()
	{
		rtaudio = gcnew ::RtAudioNet::RtAudio();
		inputStreamParams = gcnew ::RtAudioNet::RtAudio::StreamParameters();
		
		if (Format == nullptr)
		{
			Format = gcnew RtStreamFormat();
			Format->sampleRate = 22050;
			Format->channels = 2;
			Format->bitsPerSample = 16;

		} // end if

		// (Frames per sample * channels) * numSamplesToBuffer
		internalBuffer = gcnew CircularBuffer<float>((Frames * Format->channels * 2), true);

		// Set stream properties
		_canRead = true;
		_canWrite = false;
		_canSeek = false;
		Name = "";
	} // end initialize
	
	RtInputStream::~RtInputStream()	
	{
		if (rtaudio->isStreamRunning())
		{
			rtaudio->stopStream();
		} // end if

		if (rtaudio->isStreamOpen())
		{
			rtaudio->closeStream();
		} // end if
	} // end ~RtDuplexStream

	
	// Selects the correct input device
	void RtInputStream::selectInputDevice(int devID)
	{
		DeviceID = devID;

		RtAudioNet::RtAudio::DeviceInfo^ info = rtaudio->getDeviceInfo(devID);
		Format->channels = info->inputChannels;

		inputStreamParams->deviceId = devID;
		inputStreamParams->nChannels = Format->channels;
	} // end selectInputDevice

	// Selects the correct input device
	void RtInputStream::selectInputDevice(String^ devString)
	{
		unsigned int devID = 0;

		for (unsigned int idx = 0; rtaudio->getDeviceCount() > idx; idx++)
		{
			::RtAudioNet::RtAudio::DeviceInfo^ info = rtaudio->getDeviceInfo(idx);

			if (info->name->Contains(devString))
			{
				devID = idx;
			} // end if
		} // end for

		selectInputDevice(devID);
	} // end selectInputDevice

	// Opens the stream
	void RtInputStream::Open()
	{
		if (Format->options == nullptr)
		{
			rtaudio->openStream(nullptr, inputStreamParams, Format->type, Format->sampleRate, Frames, gcnew ::RtAudioNet::RtAudioNetCallback(this, &RtInputStream::callback));
		}
		else
		{
			rtaudio->openStream(nullptr, inputStreamParams, Format->type, Format->sampleRate, Frames, gcnew ::RtAudioNet::RtAudioNetCallback(this, &RtInputStream::callback), nullptr, Format->options);
		} // end if
	} // end Open

	// Starts the stream
	void RtInputStream::Start()
	{
		int count = 0;	
		while (!(rtaudio->isStreamOpen() && count < 100))
		{
			System::Threading::Thread::Sleep(10);
			count++;
		} // end while

		rtaudio->startStream();
	} // end Start

	// Stops the stream
	void RtInputStream::Stop()
	{
		rtaudio->stopStream();
	} // end Stop
	
	// Stops the stream
	void RtInputStream::Finish()
	{
		rtaudio->closeStream();
	} // end Stop

	// Aborts the stream
	void RtInputStream::Abort()
	{
		rtaudio->abortStream();
	} // end Abort
	
	// Read class required by the stream base class.
	int RtInputStream::Read([InAttribute] [OutAttribute] array<float>^ buffer, int offset, int count)
	{
		int dataRead = 0;
		msclr::lock lk(internalBuffer);
		dataRead = internalBuffer->Get(buffer, offset, count); 

		if (dataRead != count)
		{
			BufferUnderrun(this, gcnew EventArgs());
		} // end if

		return dataRead;
	} // end Read
	
	// Write class required by the stream base class.
	void RtInputStream::Write(array<float>^ buffer, int offset, int count)
	{
		throw gcnew System::NotImplementedException();
	} // end Write

	// Is the stream a live stream, or a buffered stream?
	bool RtInputStream::IsLive()
	{
		return true;
	} // end write

	int RtInputStream::callback(IntPtr outputBufferPtr, IntPtr inputBufferPtr, unsigned int frames, double streamTime, ::RtAudioNet::RtAudioStreamStatus status, Object^ userData)
	{
		// Create our temporary buffer
		unsigned int floatsToCopy = frames * Format->channels;
		array<float>^ tempBuff = gcnew array<float>(floatsToCopy);

		// Not sure this shouldn't be a memcopy. However, I know this works for managed types, and we don't lose that much speed. Future optimization?
		Marshal::Copy(inputBufferPtr, tempBuff, 0, floatsToCopy);
		
		msclr::lock lk(internalBuffer);

		// And now, we add out temp buffer to our main buffer.
		internalBuffer->Put(tempBuff);
		
		lk.release();

		// Fire the callback event.
		CallbackFired(this, nullptr);
		
		// We always return zero.
		return 0;
	} // end callback


	//////////////////////////////////////////////////////////////////////////
	/// RtOutputStream Class

	// Finish initializing the stream
	void RtOutputStream::initialize()
	{
		rtaudio = gcnew ::RtAudioNet::RtAudio();
		outputStreamParams = gcnew ::RtAudioNet::RtAudio::StreamParameters();

		if (Format == nullptr)
		{
			Format = gcnew RtStreamFormat();
			Format->sampleRate = 22050;
			Format->channels = 2;
			Format->bitsPerSample = 16;
		} // end if
		
		// (Frames per sample * channels) * numSamplesToBuffer
		internalBuffer = gcnew CircularBuffer<float>((Frames * Format->channels * 2), true);

		// Set stream properties
		_canRead = false;
		_canWrite = true;
		_canSeek = false;
		Name = "";
	} // end initialize
	
	RtOutputStream::~RtOutputStream()	
	{
		if (rtaudio->isStreamRunning())
		{
			rtaudio->stopStream();
		} // end if

		if (rtaudio->isStreamOpen())
		{
			rtaudio->closeStream();
		} // end if
	} // end ~RtDuplexStream

	// Selects the correct output device
	void RtOutputStream::selectOutputDevice(int devID)
	{
		DeviceID = devID;
		outputStreamParams->deviceId = devID;
		outputStreamParams->nChannels = Format->channels;
	} // end selectOutputDevice

	// Selects the correct output device
	void RtOutputStream::selectOutputDevice(String^ devString)
	{
		unsigned int devID = 0;

		for (unsigned int idx = 0; rtaudio->getDeviceCount() > idx; idx++)
		{
			::RtAudioNet::RtAudio::DeviceInfo^ info = rtaudio->getDeviceInfo(idx);

			if (info->name->Contains(devString))
			{
				devID = idx;
			} // end if
		} // end for

		selectOutputDevice(devID);
	} // end selectOutputDevice

	// Opens the stream
	void RtOutputStream::Open()
	{
		if (Format->options == nullptr)
		{
			rtaudio->openStream(outputStreamParams, nullptr, Format->type, Format->sampleRate, Frames, gcnew ::RtAudioNet::RtAudioNetCallback(this, &RtOutputStream::callback));
		}
		else
		{
			rtaudio->openStream(outputStreamParams, nullptr, Format->type, Format->sampleRate, Frames, gcnew ::RtAudioNet::RtAudioNetCallback(this, &RtOutputStream::callback), nullptr, Format->options);
		} // end if
	} // end Open

	// Starts the stream
	void RtOutputStream::Start()
	{
		int count = 0;	
		while (!(rtaudio->isStreamOpen() && count < 100))
		{
			System::Threading::Thread::Sleep(10);
			count++;
		} // end while

		rtaudio->startStream();
	} // end Start

	// Stops the stream
	void RtOutputStream::Stop()
	{
		rtaudio->stopStream();
	} // end Stop
	
	// Stops the stream
	void RtOutputStream::Finish()
	{
		rtaudio->closeStream();
	} // end Stop

	// Aborts the stream
	void RtOutputStream::Abort()
	{
		rtaudio->abortStream();
	} // end Abort

	// Read class required by the stream base class.
	int RtOutputStream::Read([InAttribute] [OutAttribute] array<float>^ buffer, int offset, int count)
	{
		throw gcnew System::NotImplementedException();
	} // end Read
	
	// Write class required by the stream base class.
	void RtOutputStream::Write(array<float>^ buffer, int offset, int count)
	{
		msclr::lock lk(internalBuffer);
		internalBuffer->Put(buffer, offset, count);
	} // end Write

	// Is the stream a live stream, or a buffered stream?
	bool RtOutputStream::IsLive()
	{
		return true;
	} // end write

	int RtOutputStream::callback(IntPtr outputBufferPtr, IntPtr inputBufferPtr, unsigned int frames, double streamTime, ::RtAudioNet::RtAudioStreamStatus status, Object^ userData)
	{
		// Create our temporary buffer
		unsigned int floatsToCopy = frames * Format->channels;
		array<float>^ tempBuff = gcnew array<float>(floatsToCopy);

		msclr::lock lk(internalBuffer);
		
		// And now, we add out temp buffer to our main buffer.
		int bytesCopied = internalBuffer->Get(tempBuff);

		lk.release();

		// Not sure this shouldn't be a memcopy. However, I know this works for managed types, and we don't lose that much speed. Future optimization?
		Marshal::Copy(tempBuff, 0, outputBufferPtr, bytesCopied);

		// Fire the callback event.
		CallbackFired(this, nullptr);
		
		// We always return zero.
		return 0;
	} // end callback


	//////////////////////////////////////////////////////////////////////////
	/// RtDuplexStream Class

	// Finish initializing the stream
	void RtDuplexStream::initialize()
	{
		rtaudio = gcnew ::RtAudioNet::RtAudio();
		inputStreamParams = gcnew ::RtAudioNet::RtAudio::StreamParameters();
		outputStreamParams = gcnew ::RtAudioNet::RtAudio::StreamParameters();

		if (Format == nullptr)
		{
			Format = gcnew RtStreamFormat();
			Format->sampleRate = 22050;
			Format->channels = 2;
			Format->bitsPerSample = 16;
		} // end if
		
		DeviceID = -1;

		// (Frames per sample * channels * bytes) * numSamplesToBuffer in an int	
		//internalBuffer = gcnew CircularBuffer<unsigned char>((frames * Format->channels * 4 * 2), true);

		// Set stream properties
		_canRead = false;
		_canWrite = false;
		_canSeek = false;
		Name = "";
	} // end initialize
	
	RtDuplexStream::~RtDuplexStream()	
	{
		if (rtaudio->isStreamRunning())
		{
			rtaudio->stopStream();
		} // end if

		if (rtaudio->isStreamOpen())
		{
			rtaudio->closeStream();
		} // end if
	} // end ~RtDuplexStream

	// Selects the correct input device
	void RtDuplexStream::selectInputDevice(int devID)
	{
		inputStreamParams->deviceId = devID;
		inputStreamParams->nChannels = Format->channels;
	} // end selectInputDevice

	// Selects the correct input device
	void RtDuplexStream::selectInputDevice(String^ devString)
	{
		unsigned int devID = 0;

		for (unsigned int idx = 0; rtaudio->getDeviceCount() > idx; idx++)
		{
			::RtAudioNet::RtAudio::DeviceInfo^ info = rtaudio->getDeviceInfo(idx);

			if (info->name->Contains(devString))
			{
				devID = idx;
			} // end if
		} // end for

		selectInputDevice(devID);
	} // end selectInputDevice
	// Selects the correct output device
	void RtDuplexStream::selectOutputDevice(int devID)
	{
		outputStreamParams->deviceId = devID;
		outputStreamParams->nChannels = Format->channels;
	} // end selectOutputDevice

	// Selects the correct output device
	void RtDuplexStream::selectOutputDevice(String^ devString)
	{
		unsigned int devID = 0;

		for (unsigned int idx = 0; rtaudio->getDeviceCount() > idx; idx++)
		{
			::RtAudioNet::RtAudio::DeviceInfo^ info = rtaudio->getDeviceInfo(idx);

			if (info->name->Contains(devString))
			{
				devID = idx;
			} // end if
		} // end for

		selectOutputDevice(devID);
	} // end selectOutputDevice

	// Opens the stream
	void RtDuplexStream::Open()
	{
		if (Format->options == nullptr)
		{
			rtaudio->openStream(outputStreamParams, inputStreamParams, Format->type, Format->sampleRate, Frames, gcnew ::RtAudioNet::RtAudioNetCallback(this, &RtDuplexStream::callback));
		}
		else
		{
			rtaudio->openStream(outputStreamParams, inputStreamParams, Format->type, Format->sampleRate, Frames, gcnew ::RtAudioNet::RtAudioNetCallback(this, &RtDuplexStream::callback), nullptr, Format->options);
		} // end if
	} // end Open

	// Starts the stream
	void RtDuplexStream::Start()
	{
		int count = 0;	
		while (!(rtaudio->isStreamOpen() && count < 100))
		{
			System::Threading::Thread::Sleep(10);
			count++;
		} // end while

		rtaudio->startStream();
	} // end Start

	// Stops the stream
	void RtDuplexStream::Stop()
	{
		rtaudio->stopStream();
	} // end Stop
	
	// Stops the stream
	void RtDuplexStream::Finish()
	{
		rtaudio->closeStream();
	} // end Stop

	// Aborts the stream
	void RtDuplexStream::Abort()
	{
		rtaudio->abortStream();
	} // end Abort

	// Read class required by the stream base class.
	int RtDuplexStream::Read([InAttribute] [OutAttribute] array<float>^ buffer, int offset, int count)
	{
		throw gcnew System::NotImplementedException();
	} // end Read
	
	// Write class required by the stream base class.
	void RtDuplexStream::Write(array<float>^ buffer, int offset, int count)
	{
		throw gcnew System::NotImplementedException();
	} // end Write

	// Is the stream a live stream, or a buffered stream?
	bool RtDuplexStream::IsLive()
	{
		return true;
	} // end write

	int RtDuplexStream::callback(IntPtr outputBufferPtr, IntPtr inputBufferPtr, unsigned int frames, double streamTime, ::RtAudioNet::RtAudioStreamStatus status, Object^ userData)
	{
		// Create our temporary buffer
		unsigned int bytesToCopy = frames * Format->channels * (Format->bitsPerSample / 8);

		// XXX: On Winows XP, this code gets an access violation:

		/*
		array<float>^ tempBuff = gcnew array<float>(bytesToCopy);

		// Not sure this shouldn't be a memcopy. However, I know this works for managed types, and we don't lose that much speed. Future optimization?
		Marshal::Copy(inputBufferPtr, tempBuff, 0, bytesToCopy);

		// Not sure this shouldn't be a memcopy. However, I know this works for managed types, and we don't lose that much speed. Future optimization?
		Marshal::Copy(tempBuff, 0, outputBufferPtr, bytesToCopy);
		*/

		// This is the absolute fastest option available to us, and it seems to work well.
		memcpy(outputBufferPtr.ToPointer(), inputBufferPtr.ToPointer(), bytesToCopy);

		// We always return zero.
		return 0;
	} // end callback


	//////////////////////////////////////////////////////////////////////////
	/// RtStreamConverter Class

	RtStreamConverter::RtStreamConverter(RtAudioStream^ stream)
	{
		rtaudio = gcnew ::RtAudioNet::RtAudio();
		internalBuffer = gcnew CircularBuffer<float>(4096);
		_stream = stream;
	} // end RtWaveStream

} // end namespace