/* TRPDCLR.C
    no_list
*/

char *pt_head[] = {
                    "Pt.1", "Pt.2", "Pt.3", "Pt.4", "Pt.5",
                    "Pt.6", "Pt.7", "Pt.8", "Pt.9", "Pt.10"
                  };

char *heading[] = { "Engine serial number:",
                    "Test date:",
                    "Technician:",
                    "Fuel specific gravity:",
                    "Fuel lower heating value:",
                    "btu/lb",
                    "Flowmeter constant:",
                    "Flowmeter limit:",
                    "Tt5 compensation:",
                    "   Nomenclature of Parameter",
                    "Sym",
                    "Unit "
                  };

char *label[] = { "Engine inlet pressure (Barometer)",
                  "Total pressure Sta. 3 compressor exit",
                  "Static pressure Sta. 5 turbine exit",
                  "Total temp Sta. 2 engine inlet",
                  "Total temp Sta. 3 compressor exit",
                  "Total temp Sta. 5 turbine exit",
                  "Propeller speed",
                  "Torque",
                  "Raw fuel flow",
                  "Fuel temperature",
                  "Pressure Altitude",
                  "Compressor discharge pressure",
                  "Tester pin 17 - 18, back pressure",
                  "Inlet temperature, pin 15 - 16",
                  "Compressor discharge temperature",
                  "Compensated turbine exit temperature",
                  "Turbine speed, pin 11 - 12",
                  "Exhaust pressure",
                  "Inlet temperature",
                  "Exhaust gas temperature",
                  "Turbine speed",
                  "Torque  (LeBow)",
                  "Interstage turbine temperature",
                  "Fuel flow",
                  "Tester pin 13 - 25, back pressure",
                  "Inlet temperature, pin 7 - 8",
                  "Turbine speed, pin 42 - 45"
                };

char *sym[] = { "Pt2",
                "Pt3",
                "Ps5",
                "Tt2",
                "Tt3",
                "Tt5",
                "NP",
                "TQ",
                "WF",
                "TWF",
                "Alt",
                "P/P",
                "T3f",
                "T5f",
                "RPM",
                "ITT",
                "Pba"
              };

char *unit[] = { "in Hg",
                 "psig ",
                 "in H2O",
                 " \xf8F  ",
                 "rpm ",
                 "ft-lbs",
                 "in-lbs",
                 "lbs/hr",
                 "deg F",
                 "feet ",
                 "volts",
                 "Hz  "
               };

char *eng_type[] = { "TPE331-3",
                     "TPE331-5",
                     "TPE331-6",
                     "TPE331-8",
                     "TPE331-10",
                     "TPE331-10UA",
                     "TPE331-11",
                     "TPE331-1EGT",
                     NULL
                   };

BOOL color_monitor, color_graph;
int black_b, blue_b, green_b, cyan_b, red_b, magenta_b, brown_b, white_b;
int black_f, blue_f, green_f, cyan_f, red_f, magenta_f, brown_f, white_f;
int gray_f, ltblue_f, ltgreen_f, ltcyan_f, ltred_f;
int ltmagenta_f, yellow_f, brtwhite_f;

char *label_ptr[12];  /* These pointers address strings for current engine */
char *symbol_ptr[12];
char *units_ptr[12];

struct TESTFORM engine;

int max_point;          /* Nr of points used in test */

#ifndef DEMO
    float calc_data[60];
#else
#include "trpdemo.c"        /* creates default "readings" for graph */
#endif

struct ffblk filedata;  /* Directory file data in <direct.h> */
char filenames[5][14];  /* Menu of filenames for retrieve func */
char curfile[14] = "";  /* Current file in form */

int graph_type;
unsigned v_res, h_res;
unsigned shp_scaled_point;
unsigned t4ratio_scaled_point;
unsigned t5raw_scaled_point;
unsigned itt_scaled_point;
float max_shp, min_shp, min_tt5, max_tt5;
float t4rat_ntry, minshp_ntry, t_nominal;
float lo_x, hi_x, lo_y = 350F, hi_y = 1300F; /* graphic extremes */
float tfp_max, tfp_min;
float m1, m2, b1, b2;       /* line calculation vars   y=mx+b */
float test, offset = 0F;


int program;        /* TPE program call errorlevel */

struct vconfig screen;  /* Video configuration structure */
unsigned long crtsiz;   /* Size of video graphic memory */
char far *crtptr;       /* Pointer to physical screen */
char far *vscreen;      /* Pointer to virtual screen */
int gmode;              /* Graphic mode found */

int printmode;      /* Default printmode to Epson */
int vidmode;        /* Default vidmode to autoselect */
int vidbase;        /* video text base segment address */
int printer_port;   /* Printer port */
BOOL use_bios;      /* Bios print flag */

char infile_name[]  =  "D.CAL";
char outfile_name[] =  "D.USR";
char holdfile_name[] = "D.HLD";
char errfile_name[] =  "D.ERR";
char checksum_err[] =  "Checksum error";
int valid_cksum = 0x1EB0;     /* checksum of chk_file[] */
char buffer[300];             /* GP buffer for ASCII */
char chk_file[] = "RDCE.BAT"; /* File tested for checksum*/

BOOL tpe_err;
BOOL formmade = FALSE;
int model;
/* eof: TRPDCLR.C */
