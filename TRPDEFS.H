/*$no list*//*no trace <<< define.h >>>   */
/*  Copyright (C)1989 Applied Imagination */
/* Written by Steven R. Stuart 1989 */

#pragma trace-

/* ====== DEFINE CONSTANTS ======= */
#define SCREENSEG   0xB800       /* Where color screen memory starts */


#define BOOL        int
#define FALSE       0
#define TRUE        !FALSE

#define SOH         '\x01'  /* Start-of-heading */
#define EOT         '\x04'  /* End-of-transmission */

#define FUNCTION    '\0'
#define UPAROW      'H'
#define DNAROW      'P'
#define LTAROW      'K'
#define RTAROW      'M'
#define PGUP        'I'
#define PGDN        'Q'
#define HOME        'G'
#define END         'O'
#define ENTER       '\x0D'
#define TAB         '\x09'
#define BACKTAB     '\x0F'
#define BACKSP      '\x08'
#define ESC         '\x1B'
#define SPACEBAR    '\x20'
#define FUNC1       ';'
#define FUNC2       '<'
#define FUNC3       '='
#define FUNC4       '>'
#define FUNC5       '?'
#define FUNC6       '@'
#define FUNC7       'A'
#define FUNC8       'B'
#define FUNC9       'C'
#define FUNC10      'D'

/* WINDOW SCROLL FUNCTIONS */
#define CLEAR       0
#define UP          6
#define DOWN        7

/* Window borders */
#define TLC11       0xD5
#define TLC12       0xD5
#define TLC21       0xD5
#define TLC22       0xD5
#define TRC11       0xB8
#define TRC12       0xB8
#define TRC21       0xB8
#define TRC22       0xB8
#define BLC11       0xC0
#define BLC12       0xD4
#define BLC21       0xD4
#define BLC22       0xD4
#define BRC11       0xD9
#define BRC12       0xBE
#define BRC21       0xBE
#define BRC22       0xBE
#define TEE11       'x'
#define TEE12       'x'
#define TEE21       0xD1
#define TEE22       'x'
#define BEE11       0xC1
#define BEE12       'x'
#define BEE21       'x'
#define BEE22       'x'
#define LEE11       0xC3
#define LEE12       'x'
#define LEE21       0xC6
#define LEE22       'x'
#define REE11       0xB4
#define REE12       'x'
#define REE21       0xB5
#define REE22       'x'
#define CRX11       0xC5
#define CRX12       'x'
#define CRX21       0xD8
#define CRX22       'x'
#define H_LINE1     0xC4
#define H_LINE2     0xCD
#define V_LINE1     0xB3
#define V_LINE2     'x'

/* COLOR ATTRIBUTES */
#define BLINK       0x80
#define BLACK_B     0x00
#define BLUE_B      0x10
#define GREEN_B     0x20
#define CYAN_B      0x30
#define RED_B       0x40
#define MAGENTA_B   0x50
#define BROWN_B     0x60
#define WHITE_B     0x70
#define BLACK_F     0x00
#define BLUE_F      0x01
#define GREEN_F     0x02
#define CYAN_F      0x03
#define RED_F       0x04
#define MAGENTA_F   0x05
#define BROWN_F     0x06
#define WHITE_F     0x07
#define GRAY_F      0x08
#define LTBLUE_F    0x09
#define LTGREEN_F   0x0A
#define LTCYAN_F    0x0B
#define LTRED_F     0x0C
#define LTMAGENTA_F 0x0D
#define YELLOW_F    0x0E
#define BRTWHITE_F  0x0F

/* Reverse video colors blink suppressed */
#define INVERT( attr ) ( attr << 4 | attr >> 4 ) & 0x7F

/* File logic */
#define READ        TRUE
#define SAVE        !READ

/* NULL pointer definitions */
#define NULLCHARPTR        (char *) 0
#define NULLFARCHARPTR     (char far *) 0

/* Comm port definitions */
#define COM1        0        /* Port numbers */
#define COM2        1
#define COM3        2
#define COM4        3
#define INIT_COM    0        /* Operation codes */
#define SEND_COM    1
#define GET_COM     2
#define STAT_COM    3
#define CTS         0x10     /* Handshake modes */
#define DSR         0x20
#define XON         0x11     /* ^Q */
#define XOFF        0x02     /* ^B */
#define NOHANDSHK   0x00
#define BAUD300     0x40     /* Baudrates */
#define BAUD600     0x60
#define BAUD1200    0x80
#define BAUD2400    0xA0
#define BAUD4800    0xC0
#define BAUD9600    0xE0
#define PARITY_ODD  0x08     /* Parity selections */
#define PARITY_EVEN 0x18
#define PARITY_NONE 0x00
#define STOPBIT_1   0x00     /* Stop bit number selections */
#define STOPBIT_2   0x40
#define NUMBITS_7   0x02     /* Data bit number selections */
#define NUMBITS_8   0x03

/* Printer port definitions */
#define LPT1        0
#define LPT2        1
#define SEND_PRT    0
#define INIT_PRT    1
#define STAT_PRT    2
#define NOT_BUSY    0x80
#define ACKNOWL     0x40
#define PAPER_OUT   0x20
#define SELECTD     0x10
#define IO_ERROR    0x04
#define PTIMEOUT    0x01

/* Modem functions */
#define DISCONNECT  0
#define DIAL        1

/* Structure definitions */
struct WINDOW {
                unsigned char topleftrow;
                unsigned char topleftcol;
                unsigned char botrightrow;
                unsigned char botrightcol;
                char ident[12];
                unsigned char attr;
                };


#define VIDATTR     WHITE_F|BLACK_B
#define MA         0xb000 /* start of video memory for Monochrome Adapter */
#define CGA        0xb800 /* start of video memory for Color Graphics Adapter */

#define MODULE_NA   (-2)

/* Facility types */
#define ON_WING     1
#define PROP_STAND  2
#define DYNA_STAND  3

/* Engine models supported */
#define MDL_TPE3    0
#define MDL_TPE5    1
#define MDL_TPE6    2
#define MDL_TPE8    3
#define MDL_TPE10   4
#define MDL_TPE10UA 5
#define MDL_TPE11   6
#define MDL_TPE1EGT 7

/* Program call errorlevels */
#define TPE10       10
#define TPE8        11
#define TPE356      12
#define TPE1EGT     13

/* 100% references */
#define RPM_2000    2000
#define RPM_1591    1591

/* Reference spd per model */
#define REFSPEED_3      RPM_2000
#define REFSPEED_5      RPM_1591
#define REFSPEED_6      RPM_2000
#define REFSPEED_8      RPM_2000
#define REFSPEED_10     RPM_1591
#define REFSPEED_10UA   RPM_2000
#define REFSPEED_11     RPM_1591
#define REFSPEED_1EGT   RPM_2000

/* Nominal temperature per model */
#define TNOMINAL_3      1693F
#define TNOMINAL_5      1693F
#define TNOMINAL_6      1693F
#define TNOMINAL_8      937F
#define TNOMINAL_10     1049F
#define TNOMINAL_10UA   1049F
#define TNOMINAL_11     1079F
#define TNOMINAL_1EGT   1032F

/* Plot T4/T4 starting points */
#define T4RATNTRY_3     1F
#define T4RATNTRY_5     1F
#define T4RATNTRY_6     1F
#define T4RATNTRY_8     1F
#define T4RATNTRY_10    0.985F
#define T4RATNTRY_10UA  0.985F
#define T4RATNTRY_11    1F
#define T4RATNTRY_1EGT  1F

/* Plot SHP starting points */
#define MINSHPNTRY_3    1300F
#define MINSHPNTRY_5    840F
#define MINSHPNTRY_6    840F
#define MINSHPNTRY_8    900F
#define MINSHPNTRY_10   1020F
#define MINSHPNTRY_10UA 1020F
#define MINSHPNTRY_11   1050F
#define MINSHPNTRY_1EGT 665F

/* Minimum turbine flow parameters */
#define TFPMIN_3        1.5F
#define TFPMIN_5        1.5F
#define TFPMIN_6        1.5F
#define TFPMIN_8        1.555F
#define TFPMIN_10       1.52F
#define TFPMIN_10UA     1.52F
#define TFPMIN_11       1.52F
#define TFPMIN_1EGT     1.5F

/* Maximum turbine flow parameters */
#define TFPMAX_3        1.65F
#define TFPMAX_5        1.65F
#define TFPMAX_6        1.65F
#define TFPMAX_8        1.645F
#define TFPMAX_10       1.65F
#define TFPMAX_10UA     1.65F
#define TFPMAX_11       1.65F
#define TFPMAX_1EGT     1.65F

/* Graph types */
#define GRAPH_A     1
#define GRAPH_B     2

/* Main menu selections */
#define MENU_NEW		0
#define MENU_EDIT		1
#define MENU_SAVE		2
#define MENU_LOAD		3
#define MENU_GRAPH		4
#define MENU_PRINT		5
#define MENU_VIEW		6
#define MENU_DELETE		7
#define MENU_VERSION	8
#define MENU_EXIT		9

/* Printer macros */
#define PTR_READY( status ) (( status & 0x4030 ) == 0x4030 )
#define SELECTED( status ) (( status & 0x0010 ) == 0x0010 )

struct TESTFORM {
                int     month;
                int     day;                    /* Test date */
                int     year;
                char    comment[60];            /* Operator comment line */
                char    tech[5];                /* tech initials */
                unsigned long int   serial;     /* Engine serial number */
                int                 ref_speed;  /* 100% engine reference speed */
                int                 facility;   /* Test origination */
                int     meter_k;
                int     meter_lim;
                float   tt5c;
                float   sg;                 /* Fuel specific gravity @ 60F */
                float   lhv;                /* Fuel lower heating value */
                float   cell[10][11];        /* 11 lines of 10 input points */
                };

struct OUT_PARMS {
                float   tt5;    /* Turbine exit temp */
                float   t4t4ref; /* Turbine inlet temp over max allowed */
                float   shp;    /* Shaft horsepower */
                float   wf;     /* Fuel flow */
                float   tfp;    /* Turbine flow parameter */
                float   speed;  /* Engine speed */
                };


/* Macro to check log printer ready status */
#define LOG_READY( status ) (( status & 0x4030 ) == 0x4030 )
/* Macro to check if report printer is selected */
#define SELECTED( status ) (( status & 0x0010 ) == 0x0010 )


/* Function definitions */
/* TRPPOP */
void vpoke(int segment, int offset, int value);
int vpeek(int segment, int offset);
int popup(int row, int col, char *menu[], BOOL hots[] );

/* TRPPLOT */
int graph_( void );
BOOL graph_grid( void );
BOOL mark_pt( int x_coord, y_coord, point );
BOOL getpts( int *obs, float *TT5_pts, float *SHP_pts, float *T4r_pts );
BOOL curve_ttl( float *SHP_pos, float *T4r_pos );
BOOL maximum( float t4_entry, float *y_SHP, float *y_T4r );
BOOL minimum( float shp_entry, float *y_SHP, float *y_T4r );
BOOL result( float t_nominal, float *y_T4r );
void LinearFit(float *x, float *y, int num, float *coef);
int ypoint( float y );
int rpoint( float r );
int xpoint( float x );
void vplots( char *stn ); 
void plota( int x, int y, char *stn );
BOOL save_scn( char far *scnloc );
BOOL restore_scn( char far *scnloc );
void drain( void );

/* TRPFUNC */
void nomenclature( int model );
void say_error( char *err_msg[] );
BOOL file2edit( char f2e_name[] );
BOOL retrieve( void );
BOOL calculate( void );
BOOL put_in_file( char file_name[] );
BOOL store_params( int *edflag, int *model, int *program );
BOOL load_params( int *edflag, int *model, int *program );
int getline( char *inline, int max_char );
void printa( char *sptr, attr );
void show_commands( void );
void ftoa6( float f, char *t );
BOOL chksum( char f_name[], int checksum );
void signon( void );
BOOL set_defaults( char cswitch[] );
void set_color( BOOL want_color );
void disclaim( void );

/* TRPFORM */
BOOL fill_display( int first_pt );
void unit_adjust( int cur_col );
BOOL get_head( void );
BOOL get_data( void );
BOOL makeform( void );

/* TRPFILE */
void calc_out( void );
void view_help( void );
BOOL delete( char f_spec[] );
BOOL savefile( void );
BOOL getfiles( void );
BOOL selfile( char f_spec[] );
void listfiles( int ref_nr, struct ffblk *filedata );
int get_fchoice( int nr_choices );

/* TRPWNDO */
void scroll( struct WINDOW window, int lines, int direction );
int make_window( struct WINDOW window, BOOL save );
int take_window( struct WINDOW window, int *mem_bufr );

/* TRPVERS */
void version();

/* TRPPRINT */
BOOL pt_form( void );
BOOL pt_calcdata( void );
BOOL sendbyte( unsigned int byte );
BOOL epsprint( void );
BOOL okiprint( void );
BOOL send_print_line( char stn[], int nr_chars );
BOOL lazprint( void );
unsigned dbl( unsigned nybble );


/*$list*//*$trace       <<< define.h >>> */
