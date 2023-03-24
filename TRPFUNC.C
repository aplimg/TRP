/*
    File: TRPFUNC.C
	Copyright (C)1989, 1990 Applied Imagination, Inc.
    Written by Steven R. Stuart - Sept 1989
*/


#pragma nestcmnt

#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>
#include <string.h>
#include <malloc.h>
#include <bios.h>
#include <dos.h>
#include <process.h>
#include "trpdefs.h"
#include "trpvers.h"

extern int black_b, blue_b, green_b, cyan_b, red_b, magenta_b, brown_b, white_b;
extern int black_f, blue_f, green_f, cyan_f, red_f, magenta_f, brown_f, white_f;
extern int gray_f, ltblue_f, ltgreen_f, ltcyan_f, ltred_f;
extern int ltmagenta_f, yellow_f, brtwhite_f;

#pragma trace-
void nomenclature( int model )
/* Set global nomenclature pointers to strings for model engine desired */
{
extern char *heading[];
extern char *label[],     *sym[],        *unit[];
extern char *label_ptr[], *symbol_ptr[], *units_ptr[];
extern int graph_type;
extern struct TESTFORM engine;

    int i;

    label_ptr[0] = heading[9];   /* Nomenclature title */
    symbol_ptr[0] = heading[10]; /* Nomen symbol title */
    units_ptr[0] = heading[11];  /* Nomen units title  */

    /* Next pointers set for -8, -10, -10UA, -11 propstand and dyna */
    /*  as they are all alike (on_wing is different for -8 / -10,11 */
    for( i=1; i<11; i++ ) {
        label_ptr[i] =  label[i-1];
        symbol_ptr[i] = sym[i-1];
        }
    units_ptr[1] =  unit[0];
    units_ptr[2] =  unit[1];
    units_ptr[3] =  unit[2];
    units_ptr[4] =  unit[3];
    units_ptr[5] =  unit[3];
    units_ptr[6] =  unit[3];
    units_ptr[7] =  unit[4];
    units_ptr[8] =  unit[5];
    units_ptr[9] =  unit[7];
    units_ptr[10] =  unit[3];
    label_ptr[11] = symbol_ptr[11] = units_ptr[11] = NULL;

    /* Change all labels that are different from above */
    switch( model ) {

#ifdef MODULE_C
        case MDL_TPE3   :
        case MDL_TPE5   :
        case MDL_TPE6   :
                graph_type = GRAPH_B;

                /* ON_WING */
                label_ptr[1] = label[10]; /* pressure alt */
                label_ptr[2] = label[11]; /* comp disc press */
                label_ptr[3] = label[17]; /* exhaust press */
                label_ptr[4] = label[18]; /* inlet temp */
                label_ptr[5] = label[14]; /* comp disch temp */
                label_ptr[6] = label[19]; /* exhaust gas temp */
                label_ptr[7] = label[20]; /* turbine speed */
                label_ptr[8] = label[21]; /* torque (lebow) */
                label_ptr[9] = label[23];  /* fuel flow */
                label_ptr[10] = label[9]; /* fuel temp */
                label_ptr[11] = label[22]; /* interstage turbine temp */

                symbol_ptr[1] = sym[10]; /* Alt */
                symbol_ptr[2] = sym[1]; /* Pt3 */
                symbol_ptr[3] = sym[2]; /* Ps5 */
                symbol_ptr[4] = sym[3]; /* Tt2 */
                symbol_ptr[5] = sym[12]; /* T3f */
                symbol_ptr[6] = sym[13]; /* T5f */
                symbol_ptr[7] = sym[6]; /* NP */
                symbol_ptr[8] = sym[7]; /* TQ */
                symbol_ptr[9] = sym[8]; /* WF */
                symbol_ptr[10] = sym[9]; /* TWF */
                symbol_ptr[11] = sym[15]; /* ITT */

                units_ptr[1] = unit[9]; /* feet */
                units_ptr[2] = unit[0]; /* in-Hg */
                units_ptr[3] = unit[2]; /* in-H2O */
                units_ptr[4] = unit[3]; /* degs F */
                units_ptr[5] = unit[3];
                units_ptr[6] = unit[3];
                units_ptr[7] = unit[11]; /* Hz */
                units_ptr[8] = unit[5]; /* ft-lbs */
                units_ptr[9] = unit[11]; /* Hz */
                units_ptr[10] = unit[3]; /* degs F */
                units_ptr[11] = unit[3];

            if( engine.facility != ON_WING ) {
                label_ptr[1] = label[0]; /* barometer */
                label_ptr[7] = label[6]; /* propeller speed */
                symbol_ptr[1] = sym[0];  /* Pt2 */
                symbol_ptr[7] = sym[6];  /* rpm */
                units_ptr[1] = unit[0];  /* in-Hg */
                units_ptr[7] = unit[4];  /* rpm */
                units_ptr[9] = unit[7];  /* Lb/hr */
                }
            break;
#endif

#ifdef MODULE_B
        case MDL_TPE8   :
            graph_type = GRAPH_A;

            if( engine.facility == ON_WING ) {
                label_ptr[1] = label[10];
                label_ptr[2] = label[11];
                label_ptr[3] = label[24];
                label_ptr[4] = label[25];
                label_ptr[5] = label[14];
                label_ptr[6] = label[15];
                label_ptr[7] = label[26];
                label_ptr[8] = label[7];
                label_ptr[9] = label[23]; /* fuel flow */
                label_ptr[10] = label[9];
                label_ptr[11] = NULL;

                symbol_ptr[1] = sym[10];
                symbol_ptr[2] = sym[1];
                symbol_ptr[3] = sym[11];
                symbol_ptr[4] = sym[3];
                symbol_ptr[5] = sym[12];
                symbol_ptr[6] = sym[13];
                symbol_ptr[7] = sym[14];
                symbol_ptr[8] = sym[7];
                symbol_ptr[9] = sym[8];
                symbol_ptr[10] = sym[9];
                symbol_ptr[11] = NULL;

                units_ptr[1] = unit[9];
                units_ptr[2] = unit[0];
                units_ptr[3] = unit[10];
                units_ptr[4] = unit[10];
                units_ptr[5] = unit[3];
                units_ptr[6] = unit[3];
                units_ptr[7] = unit[11];
                units_ptr[8] = unit[5]; /* ft-lbs */
                units_ptr[9] = unit[11];
                units_ptr[10] = unit[3];
                units_ptr[11] = NULL;

                }

            break;
#endif

#ifdef MODULE_A
        case MDL_TPE10  :
        case MDL_TPE10UA:
        case MDL_TPE11  :
        case MDL_TPE1EGT:
                graph_type = GRAPH_A;

            if( engine.facility == ON_WING ) {

                label_ptr[1] = label[10];
                label_ptr[2] = label[11];
                label_ptr[3] = label[12];
                label_ptr[4] = label[13];
                label_ptr[5] = label[14];
                label_ptr[6] = label[15];
                label_ptr[7] = label[16];
                label_ptr[8] = label[7];
                label_ptr[9] = label[8];
                label_ptr[10] = label[9];
                label_ptr[11] = NULL;

                symbol_ptr[1] = sym[10];
                symbol_ptr[2] = sym[1];
                symbol_ptr[3] = sym[11];
                symbol_ptr[4] = sym[3];
                symbol_ptr[5] = sym[12];
                symbol_ptr[6] = sym[13];
                symbol_ptr[7] = sym[14];
                symbol_ptr[8] = sym[7];
                symbol_ptr[9] = sym[8];
                symbol_ptr[10] = sym[9];
                symbol_ptr[11] = NULL;

                units_ptr[1] = unit[9];
                units_ptr[2] = unit[0];
                units_ptr[3] = unit[10];
                units_ptr[4] = unit[10];
                units_ptr[5] = unit[3];
                units_ptr[6] = unit[3];
                units_ptr[7] = unit[11];
                units_ptr[8] = unit[10];
                units_ptr[9] = unit[11];
                units_ptr[10] = unit[3];
                units_ptr[11] = NULL;
                } /* if ON_WING */
            break;
#endif
        case -1: /* Remove function chars in strings for ptr */
            for( i=0; i<12; i++ ) {
                /* Take degree symbol out of degrees F string */
                if( units_ptr[i] == unit[3] ) units_ptr[i] = unit[8];
                }
            break;
            } /* switch */
#ifdef DEMO
    graph_type = GRAPH_A;
#endif

} /* nomenclature */

#pragma trace-
void say_error( char *err_msg[] )
{
/*
+----------------------------------------------------------------------+
| Change model definition w/ new engines.                              |
+----------------------------------------------------------------------+
*/
extern buffer[];

    int *under_error;
    struct WINDOW error = { 5, 10, 7, 70, "µ ERROR Æ", red_b + brtwhite_f };

    under_error = make_window( error, TRUE );
    sound( 200, 5 );
    cursoff();
    poscurs( 6, 40 - (strlen( err_msg ) / 2) );
    sprintf( buffer, "%s", err_msg );
    printa( buffer, error.attr );
    while( getkey() != EOF );  /* Flush buffer */
    getch(); /* Wait for keypress */
    take_window( error, under_error );
    curson();

} /* say_error */

#pragma trace-
BOOL file2edit( char f2e_name[] )
{ /* Put .USR file f2e_name[] in engine.<structure> */
extern int max_point, model, program;
extern float t4rat_ntry, minshp_ntry, t_nominal, tfp_min, tfp_max;
extern char outfile_name[];
extern char buffer[];
extern struct TESTFORM engine;

    int i, status;
    char *loc;
    FILE *f_in;

    if( (f_in = fopen( f2e_name, "r" )) != NULL ) {

        if( strstr( outfile_name, f2e_name ) == NULL ) {
            /* Not D.USR file so get model */
            loc = strstr( f2e_name, "." );        /* Locate fname ext */
            model = *++loc;                       /* Assign */
            model -= 48;                          /* Adjust */
            } /* if !D.USR */

        switch( model ) { /* Set model specifics */

#ifdef MODULE_C
            case MDL_TPE3   :
                t_nominal =   TNOMINAL_3;
                t4rat_ntry =  T4RATNTRY_3;
                minshp_ntry = MINSHPNTRY_3;
                tfp_min =     TFPMIN_3;
                tfp_max =     TFPMAX_3;
                program =     TPE356; /* errorlevel 12 */
                break;

            case MDL_TPE5   :
                t_nominal =   TNOMINAL_5;
                t4rat_ntry =  T4RATNTRY_5;
                minshp_ntry = MINSHPNTRY_5;
                tfp_min =     TFPMIN_5;
                tfp_max =     TFPMAX_5;
                program =     TPE356; /* errorlevel 12 */
                break;

            case MDL_TPE6   :
                t_nominal =   TNOMINAL_6;
                t4rat_ntry =  T4RATNTRY_6;
                minshp_ntry = MINSHPNTRY_6;
                tfp_min =     TFPMIN_6;
                tfp_max =     TFPMAX_6;
                program =     TPE356;
                break;
#endif

#ifdef MODULE_B
            case MDL_TPE8   :
                t_nominal =   TNOMINAL_8;
                t4rat_ntry =  T4RATNTRY_8;
                minshp_ntry = MINSHPNTRY_8;
                tfp_min =     TFPMIN_8;
                tfp_max =     TFPMAX_8;
                program =     TPE8; /* errorlevel 11 */
                break;
#endif

#ifdef MODULE_A
            case MDL_TPE10  :
                t_nominal =   TNOMINAL_10;
                t4rat_ntry =  T4RATNTRY_10;
                minshp_ntry = MINSHPNTRY_10;
                tfp_min =     TFPMIN_10;
                tfp_max =     TFPMAX_10;
                program =     TPE10; /* errorlevel 10 */
                break;

            case MDL_TPE10UA:
                t_nominal =   TNOMINAL_10UA;
                t4rat_ntry =  T4RATNTRY_10UA;
                minshp_ntry = MINSHPNTRY_10UA;
                tfp_min =     TFPMIN_10UA;
                tfp_max =     TFPMAX_10UA;
                program =     TPE10; /* errorlevel 10 */
                break;

            case MDL_TPE11  :
                t_nominal =   TNOMINAL_11;
                t4rat_ntry =  T4RATNTRY_11;
                minshp_ntry = MINSHPNTRY_11;
                tfp_min =     TFPMIN_11;
                tfp_max =     TFPMAX_11;
                program =     TPE10; /* errorlevel 10 */
                break;

            case MDL_TPE1EGT  :
                t_nominal =   TNOMINAL_1EGT;
                t4rat_ntry =  T4RATNTRY_1EGT;
                minshp_ntry = MINSHPNTRY_1EGT;
                tfp_min =     TFPMIN_1EGT;
                tfp_max =     TFPMAX_1EGT;
                program =     TPE1EGT; /* errorlevel 13 */
                break;
#endif

            default         :
            /* 1st char of filename ext not 0 - 7 */
                say_error( "Model not implemented" );
                fclose( f_in );
                return FALSE;
            } /* switch */

        nomenclature( model ); /* Set nomenclature pointers */

        /* Get date, comments, technician */
        status = fgets( buffer, 300, f_in );                 /* Get line1 */
        if( status != NULL ) {
            /* If line1 too short, pad w/spaces */
            while( strlen( buffer ) < 73 ) strcat( buffer, " " );
            engine.month = atoi( buffer );
            engine.day   = atoi( buffer+3 );
            engine.year  = atoi( buffer+6 );
            strncpy( engine.comment, buffer+9, 59 );
            strncpy( engine.tech, buffer+69, 4 );
            }
        else {
            fclose( f_in );
            return FALSE;
            }

        /* Get: serial, 100% engine speed, test facility */
        status = fgets( buffer, 300, f_in );
        if( status != NULL )
            if( model == MDL_TPE1EGT ) 
            /* ref_speed and facility reversed for this engine cuz
               Garrett calc program doesn't use ref_speed */
                sscanf( buffer, "%li %i %i",
                                &engine.serial,
                                &engine.facility,
                                &engine.ref_speed
                      );
            else
                sscanf( buffer, "%li %i %i",
                                &engine.serial,
                                &engine.ref_speed,
                                &engine.facility
                      );
            
        else {
            fclose( f_in );
            return FALSE;
            }

        /* Get: */
        status = fgets( buffer, 300, f_in );
        if( status != NULL ) {
            if( engine.facility == ON_WING ) {
                sscanf( buffer, "%i %i %f %f",
                                &engine.meter_k,    /* fuel meter constant     */
                                &engine.meter_lim,  /* flow meter break        */
                                &engine.sg,         /* fuel sp gravity         */
                                &engine.tt5c        /* engine TT5 compensation */
                                );
               } /* if */
            else { /* PROP_STAND or DYNA_STAND */
                sscanf( buffer, "%f %f",
                                &engine.sg,     /* fuel sp gravity          */
                                &engine.lhv     /* fuel lower heating value */
                                );
                } /* else */
            } /* if */
        else {
            fclose( f_in );
            return FALSE;
            }

        max_point = 0; /* Reset nr points */
        do {
            status = fgets( buffer, 300, f_in );
            if( status == NULL ) {
                say_error( "101  Encountered premature EOF" );
                fclose( f_in );
                return FALSE;
                break;
                }
            if( (strstr( buffer, "9*0" )) ||
                (strstr( buffer, "10*0" )) ||
                (max_point == 10) ) {
                /* End of data if "9*0", "10*0" found or max_point > 9 */
                status = NULL;
                }
            else { /* Get the data points */
                if( model == MDL_TPE3 ||
                    model == MDL_TPE5 ||
                    model == MDL_TPE6 )
                    sscanf( buffer, "%f %f %f %f %f %f %f %f %f %f %f",
                            &engine.cell[max_point][0],
                            &engine.cell[max_point][1],
                            &engine.cell[max_point][2],
                            &engine.cell[max_point][3],
                            &engine.cell[max_point][4],
                            &engine.cell[max_point][5],
                            &engine.cell[max_point][6],
                            &engine.cell[max_point][7],
                            &engine.cell[max_point][8],
                            &engine.cell[max_point][9],
                            &engine.cell[max_point][10]
                          );
                else /* TPE331-8/-10/-11 */
                    sscanf( buffer, "%f %f %f %f %f %f %f %f %f %f",
                            &engine.cell[max_point][0],
                            &engine.cell[max_point][1],
                            &engine.cell[max_point][2],
                            &engine.cell[max_point][3],
                            &engine.cell[max_point][4],
                            &engine.cell[max_point][5],
                            &engine.cell[max_point][6],
                            &engine.cell[max_point][7],
                            &engine.cell[max_point][8],
                            &engine.cell[max_point][9]
                        );

                max_point++; /* Set nr valid points read */
                } /* else */
            } /* do */
        while( status != NULL );
        /* Flag a good read */
        if( max_point > 0 ) status = TRUE;
        max_point--; /* Don't overshoot array */
        fclose( f_in );
        if( status ) return TRUE; /* Good read */
        }
    else return FALSE;

} /* file2edit */


#ifndef DEMO
#pragma trace-
BOOL retrieve( void )
{ /* Retrieve calculated data from D.CAL file */
    extern int max_point;
    extern float calc_data[];
    extern char buffer[], infile_name[];

    int i, status;
    FILE *f_in;

    if( (f_in = fopen( infile_name, "r" )) != NULL ) {
        do {
            status = fgets( buffer, 300, f_in );
            if( strstr( buffer, "PERCENT" ) ) {
                if( fgets( buffer, 300, f_in ) == NULL ) {
                    fclose( f_in );
                    say_error( "202  Encountered premature EOF" );
                    return FALSE;
                    }
                break;
                }
            } /* do */
        while( status != NULL );

        max_point = 0; /* Reset nr points */
        for( i=0; i<10; i++ ) { /* Get nr points sent to D.USR (10 max) */
            if( status = fgets( buffer, 300, f_in ) == NULL ) {
                fclose( f_in );
                say_error( "203  Encountered premature EOF" );
                return FALSE;
                }
            if( (strlen( buffer ) < 3) ||
                (strstr( buffer, "+++" )) ) break;
            sscanf( buffer, "%f %f %f %f %f %f",
                            &calc_data[i*6+0],
                            &calc_data[i*6+1],
                            &calc_data[i*6+2],
                            &calc_data[i*6+3],
                            &calc_data[i*6+4],
                            &calc_data[i*6+5]
                        );
            if( calc_data[i*6+0] == 0F ||
                calc_data[i*6+1] == 0F ||
                calc_data[i*6+2] == 0F ||
                calc_data[i*6+3] == 0F ||
                calc_data[i*6+4] == 0F ||
                calc_data[i*6+5] == 0F ) i--; /* Don't accept this point */
            else max_point++; /* Set nr valid points read */
            } /* for */
        fclose( f_in );
        return TRUE;
        }
    else {
        say_error( "204  Missing D.CAL" );
        return FALSE;
        }

} /* retreive */
#endif

#ifndef DEMO
#pragma trace-
BOOL calculate( void )
{
    extern int program; /* TPE program number */
    extern char chk_file[], checksum_err[];
    extern int valid_cksum;

    int i, status;
    char far *key_buff = 0x0040001a;
    char f_names[] = {
                    '\x1e', '\x00',  /* Head pointer     */
                    '\x36', '\x00',  /* Tail pointer     */
                    '\x44', '\x20',  /*     D            */
                    '\x2e', '\x34',  /*     .            */
                    '\x55', '\x16',  /*     U            */
                    '\x53', '\x1f',  /*     S            */
                    '\x52', '\x13',  /*     R            */
                    '\x0d', '\x1c',  /*     <cr>         */
                    '\x44', '\x20',  /*     D            */
                    '\x2e', '\x34',  /*     .            */
                    '\x43', '\x2e',  /*     C            */
                    '\x41', '\x1e',  /*     A            */
                    '\x4c', '\x26',  /*     L            */
                    '\x0d', '\x1c'   /*     <cr>         */
                   };

    if( ! chksum( chk_file, valid_cksum ) ) {
        say_error( checksum_err );
        exit( 1 );
        }

    clrscrn();
    printf( "Calculating, stand-by..\n" );
    for( i=0; i<28; i++ ) *key_buff++ = f_names[i]; /* Poke filenames into keybd buffer */
    exit( program );

} /* calculate */

#else
/* DEMO = T */
BOOL calculate( void )
{
extern char *eng_type[];
extern int model;

    /* DEMO calculate function */
    clrscrn();
    printf( "Calculating, stand-by..\n" );
    printf( "%s\n\n", eng_type[model] );
    sleep( 1 );
    printf( "ENTER INPUT FILE NAME\nD.USR\nENTER OUTPUT FILE NAME\nD.CAL\n" );
    sleep( 3 );
    printf( "\n\nExecution terminated : 0\n" );
    sleep( 2 );
    printf( "\n\n\n\nThe APPLIED IMAGINATION\n" );
    printf( "Turbine Data Reduction Processor demo\n" );
    printf( "has just simulated a call to the Garrett\n" );
    printf( "calculation program for the %s engine.\n\n", eng_type[model] );
    sleep( 3 );
    printf( "The following graph would be derived from the results\n" );
    printf( "of the calculation.\n" );
    sleep( 2 );
    printf( "\n\n\nPress any key to continue.." );
    getch();
    clrscrn();
    return TRUE;

} /* demo calculate */
#endif

#ifndef DEMO
#pragma trace-
BOOL put_in_file( char file_name[] )
{
    extern int max_point, model;
    extern struct TESTFORM engine;

    int i;
    FILE *f_out;

    max_point = 0;
    for( i=0; i<10; i++ ) {
        if( (engine.cell[i][5] == 0F) &&
            (engine.cell[i][6] == 0F) ) break;
        else max_point++;
        }
    if( (f_out = fopen( file_name, "w" )) != NULL ) {
        fprintf( f_out, "%02i-%02i-%02i %-60s%-4s\n",
                         engine.month,
                         engine.day,
                         engine.year,
                         engine.comment,
                         engine.tech );
        if( model == MDL_TPE1EGT ) 
        /* engine.ref_speed not used in this calculation so put
           it after facility so Garrett program can find the
           facility without error. We still have it for CRT display */
            fprintf( f_out, "%li %i %i\n",
                             engine.serial,
                             engine.facility,
                             engine.ref_speed ); 
        else
            fprintf( f_out, "%li %i %i\n",
                             engine.serial,
                             engine.ref_speed,
                             engine.facility );

        if( engine.facility == ON_WING )
            fprintf( f_out, "%i %i %.5f %.2f\n",
                                engine.meter_k,
                                engine.meter_lim,
                                engine.sg,
                                engine.tt5c );

        else /* propst / dyno */
            fprintf( f_out, "%.5f %.2f\n",
                             engine.sg,
                             engine.lhv );

        for( i=0; i<max_point; i++ ) {
            if( (model == MDL_TPE3) ||
                (model == MDL_TPE5) ||
                (model == MDL_TPE6) )
                fprintf( f_out,
                     "%.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f\n",
                     engine.cell[i][0],
                     engine.cell[i][1],
                     engine.cell[i][2],
                     engine.cell[i][3],
                     engine.cell[i][4],
                     engine.cell[i][5],
                     engine.cell[i][6],
                     engine.cell[i][7],
                     engine.cell[i][8],
                     engine.cell[i][9],
                     engine.cell[i][10]
                   );

            else
                fprintf( f_out,
                     "%.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f\n",
                     engine.cell[i][0],
                     engine.cell[i][1],
                     engine.cell[i][2],
                     engine.cell[i][3],
                     engine.cell[i][4],
                     engine.cell[i][5],
                     engine.cell[i][6],
                     engine.cell[i][7],
                     engine.cell[i][8],
                     engine.cell[i][9]
                   );
            } /* for */
        if( (model == MDL_TPE3) ||
            (model == MDL_TPE5) ||
            (model == MDL_TPE6) )
            fprintf( f_out, "%s\n", "99999 10*0." );
        else
            fprintf( f_out, "%s\n", "99999 9*0." );
        } /* if fopen */
    else return FALSE;
    if( fclose( f_out ) == 0 ) return TRUE;
    else return FALSE;


} /* put_in_file */

#pragma trace-
BOOL store_params( int *edflag, int *model, int *program )
{
extern char holdfile_name[];

    FILE *f_out;

    if( (f_out = fopen( holdfile_name, "w" )) != NULL ) {
        fprintf( f_out, "%i %i %i\n", *edflag, *model, *program );
        if( fclose( f_out ) == 0 ) return TRUE;
        }
    return FALSE;

} /* store_params */

#pragma trace-
BOOL load_params( int *edflag, int *model, int *program )
{
extern char holdfile_name[];

    FILE *f_in;

    if( (f_in = fopen( holdfile_name, "r" )) != NULL ) {
        fscanf( f_in, "%i %i %i", edflag, model, program );
        if( fclose( f_in ) == 0 ) return TRUE;
        }
    return FALSE;

} /* load_params */

#else
/* DEMO = T */
#pragma trace-
BOOL put_in_file( char file_name[] )
{
    return TRUE;

} /* demo put_in_file */

#pragma trace-
BOOL store_params( int *edflag, int *model, int *program )
{
    return TRUE;

} /* demo store_params */
#endif


#pragma trace-
int getline( char *inline, int max_char )
/*  Get a line of info from user, place in calling function's inline[] buffer.
    If a function key is pressed, return the second character else return 0.
    Return -1 if Escape pressed.
*/
{
    int  i, j, attr, row, col, init_col;
    char c;

    row = cursrow();
    col = init_col = curscol();
    attr = readattr(); /* Get current screen attribute */
    for( i=0; i<max_char; i++ ) inline[i] = '\0'; /* Clear input buffer */
    for( i=0; i<max_char; i++ ) {
        poscurs( row, col );
        switch( c = getch() ) {
            case FUNCTION:
                switch( c = getch() ) {
                    default: /* return func key char */
                        return c;
                        break;
                    } /* switch */
                break; /* function */
            case ESC:   /* If input aborted--return FALSE with inline[] = '\0' */
                inline[0] = '\0';
                return -1;
                break;
            case '\r':  /* Terminate line and return if ENTER */
                inline[i] = '\0';
                return 0;
                break;
            case BACKSP:
                if( i > 0 ) {
                    inline[i-1] = '\0';
                    i -= 2;
                    col--;
                    poscurs( row, col );
                    writechs( ' ', attr, 1 ); /* Echo char to screen and    */
                    }
                else i--; /* Don't backup if at inline[0] */
                break;
            default:
                if( isprint( c ) ) {
                    writechs( c, attr, 1 ); /* Echo char to screen and    */
                    col++;
                    inline[i] = c; /*  put the char in inline[] */
                    for( j=0; j<max_char; j++ ) {
                        poscurs( row, init_col+j ); /* Position to original loc + */
                        if( inline[j] == '\0' ) writechs( ' ', attr, 1 );
                        else writechs( inline[j], attr, 1 );
                        } /* for */
                    } /* if */
            } /* end switch */
        } /* end for */
    inline[max_char] = '\0';
    return 0;       /* Return with a full line */

} /* getline */

#pragma trace-
void printa( char *sptr, attr )
/*
  +----------------------------------------------------------------------+
  |  Function printa prints string with designated attribute             |
  |  directly to video ram                                               |
  +----------------------------------------------------------------------+
*/
{
    extern int vidbase;

    int cc, cr, i=0;
    unsigned char far *crtmemptr;

    cc = curscol();
    cr = cursrow();

    crtmemptr = MK_FP( vidbase, (cc << 1) + cr * 160 );
    while(*sptr) {
        *crtmemptr++ = *sptr++;
        *crtmemptr++ = attr;
        i++;
    }
    poscurs( cr, cc+i ); /* Place cursor at end of string */

} /* printa */


#pragma trace-
void show_commands( void )
{
    int i;
    int *uwindow;
    extern char buffer[];
    struct WINDOW help = { 4, 15, 12, 52, "µ HELP Æ", red_b + brtwhite_f };
    char *commands[] = {
                         " VALID COMMANDS:",
                         " \x1b \x18 \x19 \x1a - move into cell",
                         " ENTER   - next cell down",
                         " TAB     - next cell to right",
                         " ESC     - exit the form entry screen",
                         " F1      - clear cell",
                         " F2      - re-edit heading items",
                         " H, ?    - this list",
                         "        ..press any key to continue..",
                         NULL
                         };

    cursoff();
    uwindow = make_window( help, TRUE );
    for( i=0; i<commands[i]!=NULL; i++ ) {
        poscurs( help.topleftrow+i, help.topleftcol );
        sprintf( buffer, "%s", commands[i] );
        printa( buffer, help.attr );
        }
    getch();
    take_window( help, uwindow );
    curson();

} /* show_commands */


#pragma trace-
void ftoa6( float f, char *t ) /* Float to 5 char ascii */
{
        /* Sets up "t" with the proper 5 char numberic string */

    int i;
    char buffer[81];

    if( f == 0 ) {
        strcpy( t, "     0");
        return;
        }
    ftoa( f, buffer, 0, 5, 3 ); /* Convert to ascii */
    while( buffer[0] == ' ' )   /* Remove leading spaces */
        for( i=0; (i<80 && buffer[i]!=NULL); i++ )
            buffer[i] = buffer[i+1];
    if( f < 0F && f > -1F ) {   /* if -1 < f < 0 make it "-.n" */
        strncpy( t, buffer, 6 );      /* get 5 signif digits */
        t[0] = '-';                             /* reinsert "-" */
        }
    else strncpy( t, buffer, 6 ); /* Get 5 most significant digits */
    t[6] = NULL; /* Force end of string */
    while( strlen( t ) < 6 ) {  /* Insert leading space(s) if not 6 chars */
        for( i=5; i!=0; i-- ) t[i] = t[i-1];
        t[0] = ' ';
        }
    return;

} /* ftoa6 */

#pragma trace-
/* Calculate checksum on file f_name[] */
BOOL chksum( char f_name[], int checksum )
{ /* Return TRUE if sum of f_name = checksum */
  int   sum = 0, operand;
  FILE *f_tst;

    f_tst = fopen( f_name, "r" );          /* open file */
    if( f_tst == NULL ) return FALSE;
    while( (operand = fgetc( f_tst )) != EOF ) sum += operand;
    fclose( f_tst );
#ifdef PROTOTYPE
	printf("\nChecksum: 0x%X\n", sum);  /* For AI use only */
#endif
    if( sum == checksum ) return TRUE;
    else return FALSE;
} /* chksum */

#pragma trace-
void signon( void )
{
    extern struct TESTFORM engine;
    extern buffer[];
    extern int model;
    extern int max_point;
    extern BOOL formmade;
    extern char *eng_type[];

    struct WINDOW sign_on = {  1,  1, 23, 78, "µ REDUCE Æ", blue_b + ltblue_f };
    struct WINDOW cur_dat = {  8, 40, 17, 74, "", green_b + blue_f };
    struct WINDOW comment = { 21, 10, 21, 70, "µ COMMENT Æ", green_b + blue_f };
    char *facility[] = { "unspecified",
                         "Wing",
                         "Propeller stand",
                         "Dynamometer stand",
                        };


    clrscrn();
    make_window( sign_on, FALSE ); /* Don't save underlying scn */
    scroll( sign_on, 0, DOWN ); /* Clear screen */
    cursoff();
#ifndef DEMO
    poscurs( 2, 8 );
    printa( "T U R B I N E   D A T A   R E D U C T I O N   P R O C E S S O R",
        blue_b + brtwhite_f );
#else
    /* DEMO = T */
    poscurs( 2, 3 );
    printa( "T U R B I N E   D A T A   R E D U C T I O N   P R O C E S S O R  D E M O",
        blue_b + brtwhite_f );
#endif
#ifndef TRAINING
	poscurs( 4, 14 );
	printa( "Copyright (C)1990,1993 Applied Imagination, Inc.", sign_on.attr );
    poscurs( 5, 24 );
    printa( "All Rights Reserved Worldwide", sign_on.attr );
#else
	poscurs( 3, 14 );
	printa( "Copyright (C)1990,1993 Applied Imagination, Inc.", sign_on.attr );
    poscurs( 4, 24 );
    printa( "All Rights Reserved Worldwide", sign_on.attr );
	poscurs( 5, 17 );
	printa( "Garrett General Aviation Services Division", blue_b + brtwhite_f );
	poscurs( 6, 19 );
	printa( "Technical Training Department use only", blue_b + brtwhite_f );
#endif
    make_window( cur_dat, FALSE ); /* Current data file stats */
    scroll( cur_dat, 0, DOWN );

    if( formmade ) {
        poscurs( cur_dat.topleftrow+1, cur_dat.topleftcol+2 );
        printa( "CURRENT FORM DATA:", cur_dat.attr );
        poscurs( cur_dat.topleftrow+3, cur_dat.topleftcol+1 );
        sprintf( buffer, "Engine model  : %s", eng_type[model] );
        printa( buffer, cur_dat.attr );
        poscurs( cur_dat.topleftrow+4, cur_dat.topleftcol+1 );
        sprintf( buffer, "Engine serial : %lu", engine.serial );
        printa( buffer, cur_dat.attr );
        poscurs( cur_dat.topleftrow+5, cur_dat.topleftcol+1 );
        sprintf( buffer, "Test date     : %02i-%02i-%02i",
                engine.month, engine.day, engine.year );
        printa( buffer, cur_dat.attr );
        poscurs( cur_dat.topleftrow+6, cur_dat.topleftcol+1 );
        sprintf( buffer, "100%% reference: %i RPM", engine.ref_speed );
        printa( buffer, cur_dat.attr );
        poscurs( cur_dat.topleftrow+7, cur_dat.topleftcol+1 );
        sprintf( buffer, "Test facility : %s", facility[engine.facility] );
        printa( buffer, cur_dat.attr );
        poscurs( cur_dat.topleftrow+8, cur_dat.topleftcol+1 );
        sprintf( buffer, "Technician    : %s", engine.tech );
        printa( buffer, cur_dat.attr );
        poscurs( cur_dat.topleftrow+9, cur_dat.topleftcol+1 );
        sprintf( buffer, "Test points   : %i", max_point );
        printa( buffer, cur_dat.attr );
        make_window( comment, FALSE );
        sprintf( buffer, " %s", engine.comment );
        printa( buffer, cur_dat.attr );
        }
    else {
        poscurs( cur_dat.topleftrow+4, cur_dat.topleftcol+9 );
        printa( "No form loaded.", cur_dat.attr );
        }

} /* signon */


#pragma trace-
BOOL set_defaults( char cswitch[] )
{   /* Returns disclaimer flag */
/*
  +----------------------------------------------------------------------+
  | Command line switches:                                               |
  |     P0 = Epson FX-80 (default)      V0 = Auto select video (default) |
  |     P1 = Okidata                    V1 = CGA      640 x 200          |
  |     P2 = HP LaserJet                V2 = EGA      640 x 350          |
  |     L0 = Print thru bios            V3 = VGA      640 x 480          |
  |     L1 = LPT1                       V4 = Hercules 720 x 348          |
  |     L2 = LPT2                       C  = Color on                    |
  |     D  = No disclaimer message      E  = TPE calculation program err |
  |   Usage: C\> REDUCE /P#V#L#DE                                        |
  | Example: C\> REDUCE /P2V2   Sets printer to LaserJet, video to EGA   |
  +----------------------------------------------------------------------+
*/
extern int vidmode, printmode, printer_port;
extern BOOL tpe_err;
extern BOOL use_bios;
extern BOOL color_graph;

    int i;
    BOOL d_flag = TRUE;

    /* Set defaults */
    tpe_err = FALSE;       /* Assume no error           */
    vidmode = 0;           /* Auto video select         */
    printmode = 0;         /* Epson printer             */
    use_bios = FALSE;      /* Don't use bios for print  */
    printer_port = LPT1;   /* Parallel #1               */
    color_graph = FALSE;   /* B/W graphic               */
    set_color( FALSE );    /* No colors                 */

    if( cswitch[0] != '/' ) { /* No swiches- return */
        return d_flag;       /* Print disclaimer */
        }

    /* Get switches */
    for( i=1; cswitch[i]!='\0'; i++ ) {
        switch( toupper( cswitch[i] ) ) {
            case 'C': /* Turn on colors */
                set_color( TRUE );
                if( isdigit( cswitch[i+1] ) ) color_graph = TRUE;
                break;
            case 'D': /* Disclaimer switch */
                d_flag = FALSE;
                break;
            case 'E': /* TPE pgm error */
                tpe_err = TRUE;
                break;
            case 'P': /* Printer protocol switch */
                if( isdigit( cswitch[i+1] ) ) printmode = cswitch[i+1] - 48;
                break;
            case 'V': /* Video mode switch */
                if( isdigit( cswitch[i+1] ) ) vidmode = cswitch[i+1] - 48;
                /* if( vidmode == 2 || vidmode == 3 ) color_graph = TRUE; */
                break;
            case 'L': /* Printer port switch */
                if( cswitch[i+1] == '0' ) { /* Bios switch on */
                    use_bios = TRUE;
                    break;
                    }
                if( isdigit( cswitch[i+1] ) )
                    printer_port = cswitch[i+1] - 49;
                if( (printer_port < 0) || (printer_port > 1) )
                    printer_port = LPT1; /* Force LPT1 if invalid setup */
                break;
            } /* switch */
        } /* for */
    return d_flag; /* return disclaimer flag */

} /* set_defaults */


#pragma trace-
void set_color( BOOL want_color )
{
extern color_monitor;

    if( want_color ) {

        color_monitor = TRUE;

        black_b     = BLACK_B;
        blue_b      = BLUE_B;
        green_b     = GREEN_B;
        cyan_b      = CYAN_B;
        red_b       = RED_B;
        magenta_b   = MAGENTA_B;
        brown_b     = BROWN_B;
        white_b     = WHITE_B;

        black_f     = BLACK_F;
        blue_f      = BLUE_F;
        green_f     = GREEN_F;
        cyan_f      = CYAN_F;
        red_f       = RED_F;
        magenta_f   = MAGENTA_F;
        brown_f     = BROWN_F;
        white_f     = WHITE_F;
        gray_f      = GRAY_F;
        ltblue_f    = LTBLUE_F;
        ltgreen_f   = LTGREEN_F;
        ltcyan_f    = LTCYAN_F;
        ltred_f     = LTRED_F;
        ltmagenta_f = LTMAGENTA_F;
        yellow_f    = YELLOW_F;
        brtwhite_f  = BRTWHITE_F;
        } /* if */

    else { /* no colors */

        color_monitor = FALSE;

        black_b     =
        blue_b      =
        green_b     =
        cyan_b      =
        red_b       =
        magenta_b   =
        brown_b     =
        white_b     = BLACK_B;

        black_f     =
        blue_f      =
        green_f     =
        cyan_f      =
        red_f       =
        magenta_f   =
        brown_f     =
        white_f     =
        gray_f      =
        ltblue_f    =
        ltgreen_f   =
        ltcyan_f    =
        ltred_f     =
        ltmagenta_f =
        yellow_f    = WHITE_F;
        brtwhite_f  = BRTWHITE_F;
        } /* else */
} /* set_color */


void disclaim( void )
{
#ifndef DEMO
    clrscrn();
    poscurs( 0, 0 );
    printf( "PROGRAM LICENSE AGREEMENT\n\n" );
	printf( "Copyright to this software is held by Applied Imagination, Incorporated.\n\n" );
    printf( "         In consideration of your payment of the agreed purchase price\n" );
    printf( "         and your agreement to these terms, you are hereby licensed to\n" );
    printf( "         use the software.\n\n" );
    printf( "TERMS:   You must treat the  software  as if  it  were a  book.    For\n" );
    printf( "         example, you may loan the  software to someone, but you  must\n" );
    printf( "         not retain a copy for your own use if  you do.  You must  not\n" );
	printf( "         allow the  software to  be  used by  any other individual  or\n" );
	printf( "         company to the extent that Applied Imagination, Inc. could be\n" );
    printf( "         deprived of a  sale.   You  agree not  to copy  the  software\n" );
    printf( "         except for the  purpose of  making a backup  to protect  your\n" );
	printf( "         investment.  Applied Imagination, Inc.  does not warrant that\n" );
	printf( "         the software will  meet  your requirements  or  that  it will\n" );
	printf( "         operate without error.\n\n" );
    printf( "         Applied Imagination's liability to you for actual damages for\n" );
    printf( "         any cause whatsoever, and regardless of  the form of  action,\n" );
    printf( "         shall be limited to the amount paid for the software by  you.\n" );

    getch();
#endif
} /* disclaim */

/* eof: TRPFUNC.C */

