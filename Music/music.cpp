#include "music.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <filesystem>
#include <random>
#include <vector>


const auto LAST_PICKED_FILE_NAME = std::make_unique<std::string>("last_picked.txt");
std::string FOLDER_NAMES[] = {"under_60BPM", "60-120BPM", "over_120BPM"};

std::string Music::selectSong() {
    // Read last_picked.txt to determine next folder to choose
    int folder = readAndUpdateLastPicked();

    // Get a random song from the designated folder
    auto song = randomSongFromFolder(folder);

    return song;
}

// Thanks chatgpt
std::string Music::randomSongFromFolder(int folder) {
    std::vector<std::filesystem::path> files;

    for (const auto& entry : std::filesystem::directory_iterator(FOLDER_NAMES[folder])) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path());
        }
    }

    if (files.empty()) {
        throw std::runtime_error("No files found");
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_int_distribution<size_t> dist(0, files.size() - 1);

    return files[dist(gen)].string();
}

int Music::readAndUpdateLastPicked() {
    std::fstream file;

    // Confirm file exists
    if (!std::filesystem::exists(*LAST_PICKED_FILE_NAME)) {
        file.open(*LAST_PICKED_FILE_NAME, std::ios::out);
        file << "0";
        file.close();
    }

    // Read
    file.open(*LAST_PICKED_FILE_NAME, std::ios::in);
    int ret;
    file >> ret;
    file.close();


    // Update
    file.open(*LAST_PICKED_FILE_NAME, std::ios::out);
    file << ((ret + 1) % 3);
    file.close();

    return ret;
}