#pragma once

#include <string>
#include <memory>

const std::string participantCSVPath = "./participant.csv";
const std::string trialdataCSVPath = "./trialdata.csv";
const std::string songtableCSVPath = "./songtable.csv";

class Database {
private:

    enum Table {
        Participant,
        TrialData,
        SongTable,
        Unknown
    };

    struct CSVLine {
        virtual std::string outputString() const = 0;
        virtual void parse(std::string& data) = 0;
        virtual void writeHeaders(std::ofstream& file) = 0;
        virtual ~CSVLine() = default;
        bool init = false;
        std::string filePath;
        CSVLine(std::string path): filePath(path) {}
    };

    struct ParticipantLine : public CSVLine {
        std::string participantID;
        std::string name;
        int height;
        int weight;
        std::string gender;
        std::string dominantHand;
        bool experience;
        std::string notes;

        std::string outputString() const;
        void writeHeaders(std::ofstream& file);
        void parse(std::string& data);

        ParticipantLine() : CSVLine(participantCSVPath) {}
    };

    struct TrialDataLine : public CSVLine {
        std::string trialID;
        std::string participantID;
        int attempt;
        int bpm;
        int force;
        int totalPunches;
        float accuracy;
        int maxForce;
        int minForce;
        int avgForce;

        std::string outputString() const;
        void writeHeaders(std::ofstream& file);
        void parse(std::string& data);

        TrialDataLine() : CSVLine(trialdataCSVPath) {}
    };

    struct SongTableLine : public CSVLine {
        std::string songID;
        std::string songName;
        std::string bpmRange;

        std::string outputString() const;
        void writeHeaders(std::ofstream& file);
        void parse(std::string& data);

        SongTableLine() : CSVLine(songtableCSVPath) {}
    };

    void createFile(std::string& path);
    void appendToFile(std::unique_ptr<CSVLine>& line);
    
    std::unique_ptr<CSVLine> parseTable(std::string& data);

public: 
    void saveData(std::string& data);
    void loadResult();
    Database() {}
};