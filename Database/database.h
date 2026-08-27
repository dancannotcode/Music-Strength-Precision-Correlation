#pragma once

#include <string>


class Database {
private:
    const std::string participantCSVPath = "./participant.csv";
    const std::string trialdataCSVPath = "./trialdata.csv";
    const std::string songtableCSVPath = "./songtable.csv";

    enum Table {
        Participant,
        TrialData,
        SongTable,
        Unknown
    };

    struct CSVLine {
        virtual std::string outputString() const = 0;
        virtual ~CSVLine() = default;
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
        static void writeHeaders(std::ofstream& file);
        static ParticipantLine parse(std::string& data);

        ParticipantLine(std::string id, std::string name, int height, int weight, std::string gender, std::string dominantHand, bool exp, std::string notes)
        : participantID(id), name(name), height(height), weight(weight), gender(gender), dominantHand(dominantHand), experience(exp), notes(notes) {}
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
        static void writeHeaders(std::ofstream& file);
        static TrialDataLine parse(std::string& data);

        TrialDataLine(std::string id, std::string pid, int attempt, int bpm, int force, int totalPunches, float accuracy, int maxForce, int minForce, int avgForce)
        : trialID(id), participantID(pid), attempt(attempt), bpm(bpm), force(force), totalPunches(totalPunches), 
        accuracy(accuracy), maxForce(maxForce), minForce(minForce), avgForce(avgForce) {}
    };

    struct SongTableLine : public CSVLine {
        std::string songID;
        std::string songName;
        std::string bpmRange;

        std::string outputString() const;
        static void writeHeaders(std::ofstream& file);
        static SongTableLine parse(std::string& data);

        SongTableLine(std::string id, std::string songName, std::string bpmRange)
        : songID(id), songName(songName), bpmRange(bpmRange) {}
    };

    void createFile(std::string& path);
    void appendToFile(Table t, CSVLine& line);
    
    Table parseTable(std::string& data);

public: 
    void saveData(std::string& data);
    void loadResult();
    Database() {}
};