#include <string>

class Music {

    int readAndUpdateLastPicked();
    std::string randomSongFromFolder(int folder);

    public:
        std::string selectSong();
};