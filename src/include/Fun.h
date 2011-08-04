#ifndef FUN_H
#define FUN_H
#include <interfaces/ModuleInterface.h>
#include <interfaces/DataInterface.h>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>

class DataInterface;
class Fun : public ModuleInterface
{
public:
    Fun();
    ~Fun();
    void read();
    void stop();
    void Init(DataInterface* pData);
    void timerrun();

private:
    //vars

    bool bRun;
    std::vector< std::string > vQuotes;

    //classes
    DataInterface* mpDataInterface;

    //timer vars
    std::vector<int> timer_sec;
    std::vector< std::string > timer_command;
    std::vector<int> timer_long_sec;
    std::vector< std::string > timer_long_command;
    int iLongTime;

    //thread vars
    boost::shared_ptr<boost::thread> raw_parse_thread;
    boost::shared_ptr<boost::thread> privmsg_parse_thread;

    //functions
    bool Send(std::string sData);
    //parse functions
    void parse_raw();
    void parse_privmsg();
    void ParseData(std::vector< std::string > vData);
    void ParsePrivmsg(std::vector< std::string > vData);

    void INVITE(std::vector< std::string > data);

    //post parse functions
    void Sample();
    void Quote();

    //timer
    void timerlong();
    void ParseTimedCommand(std::string msCommand, int miTime);
};

#endif // FUN_H


