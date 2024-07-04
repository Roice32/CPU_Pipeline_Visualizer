#pragma once

#include "Config.h"

class ParameterMetadata
{
public:
    const byte* const typesItCanBe;
    const byte* const mutuallyExclusiveWith;

    ParameterMetadata(const byte* const typesItCanBe, const byte* const mutuallyExclusiveWith):
        typesItCanBe(typesItCanBe), mutuallyExclusiveWith(mutuallyExclusiveWith) {};
};