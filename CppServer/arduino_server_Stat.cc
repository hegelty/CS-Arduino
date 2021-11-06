#include <fstream>
#include "arduino_server_Stat.h"
using namespace arduino::server;
//add definition of your processing function here

int stat = 0;
//0:없음 1:이메일만 2:이메일,LED 3:이메일,소리 4:셋다

std::string htmlStr;
bool redirect_loaded = false;
void loadHtml()
{
    std::ifstream redirect_html("redirect.html");
    if(redirect_html.is_open()) {
        redirect_html.seekg(0, std::ios::end);
        int size = redirect_html.tellg();
        htmlStr.resize(size);
        redirect_html.seekg(0, std::ios::beg);
        redirect_html.read(&htmlStr[0],size);
        redirect_loaded = true;
    }
    redirect_html.close();
}

void sendEmail(std::string title, std::string content)
{
    std::string curl = "curl --request POST   --url https://api.sendgrid.com/v3/mail/send   --header \"Authorization: \"   --header 'Content-Type: application/json'   --data '{\"personalizations\": [{\"to\": [{\"email\": \"skxodid0305@gmail.com\"}]}],\"from\": {\"email\": \"gbs.s210023@ggh.goe.go.kr\"},\"subject\": \""+title+"\",\"content\": [{\"type\": \"text/plain\", \"value\": \""+content+"\"}]}'";
    system(curl.c_str());
}

void writeLog(std::string str)
{
    time_t timer = time(NULL);
    struct tm* t = localtime(&timer);
    FILE *logFile = fopen("LogFile","a");
    fprintf(logFile,"%04d-%02d-%02d %02d:%02d:%02d %s(mode : %d)\n",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec, str.c_str(), stat);
    fclose(logFile);
}

void Stat::open(const HttpRequestPtr &req,
                std::function<void (const HttpResponsePtr &)> &&callback)
{
    writeLog("Open");
    if(stat) sendEmail("누군가가 당신의 간식을 훔치려 하고 있습니다.", "설정 : http://52.141.61.231:8000/arduino/server/Stat/page");
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k200OK);
    resp->setContentTypeCode(drogon::CT_TEXT_PLAIN);
    resp->setBody(std::to_string(stat));
    callback(resp);
}

void Stat::show(const HttpRequestPtr &req,
                std::function<void (const HttpResponsePtr &)> &&callback)
{
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k200OK);
    resp->setContentTypeCode(drogon::CT_TEXT_PLAIN);
    resp->setBody(std::to_string(stat));
    callback(resp);
}

void Stat::close(const HttpRequestPtr &req,
                std::function<void (const HttpResponsePtr &)> &&callback)
{
    writeLog("Close");
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k200OK);
    resp->setContentTypeCode(drogon::CT_TEXT_PLAIN);
    resp->setBody(std::to_string(stat));
    callback(resp);
}

void Stat::page(const HttpRequestPtr &req,
                std::function<void (const HttpResponsePtr &)> &&callback)
{
    std::string indexStr;
    std::ifstream index_html("index.html");
    if(index_html.is_open()) {
        index_html.seekg(0, std::ios::end);
        int size = index_html.tellg();
        indexStr.resize(size);
        index_html.seekg(0, std::ios::beg);
        index_html.read(&indexStr[0],size);
    }
    index_html.close();
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k200OK);
    resp->setContentTypeCode(drogon::CT_TEXT_HTML);
    resp->setBody(indexStr);
    callback(resp);
}

void Stat::set(const HttpRequestPtr &req,
               std::function<void (const HttpResponsePtr &)> &&callback,
               std::string statnum) const
{
    try
    {
        stat = std::stoi(statnum);
        writeLog("set");
    } catch(int expn){
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k200OK);
        resp->setContentTypeCode(drogon::CT_TEXT_PLAIN);
        resp->setBody("error");
        callback(resp);
    }
    if(!redirect_loaded) loadHtml();
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k200OK);
    resp->setContentTypeCode(drogon::CT_TEXT_HTML);
    resp->setBody(htmlStr);
    callback(resp);
}

void Stat::showlog(const HttpRequestPtr &req,
                   std::function<void (const HttpResponsePtr &)> &&callback)
{
    std::ifstream logFile("LogFile");
    std::string logStr;
    if(logFile.is_open()) {
        logFile.seekg(0, std::ios::end);
        int size = logFile.tellg();
        logStr.resize(size);
        logFile.seekg(0, std::ios::beg);
        logFile.read(&logStr[0],size);
    }
    logFile.close();
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k200OK);
    resp->setContentTypeCode(drogon::CT_TEXT_PLAIN);
    resp->setBody(logStr);
    callback(resp);
}

void Stat::clearlog(const HttpRequestPtr &req,
                    std::function<void (const HttpResponsePtr &)> &&callback)
{
    time_t timer = time(NULL);
    struct tm* t = localtime(&timer);
    FILE *logFile = fopen("LogFile","w");
    fprintf(logFile,"%04d-%02d-%02d %02d:%02d:%02d Log Cleared\n",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec);
    fclose(logFile);

    if(!redirect_loaded) loadHtml();
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k200OK);
    resp->setContentTypeCode(drogon::CT_TEXT_HTML);
    resp->setBody(htmlStr);
    callback(resp);
}