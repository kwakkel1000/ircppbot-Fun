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

#include <core/Global.h>
#include <core/Output.h>
#include <iostream>
#include <algorithm>
#include <cstdio>
#include <vector>
#include <string>


extern "C" ModuleInterface* create()
{
    return new Fun;
}

extern "C" void destroy(ModuleInterface* x)
{
    delete x;
}

Fun::Fun()
{
}

Fun::~Fun()
{
    stop();
    Global::Instance().get_IrcData().DelConsumer(mpDataInterface);
    delete mpDataInterface;
}
void Fun::Init(DataInterface* pData)
{
    mpDataInterface = pData;
    mpDataInterface->Init(true, false, false, true);
    Global::Instance().get_IrcData().AddConsumer(mpDataInterface);

    timerlong();
}


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


void Fun::ParseData(std::vector< std::string > data)
{
    ;
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
            sSendString = sSendString + char(3) + Output::Instance().StringFromInt(rand()%13+2) + "," + Output::Instance().StringFromInt(rand()%13+2) + sColorString[uiStringIndex];
        }
        Send(sSendString);
    }
}

void Fun::Sample()
{
    ;
}

void Fun::timerrun()
{
    int Tijd;
    time_t t= time(0);
    Tijd = t;
    iLongTime++;
    if (iLongTime >= 30)
    {
        std::cout << "timed" << std::endl;
        timerlong();
        iLongTime = 0;
    }
    for (int i = timer_sec.size() - 1; i >= 0; i--)
    {
        if (timer_sec[i] < Tijd)
        {
            std::cout << timer_command[i] << std::endl;
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

bool Fun::Send(std::string sData)
{
    Global::Instance().get_IrcData().AddSendQueue(sData);
    return true;
}


