#include <string>

class CsdkWrapper
{
public:
    enum EntityHandlerResult
    {
        EH_RESULT_ERROR = 0,
        EH_RESULT_OK,
        EH_RESULT_SLOW
    };

    typedef void *RequestHandle;
    typedef void *ResourceHandle;

    static const size_t NUM_PARAMS = 4;

    struct EntityHandlerInfo
    {
        // Resource name that callback is invoked for
        std::string resource;
        // Resource handle
        ResourceHandle resourceHandle;
        // Handle of request
        RequestHandle requestHandle;
        // REST method from received request
        std::string method;
        // Resource parameters
        std::string params[NUM_PARAMS];

        std::string json();
    };

    typedef EntityHandlerResult (*EntityHandler)(EntityHandlerInfo *request);

    CsdkWrapper();
    bool start(EntityHandler handler, std::string uri);
    bool stop();
    bool respond(EntityHandlerInfo *response);
    bool process();

private:
    bool createResource();
};