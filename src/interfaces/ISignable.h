#pragma once

class ISignable
{
public:
    virtual ~ISignable() {}
    virtual bool IsSignal() = 0;
};