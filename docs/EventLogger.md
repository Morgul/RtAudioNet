# EventLogger

The EventLogger is a relatively simple set of classes. EventLoggerManager is a static class that contains all of the events you'll need to worry about if you're trying to integrate your logging system with RtAudioNet's. EventLogger, on the other hand, is the class for the individual loggers. _You do not need to worry about this class if you're just trying to integrate RtAudioNet with your logging system._

## Simple Console-based Integration

Put the code under 'Constructor' in your class's constructor, and then put the code under 'Event Handlers' in your class.

Constructor:
{code:c#}
// Connect Logging Events
EventLoggerManager.TraceLoggingEvent += new LoggingEventHandler(EventLoggerManager_TraceLoggingEvent);
EventLoggerManager.DebugLoggingEvent += new LoggingEventHandler(EventLoggerManager_DebugLoggingEvent);
EventLoggerManager.InfoLoggingEvent += new LoggingEventHandler(EventLoggerManager_InfoLoggingEvent);
EventLoggerManager.WarnLoggingEvent += new LoggingEventHandler(EventLoggerManager_WarnLoggingEvent);
EventLoggerManager.ErrorLoggingEvent += new LoggingEventHandler(EventLoggerManager_ErrorLoggingEvent);
EventLoggerManager.CriticalLoggingEvent += new LoggingEventHandler(EventLoggerManager_CriticalLoggingEvent);
{code:c#}

Event handlers:
{code:c#}
void EventLoggerManager_TraceLoggingEvent(object sender, LoggingEventArgs e)
{
    if (e.ex == null)
    {
        Console.WriteLine("[TRACE](TRACE) ({0}) {1}", e.logger, e.message);
    }
    else
    {
        Console.WriteLine("[TRACE](TRACE) ({0}) {1}, Exception: {2}", e.logger, e.message, e.ex.Message);
    } // end if
} // end EventLoggerManager_TraceLoggingEvent

void EventLoggerManager_DebugLoggingEvent(object sender, LoggingEventArgs e)
{
    if (e.ex == null)
    {
        Console.WriteLine("[DEBUG](DEBUG) ({0}) {1}", e.logger, e.message);
    }
    else
    {
        Console.WriteLine("[DEBUG](DEBUG) ({0}) {1}, Exception: {2}", e.logger, e.message, e.ex.Message);
    } // end if
} // end EventLoggerManager_DebugLoggingEvent

void EventLoggerManager_InfoLoggingEvent(object sender, LoggingEventArgs e)
{
    if (e.ex == null)
    {
        Console.WriteLine("[INFO](INFO) ({0}) {1}", e.logger, e.message);
    }
    else
    {
        Console.WriteLine("[INFO](INFO) ({0}) {1}, Exception: {2}", e.logger, e.message, e.ex.Message);
    } // end if
} // end EventLoggerManager_InfoLoggingEvent

void EventLoggerManager_WarnLoggingEvent(object sender, LoggingEventArgs e)
{
    if (e.ex == null)
    {
        Console.WriteLine("[WARN](WARN) ({0}) {1}", e.logger, e.message);
    }
    else
    {
        Console.WriteLine("[WARN](WARN) ({0}) {1}, Exception: {2}", e.logger, e.message, e.ex.Message);
    } // end if
} // end EventLoggerManager_WarnLoggingEvent

void EventLoggerManager_ErrorLoggingEvent(object sender, LoggingEventArgs e)
{
    if (e.ex == null)
    {
        Console.WriteLine("[ERROR](ERROR) ({0}) {1}", e.logger, e.message);
    }
    else
    {
        Console.WriteLine("[ERROR](ERROR) ({0}) {1}, Exception: {2}", e.logger, e.message, e.ex.Message);
    } // end if
} // end EventLoggerManager_ErrorLoggingEvent

void EventLoggerManager_CriticalLoggingEvent(object sender, LoggingEventArgs e)
{
    if (e.ex == null)
    {
        Console.WriteLine("[CRITICAL](CRITICAL) ({0}) {1}", e.logger, e.message);
    }
    else
    {
        Console.WriteLine("[CRITICAL](CRITICAL) ({0}) {1}, Exception: {2}", e.logger, e.message, e.ex.Message);
    } // end if
} // end EventLoggerManager_CriticalLoggingEvent
{code:c#}

Output:
{code:html}
[DEBUG](DEBUG) (RtAudio TestSuit) Compiled Apis:
[DEBUG](DEBUG) (RtAudio TestSuit)   WINDOWS_ASIO
[DEBUG](DEBUG) (RtAudio TestSuit)   WINDOWS_DS
[DEBUG](DEBUG) (RtAudio TestSuit) Device Count: 4
[DEBUG](DEBUG) (RtAudio TestSuit) Found Devices:
[DEBUG](DEBUG) (RtAudio TestSuit) ID 0: Default Device:
[DEBUG](DEBUG) (RtAudio TestSuit)     InputChannels: 2
[DEBUG](DEBUG) (RtAudio TestSuit)     OutputChannels: 2
[DEBUG](DEBUG) (RtAudio TestSuit)     DuplexChannels: 2
[DEBUG](DEBUG) (RtAudio TestSuit) ID 1: Speakers (SoundMAX Integrated Digital HD Audio):
[DEBUG](DEBUG) (RtAudio TestSuit)     InputChannels: 0
[DEBUG](DEBUG) (RtAudio TestSuit)     OutputChannels: 2
[DEBUG](DEBUG) (RtAudio TestSuit)     DuplexChannels: 0
[DEBUG](DEBUG) (RtAudio TestSuit) ID 2: SPDIF Interface (SoundMAX Integrated Digital HD Audio):
[DEBUG](DEBUG) (RtAudio TestSuit)     InputChannels: 0
{code:html}