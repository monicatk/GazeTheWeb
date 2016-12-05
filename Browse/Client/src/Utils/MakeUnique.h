//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Make unique is not available in GCC.

#ifndef MAKE_UNIQUE_H_
#define MAKE_UNIQUE_H_

#ifdef __linux__

#include <memory>

namespace std
{

    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

}

#endif

#endif // MAKE_UNIQUE_H_
