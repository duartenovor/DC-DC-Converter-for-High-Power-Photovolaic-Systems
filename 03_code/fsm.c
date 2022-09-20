#include "fsm.h"

/* --------------------------- */
/* --- Function Prototypes --- */
/* --------------------------- */

/* --- FSM functions --- */

/* Increment PWM slowly */
void code_init();

/* MPPT and PI algorithms */
void code_run();

/* -------------------------- */
/* --- Internal Variables --- */
/* -------------------------- */

/* --- States --- */

typedef enum STATE {ST_INIT, ST_MPPT} E_STATE;

static volatile E_STATE state;
static volatile E_STATE next_state;

/* ----------------------- */
/* --- Functions Array --- */
/* ----------------------- */

void (*ptr_func[])(void) = {code_init, code_run};

/* ----------------- */
/* --- Functions --- */
/* ----------------- */

void encode_fsm() {
	state = next_state;
	ptr_func[state]();
}

void code_init() {



	next_state = ST_MPPT;
}

void code_run() {
	
}

