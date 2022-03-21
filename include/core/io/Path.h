//
// Created by tyll on 2022-03-20.
//

#ifndef SGLOGGER_PATH_H
#define SGLOGGER_PATH_H
#include "core/StringBuilder.h"

namespace core { namespace io { namespace path {
    /* Combines two strings into a path.
     * Returns: The combined paths. If one of the specified paths is a zero-length string, this method returns the other path. If path2 contains an absolute path, this method returns path2.
     */
    core::StringBuilder combine(core::StringBuilder sb, const char * path1, const char * path2) {
        if (path2[0] == '/') { // absolute path
            sb.append(path2);
        } else {
            sb.append(path1);
            if (sb.toString()[sb.length() - 1] != '/')
                sb.append('/');
            sb.append(path2);
        }
        return sb;
    }

    core::StringBuilder getDirectoryName(core::StringBuilder sb, const char* path) {
        uint32_t subPathLength = strchr(path, '/') - path;
        sb.append(path, subPathLength);
        return sb;
    }
}}}
#endif //SGLOGGER_PATH_H
