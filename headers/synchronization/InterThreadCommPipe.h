#pragma once

#include "CommQueue.h"

template <typename SentByA, typename SentByB>
class InterThreadCommPipe
{
private:
    CommQueue<SentByA> AQueue;
    CommQueue<SentByB> BQueue;

public:
    InterThreadCommPipe() {};
    
    void sendA(SentByA a)
    {
        AQueue.push(a);
    }
    
    bool pendingA()
    {
        return !AQueue.isEmpty();
    }

    SentByA getA()
    {
        return AQueue.pop();
    }

    void sendB(SentByB b)
    {
        BQueue.push(b);
    }

    bool pendingB()
    {
        return !BQueue.isEmpty();
    }
    
    SentByB getB()
    {
        return BQueue.pop();
    }
};