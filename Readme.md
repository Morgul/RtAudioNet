RtAudioNet is a managed C++ wrapper around the unmanaged RtAudio C++ Library. 

It's tested and used mainly in C#.

Original RtAudio:
http://www.music.mcgill.ca/~gary/rtaudio/index.html

## About

This is a Managed C++ DLL that wraps RtAudio. Since it's my first .Net wrapper (and I'm not that clever with C++ in general), it's more a half port, rather than just a simple wrapper. My goal is to keep the syntax as close to RtAudio as possible, but I intend to break away from that in any instances that would greatly ease use in .Net.

In addition to the RtAudio wrapper class, I've written a new Stream Api designed to be more high level than RtAudio and ease use of RtAudio in .Net Applications. It only takes _three lines of code_ to setup a mixer that sends the default input and sends it to the default output.

It should also be noted that since I'm a C# developer and find VB.Net to be syntactically obtuse and unusable, this wrapper really is only tested in C#. I may include an IronPython sample at some point later. As for Mono support, that's 'planned'. I'm an avid Linux user, however, this is mostly developed in conjunction with my work, sadly. I haven't had time to sort through getting this to compile in mono. Since I'm using VS2010 for all my development, and I can use .Net 4.0 everywhere, this wrapper is written in .Net 4.0. I may, at some point, provide 3.5 bindings, but I doubt I'll go lower than that. And, really, by the time I get around to it, most people shouldn't be stuck on 3.5 anymore.

**Update**: I've discovered that mono support will be much more painful than I initially assumed. Feel free to track the discussion here.

## Status

Currently, the RtAudio wrapper is decently tested and works well. I've tested it on several systems, and it seems to just work. I've added in custom exceptions, and will be rearranging the class into something a bit more .Net centric shortly (still mirroring the original RtAudio class as much as possible.)

Most of the work lately has been on the Stream Api. Currently it's nearing feature completion, I'm just sorting out the few remaining known bugs and some code cleanup before putting out the 0.6.0 release. Once 0.6.0 is released, RtAudioNet will move into Beta Quality status.

**Note**:The repository will _always_ be kept in a state where the bindings compile.

**7/11/11**: I've added an EventLogger and EventLoggerManager class to RtAudioNet. Now, it's possible for RtAudioNet to log internally, and have that logging be integrated easily with any other logging scheme in C# or managed C++. It should also be noted that this logger can be used by C# code external to RtAudioNet, if it's really desired. (I don't know why you would, but you can!) This addition of logging now makes is much simpler to debug RtAudioNet. For more information on how to integrate with the logging, check out [EventLogger](EventLogger).

**5/2/11**: The Stream API is feature complete. There's currently a bug with detecting device enumeration changes, but that's being sorted out. Once that's taken care of, the Stream API will be ready for 0.6.0. I have one or two other minor things on my list to do before the release, but we're getting close!