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

#include "RtAudio\RtError.h"

using namespace System;

namespace RtAudioNet
{
	public ref class RtErrorEventArgs: public EventArgs
	{
	private:
		String^ msg;
	
	public:
		RtErrorEventArgs()
		{
			msg = ""; 
		} // end RtErrorEventArgs

		RtErrorEventArgs(String^ messageData)
		{
			msg = messageData; 
		} // end RtErrorEventArgs
		
		property String^ Message 
		{
			String^ get()
			{
				return msg;
			} // end get
		
			void set(String^ value)
			{
				msg = value;
			} // end set
		} // end Message
	}; // end RtErrorEventArgs

	// The intention is for this clase to be used as a base clase by any other class that needs to work
	// with ::RtError. When an ::RtError is returned, the class would call throwError, and the correct
	// event will fire. User's code would handle the events, without being aware of the underlying 
	// ::RtError class, or this mechanic
	public ref class RtError
	{
	public:
		event EventHandler<RtErrorEventArgs^>^ rtErrorWarning;
		event EventHandler<RtErrorEventArgs^>^ rtErrorDebugWarning;
		event EventHandler<RtErrorEventArgs^>^ rtErrorUnspecified;
		event EventHandler<RtErrorEventArgs^>^ rtErrorNoDevicesFound;
		event EventHandler<RtErrorEventArgs^>^ rtErrorInvalidDevice;
		event EventHandler<RtErrorEventArgs^>^ rtErrorMemoryError;
		event EventHandler<RtErrorEventArgs^>^ rtErrorInvalidParameter;
		event EventHandler<RtErrorEventArgs^>^ rtErrorInvalidUse;
		event EventHandler<RtErrorEventArgs^>^ rtErrorDriverError;
		event EventHandler<RtErrorEventArgs^>^ rtErrorSystemError;
		event EventHandler<RtErrorEventArgs^>^ rtErrorThreadError;

		void throwError(::RtError* error)
		{
			RtErrorEventArgs^ args = gcnew RtErrorEventArgs();

			// Handle error types appropriately
			switch(error->getType())
			{
				case ::RtError::WARNING:
				{
					args->Message = gcnew String(error->what());
					rtErrorWarning(this, args);
					break;
				} // end case

				case ::RtError::DEBUG_WARNING:
				{
					args->Message = gcnew String(error->what());
					rtErrorDebugWarning(this, args);
					break;
				} // end case

				case ::RtError::NO_DEVICES_FOUND:
				{
					args->Message = gcnew String(error->what());
					rtErrorNoDevicesFound(this, args);
					break;
				} // end case

				case ::RtError::INVALID_DEVICE:
				{
					args->Message = gcnew String(error->what());
					rtErrorInvalidDevice(this, args);
					break;
				} // end case

				case ::RtError::MEMORY_ERROR:
				{
					args->Message = gcnew String(error->what());
					rtErrorMemoryError(this, args);
					break;
				} // end case

				case ::RtError::INVALID_PARAMETER:
				{
					args->Message = gcnew String(error->what());
					rtErrorInvalidParameter(this, args);
					break;
				} // end case

				case ::RtError::INVALID_USE:
				{
					args->Message = gcnew String(error->what());
					rtErrorInvalidUse(this, args);
					break;
				} // end case

				case ::RtError::DRIVER_ERROR:
				{
					args->Message = gcnew String(error->what());
					rtErrorDriverError(this, args);
					break;
				}// end case

				case ::RtError::SYSTEM_ERROR:
				{
					args->Message = gcnew String(error->what());
					rtErrorSystemError(this, args);
					break;
				} // end case

				case ::RtError::THREAD_ERROR:
				{
					args->Message = gcnew String(error->what());
					rtErrorThreadError(this, args);
					break;
				 } // end case

				case ::RtError::UNSPECIFIED:
				default:
				{
					args->Message = gcnew String(error->what());
					rtErrorUnspecified(this, args);
					break;
				} // end case
			} // end switch
		} // end throwError
	}; // end RtError
} // end namespace