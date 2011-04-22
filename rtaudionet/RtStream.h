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

#pragma once

#include "rtaudionet.h"

using namespace System::Runtime::InteropServices;
using namespace System::IO;
using namespace RtAudioNet;

namespace RtStream
{
	public ref struct RtStreamFormat
	{
	public:
		::RtAudioNet::RtAudioFormat type;
		int sampleRate;
		int bitesPerSample;
	}; // end RtStreamFormat

	public ref class RtAudioStream : Stream
	{
	public:
		RtAudioStream();

		// CanRead property required by the stream base class.
		virtual property bool CanRead { bool get () override { return _canRead; }; };

		// CanSeek property required by the stream base class.
		virtual property bool CanSeek { bool get () override { return _canSeek; }; };

		// CanWrite property required by the stream base class.
		virtual property bool CanWrite { bool get () override { return _canWrite; }; };

		// Length property required by the stream base class.
		virtual property long long Length { long long get () override { throw gcnew NotSupportedException(); }; };

		// Position property required by the stream base class.
		virtual property long long Position { long long get () override { return 0; }; void set (long long value) override { throw gcnew NotSupportedException(); }; }

		// Seek property required by the stream base class.
		virtual long long Seek(long long offset, SeekOrigin origin) override { throw gcnew NotSupportedException(); };

		// SetLength property required by the stream base class.
		virtual void SetLength(long long value) override { throw gcnew NotSupportedException(); };

		// Flush method required by the stream base class.
		virtual void Flush() override {System::Array::Clear(buffer, 0, buffer->Length);};

		// Read method required by the stream base class.
		virtual int Read([InAttribute] [OutAttribute] array<unsigned char>^ buffer, int offset, int count) override { return 0; };

		// Read class that's more convienent.
		int Read([InAttribute] [OutAttribute] array<unsigned char>^ buffer);
		
		// Write method required by the stream base class.
		virtual void Write(array<unsigned char>^ buffer, int offset, int count) override { };

		// Write method required by the stream base class.
		void Write(array<unsigned char>^ buffer);

		// Is the stream a live stream, or a buffered stream?
		bool IsLive() { return false; };

		// Stream's format
		RtStreamFormat format;

	protected:
		// Our internal RtAudio instance
		::RtAudioNet::RtAudio^ rtaudio;

		// Our internal buffer
		array<unsigned char>^ buffer;

		// Property variables
		bool _canRead;
		bool _canWrite;
		bool _canSeek;
	}; // end RtWaveStream

	// RtInputStream takes an input device and outputs that to a buffer.
	public ref class RtInputStream : RtAudioStream
	{
	public:
		RtInputStream();

		// Selects the correct input device
		void selectInputDevice(int devID);

		// Read method required by the stream base class.
		virtual int Read([InAttribute] [OutAttribute] array<unsigned char>^ buffer, int offset, int count) override;
		
		// Write method required by the stream base class.
		virtual void Write(array<unsigned char>^ buffer, int offset, int count) override;

		// Is the stream a live stream, or a buffered stream?
		bool IsLive();
	}; // end RtInputStream

	// RtOutputStream takes a buffer and outputs that to the output device.
	public ref class RtOutputStream : RtAudioStream
	{
	public:
		RtOutputStream();

		// Selects the correct input device
		void selectOutputDevice(int devID);

		// Read method required by the stream base class.
		virtual int Read([InAttribute] [OutAttribute] array<unsigned char>^ buffer, int offset, int count) override;
		
		// Write method required by the stream base class.
		virtual void Write(array<unsigned char>^ buffer, int offset, int count) override;

		// Is the stream a live stream, or a buffered stream?
		bool IsLive();
	}; // end RtOutputStream

	// RtDuplexStream takes an input device and outputs that to the output device.
	public ref class RtDuplexStream : RtInputStream
	{
	public:
		RtDuplexStream();

		// Selects the correct input device
		void selectOutputDevice(int devID);

		// Read method required by the stream base class.
		virtual int Read([InAttribute] [OutAttribute] array<unsigned char>^ buffer, int offset, int count) override;
		
		// Write method required by the stream base class.
		virtual void Write(array<unsigned char>^ buffer, int offset, int count) override;

		// Is the stream a live stream, or a buffered stream?
		bool IsLive();
	}; // end RtDuplexStream

	// RtStreamConverter will be a lightweight wrapper around an RtAudioStream. Everytime a Read is called, it will read from the RtAudioStream
	// and then convert the results into the desired format. All other functionality is passthrough to the wrapped RtAudioStream.
	public ref class RtStreamConverter : RtAudioStream
	{
	public:
		RtStreamConverter(RtAudioStream^ stream);

	protected:
		RtAudioStream^ _stream;
	}; // end RtStreamConverter
} // end namespace