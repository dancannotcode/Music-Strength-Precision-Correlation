#include "database.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <filesystem>

void Database::saveData(std::string& data) {
    Table t = parseTable(data);

    if (t == Participant) {
        ParticipantLine line = ParticipantLine::parse(data);
        appendToFile(t, line);
    }
    else if (t == TrialData) {
        TrialDataLine line = TrialDataLine::parse(data);
        appendToFile(t, line);
    }
    else if (t == SongTable) {
        SongTableLine line = SongTableLine::parse(data);
        appendToFile(t, line);
    }
    else {
        throw std::runtime_error("Invalid table selection in appendToFile");
    }
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

std::string Database::ParticipantLine::outputString() const {
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
    std::stringstream out;
    out << songID << ",";
    out << songName << ",";
    out << bpmRange << ",";
    return out.str();
}

Database::ParticipantLine Database::ParticipantLine::parse(std::string& data) {
    auto parts = split(data);

    std::string id = parts[0];

    if (id.length() > 10) {
        throw std::runtime_error("participant id is too many characters\n");
    }

    std::string name = parts[1];

    if (name.length() > 255) {
        throw std::runtime_error("participant name is too many characters\n");
    }

    int height = std::stoi(parts[2]);
    int weight = std::stoi(parts[3]);

    std::string gender = parts[4];
    if (gender.length() > 10) {
        throw std::runtime_error("participant gender is too many characters\n");
    }

    std::string dominantHand = parts[5];    
    if (dominantHand.length() > 10) {
        throw std::runtime_error("participant dominant hand is too many characters\n");
    }

    bool experience = parts[6] == "yes" || parts[6] == "Yes" || parts[6] == "YES" ? true : false;

    std::string notes = parts[7];
    if (notes.length() > 50) {
        throw std::runtime_error("participant notes are too many characters\n");
    }

    return ParticipantLine(id, name, height, weight, gender, dominantHand, experience, notes);
}

Database::TrialDataLine Database::TrialDataLine::parse(std::string& data) {
    auto parts = split(data);

    std::string trialID = parts[0];

    if (trialID.length() > 10) {
        throw std::runtime_error("Trial data id is too many characters\n");
    }

    std::string participantID = parts[1];

    if (participantID.length() > 10) {
        throw std::runtime_error("trial data's participant id is too many characters\n");
    }

    int attempt = std::stoi(parts[2]);
    int bpm = std::stoi(parts[3]);
    int force = std::stoi(parts[4]);
    int totalPunches = std::stoi(parts[5]);
    float accuracy = std::stof(parts[6]);
    int maxForce = std::stoi(parts[7]);
    int minForce = std::stoi(parts[8]);
    int avgForce = std::stoi(parts[9]);

    return TrialDataLine(trialID, participantID, attempt, bpm, force, totalPunches, accuracy, maxForce, minForce, avgForce);
}

Database::SongTableLine Database::SongTableLine::parse(std::string& data) {
    auto parts = split(data);

    std::string songID = parts[0];

    std::string songName = parts[1];
    if (songName.length() > 50) {
        throw std::runtime_error("song name has too many characters\n");
    }

    std::string bpmRange = parts[2];
    if (bpmRange.length() > 10) {
        throw std::runtime_error("bpm range has too many characters\n");
    }

    return SongTableLine(songID, songName, bpmRange);
}

Database::Table Database::parseTable(std::string& data) {
    int nCommas = std::count(data.begin(), data.end(), ',');
    if (nCommas == 7) {
        return Participant;
    }
    if (nCommas == 9) {
        return TrialData;
    }
    if (nCommas == 2) {
        return SongTable;
    }
    std::cout << nCommas << " commas" << std::endl;
    throw std::runtime_error("wrong number of commas in data given to parseTable\n");
    return Unknown;
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

void Database::appendToFile(Table t, CSVLine& line) {
    std::ofstream file;
    if (t == Participant) {
        if (!std::filesystem::exists(participantCSVPath)) {
            file.open(participantCSVPath);
            Database::ParticipantLine::writeHeaders(file);
            file.close();
        }
        file.open(participantCSVPath, std::ios::app);
    }
    else if (t == TrialData) {
        if (!std::filesystem::exists(trialdataCSVPath)) {
            file.open(trialdataCSVPath);
            Database::TrialDataLine::writeHeaders(file);
            file.close();
        }
        file.open(trialdataCSVPath, std::ios::app);
    }
    else if (t == SongTable) {
        if (!std::filesystem::exists(songtableCSVPath)) {
            file.open(songtableCSVPath);
            Database::SongTableLine::writeHeaders(file);
            file.close();
        }
        file.open(songtableCSVPath, std::ios::app);
    }
    else {
        throw std::runtime_error("Invalid table selection in appendToFile");
    }

    file << line.outputString() << "\n";
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
    std::string pLine, tLine, sLine;
    if (std::filesystem::exists(participantCSVPath)) {
        pLine = getLastLine(participantCSVPath);
    }
    if (std::filesystem::exists(trialdataCSVPath)) {
        tLine = getLastLine(trialdataCSVPath);
    }
    if (std::filesystem::exists(songtableCSVPath)) {
        sLine = getLastLine(songtableCSVPath);
    }
    ParticipantLine participant = ParticipantLine::parse(pLine);
    TrialDataLine trial = TrialDataLine::parse(tLine);
    SongTableLine song = SongTableLine::parse(sLine);
    std::cout << "Participant: " << participant.participantID << " " << participant.name << std::endl; 
    std::cout << "Attempt " << trial.attempt << std::endl;
    std::cout << "Max: " << trial.maxForce <<  ", Avg: " << trial.avgForce <<  ", Acc: " << trial.accuracy << std::endl;
    std::cout << song.bpmRange << "bpm" << std::endl;
}