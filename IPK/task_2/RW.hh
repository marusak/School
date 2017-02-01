#include <string>
#include <fstream>


class RW{
public:
    RW();
    int addInput(std::string fileName);
    int readNext(int amount, std::string &buff);
    void writeNext(std::string in);
    void addNext(const char *in, int amount);
    int writeToFile(std::string fileName);
    std::string splitHead();
private:
    std::string content;
    unsigned len;
    unsigned pos;
};
