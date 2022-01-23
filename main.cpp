#include <iostream>
#include <string>
#include <curl/curl.h>
#include "rapidxml/rapidxml.hpp"
#include <filesystem>
#include <fstream>

using namespace std;
using namespace rapidxml;
namespace fs = std::filesystem;
xml_document<> doc;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

static string curlget(string url);

int main()
{
    fs::create_directory(fs::current_path() / "xkcd");
    ifstream file;
    file.open(fs::current_path() / "xkcd" / "last.dat");
    string lastrecordedxkcd;
    file >> lastrecordedxkcd;

    doc.parse<0>((char*)curlget("https://xkcd.com/atom.xml").c_str());
    xml_node<> * root_node = doc.first_node("feed");
    string lastxkcd = root_node->first_node("entry")->first_node("link")->first_attribute("href")->value();

    if (lastxkcd != lastrecordedxkcd){
        ofstream file2;
        file2.open(fs::current_path() / "xkcd" / "last.dat");
        file2 << lastxkcd;
        string xkpage = curlget(lastxkcd);
        int index = xkpage.find("g):");
        int index2 = xkpage.find('>',index);
        string xkurl = xkpage.substr(index + 14, (index2 - index) - 15);
        #ifdef _WIN32
            cout << "open " << xkurl << endl;
            return;
        #endif;
        cout << "notify-send \"New XKCD!\" && xdg-open " << xkurl << endl;
    }
    return 0;
}
static string curlget(string url){
    CURL *curl;
    CURLcode res;
    string readBuffer;

    curl = curl_easy_init();
    if (curl){
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    return readBuffer;
}

