#pragma once

#include "CommQueue.h"

template <typename Req, typename Res>
class InterThreadCommPipe
{
private:
    CommQueue<Req> requests;
    CommQueue<Res> responses;

public:
    InterThreadCommPipe() {};
    
    void sendRequest(Req r)
    {
        requests.push(r);
    }
    
    bool pendingRequest()
    {
        return !requests.isEmpty();
    }

    Req getRequest()
    {
        return requests.pop();
    }

    void sendResponse(Res r)
    {
        responses.push(r);
    }

    bool pendingResponse()
    {
        return !responses.isEmpty();
    }
    
    Res getResponse()
    {
        return responses.pop();
    }
    
    ~InterThreadCommPipe() {};
};