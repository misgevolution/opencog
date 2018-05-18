/*
 (_logger_agent)-> Requirements
 * -------------
 * -1 Distribution of residence-duration of nlp-parse atoms
 * - Percentage of atoms in AF directly from nlp-parse //XXX From scheme or C++?

 * -2 Distribution of residence-duration in the AF 
 * -3 Log af size every few ms
 * - Log percentage of sti gain from spreading and direct stimulation
 * - Percentage of the AF consisting of words from the insect corpus vs poison
 *   right after the transition from one corpus to the other
 * /


 
 * Design
 * --------
 *  struct AtomStat { bool from_sentence; Handle h; update last_update time;} 
 *  Dump Log at time t (using a command from the shell)
 *
 */

/*
 * LoggerAgent.h
 *
 */

#ifndef _LOGGERAGENT_H_
#define _LOGGERAGENT_H_

#include <opencog/guile/SchemeEval.h>
#include <opencog/cogserver/server/Agent.h>
#include <opencog/cogserver/server/CogServer.h>
#include <opencog/nlp/types/atom_types.h>
#include <opencog/attentionbank/AttentionBank.h>

#include <unordered_set>
#include <chrono>

using namespace std::chrono;

namespace opencog {

    class LoggerAgent: public Agent {
        private:
            SchemeEval * _scm_eval;
            AtomSpace * _as;
            AttentionBank * _bank;
            time_point<system_clock> _start;
            std::map<Handle, AttentionValue::sti_t> _stimulus_rec;
            // FIXME This should be read from a file. But for now we should do
            // it asap.
            const std::vector<std::string> insecticides = {
                "abamectin",
                "acetamiprid",
                "alachlor",
                "aldicarb",
                "aldrin",
                "bactericide",
                "benazolin",
                "bendiocarb",
                "bromacil",
                "bulan",
                "carbaryl",
                "diazinon",
                "dichloropropene",
                "dichlorvos",
                "dicofol",
                "dilan",
                "dinoseb",
                "ethiofencarb",
                "ethion",
                "fenson",
                "fluvalinate",
                "fosmethilan",
                "herbicide",
                "hydramethylnon",
                "insecticide",
                "lindane",
                "lufenuron",
                "methidathion",
                "nematocide",
                "trichlorophenol"
            };

        public:
            virtual ~LoggerAgent();
            LoggerAgent(CogServer& cs);

            virtual const ClassInfo& classinfo() const;
            static const ClassInfo& info();

            struct AtomStat{
                Handle h;
                time_point<system_clock> entered_at; //duration of stay in sec.
                duration<double> dr; //duration of stay in sec.
                time_point<system_clock> last_active;
                bool is_nlp_parse_output = false;
                //float psti_gain_via_heblink;
                //float psti_gain_via_link;
            };

            struct atomstat_hash : public std::unary_function<AtomStat, size_t>{
                size_t operator()(const AtomStat& atomstat) const
                {   
                    return hash_value(atomstat.h);
                }   

            };

            float nlp_parse_percentage_in_af;
            float prev_topic_percentage, current_topic_percentage;
            //std::unordered_set<AtomStat, atomstat_hash> atomstat_set;
            std::map<Handle,AtomStat> handle_atomstat_map;
            struct AFSizeStat
            {
                time_point<system_clock> at_time;
                long int af_size;
                float nlp_parse_percentage;
            };
            std::vector<AFSizeStat> af_size_stat;
            std::vector<std::vector<float>> insect_poison_percentage;
            time_point<system_clock> last_probing_time; //duration of stay in sec.
            
            HandleSeq insect_atoms;
            HandleSeq poison_atoms;
            HandleSeq insecticide_atoms;
            static bool topic_changed; 

            // This is guarenteed to function properly only when you load the
            // agent before starting nlp parse and you won't be loading any more
            // from other sources after starting it. i.e, load external sources first,
            // start this agent, do nlp-parsing and make sure not to add any more word nodes
            // from other sources after these steps.
            void atomAddedListener(const Handle& h);
            void run(void);
    };

}

#endif /* _SENTENCEGENAGENT_H_ */