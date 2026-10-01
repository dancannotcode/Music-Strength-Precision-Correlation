#include "database.cpp"

int main() {
    Database d = Database();
    std::string teststring1 = "T1, george, 62, 130, Male, R, No, “ ”";
    std::string teststring2 = "T1, P1, 1, 90, 100, 25, 5.1, 150, 50, 90";
    std::string teststring3 = "S1, “Expresso”, 90";

    d.saveData(teststring1);
    d.saveData(teststring2);
    d.saveData(teststring3);

    d.loadResult();
}