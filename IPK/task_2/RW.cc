/* Projekt 1: Jednoducha trieda na podpru nacitania za zapisu pre VUT FIT predmet IPK
 * Autor: Matej Marusak, xmarus06@fit.vutbr.cz
 * 2015/2016
 */

#include <string>
#include <fstream>
#include "RW.hh"

RW::RW(){
    len = 0;
    pos = 0;
    content = "";
}

int RW::addInput(std::string fileName){
    std::ifstream fin(fileName, std::ios::in | std::ios::binary);
    if (!fin)
        return 1;
    while(!fin.eof())
        content.append(1, fin.get());
    len = content.length();
    fin.close();
    return 0;
}

int RW::readNext(int amount, std::string &buff){
    buff.clear();
    if (pos > content.length())
        return 0;
    if (pos +  amount > len -1)
        amount = len - pos -1;
    buff = content.substr(pos, amount);
    pos += amount;
    return amount;
}

void RW::addNext(const char *in, int amount){
    content.append(in, amount);
    len += amount;
}

int RW::writeToFile(std::string fileName){
    std::ofstream outF(fileName, std::ofstream::out | std::ofstream::binary);
    if (!outF)
        return 1;
    outF.write(content.c_str(), len);
    outF.close();
    return 0;
}

std::string RW::splitHead(){
    size_t p= content.find("\n");
    std::string head = content.substr(0, p);
    content = content.substr(p+1, len);
    len = len - p -1;
    return head;
}
