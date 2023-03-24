/*
    File: TRPFORM.C
    Section of TRP.C
	Copyright (C)1989,1990 Applied Imagination, Inc.
    Written by Steven R. Stuart - Sept 1989
    Update Sept 1990: Change screen form to display only
                      4 points each holding 6 digits.
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

extern int black_b, blue_b, green_b, cyan_b, red_b, magenta_b, brown_b, white_b;
extern int black_f, blue_f, green_f, cyan_f, red_f, magenta_f, brown_f, white_f;
extern int gray_f, ltblue_f, ltgreen_f, ltcyan_f, ltred_f;
extern int ltmagenta_f, yellow_f, brtwhite_f;


#pragma trace-
BOOL fill_display( int first_pt )
{
    extern char *pt_head[], *unit[], *units_ptr[];
    extern char buffer[]; /* Buffer for printa subr */
    extern struct TESTFORM engine;

    int r, c;
    char asc[7];
    int wrow = 2, wcol = 20; /* window topleft from "get_head() */

    cursoff();

    if( first_pt == -1 ) { /* Fill header */

        poscurs( wrow, wcol+22 );                   /* Serial nr */
        sprintf( buffer, "%-6lu  ", engine.serial );
        printa( buffer, brown_b + yellow_f );

        poscurs( wrow+1, wcol+11 );                 /* date */
        sprintf( buffer, "%02i-%02i-%02i   ",
                         engine.month,
                         engine.day,
                         engine.year );
        printa( buffer, brown_b + yellow_f );

        poscurs( wrow+2, wcol+12 );                 /* Tech inits */
        sprintf( buffer, "%4s", engine.tech );
        printa( buffer, brown_b + yellow_f );

        poscurs( wrow+3, wcol+23 );                 /* spec grav. */
        sprintf( buffer, "%01.5f", engine.sg );
        printa( buffer, brown_b + yellow_f );

        if( engine.facility != ON_WING ) { /* dyno / propstand */
            poscurs( wrow+4, wcol+26 );             /* lwr htg val */
            sprintf( buffer, "%5.0f ", engine.lhv );
            printa( buffer, brown_b + yellow_f );
            } /* if !onwing */

        else { /* on wing */
            poscurs( wrow+4, wcol+20 );             /* meter const */
            sprintf( buffer, "%i   ", engine.meter_k );
            printa( buffer, brown_b + yellow_f );

            poscurs( wrow+5, wcol+17 );             /* meter limit */
            sprintf( buffer, "%i    ", engine.meter_lim );
            printa( buffer, brown_b + yellow_f );

            poscurs( wrow+6, wcol+18 );             /* tt5 constant */
            sprintf( buffer, "%5.0f  ", engine.tt5c );
            printa( buffer, brown_b + yellow_f );

            } /* else on wing */

        } /* if 1stpt = -1 */

    else { /* Fill grid starting at 'first_pt' */
        for( c=0; c<4; c++ ) {
            poscurs( 1, c*7+52 );
            sprintf( buffer, "%-5s", pt_head[first_pt+c] );
            printa( buffer, white_b + blue_f );
            for( r=0; r<=((units_ptr[11]==NULL) ? 9 : 10); r++ ) {
                poscurs( r*2+3, c*7+51 );
                ftoa6( engine.cell[first_pt+c][r], asc ); /* Convert to ascii */
                sprintf( buffer, "%s", asc );
                printa( buffer, white_b + yellow_f );
                } /* for r */
            } /* for c */
        unit_adjust( first_pt );
        } /* else */
    curson();

} /* fill_display */

#pragma trace-
void unit_adjust( int cur_col )
{
extern struct TESTFORM engine;
extern char *unit[], *units_ptr[];
extern char buffer[];

        if( engine.facility != ON_WING ) {
            /* If Pt3 < 200 then psig else in-Hg */
            units_ptr[2] = (engine.cell[cur_col][1] < 200) ? unit[1] : unit[0];
            poscurs( 5, 44 );
            sprintf( buffer, "%6s", units_ptr[2] );
            printa( buffer, white_b + blue_f );

            /* If TQ < 10000 then ft-lbs else in_lbs */
            units_ptr[8] = (engine.cell[cur_col][7] < 10000) ? unit[5] : unit[6];
            poscurs( 17, 44 );
            sprintf( buffer, "%6s", units_ptr[8] );
            printa( buffer, white_b + blue_f );
            } /* if on_wing */

} /* unit_adjust */



#pragma trace-
BOOL get_head( void ) /* Get heading data */
{
    extern char *heading[];
    extern struct TESTFORM engine;
    extern char buffer[];

    int j, result;
    char *tokn, *part;
    int *under_head, *under_window;
    struct WINDOW head    = {  2, 20,  0, 60, "", brown_b + blue_f };
    struct WINDOW comment = { 14, 10, 14, 70, "µ COMMENT Æ", brown_b + blue_f };

    /* Fill in display header */
    head.botrightrow = (engine.facility==ON_WING) ? 8 : 6;
    under_head = make_window( head, TRUE );

    poscurs( head.topleftrow, head.topleftcol );
    printa( heading[0], head.attr );                /* sn */
    poscurs( head.topleftrow+1, head.topleftcol );
    printa( heading[1], head.attr );                /* date */
    poscurs( head.topleftrow+2, head.topleftcol );
    printa( heading[2], head.attr );                /* tech */
    poscurs( head.topleftrow+3, head.topleftcol );
    printa( heading[3], head.attr );                /* sg */
    poscurs( head.topleftrow+4, head.topleftcol );
    if( engine.facility != ON_WING ) { /* dyno / propstand */
        printa( heading[4], head.attr );            /* LHV */
        poscurs( head.topleftrow+4, head.topleftcol+33 );
        printa( heading[5], head.attr );            /* btu/lb */
        } /* if ! on wing */
    else { /* on wing */
        printa( heading[6], head.attr );            /* meter_k */
        poscurs( head.topleftrow+5, head.topleftcol );
        printa( heading[7], head.attr );            /* meter_lim */
        poscurs( head.topleftrow+6, head.topleftcol );
        printa( heading[8], head.attr );            /* Tt5 comp */
        poscurs( head.topleftrow+6, head.topleftcol+26 );
        printa( "\xf8F", head.attr );               /* degs F */
        } /* else on wing */

    fill_display( -1 );         /* Fill in current entries */
    under_window = make_window( comment, TRUE );
    poscurs( comment.topleftrow, 11 );
    sprintf( buffer, "%58s", engine.comment );
    printa( buffer, brown_b + brtwhite_f );

    /* Get serial number */
    do {
        poscurs( head.topleftrow, head.topleftcol+22 );
        sprintf( buffer, "%-6lu  ", engine.serial );
        printa( buffer, brown_b + brtwhite_f );
        poscurs( head.topleftrow, head.topleftcol+22 );
        if( ( (result = getline( buffer, 6 )) == 0 ) && buffer[0] != NULL ) {
            engine.serial = atol( buffer ); /* Get input */
            }
        if( result == -1 ) { /* Exit form entry */
            take_window( head, under_head );
            take_window( comment, under_window );
            return FALSE;
            }
        }
    while( engine.serial == 0 );
    fill_display( -1 ); /* Fill heading of display */

    /* Get date */
    do {
        poscurs( head.topleftrow+1, head.topleftcol+11 );
        sprintf( buffer, "%02i-%02i-%02i   ",
                         engine.month,
                         engine.day,
                         engine.year );
        printa( buffer, brown_b + brtwhite_f );
        poscurs( head.topleftrow+1, head.topleftcol+11 );
        if( ( (result = getline( buffer, 9 )) == 0 ) && buffer[0] != NULL ) {
            part = buffer; /* Point to buffer first time around */
            for( j=0; j<3; j++ ) { /* Get date parts */
                tokn = strtok( part, "/- ." );
                if( tokn == NULLCHARPTR ) break;
                part = NULLCHARPTR;
                switch( j ) { /* Form start date */
                    case 0: /* Month */
                        engine.month = atoi( tokn );
                        break;
                    case 1: /* Day */
                        engine.day = atoi( tokn );
                        break;
                    case 2: /* Year */
                        engine.year = atoi( tokn );
                        break;
                    } /* switch */
                } /* for */
            } /* if */
        else if( result == -1 ) { /* Exit form entry */
            take_window( head, under_head );
            take_window( comment, under_window );
            return FALSE;
            }
        } /* do */
    while( engine.month <= 0 || engine.month > 99 ||
           engine.day   <= 0 || engine.day   > 99 ||
           engine.year  <= 0 || engine.year  > 99 );
    fill_display( -1 ); /* Fill heading of display */

    /* Get tech inits */
    poscurs( head.topleftrow+2, head.topleftcol+12 );
    sprintf( buffer, "%4s", engine.tech );
    printa( buffer, brown_b + brtwhite_f );
    poscurs( head.topleftrow+2, head.topleftcol+12 );
    if( ( (result = getline( buffer, 4 )) == 0 ) && buffer[0] != NULL )
        strcpy( engine.tech, buffer ); /* Get input */
    else if( result == -1 ) { /* Exit form entry */
        take_window( head, under_head );
        take_window( comment, under_window );
        return FALSE;
        }
    fill_display( -1 ); /* Fill heading of display */

    /* Get Fuel SG */
    do {
        poscurs( head.topleftrow+3, head.topleftcol+23 );
        sprintf( buffer, "%01.5f", engine.sg );
        printa( buffer, brown_b + brtwhite_f );
        poscurs( head.topleftrow+3, head.topleftcol+23 );
        if( ( (result = getline( buffer, 7 )) == 0 ) && buffer[0] != NULL )
           engine.sg = atof( buffer );
        else if( result == -1 ) { /* Exit form entry */
            take_window( head, under_head );
            take_window( comment, under_window );
            return FALSE;
            } /* else */
        } /* do */
    while( engine.sg == 0 );
    fill_display( -1 ); /* Fill heading of display */

    if( engine.facility == ON_WING ) {

        /* Meter constant */
        do {
            poscurs( head.topleftrow+4, head.topleftcol+20 );
            sprintf( buffer, "%i   ", engine.meter_k );
            printa( buffer, brown_b + brtwhite_f );
            poscurs( head.topleftrow+4, head.topleftcol+20 );
            if( ( (result = getline( buffer, 5 )) == 0 ) && buffer[0] != NULL )
               engine.meter_k = atoi( buffer );
            else if( result == -1 ) { /* Exit form entry */
                take_window( head, under_head );
                take_window( comment, under_window );
                return FALSE;
                }
            } /* do */
        while( engine.meter_k == 0 );
        fill_display( -1 ); /* Fill heading of display */

        /* Meter limit */
            poscurs( head.topleftrow+5, head.topleftcol+17 );
            sprintf( buffer, "%i    ", engine.meter_lim );
            printa( buffer, brown_b + brtwhite_f );
            poscurs( head.topleftrow+5, head.topleftcol+17 );
            if( ( (result = getline( buffer, 5 )) == 0 ) && buffer[0] != NULL )
               engine.meter_lim = atoi( buffer );
            else if( result == -1 ) { /* Exit form entry */
                take_window( head, under_head );
                take_window( comment, under_window );
                return FALSE;
                }
        fill_display( -1 ); /* Fill heading of display */

        /* Tt5 compensation */
            poscurs( head.topleftrow+6, head.topleftcol+18 );
            sprintf( buffer, "%5.0f  ", engine.tt5c );
            printa( buffer, brown_b + brtwhite_f );
            poscurs( head.topleftrow+6, head.topleftcol+18 );
            if( ( (result = getline( buffer, 6 )) == 0 ) && buffer[0] != NULL )
               engine.tt5c = atof( buffer );
            else if( result == -1 ) { /* Exit form entry */
                take_window( head, under_head );
                take_window( comment, under_window );
                return FALSE;
                }
        fill_display( -1 ); /* Fill heading of display */
        } /* if */

    else { /* Dyno or propstand */
        /* Get fuel LHV */
        do {
            poscurs( head.topleftrow+4, head.topleftcol+26 );
            sprintf( buffer, "%5.0f ", engine.lhv );
            printa( buffer, brown_b + brtwhite_f );
            poscurs( head.topleftrow+4, head.topleftcol+26 );
            if( ( (result = getline( buffer, 6 )) == 0 ) && buffer[0] != NULL )
                engine.lhv = atof( buffer );
            else if( result == -1 ) { /* Exit form entry */
                take_window( head, under_head );
                take_window( comment, under_window );
                return FALSE;
                }
            }
        while( engine.lhv == 0 );
        fill_display( -1 ); /* Fill heading of display */
        } /* else */

    /* Technician's comments */
    poscurs( comment.topleftrow, 11 );
    if( ( (result = getline( buffer, 58 )) == 0 ) && buffer[0] != NULL )
        strcpy( engine.comment, buffer ); /* Get the input */
    else if( result == -1 ) { /* Exit form entry */
        take_window( comment, under_window );
        take_window( head, under_head );
        return FALSE;
        }
    take_window( comment, under_window );
    take_window( head, under_head );

    /* Header info entered */
    return TRUE;

} /* get_head */


#pragma trace-
BOOL get_data( void )
{
/*  rel_point = relative test point, phy_point = physical screen location pt. */
    extern struct TESTFORM engine;
    extern BOOL formmade;
    extern char *units_ptr[];
    extern char buffer[];

    int i;
    int rel_point=0, phy_point=0, item=0, position;
    char c, temp[7];
    BOOL done = FALSE, new_cell = TRUE, f1 = TRUE, f2 = TRUE;

    ftoa6( engine.cell[0][0], temp ); /* Convert to ascii */
    while( !done ) {
        poscurs( item*2+3, phy_point*7+51 );
        sprintf( buffer, "%s", temp );
        printa( buffer, blue_b + brtwhite_f );
        poscurs( item*2+3, phy_point*7+56 );
        switch( c=getch() ) {
            case FUNCTION: /* It's a function key */
                engine.cell[rel_point][item] = atof( temp );
                ftoa6( engine.cell[rel_point][item], temp ); /* Convert to ascii */
                poscurs( item*2+3, phy_point*7+51 );
                sprintf( buffer, "%s", temp );
                printa( buffer, white_b + yellow_f );
                switch( c=getch() ) { /* ..so get the rest */
                    case UPAROW:
                        new_cell = TRUE;
                        if( --item < 0 ) {
                            item = (units_ptr[11]==NULL) ? 9: 10;
                            sound( 2000, 2 );
                            }
                        new_cell = TRUE;
                        ftoa6( engine.cell[rel_point][item], temp ); /* Convert to ascii */
                        break;
                    case DNAROW:
                        new_cell = TRUE;
                        if( ++item > ((units_ptr[11]==NULL) ? 9 : 10) ) {
                            item = 0;
                            sound( 2000, 2 );
                            }
                        new_cell = TRUE;
                        ftoa6( engine.cell[rel_point][item], temp ); /* Convert to ascii */
                        break;
                    case LTAROW: case BACKTAB:
                        new_cell = TRUE;
                        rel_point--;
                        if( --phy_point < 0 ) {
                            phy_point = 0;
                            if( rel_point < 0 ) {
                                rel_point = 0;
                                sound( 200, 5 );
                                break; /* from "if( --phy_.." */
                                }
                            else {
                                new_cell = TRUE;
                                fill_display( rel_point );
                                }
                            } /* if */
                        ftoa6( engine.cell[rel_point][item], temp ); /* Convert to ascii */
                        break;
                    case RTAROW:
                        new_cell = TRUE;
                        rel_point++;
                        if( ++phy_point > 3 ) {
                            phy_point = 3;
                            if( rel_point > 9 ) {
                                rel_point = 9;
                                sound( 200, 5 );
                                break; /* from if( ++phy_.. */
                                }
                            else {
                                new_cell = TRUE;
                                fill_display( rel_point-3 );
                                }
                            } /* if */
                        ftoa6( engine.cell[rel_point][item], temp ); /* Convert to ascii */
                        break;
                    case FUNC1: /* Clear cell */
                        engine.cell[rel_point][item] = 0;
                        ftoa6( engine.cell[rel_point][item], temp ); /* Convert to ascii */
                        new_cell = FALSE;
                        break;
                    case FUNC2: /* Return to heading edit */
                        get_head();
                        break;
                    default: sound( 200, 5 );
                    break;
                    } /* switch FUNC */
                break; /* case FUNCTION */

            case ENTER:
                new_cell = TRUE;
                engine.cell[rel_point][item] = atof( temp );
                /* If at point 1 barometer item, make all points equal */
                if( formmade == FALSE  &&  /* If making new form       */
                        rel_point == 0 &&  /* and at first point       */
                        item == 0      &&  /* at first item            */
                        f1 == TRUE ) {     /* and didn't do this yet.. */
                    for( i=1; i<10; i++ ) engine.cell[i][0] = engine.cell[0][0];
                    fill_display( 0 );
                    f1 = FALSE;
                    }
                /* If at point 1 fuel temperature, make all points equal */
                if( formmade == FALSE  &&  /* If making new form       */
                        rel_point == 0 &&  /* and at first point       */
                        item == 9      &&  /* at last item             */
                        f2 == TRUE ) {     /* and didn't do this yet.. */
                    for( i=1; i<10; i++ ) engine.cell[i][9] = engine.cell[0][9];
                    fill_display( 0 );
                    f2 = FALSE;
                    }
                ftoa6( engine.cell[rel_point][item], temp ); /* Convert to ascii */
                poscurs( item*2+3, phy_point*7+51 );
                sprintf( buffer, "%s", temp );
                printa( buffer, white_b + yellow_f );
                if( ++item > ((units_ptr[11]==NULL) ? 9 : 10) ) {
                    sound( 2000, 1 );
                    item = 0;
                    rel_point++;
                    if( ++phy_point > 3 ) {
                        phy_point = 3;
                        if( rel_point > 9 ) {
                            rel_point = 9;
                            sound( 200, 5 );
                            } /* if */
                        else fill_display( rel_point-3 );
                        } /* if */
                    } /* if */
                ftoa6( engine.cell[rel_point][item], temp ); /* Convert to ascii */
                break;
            case TAB:
                new_cell = TRUE;
                engine.cell[rel_point][item] = atof( temp );
                ftoa6( engine.cell[rel_point][item], temp ); /* Convert to ascii */
                poscurs( item*2+3, phy_point*7+51 );
                sprintf( buffer, "%s", temp );
                printa( buffer, white_b + yellow_f );
                rel_point++;
                if( ++phy_point > 3 ) {
                    phy_point = 3;
                    if( rel_point > 9 ) {
                        rel_point = 9;
                        sound( 200, 5 );
                        break; /* from if( ++phy_.. */
                        }
                    else {
                        new_cell = TRUE;
                        fill_display( rel_point-3 );
                        }
                    } /* if */
                ftoa6( engine.cell[rel_point][item], temp ); /* Convert to ascii */
                break;
            case BACKSP:
                for( position = 4; position > -1; position-- )
                    temp[position] = temp[position-1];
                temp[0] = ' ';
                new_cell = FALSE;
                break;
            case ESC:
                engine.cell[rel_point][item] = atof( temp );
                done = TRUE;
                break;
            case 'h': case 'H': case '?': /* Show commands */
                show_commands();
                break;
            default:
                if( isdigit( c ) ) { /* It's a valid digit */
                    if( new_cell ) { /* Clear cell if just entered and digit */
                        new_cell = FALSE;
                        engine.cell[rel_point][item] = 0;
                        ftoa6( engine.cell[rel_point][item], temp ); /* Convert to ascii */
                        }
                    for( position = (temp[0]=='-') ? 1 : 0;
                         position < 6;
                         position++ ) temp[position] = temp[position+1];
                    temp[5] = c;
                    }
                else
                    if( c == '.' && !strchr( temp, c ) ) { /* Insert decimal point */
                        if( new_cell ) { /* Clear cell if just entered and digit */
                            new_cell = FALSE;
                            engine.cell[rel_point][item] = 0;
                            ftoa6( engine.cell[rel_point][item], temp ); /* Convert to ascii */
                            }
                        for( position = (temp[0]=='-') ? 1 : 0;
                            position < 6;
                            position++ ) temp[position] = temp[position+1];
                        temp[5] = c;
                        }
                else
                    if( c == '.' && new_cell ) { /* Clear cell if just entered and digit */
                        new_cell = FALSE;
                        engine.cell[rel_point][item] = 0;
                        ftoa6( engine.cell[rel_point][item], temp ); /* Convert to ascii */
                        temp[5] = c;
                        }
                else
                    if( c == '-' && !strchr( temp, c ) ) { /* Negate */
                        if( new_cell ) { /* Clear cell if just entered and digit */
                            new_cell = FALSE;
                            engine.cell[rel_point][item] = 0;
                            ftoa6( engine.cell[rel_point][item], temp ); /* Convert to ascii */
                            }
                        for( position = 5; position > -1; position-- )
                            temp[position] = temp[position-1];
                        temp[0] = '-';
                        temp[6] = NULL;
                        }
                else
                    if( c == '-' ) {
                        if( new_cell ) { /* Clear cell if just entered and digit */
                            new_cell = FALSE;
                            engine.cell[rel_point][item] = 0;
                            ftoa6( engine.cell[rel_point][item], temp ); /* Convert to ascii */
                            }
                        temp[0] = ' ';
                        }
                else sound( 3000, 5 );

            } /* switch getch() */
        if( phy_point == 0 && (item == 2 || item == 8) ) unit_adjust( rel_point );
        } /* while */
    return TRUE;

} /* get_data */


#pragma trace
BOOL makeform( void )
{
    extern char *pt_head[];
    extern char *heading[];
    extern char *label_ptr[];
    extern char *symbol_ptr[];
    extern char *units_ptr[];
    extern char buffer[]; /* Buffer for printa subr */

    int i;
    int formlines, formnomen, formnumer;
    char space[2] = " ";
    struct WINDOW form = { 1, 2, 23, 77, "", white_b + brown_f };

    formlines = white_b + brown_f;
    formnomen = white_b + blue_f;
    formnumer = white_b + yellow_f;

    cursoff();
    clrscrn();

    make_window( form, FALSE );

    for( i=0; i<3; i+=2 ) { /* Horizontal lines of heading grid */
        poscurs( i, 1 );
        writechs( H_LINE2, formlines, 77 );
        }
    for( i=4; i<26; i+=2 ) { /* Horz lines of input grid */
        poscurs( i, 1 );
        writechs( H_LINE1, formlines, 77 );
        }

    poscurs( 0, 1 ); /* Insert top line intersections */
    writechs( TLC21, formlines, 1 );
    poscurs( 0, 39 );
    writechs( TEE21, formlines, 1 );
    poscurs( 0, 43 );
    writechs( TEE21, formlines, 1 );
    poscurs( 0, 50 );
    writechs( TEE21, formlines, 1 );
    poscurs( 0, 57 );
    writechs( TEE21, formlines, 1 );
    poscurs( 0, 64 );
    writechs( TEE21, formlines, 1 );
    poscurs( 0, 71 );
    writechs( TEE21, formlines, 1 );
    poscurs( 0, 78 );
    writechs( TRC21, formlines, 1 );

    poscurs( 2, 1 ); /* Insert 2nd line intersections */
    writechs( LEE21, formlines, 1 );
    poscurs( 2, 39 );
    writechs( CRX21, formlines, 1 );
    poscurs( 2, 43 );
    writechs( CRX21, formlines, 1 );
    poscurs( 2, 50 );
    writechs( CRX21, formlines, 1 );
    poscurs( 2, 57 );
    writechs( CRX21, formlines, 1 );
    poscurs( 2, 64 );
    writechs( CRX21, formlines, 1 );
    poscurs( 2, 71 );
    writechs( CRX21, formlines, 1 );
    poscurs( 2, 78 );
    writechs( REE21, formlines, 1 );

    for( i=4; i<23; i+=2 ) { /* Insert next 10 lines intersections */
        poscurs( i, 1 );
        writechs( LEE11, formlines, 1 );
        poscurs( i, 39 );
        writechs( CRX11, formlines, 1 );
        poscurs( i, 43 );
        writechs( CRX11, formlines, 1 );
        poscurs( i, 50 );
        writechs( CRX11, formlines, 1 );
        poscurs( i, 57 );
        writechs( CRX11, formlines, 1 );
        poscurs( i, 64 );
        writechs( CRX11, formlines, 1 );
        poscurs( i, 71 );
        writechs( CRX11, formlines, 1 );
        poscurs( i, 78 );
        writechs( REE11, formlines, 1 );
        }; /* for */

    poscurs( 24, 1 ); /* Insert bottom line intersections */
    writechs( BLC11, formlines, 1 );
    poscurs( 24, 39 );
    writechs( BEE11, formlines, 1 );
    poscurs( 24, 43 );
    writechs( BEE11, formlines, 1 );
    poscurs( 24, 50 );
    writechs( BEE11, formlines, 1 );
    poscurs( 24, 57 );
    writechs( BEE11, formlines, 1 );
    poscurs( 24, 64 );
    writechs( BEE11, formlines, 1 );
    poscurs( 24, 71 );
    writechs( BEE11, formlines, 1 );
    poscurs( 24, 78 );
    writechs( BRC11, formlines, 1 ); /* Writechs to prevent scrolling */

    /* Input form header fields */
    poscurs( 1, 4 );
    sprintf( buffer, "%-34s", heading[9] );
    printa( buffer, formnomen );
    poscurs( 1, 39 );
    sprintf( buffer, "%3s", heading[10] );
    printa( buffer, formnomen );
    poscurs( 1, 43 );
    sprintf( buffer, "%6s", heading[11] );
    printa( buffer, formnomen );

    for( i=0; i<12; i++ ) { /* Vertical lines of grid */
        poscurs( i*2+1, 1 );
        sprintf( buffer, "%c%-37s%c%3s%c%6s%c%6s%c%6s%c%6s%c%6s%c",
                V_LINE1, space,
                V_LINE1, space,
                V_LINE1, space,
                V_LINE1, space,
                V_LINE1, space,
                V_LINE1, space,
                V_LINE1, space,
                V_LINE1
                );
        printa( buffer, formlines );

        poscurs( i*2+1, 2 );
        sprintf( buffer, "%-37s", (label_ptr[i]==NULL) ? space : label_ptr[i] );;
        printa( buffer, formnomen ); /* Print nomenclature */
        poscurs( i*2+1, 40 );
        sprintf( buffer, "%3s", (symbol_ptr[i]==NULL) ? space : symbol_ptr[i] );;
        printa( buffer, formnomen ); /* Print symbol of nomenclature */
        poscurs( i*2+1, 44 );
        sprintf( buffer, "%6s", (units_ptr[i]==NULL) ? space : units_ptr[i] );;
        printa( buffer, formnomen ); /* Print units of nomenclature */
        } /* for */

    curson();

} /* makeform */

/* eof: TRPFORM.C */
