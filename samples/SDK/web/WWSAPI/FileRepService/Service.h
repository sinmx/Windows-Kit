//------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------

#include "common.h"

// This header file contains all definitions used by both client and server services.

// Size of each file access when serializing the file into messages. 
// A bigger number here results in fewer file reads while a smaller number reduces 
// memory consumption. This is a sub-chunking of the chunk size set by the user 
// when starting the services. If the chunk size set then is bigger than FILE_CHUNK
// a chunk is read in more than one file access.
#define FILE_CHUNK 131072

#define DISCOVERY_REQUEST -1

// Error Uris used to transmit errors from server service to client service.
namespace GlobalStrings
{   
    static const WCHAR noError[] = L"http://tempuri.org/FileRep/NoError";
    static const WCHAR serializationFailed[] = L"http://tempuri.org/FileRep/SerializationFailed";
    static const WCHAR invalidFileName[] = L"http://tempuri.org/FileRep/InvalidFileName";
    static const WCHAR unableToDetermineFileLength[] = L"http://tempuri.org/FileRep/UnableToDetermineFileLength";
    static const WCHAR invalidRequest[] = L"http://tempuri.org/FileRep/InvalidRequest";
    static const WCHAR outOfRange[] = L"http://tempuri.org/FileRep/OutOfRange";
    static const WCHAR unableToSetFilePointer[] = L"http://tempuri.org/FileRep/UnableToSetFilePointer";
}

class CChannelManager;
class CRequest;

typedef enum
{
    REPORT_ERROR = 1,
    REPORT_INFO = 2,
    REPORT_VERBOSE = 3,
} REPORTING_LEVEL;

typedef enum
{
    INVALID_REQUEST = 1,
    FILE_DOES_NOT_EXIST = 2,
    FAILED_TO_CREATE_FILE,

} FAULT_TYPE;

// This class is the base class for both the client and server service.
class CFileRep
{
public:
    CFileRep(REPORTING_LEVEL errorReporting, long maxChannels, TRANSPORT_MODE transport, 
        SECURITY_MODE security, MESSAGE_ENCODING encoding);    

    ~CFileRep();

    HRESULT Start(__in_ecount(uriLength) const LPWSTR uri, DWORD uriLength);

    // A stop resets all custom state and releases all resources associated with a running instance of the service.
    HRESULT Stop(); 

    inline bool IsRunning() { return started; }

    WS_LISTENER* GetListener() { return listener; }

    CChannelManager* GetChannelManager() { return channelManager; }
    
    virtual HRESULT ProcessMessage(CRequest* request, const WS_XML_STRING* receivedAction) = 0;

    // These have to be public since things like thread creation can fail outside of the class.
    void PrintVerbose(__in_z const WCHAR message[]);  
  
    void PrintInfo(__in_z const WCHAR message[]);   
 
    void PrintError(HRESULT hr, WS_ERROR* error, bool displayAlways);

    void PrintError(__in_z const WCHAR message[], bool displayAlways);    
    
    // The default maximum heap size is set very conservatively to protect against headers of excessive size.
    // Since we are sending large chunks of data round that is not sufficient for us.
    static WS_MESSAGE_PROPERTY CreateHeapProperty()
    {
        static SIZE_T heapSize = MAXMESSAGESIZE * 2;

        static WS_HEAP_PROPERTY heapPropertyArray[] = 
        {
            { WS_HEAP_PROPERTY_MAX_SIZE, &heapSize, sizeof(heapSize) }
        };

        static WS_HEAP_PROPERTIES heapProperties = 
        {
            heapPropertyArray, WsCountOf(heapPropertyArray)
        };

        WS_MESSAGE_PROPERTY ret;
        ret.id = WS_MESSAGE_PROPERTY_HEAP_PROPERTIES;
        ret.value = &heapProperties;
        ret.valueSize = sizeof(heapProperties);
        return ret;
    }

    void GetEncoding(WS_ENCODING* encodingProperty, ULONG *propertyCount);
     
protected: 
    HRESULT InitializeListener();     
    
    WS_STRING uri;
    volatile bool started;
    REPORTING_LEVEL errorReporting;
    long maxChannels;
    TRANSPORT_MODE transportMode;
    MESSAGE_ENCODING encoding;
    SECURITY_MODE securityMode;
    WS_LISTENER* listener;
    CChannelManager* channelManager;
};

// Manages the channels and related state to maximize reuse of structures and performance.
// If there are less channels that are ready to accept a request (called idle channels) 
// than minIdleChannels then we create a new channel and make it listen for incoming requests. 
// If there are more idle channels than maxIdleChannels then we destroy the next channel that 
// becomes idle. There are never more than maxTotalChannels channels overall.
// The reason for having a minimum number of idle channels is that otherwise there would be a 
// bottleneck when multiple requests come in simultaniously as channel creation takes some time.
// The reason for having a maximum number of idle channels is to limit resource consumption.
// Resource reuse is an important performance booster. Resetting a data structure is a lot cheaper 
// than destroying and recreating it later, which is why there are so many reset APIs. 
// However, never destroying resources can also be problematic as you can potentially hold on
// to significant resources much longer than neccessary. This class, using the algorithm described 
// above, tries to find a middle ground.
// If the last issue is not a concern, a simpler and most likely superior implementation is to 
// simply create as many channels and associated resources as needed and to reuse them perpetually
// in their own loop.
class CChannelManager
{
public:
    CChannelManager(CFileRep *server, long minIdleChannels, long maxIdleChannels, long maxTotalChannels);

    ~CChannelManager();

    HRESULT Initialize();

    HRESULT CreateChannels();

    static ULONG WINAPI CreateChannelWorkItem(void* state);

    void CreateChannel(CRequest *request);

    void ChannelCreated();

    void ChannelInUse(); 

    void ChannelFreed();

    void ChannelIdle(); 
    
    static void CALLBACK CleanupCallback(HRESULT hr, WS_CALLBACK_MODEL callbackModel, void* state);    

    void Stop();

    void WaitForCleanup();

    inline long GetChannelCount() { return totalChannels; }

    inline bool IsRunning() { return running; }

    inline bool ShouldDestroyChannel() { return (!IsRunning() || idleChannels > maxIdleChannels); }

private:    
    inline void PrintVerbose(__in_z const WCHAR message[]) { server->PrintVerbose(message); }

    long minIdleChannels;
    long maxIdleChannels;
    long maxTotalChannels;

    long idleChannels;
    long activeChannels;
    long totalChannels;

    CFileRep* server;
    HANDLE stopEvent;
    volatile bool running;

#if (DBG || _DEBUG)
    bool initialized;
#endif
};

// This class implements the part of the message processing loop that is common to the client and server service.
// It contains the per-channel state as well as the common processing methods.
class CRequest
{
public:
    CRequest(CFileRep* server);

    ~CRequest();

    HRESULT Initialize();

    // Static callback methods. They delegate to their non-static counterparts.
    static HRESULT CALLBACK ResetChannelCallback(HRESULT hr, WS_CALLBACK_MODEL callbackModel, void* callbackState, 
        WS_ASYNC_OPERATION* next, const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error);   

    static HRESULT CALLBACK AcceptChannelCallback(HRESULT hr, WS_CALLBACK_MODEL callbackModel, void* callbackState, 
        WS_ASYNC_OPERATION* next, const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error);    

    static HRESULT CALLBACK ReceiveFirstMessageCallback(HRESULT hr, WS_CALLBACK_MODEL callbackModel, void* callbackState, 
        WS_ASYNC_OPERATION* next, const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error);   

    static HRESULT CALLBACK ReceiveMessageCallback(HRESULT hr, WS_CALLBACK_MODEL callbackModel, void* callbackState, 
        WS_ASYNC_OPERATION* next, const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error);    

    static HRESULT CALLBACK ReadHeaderCallback(HRESULT hr, WS_CALLBACK_MODEL callbackModel, void* callbackState, 
        WS_ASYNC_OPERATION* next, const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error);   

    static HRESULT CALLBACK CloseChannelCallback(HRESULT hr, WS_CALLBACK_MODEL callbackModel, void* callbackState, 
        WS_ASYNC_OPERATION* next, const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error);   

    static HRESULT CALLBACK RequestCompleteCallback(HRESULT hr, WS_CALLBACK_MODEL callbackModel, void* callbackState, 
        WS_ASYNC_OPERATION* next, const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error);   

    static HRESULT CALLBACK HandleFailureCallback(HRESULT hr, WS_CALLBACK_MODEL callbackModel, void* callbackState, 
        WS_ASYNC_OPERATION* next, const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error);   

    // The non-static counterparts that actually do the work.
    HRESULT ResetChannel(HRESULT hr, WS_ASYNC_OPERATION* next, 
        WS_CALLBACK_MODEL callbackModel, WS_ERROR* error);   

    HRESULT AcceptChannel(HRESULT hr, WS_ASYNC_OPERATION* next, WS_CALLBACK_MODEL callbackModel, 
        const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error); 

    HRESULT ReceiveFirstMessage(HRESULT hr, WS_ASYNC_OPERATION* next, WS_CALLBACK_MODEL callbackModel); 

    HRESULT ReceiveMessage(HRESULT hr, WS_ASYNC_OPERATION* next, WS_CALLBACK_MODEL callbackModel,
        const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error);  

    HRESULT ReadHeader(HRESULT hr, WS_ASYNC_OPERATION* next,
        WS_CALLBACK_MODEL callbackModel, WS_ERROR* error);

    HRESULT CloseChannel(HRESULT hr, WS_ASYNC_OPERATION* next, WS_CALLBACK_MODEL callbackModel,
        const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error);

    HRESULT RequestComplete(HRESULT hr, WS_ASYNC_OPERATION* next); 

    HRESULT HandleFailure(HRESULT hr, WS_ASYNC_OPERATION* next, WS_ERROR* error); 


    WS_CHANNEL* GetChannel() { return channel; }

    WS_MESSAGE* GetRequestMessage() { return requestMessage; }

    WS_MESSAGE* GetReplyMessage() { return replyMessage; }

    WS_ERROR* GetError() { return error; }

    CFileRep* GetServer() { return server; }

    // We use SOAP faults to communicate to the tool errors that might be expected 
    // during execution, such as file not found. For critical errors we abort the channel.
    // For internal communication between the services we use status messages that are part
    // of the regular message exchange instead of faults. We could use faults there as well,
    // but we do not want to fault internal communication because for example the user 
    // requested an invalid file.
    HRESULT SendFault(FAULT_TYPE faultType);

    static inline CRequest* GetRequest(void* callbackState);

    WS_ASYNC_STATE asyncState;

private:
    inline void PrintVerbose(__in_z const WCHAR message[]) { server->PrintVerbose(message); }
    
    // State associated with a request. Except the CFileRep pointer, this is not application specific.
    WS_CHANNEL* channel;
    WS_MESSAGE* requestMessage;
    WS_MESSAGE* replyMessage;
    WS_ERROR* error;
    CFileRep* server;
    bool channelInUse;
};


// Server service.
class CFileRepServer : public CFileRep
{
public:
    CFileRepServer(REPORTING_LEVEL errorReporting, DWORD maxChannels, TRANSPORT_MODE transport, 
        SECURITY_MODE security, MESSAGE_ENCODING encoding, DWORD chunkSize) : CFileRep(errorReporting,
        maxChannels, transport, security, encoding)
    {
        this->chunkSize = chunkSize;
    }

    HRESULT ProcessMessage(CRequest* request, const WS_XML_STRING* receivedAction);

protected:
    
    HRESULT SendError(CRequest* request, __in const WCHAR errorMessage[]);
    HRESULT ReadAndSendFile(CRequest* request, __in const LPWSTR fileName, LONGLONG chunkPosition, WS_ERROR *error);
    HRESULT SendFileInfo(CRequest* requeste, __in const LPWSTR fileName, LONGLONG llFileLength, DWORD chunkSize);
    HRESULT ReadAndSendChunk(CRequest* request, long chunkSize, LONGLONG chunkPosition, HANDLE file);

    long chunkSize;
};

// Client service.
class CFileRepClient : public CFileRep
{
public:
    CFileRepClient(REPORTING_LEVEL errorReporting, DWORD maxChannels, TRANSPORT_MODE transport, 
        SECURITY_MODE security, MESSAGE_ENCODING encoding) : CFileRep(errorReporting, maxChannels, 
        transport, security, encoding)
    {
    }

    HRESULT ProcessMessage(CRequest* request, const WS_XML_STRING* receivedAction);

protected:
    HRESULT SendUserResponse(CRequest* request, TRANSFER_RESULTS result);

    HRESULT ProcessUserRequest(CRequest* request, __in const LPWSTR sourcePath,
        __in const LPWSTR destinationPath, __in const LPWSTR serverUri, TRANSPORT_MODE transportMode, 
        SECURITY_MODE securityMode, MESSAGE_ENCODING encoding, REQUEST_TYPE requestType);

    
    HRESULT CreateServerChannel(MESSAGE_ENCODING serverEncoding, TRANSPORT_MODE serverTransportMode, 
        SECURITY_MODE serverSecurityMode, WS_ERROR *error, WS_CHANNEL **channel);

    HRESULT ExtendFile(HANDLE file, LONGLONG length);

    HRESULT ProcessChunk(long chunkSize, HANDLE file, LONGLONG fileLength, WS_MESSAGE *requestMessage, 
        WS_MESSAGE *replyMessage, WS_CHANNEL *channel, WS_ERROR *error, FileRequest *request);


    HRESULT DeserializeAndWriteMessage(WS_MESSAGE *message, long chunkSize, LONGLONG *chunkPosition,
        long *contentLength, HANDLE file);
};

// Helper functions.
void PrintError(HRESULT errorCode, WS_ERROR* error);

HRESULT ParseTransport(__in const LPWSTR url, TRANSPORT_MODE *transport, SECURITY_MODE *securityMode);

void CleanupChannel(WS_CHANNEL* channel);


