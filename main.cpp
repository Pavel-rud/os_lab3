#include "process_handler.hpp"
#include "shared_memory.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <ctime>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#define GET_PID() GetCurrentProcessId()
#else
#include <unistd.h>
#define GET_PID() getpid()
#endif

void logMainProcessStart(int *counter) {
    logToFile("Main process started. PID:" + std::to_string(GET_PID()));
}

void changeCounterValue(int *counter) {
    int newValue;
    std::cout << "Enter number to set counter to: ";
    std::cin >> newValue;
    *counter = newValue;
}

void showCounterValue(int *counter) {
    std::cout << "Counter value: " << *counter << std::endl;
}

void timerThreadFunc(int *counter) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        (*counter)++;
    }
}

void loggerThreadFunc(int *counter) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        logToFile("[LOG] Counter: " + std::to_string(*counter));
    }
}

void subprocessThreadFunc() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        spawnChild(1);
        spawnChild(2);
    }
}

int main() {
    init_shared_memory();
    int *counter = getSharedCounter();

    logMainProcessStart(counter);

    std::thread timerThread(timerThreadFunc, counter);
    std::thread loggerThread(loggerThreadFunc, counter);
    std::thread childSpawnerThread(subprocessThreadFunc);

    std::string userInput;
    while (true) {
        std::cin >> userInput;

        if (userInput == "exit") {
            break;
        } else if (userInput == "change") {
            changeCounterValue(counter);
        } else if (userInput == "show") {
            showCounterValue(counter);
        }
    }

    timerThread.join();
    loggerThread.join();
    childSpawnerThread.join();

    cleanupSharedMemory();
    std::cout << "Exiting...\n";

    return 0;
}
