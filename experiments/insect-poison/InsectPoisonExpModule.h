/*
 * PLNDynamicsExpSetupModule.h
 *
 *  Created on: Apr 19, 2015
 *      Author: misgana
 */

#ifndef PLNDYNAMICSEXPSETUPMODULE_H_
#define PLNDYNAMICSEXPSETUPMODULE_H_

#include "LoggerAgent.h"

#include <opencog/cogserver/server/Agent.h>
#include <opencog/cogserver/server/Factory.h>
#include <opencog/attention/AttentionModule.h>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace opencog {

class AtomSpace;
class CogServer;
class SchemeEval;
class Module;

//namespace ECANExperiment {

//extern int current_group;
using namespace std::chrono;

class InsectPoisonExpModule: public Module {
private:
    AgentPtr _logger_agentptr;
    LoggerAgent* _logger_agent;

    Factory<LoggerAgent, Agent> loggerAgentFactory;

    AtomSpace * _as;
    CogServer& _cs;


    DECLARE_CMD_REQUEST(InsectPoisonExpModule, "dump-af-size-log",do_dump_af_size,
            "Dumps AF size stastics to a file",
            "Usage: dump-af-size-log <FILE_NAME>\n",
            false, true)

    //Load word dict.
    DECLARE_CMD_REQUEST(InsectPoisonExpModule, "dump-af-stat",do_dump_af_stat,
            "Dumps AF stastics to a file",
            "Usage: dump-af-stat <FILE_NAME>\n",
            false, true)

    DECLARE_CMD_REQUEST(InsectPoisonExpModule, "topic-switched",do_set_topic_switched,
            "A flag to announce topic has been changed.This is used by loggerAgent"
            " to do some statistics about the AF right after topic switching.\n",
            "Usage: topic-switched <1|0>\n",
            false, true)

    DECLARE_CMD_REQUEST(InsectPoisonExpModule, "start-logger",do_start_logger,
            "Starts logger agent",
            "Usage: start-logger\n",
            false, true)

    void registerAgentRequests();
    void unregisterAgentRequests();

    std::string print_timept(time_point<system_clock> tpt){
        std::time_t t = system_clock::to_time_t(tpt);
        std::stringstream ss;
        std::tm tm = *std::localtime(&t);
        ss << std::put_time(&tm, "%H:%M:%S");
        return ss.str();
    };


public:
    InsectPoisonExpModule(CogServer&);
    virtual ~InsectPoisonExpModule();
    static inline const char* id();
    virtual void init(void);
};
//}

} /* namespace opencog */

#endif /* EXPERIMENTSETUPMODULE_H_ */
