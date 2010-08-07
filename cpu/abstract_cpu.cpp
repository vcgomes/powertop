#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"

void abstract_cpu::measurement_start(void)
{
	unsigned int i;

	for (i = 0; i < cstates.size(); i++)
		delete cstates[i];
	cstates.resize(0);

	for (i = 0; i < pstates.size(); i++)
		delete pstates[i];
	pstates.resize(0);

	for (i = 0; i < children.size(); i++)
		if (children[i])
			children[i]->measurement_start();

	gettimeofday(&stamp_before, NULL);
}

void abstract_cpu::measurement_end(void)
{
	unsigned int i, j;

	gettimeofday(&stamp_after, NULL);

	time_factor = 1000000.0 * (stamp_after.tv_sec - stamp_before.tv_sec) + stamp_after.tv_usec - stamp_before.tv_usec;


	for (i = 0; i < children.size(); i++)
		if (children[i])
			children[i]->measurement_end();

	for (i = 0; i < children.size(); i++)
		if (children[i]) {
			for (j = 0; j < children[i]->cstates.size(); j++) {
				struct idle_state *state;
				state = children[i]->cstates[j];
				if (!state)
					continue;

				update_cstate( state->linux_name, state->human_name, state->usage_before, state->duration_before, state->before_count);
				finalize_cstate(state->linux_name,                   state->usage_after,  state->duration_after,  state->after_count);
			}
			for (j = 0; j < children[i]->pstates.size(); j++) {
				struct frequency *state;
				state = children[i]->pstates[j];
				if (!state)
					continue;

				update_pstate(  state->freq, state->human_name, state->time_before, state->before_count);
				finalize_pstate(state->freq,                    state->time_after,  state->after_count);
			}
		}


	for (i = 0; i < cstates.size(); i++) {
		struct idle_state *state = cstates[i];

		if (state->after_count == 0) {
			cout << "after count is 0 " << state->linux_name << "\n";
			continue;
		}

		if (state->after_count != state->before_count) {
			cout << "count mismatch " << state->after_count << " " << state->before_count << " on cpu " << number << "\n";
			continue;
		}

		state->usage_delta =    (state->usage_after    - state->usage_before)    / state->after_count;
		state->duration_delta = (state->duration_after - state->duration_before) / state->after_count;
	}
}

void abstract_cpu::insert_cstate(const char *linux_name, const char *human_name, uint64_t usage, uint64_t duration, int count)
{
	struct idle_state *state;
	const char *c;

	state = new struct idle_state;

	if (!state)
		return;

	memset(state, 0, sizeof(*state));

	cstates.push_back(state);

	strcpy(state->linux_name, linux_name);
	strcpy(state->human_name, human_name);

	c = human_name;
	while (*c) {
		if (strcmp(linux_name, "active")==0) {
			state->line_level = LEVEL_C0;
			break;
		}
		if (*c >= '0' && *c <='9') {
			state->line_level = strtoull(c, NULL, 10);
			break;
		}
		c++;
	}

	state->usage_before = usage;
	state->duration_before = duration;
	state->before_count = count;
}

void abstract_cpu::finalize_cstate(const char *linux_name, uint64_t usage, uint64_t duration, int count)
{
 	unsigned int i;
	struct idle_state *state = NULL;

	for (i = 0; i < cstates.size(); i++) {
		if (strcmp(linux_name, cstates[i]->linux_name) == 0) {
			state = cstates[i];
			break;
		}
	}

	if (!state) {
		cout << "Invalid C state finalize " << linux_name << " \n";
		return;
	}

	state->usage_after += usage;
	state->duration_after += duration;
	state->after_count += count;
}

void abstract_cpu::update_cstate(const char *linux_name, const char *human_name, uint64_t usage, uint64_t duration, int count)
{
 	unsigned int i;
	struct idle_state *state = NULL;

	for (i = 0; i < cstates.size(); i++) {
		if (strcmp(linux_name, cstates[i]->linux_name) == 0) {
			state = cstates[i];
			break;
		}
	}

	if (!state) {
		insert_cstate(linux_name, human_name, usage, duration, count);
		return;
	}

	state->usage_before += usage;
	state->duration_before += duration;
	state->before_count += count;

}

int abstract_cpu::has_cstate_level(int level)
{
	unsigned int i;

	if (level == LEVEL_HEADER)
		return 1;

	for (i = 0; i < cstates.size(); i++)
		if (cstates[i]->line_level == level)
			return 1;

	for (i = 0; i < children.size(); i++)
		if (children[i]) 
			if (children[i]->has_cstate_level(level))
				return 1;
	return  0;
}



void abstract_cpu::insert_pstate(uint64_t freq, const char *human_name, uint64_t duration, int count)
{
	struct frequency *state;

	state = new struct frequency;

	if (!state)
		return;

	memset(state, 0, sizeof(*state));

	pstates.push_back(state);

	state->freq = freq;
	strcpy(state->human_name, human_name);


	state->time_before = duration;
	state->before_count = count;
}

void abstract_cpu::finalize_pstate(uint64_t freq, uint64_t duration, int count)
{
 	unsigned int i;
	struct frequency *state = NULL;

	for (i = 0; i < pstates.size(); i++) {
		if (freq == pstates[i]->freq) {
			state = pstates[i];
			break;
		}
	}

	if (!state) {
		cout << "Invalid P state finalize " << freq << " \n";
		return;
	}
	state->time_after += duration;
	state->after_count += count;

}

void abstract_cpu::update_pstate(uint64_t freq, const char *human_name, uint64_t duration, int count)
{
 	unsigned int i;
	struct frequency *state = NULL;

	for (i = 0; i < pstates.size(); i++) {
		if (freq == pstates[i]->freq) {
			state = pstates[i];
			break;
		}
	}

	if (!state) {
		insert_pstate(freq, human_name, duration, count);
		return;
	}

	state->time_before += duration;
	state->before_count += count;
}