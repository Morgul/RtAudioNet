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

namespace RtStream
{
	//////////////////////////////////////////////////////////////////////////
	/// RtAudioStream Class

	RtAudioStream::RtAudioStream()
	{
		rtaudio = gcnew ::RtAudioNet::RtAudio();
		buffer = gcnew array<unsigned char>(4096);

		_canRead = false;
		_canWrite = false;
		_canSeek = false;
	} // end RtWaveStream
	
	// Read class that's more convienent.
	int RtAudioStream::Read([InAttribute] [OutAttribute] array<unsigned char>^ buffer)
	{
		return Read(buffer, 0, buffer->Length);
	} // end Read
	
	// Write class that's more convienent.
	void RtAudioStream::Write(array<unsigned char>^ buffer)
	{
		Write(buffer, 0, buffer->Length);
	} // end Write


	//////////////////////////////////////////////////////////////////////////
	/// RtInputStream Class

	RtInputStream::RtInputStream()
	{
		rtaudio = gcnew ::RtAudioNet::RtAudio();
		buffer = gcnew array<unsigned char>(4096);

		// Set stream properties
		_canRead = true;
		_canWrite = false;
		_canSeek = false;
	} // end RtWaveStream
	
	// Selects the correct input device
	void RtInputStream::selectInputDevice(int devID)
	{
	} // end selectInputDevice

	// Read class required by the stream base class.
	int RtInputStream::Read([InAttribute] [OutAttribute] array<unsigned char>^ buffer, int offset, int count)
	{
		return 0;
	} // end Read
	
	// Write class required by the stream base class.
	void RtInputStream::Write(array<unsigned char>^ buffer, int offset, int count)
	{
	} // end Write

	// Is the stream a live stream, or a buffered stream?
	bool RtInputStream::IsLive()
	{
		return true;
	} // end write


	//////////////////////////////////////////////////////////////////////////
	/// RtOutputStream Class

	RtOutputStream::RtOutputStream()
	{
		rtaudio = gcnew ::RtAudioNet::RtAudio();
		buffer = gcnew array<unsigned char>(4096);

		// Set stream properties
		_canRead = false;
		_canWrite = true;
		_canSeek = false;
	} // end RtWaveStream
	
	// Selects the correct input device
	void RtOutputStream::selectOutputDevice(int devID)
	{
	} // end selectInputDevice

	// Read class required by the stream base class.
	int RtOutputStream::Read([InAttribute] [OutAttribute] array<unsigned char>^ buffer, int offset, int count)
	{
		return 0;
	} // end Read
	
	// Write class required by the stream base class.
	void RtOutputStream::Write(array<unsigned char>^ buffer, int offset, int count)
	{
	} // end Write

	// Is the stream a live stream, or a buffered stream?
	bool RtOutputStream::IsLive()
	{
		return true;
	} // end write


	//////////////////////////////////////////////////////////////////////////
	/// RtInputStream Class

	RtDuplexStream::RtDuplexStream()
	{
		rtaudio = gcnew ::RtAudioNet::RtAudio();
		buffer = gcnew array<unsigned char>(4096);

		// Set stream properties
		_canRead = true;
		_canWrite = true;
		_canSeek = false;
	} // end RtWaveStream
	
	// Selects the correct input device
	void RtDuplexStream::selectOutputDevice(int devID)
	{
	} // end selectInputDevice

	// Read class required by the stream base class.
	int RtDuplexStream::Read([InAttribute] [OutAttribute] array<unsigned char>^ buffer, int offset, int count)
	{
		return 0;
	} // end Read
	
	// Write class required by the stream base class.
	void RtDuplexStream::Write(array<unsigned char>^ buffer, int offset, int count)
	{
	} // end Write

	// Is the stream a live stream, or a buffered stream?
	bool RtDuplexStream::IsLive()
	{
		return true;
	} // end write


	//////////////////////////////////////////////////////////////////////////
	/// RtStreamConverter Class

	RtStreamConverter::RtStreamConverter(RtAudioStream^ stream)
	{
		rtaudio = gcnew ::RtAudioNet::RtAudio();
		buffer = gcnew array<unsigned char>(4096);
		_stream = stream;
	} // end RtWaveStream

} // end namespace