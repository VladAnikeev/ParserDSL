#include "nwocg_run.h"
#include <math.h>

static struct
{
	double setpoint;
	double Ts;
	double feedback;
	double I_gain;
	double Add1;
	double Add2;
	double Add3;
	double P_gain;
	double UnitDelay1;
} state;

void generated_init()
{
	state.UnitDelay1 = 0;
}

void generated_step()
{
	state.Add1 = state.setpoint - state.feedback;
	state.I_gain = state.Add1 * 2;
	state.P_gain = state.Add1 * 3;
	state.Ts = state.I_gain * 0.01;
	state.Add2 = state.Ts + state.UnitDelay1;
	state.Add3 = state.P_gain + state.Add2;

	state.UnitDelay1 = state.Add2;
}
static const ExtPort
	ext_ports[] =
{
	{ "setpoint", &state.setpoint, 1 },
	{ "feedback", &state.feedback, 1 },
	{ "command", &state.Add3, 0 },
	{ 0, 0, 0 },
};

const ExtPort * const
	nwocg_generated_ext_ports = ext_ports;
const size_t
	generated_ext_ports_size = sizeof(ext_ports);
