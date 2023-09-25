//
// Created by Piotr Krzysztof Wyrwas on 23.09.23.
//

#include <pthread.h>
#include "global.h"
#include "threadsafe.h"
#include "gplogging.h"

#include <stdio.h>

static int lock_global_mutex()
{
        int status = pthread_mutex_lock(&global_mutex);
        if (status != 0) {
                ERROR_LOG("Failed when trying to lock mutex: %d\n", status)
                return -1;
        }
        return 0;
}

_Bool is_running()
{
        _Bool val;

        if (lock_global_mutex() < 0) {
                return false; // Default to requesting a server shutdown
        }

        val = running;
        pthread_mutex_unlock(&global_mutex);

        return val;
}

void request_server_shutdown()
{
        if (lock_global_mutex() < 0) {
                return;
        }

        running = false;
        pthread_mutex_unlock(&global_mutex);
}

unsigned long get_running_threads()
{
        unsigned long val;

        if (lock_global_mutex() < 0) {
                return 0; // There's no good return value in this case, but we have to return something
        }

        val = running_threads;
        pthread_mutex_unlock(&global_mutex);

        return val;
}

void increment_running_threads()
{
        if (lock_global_mutex() < 0) {
                return;
        }

        running_threads++;
        pthread_mutex_unlock(&global_mutex);
}

void decrement_running_threads()
{
        if (lock_global_mutex() < 0) {
                return;
        }

        running_threads--;
        pthread_mutex_unlock(&global_mutex);
}
