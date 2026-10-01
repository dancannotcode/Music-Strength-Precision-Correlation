#include "music.cpp"


int main() {
    Music m = Music();
    std::string chosen_song = m.selectSong();
    std::cout << chosen_song << std::endl;
}