//
// Created by Piotr Krzysztof Wyrwas on 23.09.23.
//

#ifndef KETAMINE_THREADSAFE_H
#define KETAMINE_THREADSAFE_H

_Bool is_running();

void request_server_shutdown();

unsigned long get_running_threads();

void increment_running_threads();

void decrement_running_threads();

#endif //KETAMINE_THREADSAFE_H
