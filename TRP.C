 /*
    File: TRP.C
    Turbine Reduction Processor
	Copyright (C)1989, 1990 Applied Imagination, Inc.
    Written by Steven R. Stuart - Sept 1989
*/

#pragma trace-
#pragma nestcmnt

#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>
#include <string.h>
#include <malloc.h>
#include <bios.h>
#include <conio.h>
#include <direct.h>
#include <dos.h>
#include <process.h>    /* Macro constants */
#include "trpdefs.h"    /* constant definitions */
#include "trpvers.h"    /* Macro flags */
#include "trpdclr.c"    /* Declares globals */


/**********  WORK STARTS HERE  ***************/

#pragma trace
int main( int argc, char *argv[], char *envp[] )
{
extern float t4rat_ntry, minshp_ntry, t_nominal, tfp_min, tfp_max;
extern char far *vscreen;       /* Pointer to virtual screen */
extern int vidbase;             /* video text base segment address */
extern char outfile_name[];     /* D.USR */
extern BOOL formmade;
extern int model, program;
extern char *eng_type[];
extern char chk_file[], checksum_err[];
extern int valid_cksum;

extern BOOL epsprint(), okiprint(), lazprint(); /* Declare external functions */


    int horiz, vert, sel, gph_rtn;
    unsigned int byte, dot;
    BOOL online = TRUE, calcmade = FALSE;
    BOOL file_edited = FALSE, graphmade = FALSE;
    char *gev, *datapath;

    /* Array of pointers to print functions */
    int (*prt_array[3])() = { epsprint, okiprint, lazprint };
    /* Graph error messages */
    char *gphmsg[] = {  "Insufficient memory for graphic",
                        "Cannot plot curve with less than 2 points",
                        "Invalid video mode selected",
                        "Cannot detect graphic adapter",
                        "Graphic plot aborted by user",
                        "Invalid return code"
    };
    char printer_err[] = "Print aborted or printer not ON-LINE.";
    char *mainmenu[] = { "New test form entry",
                         "Edit current test form",
                         "Save current form to disk",
                         "Load old test form from disk",
                         "Graph current test record",
                         "Print current test record",
                         "Calculation output viewer",
                         "Delete current form from disk",
                         "Version of program",
                         "eXit to DOS",
                         NULL
    };
    BOOL main_hots[] = { TRUE, FALSE, FALSE, TRUE, FALSE, FALSE,
                              FALSE, FALSE, TRUE, TRUE
    };
    enum { New, Edit, Save, Load, Graph, Print, Calc,
                    Delete, Vers, Exit
    };

    char *eng_menu[] = { "A: TPE331-3", "B: TPE331-5",  "C: TPE331-6",
                         "D: TPE331-8", "E: TPE331-10", "F: TPE331-10UA",
                         "G: TPE331-11", "H: TPE331-1EGT", NULL
                       };
    BOOL eng_hots[] = {

#ifdef MODULE_C
            /*   -3       -5       -6 */
                TRUE,    TRUE,    TRUE,
#else
                FALSE,   FALSE,   FALSE,
#endif
#ifdef MODULE_B
            /*   -8  */
                TRUE,
#else
                FALSE,
#endif
#ifdef MODULE_A
            /*   -10      -10UA    -11      -1EGT(-6 TT5) */
                TRUE,    TRUE,    TRUE,    TRUE
#else
                FALSE,   FALSE,   FALSE,   FALSE
#endif

    };
    char *data_source_menu[] = { "on aircraft Wing",
                                 "on Propeller stand",
                                 "on Dynamometer stand",
                                 NULL
                               };
    BOOL data_hots[] = { TRUE, TRUE, TRUE };

    char *yn[] = { "No", "Yes", NULL };
    BOOL yn_hots[] = { TRUE, TRUE };
    char *prn_menu[] = { "Calculation record", "Graphic", "Both", NULL };
    BOOL prn_hots[] = { FALSE, FALSE, FALSE };
    enum { Rec, Gph, Both };
    struct WINDOW gp_msg = { 10, 10, 11, 30, "", WHITE_F|BLACK_B };



/* The Code.. *********************/

    int (*ptr_array[3])(); /* Array of pointers to print functions */


    /* Get command line switches, disclaimer, err flags */
    if( set_defaults( argv[1] ) ) disclaim(); /* Disclaimer if D flag set */
    if( color_monitor ) gp_msg.attr = red_b + black_f;

    /* video text base segment address */
/*
    if ((biosequip() & 0x30) == 0x30) vidbase = MA;
    else vidbase = CGA;
*/
    vidbase = ((biosequip() & 0x30) == 0x30) ? MA : CGA;
    vscreen = farmalloc( 0x9600L ); /* Allocate virtual graphic screen */
    if( vscreen == NULL ) {
        say_error( gphmsg[0] ); /* Insufficient memory for graph function */
    }

/* !!! */
    datapath = getenv( "RFILES" );

#ifndef DEMO

    /* Make graphic if returning from calculation and no error */
    gev = getenv( "GRAPH" );  /* Get environmental graph flag */
    /* Get current form if it is there */
    if( gev != NULL ) {
        load_params( &file_edited, &model, &program ); /* Load model & program nr    */
        formmade = file2edit( outfile_name ); /* Set formmade if success */
        main_hots[Edit] = TRUE;
        calcmade = retrieve();
        if( calcmade ) { /* Get Garret output if it exists */
            main_hots[Calc] = TRUE;
        }
    }
    /* Make graph if returning from successful calc */
    if( (! tpe_err) && (gev != NULL) ) {   /* If flag is set.. */
        signon();       /* Show our name */
        make_window( gp_msg, FALSE );
        printa( "Stand-by for", gp_msg.attr );
        poscurs( gp_msg.topleftrow+1, gp_msg.topleftcol );
        printa( "Recompensation Plot", gp_msg.attr );
        if( calcmade ) {    /* Retrieve Garret file data  */
            main_hots[Save] =
            main_hots[Load] =
            main_hots[Graph]=
            main_hots[Print]=
            prn_hots[Rec]   = TRUE;
            main_hots[Delete] = FALSE;

            /* Call graphic func., get return code */
            gph_rtn = graph_();
            graphmade = prn_hots[Gph] = FALSE;
            switch( gph_rtn ) {
                case 0: /* Normal return */
                    graphmade = prn_hots[Gph] = TRUE;
                    break;
                case -1:
                    say_error( gphmsg[0] ); /* Insuffient mem */
                    break;
                case 1:
                    say_error( gphmsg[1] ); /* Cannot plot curve < 2 points */
                    break;
                case 2:
                    say_error( gphmsg[2] ); /* Invalid video mode selected */
                    break;
                case 3:
                    say_error( gphmsg[3] ); /* Cannot detect graphic adapter */
                    break;
                case 4:
                    say_error( gphmsg[4] ); /* Invalid return code */
                    break;
                default:
                    say_error( gphmsg[5] ); /* Graphic plot aborted by user */
                    break;
            }
        }

    }
#endif

    while( online ) {
        signon();
        if( tpe_err ) {
            tpe_err = FALSE;    /* Reset error */
            version();
            say_error( "TPE calculation program returned an error code." );
            signon();
            }
        sel = popup( 7, 5, mainmenu, main_hots );
        switch( sel ) {

    /* CLEAR form */
            case MENU_NEW:
                if( formmade && file_edited ) {
                    sound( 900, 5 );
                    make_window( gp_msg, FALSE );
                    scroll( gp_msg, 1, UP );
                    printa( "Form not saved.", gp_msg.attr );
                    scroll( gp_msg, 1, UP );
                    printa( "New anyway?", brtwhite_f + red_b );
                    if( popup( 14, 17, yn, yn_hots ) != 1 ) break; /* Don't load */
                    }
                engine.month =          /* Clear date */
                engine.day =
                engine.year =
                engine.meter_k =
                engine.meter_lim = 0;
                engine.comment[0] = '\0'; /* Clear comments, tech inits */
                engine.serial = 0L;     /* Clear engine serial number */
                engine.sg =
                engine.lhv =
                engine.tt5c = 0F;
                for( horiz=0; horiz<10; horiz++ )
                    for( vert=0; vert<10; vert++ )
                        engine.cell[horiz][vert] = 0F; /* Clear cells */
                calcmade = graphmade = formmade = file_edited = FALSE;
                /* Select engine type */
                sel = popup( 7, 5, eng_menu, eng_hots );
                if( sel == -1 ) break; /* Back to top */
                switch( sel ) {

#ifdef MODULE_C
                    case 0: /* tpe331-3 */
                        engine.ref_speed = REFSPEED_3;
                        t_nominal =            TNOMINAL_3;
                        t4rat_ntry =           T4RATNTRY_3;
                        minshp_ntry =          MINSHPNTRY_3;
                        tfp_min =              TFPMIN_3;
                        tfp_max =              TFPMAX_3;
                        model =                MDL_TPE3;
                        program = TPE356; /* errorlevel 12 */
                        break;

                    case 1: /* tpe331-5 */
                        engine.ref_speed = REFSPEED_5;
                        t_nominal =            TNOMINAL_5;
                        t4rat_ntry =           T4RATNTRY_5;
                        minshp_ntry =          MINSHPNTRY_5;
                        tfp_min =              TFPMIN_5;
                        tfp_max =              TFPMAX_5;
                        model = MDL_TPE5;
                        program = TPE356; /* errorlevel 12 */
                        break;

                    case 2: /* tpe331-6 */
                        engine.ref_speed = REFSPEED_6;
                        t_nominal =            TNOMINAL_6;
                        t4rat_ntry =           T4RATNTRY_6;
                        minshp_ntry =          MINSHPNTRY_6;
                        tfp_min =              TFPMIN_6;
                        tfp_max =              TFPMAX_6;
                        model = MDL_TPE6;
                        program = TPE356; /* errorlevel 12 */
                        break;
#endif

#ifdef MODULE_B
                    case 3: /* tpe331-8 */
                        engine.ref_speed = REFSPEED_8;
                        t_nominal =            TNOMINAL_8;
                        t4rat_ntry =           T4RATNTRY_8;
                        minshp_ntry =          MINSHPNTRY_8;
                        tfp_min =              TFPMIN_8;
                        tfp_max =              TFPMAX_8;
                        model = MDL_TPE8;
                        program = TPE8; /* errorlevel 11 */
                        break;
#endif

#ifdef MODULE_A
                    case 4: /* tpe331-10 */
                        engine.ref_speed = REFSPEED_10;
                        t_nominal =            TNOMINAL_10;
                        t4rat_ntry =           T4RATNTRY_10;
                        minshp_ntry =          MINSHPNTRY_10;
                        tfp_min =              TFPMIN_10;
                        tfp_max =              TFPMAX_10;
                        model = MDL_TPE10;
                        program = TPE10; /* errorlevel 10 */
                        break;

                    case 5: /* tpe331-10ua */
                        engine.ref_speed = REFSPEED_10UA;
                        t_nominal =            TNOMINAL_10UA;
                        t4rat_ntry =           T4RATNTRY_10UA;
                        minshp_ntry =          MINSHPNTRY_10UA;
                        tfp_min =              TFPMIN_10UA;
                        tfp_max =              TFPMAX_10UA;
                        model = MDL_TPE10UA;
                        program = TPE10; /* errorlevel 10 */
                        break;

                    case 6: /* tpe331-11 */
                        engine.ref_speed = REFSPEED_11;
                        t_nominal =            TNOMINAL_11;
                        t4rat_ntry =           T4RATNTRY_11;
                        minshp_ntry =          MINSHPNTRY_11;
                        tfp_min =              TFPMIN_11;
                        tfp_max =              TFPMAX_11;
                        model = MDL_TPE11;
                        program = TPE10; /* errorlevel 10 */
                        break;

                    case 7: /* tpe331-1EGT */
                        engine.ref_speed = REFSPEED_1EGT;
                        t_nominal =            TNOMINAL_1EGT;
                        t4rat_ntry =           T4RATNTRY_1EGT;
                        minshp_ntry =          MINSHPNTRY_1EGT;
                        tfp_min =              TFPMIN_1EGT;
                        tfp_max =              TFPMAX_1EGT;
                        model = MDL_TPE1EGT;
                        program = TPE1EGT; /* errorlevel 13 */
                        break;
#endif

                    default: /* Module not included */
                        model = MODULE_NA;
                        } /* switch */
                if( model == MODULE_NA ) {
                    sprintf( buffer,
                            "Module for %s engine is not available",
                            eng_type[sel] );
                    say_error( buffer );
                    break;
                    }

                sel = popup( 7, 5, data_source_menu, data_hots );
                if( sel == -1 ) break; /* Back to top */
                switch( sel ) {
                    case 0:
                        engine.facility = ON_WING;
                        break;
                    case 1:
                        engine.facility = PROP_STAND;
                        break;
                    case 2:
                        engine.facility = DYNA_STAND;
                        break;
                    } /* switch */

                nomenclature( model ); /* Setup nomenclature string ptrs */
                makeform();     /* Create full screen form */
                /* Place current entries in form (from pt. 0) */
                fill_display( 0 );
                if( get_head() == TRUE )
                    if( get_data() == TRUE ) {    /* Get engine parameters */
                        put_in_file( outfile_name ); /* Make input file for fortran */
                        /* Store model & program nr    */
                        formmade = TRUE; /* form data is in memory */
                        file_edited = TRUE;
                        main_hots[Edit] =
                        main_hots[Save] =
                        main_hots[Load] =
                        main_hots[Graph]= TRUE;
                        main_hots[Print]=
                        main_hots[Calc] =
                        main_hots[Delete]=
                        prn_hots[Rec] =
                        prn_hots[Gph] =
                        prn_hots[Both]= FALSE;
                        }
                break;

    /* EDIT form */
            case MENU_EDIT:
                if( !formmade ) { /* no form to edit */
                    say_error( "There is no data in the form." );
                    break;
                }
                calcmade = graphmade = FALSE;
                makeform();     /* Create full screen form */
                /* Place current entries in form (from pt. 0) */
                fill_display( 0 );
                if( get_head() ) get_data(); /* Get engine parameters */
                put_in_file( outfile_name ); /* Make input file for fortran */
                file_edited = TRUE;
                main_hots[Save] =
                main_hots[Graph]= TRUE;
                main_hots[Print]=
                main_hots[Calc] =
                main_hots[Delete]= FALSE;
                break;

    /* SAVE current record */
            case MENU_SAVE:
#ifndef DEMO
                /* Calculate checksum if not demo version */
                if( ! chksum( chk_file, valid_cksum ) ) {
                    say_error( checksum_err );
                    exit( 1 );
                }
#endif
                if( ! formmade ) say_error( "No form is loaded...cannot save." );
                else if( ! file_edited )
                    say_error( "Form has not been changed." );
                else {
                    if( savefile() ) {
                        make_window( gp_msg, FALSE );
                        printa( "File saved", gp_msg.attr );
                        file_edited = FALSE;
                        curfile[0] = '\0';   /* Clear current filename */
                        sleep( 2 );
                        main_hots[Save] = FALSE;
                    }
                    else say_error( "File could not be saved." );
                }
                break;

    /* LOAD form from disk */
            case MENU_LOAD:
                if( !main_hots[Load] ) break;
                if( formmade && file_edited ) {
                    sound( 900, 5 );
                    make_window( gp_msg, FALSE );
                    scroll( gp_msg, 1, UP );
                    printa( "Form not saved.", gp_msg.attr );
                    scroll( gp_msg, 1, UP );
                    printa( "Load anyway?", brtwhite_f + red_b );
                    if( popup( 14, 17, yn, yn_hots ) != 1 ) break; /* Don't load */
                }
                if( getfiles() ) {   /* If a good load.. */
                    put_in_file( outfile_name );  /*  put in D.USR    */
                    nomenclature( model );
                    calcmade = graphmade = file_edited = FALSE;
                    main_hots[Calc] =
                    main_hots[Graph]=
                    main_hots[Edit] =
                    main_hots[Delete]= TRUE;
                    main_hots[Save] =
                    main_hots[Print]=
                    main_hots[Calc] = FALSE;
                }
                break;

    /* GRAPH current record */
            case MENU_GRAPH:
                if( vscreen == NULL ) {
                    say_error( gphmsg[0] ); /* Insufficient memory for graph function */
                    break;
                }
                if( ! formmade ) {
                    say_error( "No form has been created or loaded." );
                    break;
                }
                if( ! calcmade ) {
                    store_params( &file_edited, &model, &program ); /* Save model & program nr, etc. */
                    /* NOTE: "calculate()" function does not return */
                    calculate();    /* Make external calculations */
                } /* if !  calcmade */
#ifdef DEMO
             /* DEMO = T */
                max_point = NR_DEMO_PTS;
                calcmade = prn_hots[Rec] = main_hots[Calc] = TRUE;
                signon();
                make_window( gp_msg, FALSE );
                printa( "NOTE: Demo plot is", gp_msg.attr );
                poscurs( gp_msg.topleftrow+1, gp_msg.topleftcol );
                printa( "   always the same.", gp_msg.attr );
                sleep( 3 );
#endif
                /* Call graphic func., get return code */
                gph_rtn = graph_();
                graphmade = prn_hots[Gph] = FALSE;
                switch( gph_rtn ) {
                    case 0: /* Normal return */
                        graphmade =
							prn_hots[Gph] =
                        	main_hots[Print]=
	                        prn_hots[Rec] =
    	                    prn_hots[Gph] =
        	                prn_hots[Both]= TRUE;
                        break;
                    case -1:
                        say_error( gphmsg[0] ); /* Error messages above */
                        break;
                    case 1:
                        say_error( gphmsg[1] );
                        break;
                    case 2:
                        say_error( gphmsg[2] );
                        break;
                    case 3:
                        say_error( gphmsg[3] );
                        break;
                    case 4:
                        say_error( gphmsg[4] );
                        break;
                    default:
                        say_error( gphmsg[5] );
                        break;
                }
                break;

    /* PRINT current test record */
            case MENU_PRINT:
                /* First check that printer is ready */
                if( (! use_bios) &&
                    (! SELECTED( biosprint( STAT_PRT, NULL, printer_port )))
                    ) {
                    say_error( printer_err );
                    break;
                }
                prn_hots[Both] =  /* Setup 'Both' hotkey */
                    (prn_hots[Rec] && prn_hots[Gph] ) ? TRUE : FALSE;

                /* Find out what the user wants to print */
                make_window( gp_msg, FALSE );
                printa( "Print: ", gp_msg.attr );
                sel = popup( 14, 10, prn_menu, prn_hots );
                if( sel == -1 ) break; /* Back to top */
                make_window( gp_msg, FALSE );
                printa( "Printing..", gp_msg.attr );
                switch( sel ) {
                    case 0: case 2:  /* Calc file */
                        /* print D.CAL, error msg if nonexistant */
                        if( ! pt_calcdata() ) say_error( "Calculation file not found" );
                        /* Fall through */
                    case 1:          /* Graphic  or both */
                        if( sel == 0 ) break; /* 'Calc file' only selected */
                        if( ! graphmade ) {
                            say_error( "Graphic has not been created on the current form." );
                            break;
                        }
                        setvmode( gmode );          /* Enter video graphic mode */
                        restore_scn( vscreen );     /* Load last recomp graph */
                        /* Go to appropriate printer routine */
                        if( !(*prt_array[printmode])() ) {
                            setvmode( DEFAULTMODE ); /* Return screen to text */
                            signon();
                            say_error( printer_err );
                        }
                        setvmode( DEFAULTMODE ); /* Return screen to text */
                        break;
                } /* switch */
                prn_hots[Both] = FALSE; /* Reset 'Both' hotkey */
                break;

/*********************************************************************
    /* Make blank form */
            case ?:
                sel = popup( 7, 5, eng_menu, eng_hots );
                if( sel == -1 ) break;
                sel = popup( 7, 5, data_source_menu, data_hots );
                if( sel == -1 ) break;
                say_error( "Blank form print function not yet implemented (2/90)" );
                pt_form();
                break;
*********************************************************************/

    /* DELETE file */
            case MENU_DELETE:
                if( curfile[0] == NULL ) {
                    say_error( "You must LOAD file before you DELETE it." );
                    break;
                    }
#ifdef DEMO
                make_window( gp_msg, FALSE );
                printa( "Demo won't", gp_msg.attr );
                poscurs( gp_msg.topleftrow+1, gp_msg.topleftcol );
                printa( "delete files.", gp_msg.attr );
                sleep( 2 );
#else
                make_window( gp_msg, FALSE );
                printa( "Verify: ", gp_msg.attr );
                printa( "DELETE FILE?", brtwhite_f + red_b );
                if( popup( 14, 17, yn, yn_hots ) != 1 ) break; /* Don't delete */
                if( delete( curfile ) ) {
                    make_window( gp_msg, FALSE );
                    printa( "File deleted.", gp_msg.attr );
                    curfile[0] = '\0';  /* Clear current file name */
                    sleep( 2 );
                    break;
                    }
#endif
                main_hots[Delete] = FALSE;
                break;

    /* VERSION */
            case MENU_VERSION:
                cursoff();
                version(); /* Display program version info */
                getch();
                curson();
                break;

    /* CALCULATION output */
            case MENU_VIEW:
                if( ! calcmade )
                    say_error( "First you must select 'Graph' to complete calculations." );
                else calc_out();
                break;
    /* QUIT */
            case MENU_EXIT:
#ifndef DEMO
                make_window( gp_msg, FALSE );
                printa( "Verify: ", gp_msg.attr );
                printa( "EXIT ?", brtwhite_f + red_b );
                if( popup( 14, 17, yn, yn_hots ) != 1 ) break; /* Don't quit */
                if( formmade && file_edited ) {
                    sound( 900, 5 );
                    make_window( gp_msg, FALSE );
                    scroll( gp_msg, 1, UP );
                    printa( "Form not saved.", gp_msg.attr );
                    scroll( gp_msg, 1, UP );
                    printa( "Exit anyway?", brtwhite_f + red_b );
                    if( popup( 14, 17, yn, yn_hots ) != 1 ) break; /* Don't quit */
                    }
                delete( infile_name );
#endif
#ifdef PROTOTYPE
                exitmsg();
#endif
                delete( errfile_name ); /* Remove temporary files  */
                delete( outfile_name );
                delete( holdfile_name );
                farfree( vscreen );     /* Release virtual graphic */
                online = FALSE;         /* Drop out of main()   */
                clrscrn();

            } /* switch */
        }


    exit( 0 );

} /* main */

/* eof: TRP.C */
