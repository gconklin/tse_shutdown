// #define DEBUG

// TO COMPILE AND LINK, using Microsoft C v10:
// cl -W3 -LD -Oxs  tseshutdown.c -link /nodefaultlib /entry:DllMain kernel32.lib advapi32.lib

#include <windows.h>

#define DLL_NAME "TSE_Shutdown: "
#ifdef DEBUG
    #define TRACE(x) \
        do \
            { \
            OutputDebugString( DLL_NAME ); \
            OutputDebugString(x); \
            OutputDebugString("\r\n"); \
            } \
        while (FALSE)

    #define TRACE1(x, a) \
        do \
            { \
            char _____sz[255]; \
            wsprintf(_____sz, x, a); \
            OutputDebugString( DLL_NAME ); \
            OutputDebugString(_____sz); \
            OutputDebugString("\r\n"); \
            } \
        while (FALSE)

    #define TRACE2(x, a, b) \
        do \
            { \
            char _____sz[255]; \
            wsprintf(_____sz, x, a, b); \
            OutputDebugString( DLL_NAME ); \
            OutputDebugString(_____sz); \
            OutputDebugString("\r\n"); \
            } \
        while (FALSE)

    #define TRACE3(x, a, b, c) \
        do \
            { \
            char _____sz[255]; \
            wsprintf(_____sz, x, a, b, c); \
            OutputDebugString( DLL_NAME ); \
            OutputDebugString(_____sz); \
            OutputDebugString("\r\n"); \
            } \
        while (FALSE)

    #define TRACE4(x, a, b, c, d) \
        do \
            { \
            char _____sz[255]; \
            wsprintf(_____sz, x, a, b, c, d); \
            OutputDebugString( DLL_NAME ); \
            OutputDebugString(_____sz); \
            OutputDebugString("\r\n"); \
            } \
        while (FALSE)
#else
    #define TRACE(x)
    #define TRACE1(x, a)
    #define TRACE2(x, a, b)
    #define TRACE3(x, a, b, c)
    #define TRACE4(x, a, b, c, d)
#endif

// The Event that triggers Exiting
HANDLE g_hShutdownEvent;

// TSD = Tse Shut Down

// TRUE if the Event exists
__declspec(dllexport)
BOOL TSD_IsAttached()
{
    return g_hShutdownEvent != NULL;
}

// Creates the Event to be watched
__declspec(dllexport)
void TSD_Attach()
{
    SECURITY_ATTRIBUTES     sa;
    SECURITY_DESCRIPTOR     sd;

    if ( TSD_IsAttached() )
    {
        TRACE( "Already attached" );
        return;
    }

    TRACE( "Attaching" );

    // init
    sa.nLength = sizeof( SECURITY_ATTRIBUTES );
    sa.bInheritHandle       = FALSE;
    sa.lpSecurityDescriptor = &sd;

    InitializeSecurityDescriptor( &sd, SECURITY_DESCRIPTOR_REVISION );

    // add a NULL disc. ACL to the secuity descriptor
    SetSecurityDescriptorDacl( &sd, TRUE, (PACL)NULL, FALSE );

    g_hShutdownEvent =
        CreateEvent( &sa, TRUE, FALSE, "TSE_Shutdown_Event" );
}

// Destroys the Event
__declspec(dllexport)
void TSD_Detach()
{
    TRACE( "Detaching" );
    if ( g_hShutdownEvent != NULL )
    {
        CloseHandle( g_hShutdownEvent );
        g_hShutdownEvent = NULL;
    }
}

// Checks to see if the Event has been fired
// Forces Detachment to keep it from being held open
__declspec(dllexport)
BOOL TSD_IsShutdown()
{
    if ( WaitForSingleObject( g_hShutdownEvent, 0 ) == WAIT_OBJECT_0 )
    {
        TRACE( "Shutdown request received" );
        // so we don't keep this event open in case TSE can't be shutdown
        // otherwise, new instances will immediately exit while this instance
        // could be at a Save/Exit prompt
        TSD_Detach();
        return TRUE;
    }

    return FALSE;
}

// Trigger the Event
__declspec(dllexport)
void TSD_DoShutdown()
{
    TRACE( "Sending shutdown request" );
    TSD_Attach();

    SetEvent( g_hShutdownEvent );
}


BOOL WINAPI DllMain(HINSTANCE hinstDLL, // handle of DLL module
                    DWORD dwReason,     // reason for calling function
                    LPVOID pvReserved   // reserved
                    )
{
    TRACE("DllMain");

    if (dwReason == DLL_PROCESS_ATTACH)
    {
        TRACE("DLL_PROCESS_ATTACH");
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        TRACE("DLL_PROCESS_DETACH");

        TSD_Detach();
    }

    return TRUE;
}