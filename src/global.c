//
// Created by Piotr Krzysztof Wyrwas on 19.09.23.
//

#include "global.h"

short listen_port = 80;

pthread_mutex_t global_mutex;
unsigned long running_threads = 0;
_Bool running = true;

const char *banner = "  _  __    _              _          \n"
                     " | |/ /___| |_ __ _ _ __ (_)_ _  ___ \n"
                     " | ' </ -_)  _/ _` | '  \\| | ' \\/ -_)\n"
                     " |_|\\_\\___|\\__\\__,_|_|_|_|_|_||_\\___|\n"
                     "                                     ";