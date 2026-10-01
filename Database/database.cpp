#include "database.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <filesystem>

void Database::saveData(std::string& data) {
    auto line = parseTable(data);
    line->parse(data);
    appendToFile(line);
}

// thanks again chatgpt
std::string trim(const std::string& str) {
    const auto start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos)
        return "";

    const auto end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

// thanks chatgpt
std::vector<std::string> split(const std::string& line) {
    std::vector<std::string> parts;
    std::stringstream ss(line);
    std::string part;

    while (std::getline(ss, part, ',')) {
        parts.push_back(trim(part));
    }

    return parts;
}

void errorIfNotInit(bool init) {
    if (!init) {
        throw std::runtime_error("Trying to generate uninitialized CSVLine outputString\n");
    }
}

std::string Database::ParticipantLine::outputString() const {
    errorIfNotInit(init);
    std::stringstream out;
    out << participantID << ",";
    out << name << ",";
    out << height << ",";
    out << weight << ",";
    out << gender << ",";
    out << dominantHand << ",";
    out << experience << ",";
    out << notes;
    return out.str();
}

std::string Database::TrialDataLine::outputString() const {
    errorIfNotInit(init);
    std::stringstream out;
    out << trialID << ",";
    out << participantID << ",";
    out << attempt << ",";
    out << bpm << ",";
    out << force << ",";
    out << totalPunches << ",";
    out << accuracy << ",";
    out << maxForce << ",";
    out << minForce << ",";
    out << avgForce;
    return out.str();
}

std::string Database::SongTableLine::outputString() const {
    errorIfNotInit(init);
    std::stringstream out;
    out << songID << ",";
    out << songName << ",";
    out << bpmRange << ",";
    return out.str();
}

void Database::ParticipantLine::parse(std::string& data) {
    auto parts = split(data);

    participantID = parts[0];

    if (participantID.length() > 10) {
        throw std::runtime_error("participant id is too many characters\n");
    }

    name = parts[1];

    if (name.length() > 255) {
        throw std::runtime_error("participant name is too many characters\n");
    }

    height = std::stoi(parts[2]);
    weight = std::stoi(parts[3]);
     
    gender = parts[4];
    if (gender.length() > 10) {
        throw std::runtime_error("participant gender is too many characters\n");
    }

    dominantHand = parts[5];    
    if (dominantHand.length() > 10) {
        throw std::runtime_error("participant dominant hand is too many characters\n");
    }

    experience = parts[6] == "yes" || parts[6] == "Yes" || parts[6] == "YES" ? true : false;

    notes = parts[7];
    if (notes.length() > 50) {
        throw std::runtime_error("participant notes are too many characters\n");
    }

    init = true;
}

void Database::TrialDataLine::parse(std::string& data) {
    auto parts = split(data);

    trialID = parts[0];

    if (trialID.length() > 10) {
        throw std::runtime_error("Trial data id is too many characters\n");
    }

    participantID = parts[1];

    if (participantID.length() > 10) {
        throw std::runtime_error("trial data's participant id is too many characters\n");
    }

    attempt = std::stoi(parts[2]);
    bpm = std::stoi(parts[3]);
    force = std::stoi(parts[4]);
    totalPunches = std::stoi(parts[5]);
    accuracy = std::stof(parts[6]);
    maxForce = std::stoi(parts[7]);
    minForce = std::stoi(parts[8]);
    avgForce = std::stoi(parts[9]);

    init = true;
}

void Database::SongTableLine::parse(std::string& data) {
    auto parts = split(data);

    songID = parts[0];

    songName = parts[1];
    if (songName.length() > 50) {
        throw std::runtime_error("song name has too many characters\n");
    }

    bpmRange = parts[2];
    if (bpmRange.length() > 10) {
        throw std::runtime_error("bpm range has too many characters\n");
    }

    init = true;
}

std::unique_ptr<Database::CSVLine> Database::parseTable(std::string& data) {
    int nCommas = std::count(data.begin(), data.end(), ',');
    if (nCommas == 7) {
        return std::make_unique<Database::ParticipantLine>();
    }
    if (nCommas == 9) {
        return std::make_unique<Database::TrialDataLine>();
    }
    if (nCommas == 2) {
        return std::make_unique<Database::SongTableLine>();
    }
    throw std::runtime_error("wrong number of commas in data given to parseTable\n");
}

void Database::ParticipantLine::writeHeaders(std::ofstream& file) {
    file << "participantID" << "," << "name" << "," << "height" << "," << "weight" << "," << "gender" << "," 
    << "dominantHand" << "," << "experience" << "," << "notes" << std::endl;
}

void Database::TrialDataLine::writeHeaders(std::ofstream& file) {
    file << "trialID" << "," << "participantID" << "," << "attempt" << "," 
    << "bpm" << "," << "force" << "," << "totalPunches" << "," << "accuracy" << ","
    << "maxForce" << "," << "minForce" << "," << "avgForce" << std::endl;
}

void Database::SongTableLine::writeHeaders(std::ofstream& file) {
    file << "songID" << "," << "songName" << "," << "bpmRange" << std::endl;
}

void Database::appendToFile(std::unique_ptr<Database::CSVLine>& line) {
    std::ofstream file;
    if (!std::filesystem::exists(line->filePath)) {
        file.open(line->filePath);
        line->writeHeaders(file);
        file.close();
    }

    file.open(line->filePath, std::ios::app);
    file << line->outputString() << "\n";
}

std::string getLastLine(const std::string path) {
    std::ifstream file(path);

    std::string line;
    std::string lastLine;

    while (std::getline(file, line)) {
        lastLine = line;
    }
    return lastLine;
}

void Database::loadResult() {
    if (!std::filesystem::exists(participantCSVPath) ||
        !std::filesystem::exists(trialdataCSVPath) ||
        !std::filesystem::exists(songtableCSVPath)
    ) {
        throw std::runtime_error("loadResult called but a CSV is missing!\n");
    }
    
    std::string pLine, tLine, sLine;
    pLine = getLastLine(participantCSVPath);
    tLine = getLastLine(trialdataCSVPath);
    sLine = getLastLine(songtableCSVPath);

    ParticipantLine participant = ParticipantLine();
    participant.parse(pLine);

    TrialDataLine trial = TrialDataLine();
    trial.parse(tLine);

    SongTableLine song = SongTableLine();
    song.parse(sLine);

    std::cout << "Participant: " << participant.participantID << " " << participant.name << std::endl; 
    std::cout << "Attempt " << trial.attempt << std::endl;
    std::cout << "Max: " << trial.maxForce <<  ", Avg: " << trial.avgForce <<  ", Acc: " << trial.accuracy << std::endl;
    std::cout << song.bpmRange << "bpm" << std::endl;
}