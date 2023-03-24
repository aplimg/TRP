/* TRPVERS.H */

#define VERS    "1.40"
#define LIC1    "Danish Aircraft"
#define LIC2    "Hangar VEJ. H.1."
#define LIC3    "Roskilde Airport"
#define LIC4    "Denmark"

#undef DEMO
#undef PROTOTYPE
#undef TRAINING

/* If demo flag on use serial A-1000 */
#ifdef DEMO
#define SER     "A-1000"
#else
#define SER     "A-1002"	/* Last = 1021 */
#endif

#define MODULE_A
#define MODULE_B
#define MODULE_C


/* eof: TRPVERS.H */
