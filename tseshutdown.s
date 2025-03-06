
dll "tseshutdown.dll"
  integer proc  TSD_IsAttached()
  proc          TSD_Attach()
  proc          TSD_Detach()
  integer proc  TSD_IsShutdown()
  proc          TSD_DoShutdown()
end

forward proc Idle_WatchForShutdown()

// Create the _IDLE_ hook and connect to the Event
proc Attach()
    if Hook( _IDLE_, Idle_WatchForShutdown )
        TSD_Attach()
    else
        // error initializing, purge the macro
		Warn("Unable to initialize";
				Upper(SplitPath(CurrMacroFilename(), _NAME_|_EXT_)))
		PurgeMacro(CurrMacroFilename())
    endif
end

// Stop the _IDLE_ hook and destroy the Event
proc Detach()
    Unhook( Idle_WatchForShutdown )
    TSD_Detach()
end

// During idle time, check to see if the Event was triggered
// and we need to exit
proc Idle_WatchForShutdown()
    if TSD_IsShutdown()
        // detach from the Event so that the Event won't remain fired
        // causing newly launched TSEs to immediately exit
        Detach()
        if  Exit() == 0
            // something in the file ring was modified so the process
            // did not exit. Re-attach to the Event and _IDLE_ hook
            Attach()
        endif
    endif
end

// Executing the macro allows for Exiting all instances
proc Main()
    if YesNo( "Terminate all TSE instances?" ) == 1
        TSD_DoShutdown()
    endif
end

proc WhenLoaded()
    Attach()
end
