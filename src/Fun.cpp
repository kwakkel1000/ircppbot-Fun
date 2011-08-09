//
//
//  @ Project : ircppbot Fun
//  @ File Name : Fun.cpp
//  @ Date : 4/18/2011
//  @ Author : Gijs Kwakkel
//
//
// Copyright (c) 2011 Gijs Kwakkel
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//


#include "include/Fun.h"

#include <unistd.h>
#include <boost/algorithm/string.hpp>

#include <core/BotLib.h>
#include <core/Global.h>
#include <core/Output.h>
#include <core/DatabaseData.h>
#include <iostream>
#include <algorithm>
#include <cstdio>
#include <vector>
#include <string>

/**
 * create
 * Return a instance of this class
 * @return New instance of this class
 *
 */
extern "C" ModuleInterface* create()
{
    return new Fun;
}

/**
 * destroy
 * Destory the given object of this class
 * @param x the object to be deleted        to be renamed later
 *
 */
extern "C" void destroy(ModuleInterface* x)
{
    delete x;
}

/**
 * Constructor
 * Create ourself
 *
 */
Fun::Fun()
{
}

/**
 * Deconstructor
 * Call some functions to remove ourself from the plain of existance
 * destroy some vars
 * Delete ourself
 *
 */
Fun::~Fun()
{
    stop();
    Global::Instance().get_IrcData().DelConsumer(mpDataInterface);
    delete mpDataInterface;
}

/**
 * Init
 * Initialise some vars
 * @param DataInterface pointer mpData
 *
 */
void Fun::Init(DataInterface* mpData)
{
    srand ( time(NULL) );
    mpDataInterface = mpData;
    mpDataInterface->Init(true, false, false, true);
    Global::Instance().get_IrcData().AddConsumer(mpDataInterface);
    uiTimeConfig =  BotLib::IntFromString(Global::Instance().get_ConfigReader().GetString("FunTimeConfig"));
    uiMininumTimeConfig = BotLib::IntFromString(Global::Instance().get_ConfigReader().GetString("FunMinimumTimeConfig"));
    sQuotesTable = Global::Instance().get_ConfigReader().GetString("FunQuotesTable");
    vQuotes = DatabaseData::Instance().GetData(sQuotesTable, "quote", "1 = 1");
    int Tijd;
    time_t t= time(0);
    Tijd = t;
    timer_long_sec.push_back(Tijd + 60);
    timer_long_command.push_back("quotes");

    timerlong();
}

/**
 * stop
 * Return a instance of this class
 * @return New instance of this class
 *
 */
void Fun::stop()
{
    bRun = false;
    mpDataInterface->stop();
    std::cout << "Fun::stop" << std::endl;
    raw_parse_thread->join();
    std::cout << "raw_parse_thread stopped" << std::endl;
    privmsg_parse_thread->join();
    std::cout << "privmsg_parse_thread stopped" << std::endl;
}

void Fun::read()
{
    bRun = true;
    assert(!raw_parse_thread);
    raw_parse_thread = boost::shared_ptr<boost::thread>(
                    new boost::thread(
                    boost::bind(
                    &Fun::parse_raw, this)));
    assert(!privmsg_parse_thread);
    privmsg_parse_thread = boost::shared_ptr<boost::thread>(
                    new boost::thread(
                    boost::bind(
                    &Fun::parse_privmsg, this)));
}

void Fun::parse_raw()
{
    std::vector< std::string > data;
    while(bRun)
    {
        data = mpDataInterface->GetRawQueue();
        ParseData(data);
    }
}

void Fun::parse_privmsg()
{
    std::vector< std::string > data;
    while(bRun)
    {
        data = mpDataInterface->GetPrivmsgQueue();
        ParsePrivmsg(data);
    }
}

void Fun::INVITE(std::vector< std::string > data)
{
    std::string returnstr = "JOIN " + data[3] + "\r\n";
    Send(returnstr);
}

void Fun::ParseData(std::vector< std::string > data)
{
    if (data.size() >= 4)
    {
        if (data[1] == "INVITE")   //INVITE
        {
            INVITE(data);
        }
    }
}


void Fun::ParsePrivmsg(std::vector< std::string > vData)
{
    if (boost::iequals(vData[3], ":rainbow"))
    {
        /*Send("PRIVMSG " + vData[2] + " :" + char(1) + "1");
        Send("PRIVMSG " + vData[2] + " :" + char(2) + "2");
        Send("PRIVMSG " + vData[2] + " :" + char(3) + "3");
        Send("PRIVMSG " + vData[2] + " :" + char(15) + "15");
        Send("PRIVMSG " + vData[2] + " :" + char(16) + "16");
        Send("PRIVMSG " + vData[2] + " :" + char(22) + "22");
        Send("PRIVMSG " + vData[2] + " :" + char(31) + "31");

        Send("PRIVMSG " + vData[2] + " :" + char(3) + "02blue on nothing");
        Send("PRIVMSG " + vData[2] + " :" + char(3) + "03,00green on white");
        Send("PRIVMSG " + vData[2] + " :" + char(3) + "08,14yellow on grey");
        Send("PRIVMSG " + vData[2] + " :" + char(3) + Output::Instance().StringFromInt(rand()%15) + "random on nothing");
        Send("PRIVMSG " + vData[2] + " :" + char(3) + Output::Instance().StringFromInt(rand()%15) + "," + Output::Instance().StringFromInt(rand()%15) + "random on random");*/
        std::string sColorString = "";
        std::string sSendString = "PRIVMSG " + vData[2] + " :";
        for (unsigned int uiVectorIndex = 4; uiVectorIndex < vData.size(); uiVectorIndex++)
        {
            sColorString = sColorString + " " + vData[uiVectorIndex];
        }
        for (unsigned int uiStringIndex = 1; uiStringIndex < sColorString.size(); uiStringIndex++)
        {
            unsigned int rand1 = rand()%13+2;
            unsigned int rand2 = rand()%13+2;
            while (rand1 == rand2)
            {
                rand2 = rand()%13+2;
            }
            sSendString = sSendString + char(3) + Output::Instance().StringFromInt(rand1) + "," + Output::Instance().StringFromInt(rand2) + sColorString[uiStringIndex];
        }
        Send(sSendString);
    }

    if (boost::iequals(vData[3], ":rainbow"))
    {
    }
}

void Fun::Sample()
{
    ;
}

void Fun::Quote()
{
    ChannelsInterface& C = Global::Instance().get_Channels();
    if (C.GetChannels().size() > 0)
    {
        unsigned int _uiRandQuote = rand()%vQuotes.size();
        unsigned int _uiRandChannel = rand()%C.GetChannels().size();
        std::vector< std::string > _vChannels = C.GetChannels();
        std::string _sRandomChannel = _vChannels[_uiRandChannel];
        std::string _sRandomQUote = "[" + BotLib::StringFromInt(_uiRandQuote + 1)+ "]" + vQuotes[_uiRandQuote];
        Send(Global::Instance().get_Reply().irc_privmsg(_sRandomChannel, _sRandomQUote));
    }
}

void Fun::timerrun()
{
    int Tijd;
    time_t t= time(0);
    Tijd = t;
    iLongTime++;
    if (iLongTime >= 30)
    {
        // std::cout << "timed" << std::endl;  // annoying
        timerlong();
        iLongTime = 0;
    }
    for (int i = timer_sec.size() - 1; i >= 0; i--)
    {
        if (timer_sec[i] < Tijd)
        {
            std::cout << timer_command[i] << std::endl;
            ParseTimedCommand(timer_command[i], timer_sec[i]);
            timer_sec.erase(timer_sec.begin()+i);
            timer_command.erase(timer_command.begin()+i);
        }
    }
}

void Fun::timerlong()
{
    int Tijd;
    time_t t= time(0);
    Tijd = t;
    Tijd = Tijd + 100;
    for (int i = timer_long_sec.size() - 1; i >= 0; i--)
    {
        if (timer_long_sec[i] < Tijd)
        {
            std::cout << "timer_long to timer " << timer_long_command[i] << std::endl;
            timer_sec.push_back(timer_long_sec[i]);
            timer_command.push_back(timer_long_command[i]);
            timer_long_sec.erase(timer_long_sec.begin()+i);
            timer_long_command.erase(timer_long_command.begin()+i);
        }
    }
}

void Fun::ParseTimedCommand(std::string msCommand, int miTime)
{
    if (msCommand == "quotes")    // splitting into [time] and command where [time] is optional and time is a INT
    {
        unsigned int _uiNextTime = (unsigned int)abs(uiTimeConfig / (Global::Instance().get_Channels().GetChannels().size() + 1));
        if (_uiNextTime < uiMininumTimeConfig)
        {
            _uiNextTime = uiMininumTimeConfig;
        }
        timer_long_sec.push_back(miTime + _uiNextTime);
        timer_long_command.push_back("quotes");
        Quote();
    }
}

bool Fun::Send(std::string sData)
{
    Global::Instance().get_IrcData().AddSendQueue(sData);
    return true;
}


