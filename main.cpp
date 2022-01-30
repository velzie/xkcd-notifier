#include "rapidxml/rapidxml.hpp"
#include <chrono>
#include <curl/curl.h>
#include <filesystem>
#include <fstream>
#include <glib.h>
#include <iostream>
#include <libnotify/notify.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>

#ifdef _WIN32
#include "DesktopNotificationManagerCompat.h"
#include <NotificationActivationCallback.h>
#include <windows.ui.notifications.h>
using namespace ABI::Windows::Data::Xml::Dom;
using namespace ABI::Windows::UI::Notifications;
using namespace Microsoft::WRL;
#endif

using namespace std;
using namespace rapidxml;
namespace fs = std::filesystem;
xml_document<> doc;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                            void *userp) {
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

static string curlget(string url);
static void sendNotif(string title, string desc, string desc2);

void run() {
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 60 * 10));
    std::cout << "checking xkcds" << std::endl;

    ifstream file;
    file.open(fs::current_path() / "xkcd" / "last.dat");
    string lastrecordedxkcd;
    file >> lastrecordedxkcd;

    doc.parse<0>((char *)curlget("https://xkcd.com/atom.xml").c_str());
    xml_node<> *root_node = doc.first_node("feed");
    string lastxkcd = root_node->first_node("entry")
                          ->first_node("link")
                          ->first_attribute("href")
                          ->value();

    if (lastxkcd != lastrecordedxkcd) {
      ofstream file2;
      file2.open(fs::current_path() / "xkcd" / "last.dat");
      file2 << lastxkcd;
      string xkpage = curlget(lastxkcd);
      int index = xkpage.find("g):");
      int index2 = xkpage.find('>', index);
      string xkurl = xkpage.substr(index + 14, (index2 - index) - 15);
#ifdef _WIN32
      system(("open " + xkurl).c_str());
#endif
      sendNotif("New XKCD!", "New XKCD!", xkurl);
      system(("xdg-open " + xkurl).c_str());
    }
  }
}

int main() {
  fs::create_directory(fs::current_path() / "xkcd");
  thread t(run);
  t.join();
  return 0;
}
static void sendNotif(string title, string desc, string desc2) {
  notify_init(title.c_str());
  NotifyNotification *n =
      notify_notification_new(desc.c_str(), desc2.c_str(), 0);
  notify_notification_set_timeout(n, 10000);
  if (!notify_notification_show(n, 0)) {
    std::cerr << "show has failed" << std::endl;
  }
}
static string curlget(string url) {
  CURL *curl;
  CURLcode res;
  string readBuffer;

  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
  }
  return readBuffer;
}
