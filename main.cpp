#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int arr[20] {}; // Array of int
    int max = 1000; // Max value for random generator
    int min = 0; // Min value for random generator
    int range = max - min + 1; // The range of numbers we want to generate
    int pipefd[2]; // Pipe file descriptors
    pid_t pid; // ProcessID type

    srand(time(0)); // Seed random number generator

    // Fill array with random numbers
    for (size_t i = 0; i < 20; i++) {
        arr[i] = rand() % range + min;
    }

    // Print the array
    std::cout << "Array: ";
    for (size_t i = 0; i < 20; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;

    // Create pipe
    if (pipe(pipefd) == -1) {
        perror("Pipe failed");
        return 1;
    }

    // Fork a child process
    pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        return 1;
    }

    if (pid == 0) { // Child process
        close(pipefd[0]); // Close read end of pipe
        int childMin = arr[10]; // Start with first element of second half
        for (size_t i = 11; i < 20; i++) {
            if (arr[i] < childMin) {
                childMin = arr[i];
            }
        }
        write(pipefd[1], &childMin, sizeof(childMin)); // Send min to parent
        std::cout << "Child Process (PID: " << getpid() << ") found min: " << childMin << std::endl;
        close(pipefd[1]); // Close write end of pipe
        exit(0);
    } else { // Parent process
        close(pipefd[1]); // Close write end of pipe
        int parentMin = arr[0]; // Start with first element of first half
        for (size_t i = 1; i < 10; i++) {
            if (arr[i] < parentMin) {
                parentMin = arr[i];
            }
        }
        int childMin;
        read(pipefd[0], &childMin, sizeof(childMin)); // Read min from child
        close(pipefd[0]); // Close read end of pipe
        wait(NULL); // Wait for child process to finish

        int overallMin = std::min(parentMin, childMin); // Get the min from both parent and child, keeping whichever is smaller

        std::cout << "Parent Process (PID: " << getpid() << ") found min: " << parentMin << std::endl;
        std::cout << "Overall Minimum Value: " << overallMin << std::endl;
    }

    return 0;
}
