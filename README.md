TSE Shutdown
============

DLL and SAL macro to shutdown all running instances of TSE.

Originaly written for TSE32 Console, ymmv for other versions.

Add `tseshutdown.mac` to the "AutoLoad List" which will load the companion dll at editor startup.

The dll creates a system wide named event called "`TSE_Shutdown_Event`".

The macro hooks the TSE `_IDLE_` event and when the dll says the event has been triggerd,
it will call `Exit()` which closes TSE. If all buffers are clean (saved), the editor will
silently exit. Otherwise a prompt to save is presented.

To trigger the shutdown event, just exeucte the `tseshutdown.mac` file which will prompt to shutdown
all instances.
