
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "ocstack.h"
#include "csdkWrapper.h"
#include "logger.h"
#include "cJSON.h"

static uint16_t OC_WELL_KNOWN_PORT = 5683;
#define TAG "csdkWrapper"

static std::string gUri = "/a/genericdevice";

static OCResourceHandle gResourceHandle;
static std::string gParams[CsdkWrapper::NUM_PARAMS];

CsdkWrapper::EntityHandler gHandler;

//This function takes the request as an input and returns the response
//in JSON format.
static char* constructJsonResponse(OCEntityHandlerRequest *ehRequest)
{
    cJSON *json = cJSON_CreateObject();
    cJSON *format;
    char *jsonResponse;

    if (OC_REST_PUT == ehRequest->method)
    {
        cJSON *putJson = cJSON_Parse((char *)ehRequest->reqJSONPayload);
        gParams[0] = cJSON_GetObjectItem(putJson,"param1")->valuestring;
        gParams[1] = cJSON_GetObjectItem(putJson,"param2")->valuestring;
        gParams[2] = cJSON_GetObjectItem(putJson,"param3")->valuestring;
        gParams[3] = cJSON_GetObjectItem(putJson,"param4")->valuestring;

        cJSON_Delete(putJson);
    }

    cJSON_AddStringToObject(json,"href", gUri.c_str());
    cJSON_AddItemToObject(json, "rep", format=cJSON_CreateObject());
    cJSON_AddStringToObject(format, "param1", gParams[0].c_str());
    cJSON_AddStringToObject(format, "param2", gParams[1].c_str());
    cJSON_AddStringToObject(format, "param3", gParams[2].c_str());
    cJSON_AddStringToObject(format, "param4", gParams[3].c_str());

    jsonResponse = cJSON_Print(json);
    cJSON_Delete(json);

    return jsonResponse;
}
std::string CsdkWrapper::EntityHandlerInfo::json()
{
    cJSON *json = cJSON_CreateObject();
    cJSON *json_params = cJSON_CreateArray();

    cJSON_AddStringToObject(json, "resource", resource.c_str());
    cJSON_AddStringToObject(json, "method", method.c_str());

    for (uint8_t i = 0; i < CsdkWrapper::NUM_PARAMS; i++) {
        cJSON_AddItemToArray(json_params, cJSON_CreateString(params[i].c_str()));
    }
    cJSON_AddItemToObject(json, "params", json_params);
    std::string ret(cJSON_Print(json));
    cJSON_Delete(json);
    return ret;
}
static char* constructJsonPayload(CsdkWrapper::EntityHandlerInfo *response)
{
    cJSON *json = cJSON_CreateObject();
    cJSON *format;
    char *jsonResponse;

    cJSON_AddStringToObject(json,"href", gUri.c_str());
    cJSON_AddItemToObject(json, "rep", format=cJSON_CreateObject());
    cJSON_AddStringToObject(format, "param1", response->params[0].c_str());
    cJSON_AddStringToObject(format, "param2", response->params[1].c_str());
    cJSON_AddStringToObject(format, "param3", response->params[2].c_str());
    cJSON_AddStringToObject(format, "param4", response->params[3].c_str());

    jsonResponse = cJSON_Print(json);
    cJSON_Delete(json);

    return jsonResponse;
}

static OCEntityHandlerResult ProcessGetRequest (OCEntityHandlerRequest *ehRequest, char *payload, uint16_t maxPayloadSize)
{
    OCEntityHandlerResult ehResult;
    CsdkWrapper::EntityHandlerResult result = CsdkWrapper::EH_RESULT_ERROR;
    CsdkWrapper::EntityHandlerInfo request;

    if (gHandler)
    {
        request.method = "GET";
        request.requestHandle = ehRequest->requestHandle;
        request.resource = gUri;
        for (unsigned int i = 0; i < CsdkWrapper::NUM_PARAMS; i++)
        {
            request.params[i] = "";
        }
        result = gHandler(&request);
    }

    if (CsdkWrapper::EH_RESULT_OK == result)
    {
        for (unsigned int i = 0; i < CsdkWrapper::NUM_PARAMS; i++)
        {
            gParams[i] = request.params[i];
        }

        char *getResp = constructJsonResponse(ehRequest);

        if (maxPayloadSize > strlen ((char *)getResp))
        {
            strncpy(payload, getResp, strlen((char *)getResp));
            ehResult = OC_EH_OK;
        }
        else
        {
            OC_LOG_V (INFO, TAG, "Response buffer: %d bytes is too small",
                    maxPayloadSize);
            ehResult = OC_EH_ERROR;
        }

        free(getResp);
    }
    else if (CsdkWrapper::EH_RESULT_SLOW == result)
    {
        ehResult = OC_EH_SLOW;
    }
    else
    {
        ehResult = OC_EH_ERROR;
    }

    return ehResult;
}

static OCEntityHandlerResult ProcessPutRequest (OCEntityHandlerRequest *ehRequest, char *payload, uint16_t maxPayloadSize)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    CsdkWrapper::EntityHandlerResult result = CsdkWrapper::EH_RESULT_ERROR;
    CsdkWrapper::EntityHandlerInfo request;

    char *putResp = constructJsonResponse(ehRequest);

    if (maxPayloadSize > strlen ((char *)putResp))
    {
        strncpy(payload, putResp, strlen((char *)putResp));
        if (gHandler)
        {
            request.method = "PUT";
            request.requestHandle = ehRequest->requestHandle;
            request.resource = gUri;
            for (unsigned int i = 0; i < CsdkWrapper::NUM_PARAMS; i++)
            {
                request.params[i] = gParams[i];
            }
            result = gHandler(&request);

            if (CsdkWrapper::EH_RESULT_OK == result)
            {
                ehResult = OC_EH_OK;
            }
            else if (CsdkWrapper::EH_RESULT_SLOW == result)
            {
                ehResult = OC_EH_SLOW;
            }
            else
            {
                ehResult = OC_EH_ERROR;
            }
        }
    }
    else
    {
        OC_LOG_V(INFO, TAG, "Response buffer: %d bytes is too small",
                maxPayloadSize);
        ehResult = OC_EH_ERROR;
    }

    free(putResp);

    return ehResult;
}
static const char *getResult(OCStackResult result) {
    switch (result)
    {
        case OC_STACK_OK:
            return "OC_STACK_OK";
        case OC_STACK_RESOURCE_CREATED:
            return "OC_STACK_RESOURCE_CREATED";
        case OC_STACK_RESOURCE_DELETED:
            return "OC_STACK_RESOURCE_DELETED";
        case OC_STACK_INVALID_URI:
            return "OC_STACK_INVALID_URI";
        case OC_STACK_INVALID_QUERY:
            return "OC_STACK_INVALID_QUERY";
        case OC_STACK_INVALID_IP:
            return "OC_STACK_INVALID_IP";
        case OC_STACK_INVALID_PORT:
            return "OC_STACK_INVALID_PORT";
        case OC_STACK_INVALID_CALLBACK:
            return "OC_STACK_INVALID_CALLBACK";
        case OC_STACK_INVALID_METHOD:
            return "OC_STACK_INVALID_METHOD";
        case OC_STACK_NO_MEMORY:
            return "OC_STACK_NO_MEMORY";
        case OC_STACK_COMM_ERROR:
            return "OC_STACK_COMM_ERROR";
        case OC_STACK_INVALID_PARAM:
            return "OC_STACK_INVALID_PARAM";
        case OC_STACK_NOTIMPL:
            return "OC_STACK_NOTIMPL";
        case OC_STACK_NO_RESOURCE:
            return "OC_STACK_NO_RESOURCE";
        case OC_STACK_RESOURCE_ERROR:
            return "OC_STACK_RESOURCE_ERROR";
        case OC_STACK_SLOW_RESOURCE:
            return "OC_STACK_SLOW_RESOURCE";
        case OC_STACK_NO_OBSERVERS:
            return "OC_STACK_NO_OBSERVERS";
        #ifdef WITH_PRESENCE
        case OC_STACK_VIRTUAL_DO_NOT_HANDLE:
            return "OC_STACK_VIRTUAL_DO_NOT_HANDLE";
        case OC_STACK_PRESENCE_STOPPED:
            return "OC_STACK_PRESENCE_STOPPED";
        case OC_STACK_PRESENCE_TIMEOUT:
            return "OC_STACK_PRESENCE_TIMEOUT";
        #endif
        case OC_STACK_ERROR:
            return "OC_STACK_ERROR";
        default:
            return "UNKNOWN";
    }
}

OCEntityHandlerResult OCEntityHandlerCb(OCEntityHandlerFlag     flag,
                                        OCEntityHandlerRequest *entityHandlerRequest)
{
    OC_LOG_V(INFO, TAG, "Inside entity handler - flags: 0x%x", flag);

    OCEntityHandlerResult ehResult = OC_EH_OK;
    OCEntityHandlerResponse response;
    char payload[MAX_RESPONSE_LENGTH] = {};

    // Validate pointer
    if (!entityHandlerRequest)
    {
        OC_LOG(ERROR, TAG, "Invalid request pointer");
        return OC_EH_ERROR;
    }

    if (flag & OC_REQUEST_FLAG)
    {
        OC_LOG(INFO, TAG, "Flag includes OC_REQUEST_FLAG");
        if (OC_REST_GET == entityHandlerRequest->method)
        {
            OC_LOG(INFO, TAG, "Received OC_REST_GET from client");
            ehResult = ProcessGetRequest(entityHandlerRequest, payload, sizeof(payload) - 1);

        }
        else if (OC_REST_PUT == entityHandlerRequest->method)
        {
            OC_LOG(INFO, TAG, "Received OC_REST_PUT from client");
            ehResult = ProcessPutRequest(entityHandlerRequest, payload, sizeof(payload) - 1);
        }
        else
        {
            OC_LOG_V(INFO, TAG, "Received unsupported method %d from client",
                    entityHandlerRequest->method);
        }
    }

    // If the result isn't an error or slow, send response
    if (ehResult == OC_EH_OK)
    {
        // Format the response.  Note this requires some info about the request
        response.requestHandle = entityHandlerRequest->requestHandle;
        response.resourceHandle = entityHandlerRequest->resource;
        response.ehResult = ehResult;
        response.payload = (unsigned char *)payload;
        response.payloadSize = strlen(payload);
        // Indicate that response is NOT in a persistent buffer
        response.persistentBufferFlag = 0;

        // Send the response
        if (OCDoResponse(&response) != OC_STACK_OK)
        {
            OC_LOG(ERROR, TAG, "Error sending response");
            ehResult = OC_EH_ERROR;
        }
    }

    return ehResult;
}

CsdkWrapper::CsdkWrapper()
{
    OC_LOG(INFO, TAG, "Creating CsdkWrapper");

    gHandler = 0;
    for (unsigned int i = 0; i < NUM_PARAMS; i++)
    {
        gParams[i] = "0";
    }

    uint8_t* paddr = NULL;
    uint16_t port = OC_WELL_KNOWN_PORT;
#if 1
    uint8_t addr[20] = {};
    uint8_t ifname[] = "eth0";

    if ( OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr,
                sizeof(addr)) == OC_ERR_SUCCESS)
    {
        OC_LOG_V(INFO, TAG, "Starting iotivity server on address %s:%d",addr,port);
        paddr = addr;
    }
#endif

    if (OCInit((char *) paddr, port, OC_SERVER) != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "OCStack init error");
    }
}
bool CsdkWrapper::start(EntityHandler handler, std::string uri)
{
    gUri = uri;
    OC_LOG(INFO, TAG, "Starting CsdkWrapper");

    if (!handler)
    {
        OC_LOG(ERROR, TAG, "Invalid entity handler");
        return false;
    }

    gHandler = handler;

    return createResource();
}

bool CsdkWrapper::stop()
{
    OC_LOG(INFO, TAG, "Stopping CsdkWrapper");

    if (OCStop() != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "OCStack process error");
        return false;
    }
    return true;
}

bool CsdkWrapper::respond(EntityHandlerInfo *response)
{
    OC_LOG(INFO, TAG, "Sending response");

    OCEntityHandlerResponse entityHandlerResponse;

    entityHandlerResponse.requestHandle = response->requestHandle;
    entityHandlerResponse.resourceHandle = response->resourceHandle;
    entityHandlerResponse.ehResult = OC_EH_OK;
    entityHandlerResponse.numSendVendorSpecificHeaderOptions = 0;
    memset(entityHandlerResponse.sendVendorSpecificHeaderOptions, 0, sizeof entityHandlerResponse.sendVendorSpecificHeaderOptions);
    memset(entityHandlerResponse.resourceUri, 0, sizeof(entityHandlerResponse.resourceUri));
    // Indicate that response is NOT in a persistent buffer
    entityHandlerResponse.persistentBufferFlag = 0;
    // Generate json for payload
    char *getResp = constructJsonPayload(response);
    entityHandlerResponse.payload = (unsigned char *)getResp;
    entityHandlerResponse.payloadSize = strlen(getResp) + 1;

    // Send the response
    if (OCDoResponse(&entityHandlerResponse) != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "Error sending response");
        return false;
    }
    free(getResp);

    return true;
}

bool CsdkWrapper::process()
{
    if (OCProcess() != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "OCStack process error");
        return false;
    }
    return true;
}

bool CsdkWrapper::createResource()
{
    OC_LOG(INFO, TAG, "Creating resource");

    OCStackResult result = OCCreateResource(&gResourceHandle,
            "core.genericdevice",
            "oc.mi.def",
            gUri.c_str(),
            OCEntityHandlerCb,
            OC_DISCOVERABLE|OC_OBSERVABLE);

    bool retVal = false;

    if (OC_STACK_OK == result)
    {
        OC_LOG_V(INFO, TAG, "Created generic device resource with result: %s", getResult(result));
        retVal = true;
    }
    else
    {
        OC_LOG(ERROR, TAG, "Error creating resource");
    }

    return retVal;
}