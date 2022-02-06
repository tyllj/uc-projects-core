//
// Created by tyll on 2022-02-03.
//

#ifndef SGLOGGER_REFERENCE_CAST_H
#define SGLOGGER_REFERENCE_CAST_H

template <typename TSource, typename TTarget>
TTarget& reference_cast(TSource& source) {
    return *((TTarget*) &source);
}

#endif //SGLOGGER_REFERENCE_CAST_H
