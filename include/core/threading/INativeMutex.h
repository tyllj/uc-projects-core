//
// Created by tyll on 2022-01-21.
//

#ifndef SGLOGGER_INATIVEMUTEX_H
#define SGLOGGER_INATIVEMUTEX_H

namespace core { namespace threading {
    class INativeMutex {
        virtual void WaitOne() = 0;
        virtual void ReleaseMutex() = 0;
    };
}}

#endif //SGLOGGER_INATIVEMUTEX_H
