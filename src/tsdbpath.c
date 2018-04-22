#include "tsdbpath.h"

#ifdef _WIN32
#include <stdlib.h>
#else
#include <stdlib.h>
#include <pwd.h>
#include <unistd.h>
#endif


sds ts_dbpath_get() {


#ifdef _WIN32

    char * appdata = getenv("APPDATA");
    if(appdata == 0) appdata = "";
    sds db_path = sdscat(sdsempty(), appdata);
    db_path = sdscat(db_path, "/tsys/.tsysdb");
    return db_path;

#else

    char * tsdbpath = getenv("TSDBPATH");
    if(tsdbpath == 0) tsdbpath = "";
    char * xdghome = getenv("XDG_CONFIG_HOME");
    if(xdghome == 0) xdghome = "";
    char * homepath = getenv("HOME");
    if(homepath == 0) homepath = "";

    sds db_path = sdscat(sdsempty(), tsdbpath);
    if(sdslen(db_path) == 0) {
        sds home = sdscat(sdsempty(), xdghome);
        if(sdslen(home) == 0) home = sdscat(home, homepath);
        if(sdslen(home) == 0) {
            struct passwd * pswd = getpwuid(geteuid());
            home = sdscat(home, pswd->pw_dir);
        }
        db_path = sdscatsds(db_path, home);
        db_path = sdscat(db_path, "/.tsysdb");
        sdsfree(home);
    }

    return db_path;


#endif
}