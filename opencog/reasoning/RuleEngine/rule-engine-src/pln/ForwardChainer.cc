/*
 * ForwardChainer.cc
 *
 * Copyright (C) 2014 Misgana Bayetta
 *
 * Author: Misgana Bayetta <misgana.bayetta@gmail.com>  Sept 2014
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include "ForwardChainer.h"

#include <opencog/reasoning/RuleEngine/rule-engine-src/Rule.h>
#include <opencog/reasoning/RuleEngine/rule-engine-src/JsonicControlPolicyLoader.h>

using namespace opencog;

ForwardChainer::ForwardChainer(AtomSpace * as) :
		Chainer(as) {
	hcurrent_choosen_rule_ = Handle::UNDEFINED;
	commons_ = new PLNCommons(as);
	fcim_ = new ForwardChainInputMatchCB(main_atom_space,
			target_list_atom_space, this); //fetching from main and copying it to target_list_atom_space
	fcpm_ = new ForwardChainPatternMatchCB(target_list_atom_space, this); // chaining PLN rules are applied on the target_list_atom_space

	init();
}

ForwardChainer::~ForwardChainer() {
	delete cpolicy_loader_;
	delete commons_;
	delete scm_eval_;
	delete fcim_;
	delete fcpm_;
}

void ForwardChainer::init(void) {
	cpolicy_loader_ = new JsonicControlPolicyLoader(main_atom_space, conf_path); //new ControlPolicyLoader(main_atom_space, conf_path);
	search_in_af = cpolicy_loader_->get_attention_alloc();
	rules_ = cpolicy_loader_->get_rules();
}

Handle ForwardChainer::tournament_select(map<Handle, float> hfitnes_map) {
	if (hfitnes_map.size() == 1) {
		return hfitnes_map.begin()->first;
	}

	map<Handle, float> winners;
	int size = hfitnes_map.size() / 2; //TODO change the way tournament size is calculated
	for (auto i = 0; i < size; i++) {
		int index = (random() % hfitnes_map.size());
		auto it = hfitnes_map.begin();
		advance(it, index);
		winners[it->first] = it->second;
	}
	auto it = winners.begin();
	Handle hbest = it->first;
	float max = it->second;
	for (; it != winners.end(); ++it) {
		if (it->second > max) {
			hbest = it->first;
			max = it->second;
		}
	}
	return hbest;
}

Handle ForwardChainer::choose_target_from_list(HandleSeq hs_list) {
	map<Handle, float> tournament_elem;
	for (Handle h : hs_list) {
		float fitness = target_tv_fitness(h);
		tournament_elem[h] = fitness;
	}
	return tournament_select(tournament_elem);
}

Handle ForwardChainer::choose_target_from_atomspace(AtomSpace * as) {
	HandleSeq hs;
	as->getHandlesByType(back_inserter(hs), ATOM, true); //xxx experimental must be replaced by atoms in AF
	for (Handle h : hs)
		add_to_target_list(h);
	return choose_target_from_list(target_list_);
}

void ForwardChainer::do_chain(Handle htarget) {
	Handle hcurrent_target;
	int steps = 0;
	auto max_iter = cpolicy_loader_->get_max_iter();
	//bool terminate = false;
	while (steps <= max_iter /*or !terminate*/) {
		if (steps == 0) {
			if (htarget == Handle::UNDEFINED)
				hcurrent_target = choose_target_from_atomspace(main_atom_space); //start FC on a random target
			else
				hcurrent_target = htarget;
		} else {
			if (!target_list_.empty())
				hcurrent_target = choose_target_from_list(target_list_);
		}
		choose_input(hcurrent_target); //add more premise via pattern matching of related atoms to hcurrent_target
		choose_rule(); //TODO use some fitness function instead of randomly selecting
		chaining_pm.do_bindlink(hcurrent_choosen_rule_, *fcpm_); //xxx guide matching to search only the target list
		steps++;
		//TODO implement termination criteria
	}

}

void ForwardChainer::choose_input(Handle htarget) {
	if (NodeCast(htarget)) {
		HandleSeq hs = main_atom_space->getIncoming(htarget);
		for (Handle h : hs)
			add_to_target_list(h); //add to potential target list
	}
	if (LinkCast(htarget)) {
		map<Handle, string> hnode_vname_map = choose_variable(htarget);
		Handle implicant = target_to_pmimplicant(htarget, hnode_vname_map);
		Handle bindLink = commons_->create_bindLink(implicant);
		//match all in main_atom_space using the above bindLink and add them to target list
		chaining_pm.do_bindlink(bindLink, *fcim_); //result is added to target_list in fcim_'s grounding call back handler
	}
}

map<Handle, string> ForwardChainer::choose_variable(Handle htarget) {
	vector<Handle> candidates = commons_->get_nodes(htarget, vector<Type>());
	map<Handle, HandleSeq> node_iset_map;

	//xxx don't choose two or more nodes linked by one Link( i.e choose only one whenever
	//there are more than one node linked by the same link)
	for (auto it = candidates.begin(); it != candidates.end(); ++it) {
		HandleSeq hs = main_atom_space->getIncoming(*it);
		if (distance(candidates.begin(), it) == 0) {
			node_iset_map[*it] = hs;
		} else {
			bool has_same_link = false;
			for (auto i = node_iset_map.begin(); i != node_iset_map.end();
					++i) {
				HandleSeq tmp;
				sort(hs.begin(), hs.end());
				sort(i->second.begin(), i->second.end());
				set_intersection(hs.begin(), hs.end(), i->second.begin(),
						i->second.end(), back_inserter((tmp)));
				if (tmp.size() > 0) {
					has_same_link = true;
					break;
				}
			}
			if (!has_same_link)
				node_iset_map[*it] = hs;
		}
	}

	map<Handle, string> hnode_vname_map;
	for (auto it = node_iset_map.begin(); it != node_iset_map.end(); ++it) {
		Handle h = it->first;
		hnode_vname_map[h] = ("$var-" + NodeCast((h))->getName());
	}
	return hnode_vname_map;
}

Handle ForwardChainer::target_to_pmimplicant(Handle htarget,
		map<Handle, string> hnode_vname_map) {
	Type link_type;
	HandleSeq hsvariablized;

	if (LinkCast(htarget)) {
		LinkPtr p_htarget = LinkCast(htarget);
		link_type = p_htarget->getType();
		HandleSeq hsoutgoing = main_atom_space->getOutgoing(htarget);
		for (auto i = hsoutgoing.begin(); i != hsoutgoing.end(); ++i) {
			Handle htmp = target_to_pmimplicant(*i, hnode_vname_map);
			hsvariablized.push_back(htmp);
		}
		return main_atom_space->addLink(link_type, hsvariablized,
				TruthValue::TRUE_TV());
	} else {
		if (NodeCast(htarget)) {
			auto it_var = hnode_vname_map.find(htarget); //TODO replace by find-if for linear complexity
			NodePtr p_htarget = NodeCast(htarget);
			if (it_var != hnode_vname_map.end())
				return main_atom_space->addNode(VARIABLE_NODE, it_var->second,
						TruthValue::TRUE_TV());
			else
				return htarget;
		}
	}
	return Handle::UNDEFINED; //unreachable?
}

void ForwardChainer::choose_rule() {
	//TODO choose rule via stochastic selection, HOW?
	Rule *choosen_rule = rules_[random() % rules_.size()];
	auto mset = choosen_rule->get_mutex_rules();
	/*for(vector<Rule*> mset: mutexe_rules){

	 }*/
	hcurrent_choosen_rule_ = choosen_rule->get_rule_handle();
}

void ForwardChainer::add_to_target_list(Handle h) {
	bool found = (find(target_list_.begin(), target_list_.end(), h)
			!= target_list_.end());
	if (NodeCast(h)) {
		if (found)
			target_list_.push_back(h);
	}
	if (LinkCast(h)) {
		if (found)
			target_list_.push_back(h);
		HandleSeq hs = main_atom_space->getOutgoing(h);
		for (Handle hi : hs)
			if (find(target_list_.begin(), target_list_.end(), hi)
					== target_list_.end())
				add_to_target_list(hi);
	}
}

HandleSeq ForwardChainer::get_chaining_result(void) {
	return chaining_results;
}

bool ForwardChainer::exists(HandleSeq& hseq, Handle& h) {
	for (Handle hi : hseq) {
		if (hi.value() == h.value())
			return true;
	}
	return false;
}

bool ForwardChainer::is_in_target_list(Handle h) {
	return exists(target_list_, h);
}
