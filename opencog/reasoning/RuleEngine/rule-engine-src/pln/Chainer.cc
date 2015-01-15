/*
 * Chainer.cc
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
#include "Chainer.h"

Chainer::Chainer(AtomSpace * as) {
	_main_atom_space = as;
	_target_list_atom_space = as;
	//target_list_atom_space = new AtomSpace(); //xxx a rejected idea about using separate atomspace.maybe later.
}
void Chainer::set_htarget(Handle& h) {

}
Chainer::~Chainer() {
	//delete target_list_atom_space;
}

float Chainer::target_tv_fitness(Handle h) {
	TruthValuePtr ptv = _target_list_atom_space->getTV(h);
	confidence_t c = ptv->getConfidence();
	strength_t s = ptv->getMean();

	return (pow((1 - s), _ctv_fitnes) * (pow(c, (2 - _ctv_fitnes))));
}

