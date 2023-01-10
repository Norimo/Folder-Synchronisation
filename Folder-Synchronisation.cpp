#include <filesystem>
#include <iostream>
#include <fstream>
#include <ctime>
#include <atomic>
#include <thread>

namespace fs = std::filesystem;

//Enumeration declaration for types of log message
enum logMessageType {
    Creating,
    Copying,
    Deleting
};

// Atomic flag to signal the synchronization thread to stop
std::atomic<bool> stopSynchronization(false);

// Function to show log message
void logMessage(logMessageType messageType, const fs::path& source, const fs::path& replica, std::ofstream& logFile)
{
    switch (messageType)
    {
    case logMessageType::Creating:
        std::cout << "Created: " << replica;
        logFile << "Created: " << replica;
        break;
    case logMessageType::Copying:
        std::cout << "Copied: " << source << " > " << replica;
        logFile << "Copied: " << source << " > " << replica;
        break;
    case logMessageType::Deleting:
        std::cout << "Deleted: " << replica;
        logFile << "Deleted: " << replica;
        break;
    }
    std::cout << std::endl;
    logFile << std::endl;
}

// Function to synchronize two folders
void synchronizeFolders(const fs::path& source, const fs::path& replica, std::ofstream& logFile)
{
    if (!fs::exists(source) || !fs::is_directory(source)) {
        std::cerr << "Error: " << source << " is not a valid directory." << std::endl;
        exit(1);
    }

    if (!fs::exists(replica)) {
        fs::create_directory(replica);
        logMessage(logMessageType::Creating, source, replica, logFile);
    }

    for (auto& entry : fs::directory_iterator(source)) {
        fs::path current = entry.path();
        if (fs::is_directory(current)) {
            // Recursively synchronize subfolders
            fs::path subfolder = replica / current.filename();
            synchronizeFolders(current, subfolder, logFile);
        }
        else {
            fs::path destination = replica / current.filename();
            if (fs::exists(destination)) {
                // Copy file from source to replica, if it is exist
                if (fs::last_write_time(current) > fs::last_write_time(destination)) {
                    fs::remove(destination);
                    fs::copy_file(current, destination);
                    logMessage(logMessageType::Copying, current, destination, logFile);
                }
            }
            else {
                // Create file from source to replica, if it isn't exist
                fs::copy_file(current, destination);
                logMessage(logMessageType::Creating, current, destination, logFile);
            }
        }
    }

    // Delete files in replica that don't exist in source
    for (auto& entry : fs::directory_iterator(replica)) {
        fs::path current = entry.path();
        if (!fs::exists(source / current.filename())) {
            fs::remove(current);
            logMessage(logMessageType::Deleting, source, current, logFile);
        }
    }
}

//Function to synchronize two folders periodically
void synchronization(const fs::path& source, const fs::path& replica, std::ofstream& logFile, int interval)
{
    while (!stopSynchronization) {
        synchronizeFolders(source, replica, logFile);

        // Sleep for the specified
        std::this_thread::sleep_for(std::chrono::seconds(interval));
    }
}

int main(int argc, char* argv[])
{
    if (argc < 4) {
        std::cerr << "Usage: Folder-Synchronisation.exe <source_folder> <replica_folder> <interval_seconds> <log_file>" << std::endl;
        system("pause");
        return 1;
    }

    fs::path source(argv[1]);
    fs::path replica(argv[2]);
    int interval = std::atoi(argv[3]);

    // Open log file
    std::ofstream logFile;
    if (argc >= 5) {
        logFile.open(argv[4], std::ofstream::app);
    }

    std::cout << "Press q to stop synchronization" << std::endl;

    // Start the synchronization thread
    std::thread syncThread(synchronization, source, replica, std::ref(logFile), interval);

    char c = getchar();
    if (c == 'q') {
        stopSynchronization = true;
    }

    // Wait for the synchronization thread to finish
    syncThread.join();

    system("pause");
    return 0;
}
