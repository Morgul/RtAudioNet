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

namespace RtAudioStream
{
	public ref class RtWaveStream : Stream
	{
	public:
		RtWaveStream();

		// RtWaveStreams cannot seek, as they're live streams from an input device. This class handles implementing the Stream baseclass


		// CanRead property required by the stream base class.
		virtual property bool CanRead { bool get () override { return _canRead; }; };

		// CanSeek property required by the stream base class.
		virtual property bool CanSeek { bool get () override { return false; }; };

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
		virtual int Read([InAttribute] [OutAttribute] array<unsigned char>^ buffer, int offset, int count) override;
		
		// Write method required by the stream base class.
		virtual void Write(array<unsigned char>^ buffer, int offset, int count) override;

	protected:
		// Our internal RtAudio instance
		::RtAudioNet::RtAudio^ rtaudio;

		// Our internal buffer
		array<unsigned char>^ buffer;

		// Property variables
		bool _canRead;
		bool _canWrite;
	}; // end RtWaveStream
} // end namespace