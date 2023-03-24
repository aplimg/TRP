/*  TRPPLOT.C
    Plot routines for Turbine Reduction Processor
	Copyright (C)1990 Applied Imagination, Inc.
    Written by Steven R. Stuart  Oct 1989
*/
#pragma nestcmnt
#pragma trace-

#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>
#include <conio.h>
#include <dos.h>
#include <malloc.h>
#include <math.h>
#include "trpdefs.h"
#include "realtype.h"
#include "worlddr.h"
#include "segraph.h"
#include "curvefit.h"

#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

#define ERASE   0
#define DRAW    !ERASE

/* Good and bad test points.. */
#define GOOD    0
#define BAD     !GOOD

extern int black_b, blue_b, green_b, cyan_b, red_b, magenta_b, brown_b, white_b;
extern int black_f, blue_f, green_f, cyan_f, red_f, magenta_f, brown_f, white_f;
extern int gray_f, ltblue_f, ltgreen_f, ltcyan_f, ltred_f;
extern int ltmagenta_f, yellow_f, brtwhite_f;


#pragma trace
int graph_( void ) {

extern int max_point;
extern float calc_data[];
extern float tfp_min, tfp_max;
extern float lo_x, hi_x, lo_y, hi_y;

extern unsigned shp_scaled_point;
extern unsigned t4ratio_scaled_point;
extern unsigned t5raw_scaled_point;
extern unsigned itt_scaled_point;
extern float min_tt5, max_tt5;
extern float min_shp, max_shp;
extern float offset;
extern float t_nominal;
extern float t4rat_ntry, minshp_ntry;
extern struct TESTFORM engine;
extern char buffer[];
extern int gmode, vidmode;
extern BOOL color_graph;
extern unsigned long crtsiz;
extern struct vconfig screen;
extern unsigned v_res, h_res;
extern char far *vscreen;
extern char far *crtptr;
extern int graph_type;
extern char checksum_err[];
extern char chk_file[];

realtype *TT5_pts, *SHP_pts, *T4r_pts;
realtype *coef, *coefsig, *yest;
int numobs,order;
realtype *resid, rsq,r,see;
char error;
char *g_choice[] = {"Linear", "Curve", NULL};
BOOL gph_hots[] = {TRUE, TRUE};

    int i, y_scaled;
    char ch;
    float x_TT5, hi_SHP=0F, lo_SHP=1300F, hi_T4r=0F, lo_T4r=1.2F;
    float b0, b1, b2, *y_SHP, *y_T4r;
    float t_nom, t4_ratio_ntry;
    float b4limit, increment = 0.005F;
    BOOL all_pts, limit = FALSE;
    char blankline[] = "                                       ";
    char *help1[] = { "VALID COMMANDS:",
                      "\x18   - increase T4 ratio",
                      "\x19   - decrease T4 ratio",
                      "A   - set APR offset value",
                      "H,? - this list",
                      "I   - toggle increment value",
                      "ENTER continue to min SHP setup graphic",
                      "SPACE skip min SHP setup graphic",
                      "ESC   escape from graphic function",
                      "      ..press any key to continue.."
                    };
    char *help2[] = { "VALID COMMANDS:",
                      "\x18   - increase min SHP value",
                      "\x19   - decrease min SHP value",
                      "H,? - this list",
                      "I   - toggle increment value",
                      "ENTER continue to nominal temperature entry",
                      "ESC   escape from graphic function",
                      "      ..press any key to continue.."
                    };




  numobs = 10;  /* maximum allocation allowed */

  /* Allocate memory for curvefitting & charting routines */
  TT5_pts = GetMem(numobs);
  if (BadMem(TT5_pts)) return -1;

  SHP_pts = GetMem(numobs);
  if (BadMem(SHP_pts)) {free(TT5_pts); return -1;}

  T4r_pts = GetMem(numobs);
  if (BadMem(T4r_pts)) {free(TT5_pts); free(SHP_pts); return -1;}

  coef = GetMem(numobs);
  if (BadMem(coef)) {
   free(TT5_pts); free(SHP_pts); free(T4r_pts); return -1;}

  coefsig = GetMem(numobs);
  if (BadMem(coefsig)) {
   free(TT5_pts); free(SHP_pts); free(T4r_pts);
   free(coef); return -1;}

  yest = GetMem(numobs);
  if (BadMem(yest)) {
   free(TT5_pts); free(SHP_pts); free(T4r_pts);
   free(coef); free(coefsig); return -1;}

  resid = GetMem(numobs);
  if (BadMem(resid)) {
   free(TT5_pts); free(SHP_pts); free(T4r_pts);
   free(coef); free(coefsig); free(yest); return -1;}

  y_SHP = calloc(1000,sizeof(float));
  if( y_SHP == NULL ) {
   free(TT5_pts); free(SHP_pts); free(T4r_pts);
   free(coef); free(coefsig); free(yest); free(resid); return -1;}

  y_T4r = calloc(1000,sizeof(float));
  if( y_T4r == NULL ) {
   free(TT5_pts); free(SHP_pts); free(T4r_pts);
   free(coef); free(coefsig); free(yest); free(resid); free(y_SHP);
   return -1;}

    if( chksum( chk_file, 0 ) ) {
        say_error( checksum_err );
        exit( 1 );
    }

    /* Get the point data returned by calculation program..
       Subr. returns TRUE if using all valid & invalid TFP points,
       FALSE if only valid pts used  */
    all_pts = getpts( &numobs, TT5_pts, SHP_pts, T4r_pts );

    if( numobs < 2 ) {
        free(TT5_pts); free(SHP_pts); free(T4r_pts);
        free(coef); free(coefsig); free(yest); free(resid);
        free(y_SHP); free(y_T4r);
        return 1;
    }

/*
This is where we can insert a popup to decide whether we want linear or curve
*/
    /* Create linear curve if less than 4 working points */
    /*order = ((numobs<4) || (engine.facility==ON_WING)) ? 1 : 2;*/
    order = (numobs < 4) ? 1 : popup( 14, 17, g_choice, gph_hots ); /* Linear or curve choice */

    if( vidmode != 0 ) { /* Video mode selected from commandline */
        switch( vidmode ) {
            case 1: /* CGA */
                gmode = 6;
                break;
            case 2: /* EGA */
                gmode = (color_graph) ? 16 : 15;
                break;
            case 3: /* VGA */
                gmode = (color_graph) ? 18 : 17;
                break;
            case 4: /* Hercules */
                gmode = 99;
                break;
            default: /* Invalid video mode */
                free(TT5_pts); free(SHP_pts); free(T4r_pts);
                free(coef); free(coefsig); free(yest); free(resid);
                free(y_SHP); free(y_T4r);
                return 2;
        } /* switch vidmode */
        setvmode( gmode ); /* Set commandline video mode */
    } /* if */
    else { /* Automatic graphic setup */
        if( setvmode( 17 ) != 17 )
            if( setvmode( 15 ) != 15 )
                if( setvmode( 6 ) != 6 )
                    if( setvmode( 5 ) != 5 )
                        if( setvmode( 99 ) != 99 ) {
                            free(TT5_pts); free(SHP_pts); free(T4r_pts);
                            free(coef); free(coefsig); free(yest); free(resid);
                            free(y_SHP); free(y_T4r);
			             /*   setvmode(DEFAULTMODE);     Restore text mode */
                            return 3;
                        }
    } /* else */

    /* Graphic mode is now selected; find it's memory */
    switch( gmode = getvmode() ) {
        case 17: /* VGA */
            crtptr = MK_FP( 0xA000, 0 );
            crtsiz = 0x9600L;
            break;
        case 15: /* EGA */
            crtptr = MK_FP( 0xA000, 0 );
            crtsiz = 0x6D60L;
            break;
        case 5: case 6: /* CGA */
            crtptr = MK_FP( 0xB000, 0x8000 );
            crtsiz = 0x3F40L;
            break;
        case 99: /* Hercules */
            crtptr = MK_FP( 0xB000, 0 );
            crtsiz = 0x8000L;
            break;
    } /* switch */

    /* Get screen resolution */
    getvconfig( &screen );
    h_res = screen.xpixels;
    v_res = screen.ypixels;
    /* Scale */
    shp_scaled_point = t4ratio_scaled_point = (v_res - 16) / 100;
    if( graph_type == GRAPH_A ) {
        lo_x = 700F;
        hi_x = 1200F;
        t5raw_scaled_point = (h_res - 64) / 50;
    }
    else {  /* graph_type == GRAPH_B */
        lo_x = 1000F;
        hi_x = 1800F;
        itt_scaled_point = (h_res - 64) / 80;
    }

    graph_grid();          /* Draw the grid form */
    if( order == 1 ) plota( xpoint( hi_x - 100F ), v_res-8, "(linear)" );
    if( all_pts ) plota( 10, v_res-8, "(invalid points incl.)" );

    /* Plot Tt5/ITT -vs- SHP points */
    for( i=0; i<max_point; i++ )
        /* Plot point if it falls within graphic area */
        if( calc_data[i*6] > lo_x && calc_data[i*6] < hi_x &&
            calc_data[i*6+2] > lo_y && calc_data[i*6+2] < hi_y )
                mark_pt( xpoint( calc_data[i*6] ),
                    ypoint( calc_data[i*6+2] ),
                    ((calc_data[i*6+4] < tfp_min) ||
                    (calc_data[i*6+4] > tfp_max)) ?
                    BAD : GOOD );

    /* Plot Tt5/ITT -vs- T4r points */
    for( i=0; i<max_point; i++ )
        /* Plot point if it falls within graphic area */
        if( calc_data[i*6] > lo_x && calc_data[i*6] < hi_x &&
            calc_data[i*6+1] > 0.25F && calc_data[i*6+1] < 1.2F )
                mark_pt( xpoint( calc_data[i*6] ),
                    rpoint( calc_data[i*6+1] ),
                    ((calc_data[i*6+4] < tfp_min) ||
                    (calc_data[i*6+4] > tfp_max)) ?
                    BAD : GOOD );

    /* Call curvefit routine,
       Get the TT5 -vs- SHP curve first */
    if( order == 2 )
        /* Input: indvar, depvar, numobs, order */
        PolyCurveFit(TT5_pts,SHP_pts,numobs,order,coef,yest,resid,&see,coefsig,&rsq,&r,&error);
    else
        LinearFit(TT5_pts,SHP_pts,numobs,coef);

    /* Get coefficients */
    b0 = *coef; b1 = *(coef+1); b2 = *(coef+2);

    /* Fill up TT5/ITT -vs- SHP charting array and draw curve */
    x_TT5 = 700F;
    for (i = 0; i < 1000; i++){
        /* Get y value for x (i) */
        y_SHP[i] = b0 + b1 * x_TT5 + b2 * x_TT5 * x_TT5;
        /* Save maximum and minimum y */
        hi_SHP = MAX( hi_SHP, y_SHP[i] );
        lo_SHP = MIN( lo_SHP, y_SHP[i] );
        /* Scale point to screen resolution */
        y_scaled  = ypoint( y_SHP[i]);
        /* In graphic display range? Show dot */
        if( (x_TT5 > lo_x) && (x_TT5 < hi_x) &&
            (y_SHP[i] > lo_y) && (y_SHP[i] < hi_y) )
                writedot( y_scaled, xpoint( x_TT5 ), 1 );
        x_TT5++;
    }

    /* Now get the TT5 -vs- T4ratio curve */
    if( order == 2 )
        PolyCurveFit(TT5_pts,T4r_pts,numobs,order,coef,yest,resid,&see,coefsig,&rsq,&r,&error);
    else
        LinearFit(TT5_pts,T4r_pts,numobs,coef);

    /* Get coefficients */
    b0 = *coef; b1 = *(coef+1); b2 = *(coef+2);

    /* Fill up TT5/ITT -vs- T4r charting array and draw curve */
    x_TT5 = 700F;
    for (i = 0; i < 1000; i++){
        /* Get y value for x (i) */
        y_T4r[i] = b0 + b1 * x_TT5 + b2 * x_TT5 * x_TT5;
        /* Save maximum and minimum y */
        hi_T4r = MAX( hi_T4r, y_T4r[i] );
        lo_T4r = MIN( lo_T4r, y_T4r[i] );
        y_scaled = rpoint( y_T4r[i] );
        /* In graphic range? Show dot */
        if( (x_TT5 > lo_x) && (x_TT5 < hi_x) &&
            (y_T4r[i] > 0.2F) && (y_T4r[i] < 1.15F) )
                writedot( y_scaled, xpoint( x_TT5  ), 1 );
        x_TT5++;
    }

    curve_ttl( y_SHP, y_T4r );  /* Place curve titles */
    fill_style( 0, 1, 1 ); /* Background in help boxes */


    offset = 0F;           /* Reset global APR offset */

    /* Plot maximum temperature lines.
       When max temp graphic is as desired, ENTER will plot a minimum
       graphic; SPACEBAR will skip minimum graphic
    */
    if( color_graph ) pen_color( 14 );
    t4_ratio_ntry = t4rat_ntry; /* Set local to global */
    plota( 0, 20, "** Set Tt4/Tt4 reference ratio" );
    save_scn( vscreen ); /* Mirror current screen in virtual screen */

    /* Set t4_ratio_ntry to performance line at hi_x if it would
       fall outside of lo_x and hi_x endpoints */
    if( t4_ratio_ntry > hi_T4r ) t4_ratio_ntry = hi_T4r;
    else if( t4_ratio_ntry < lo_T4r ) t4_ratio_ntry = lo_T4r;

    do {
        /* Plot maximum temperature lines (limit = TRUE when border hit */
        if( (limit = maximum( t4_ratio_ntry, y_SHP, y_T4r ))) {
            /* If border limit hit, restore t4_ratio_ntry and redraw */
            sound( 200, 5 );
            t4_ratio_ntry = b4limit;
            maximum( t4_ratio_ntry, y_SHP, y_T4r );
        }
        drain(); /* drain chars from keyboard buffer */
        switch( ch=getch() ) {
            case '\0': /* Function key */
                switch( ch=getch() ) { /* Get rest of func key */
                    case UPAROW: /* Increase ratio select */
                        restore_scn( vscreen );
                        if( ! limit ) {
                            b4limit = t4_ratio_ntry;
                            t4_ratio_ntry += increment;
                        }
                        limit = FALSE;
                        break;
                    case DNAROW: /* Decrease ratio select */
                        restore_scn( vscreen );
                        if( ! limit ) {
                            b4limit = t4_ratio_ntry;
                            t4_ratio_ntry -= increment;
                        }
                        limit = FALSE;
                        break;
                    default: /* Undefined function key */
                        sound( 200, 5 );
                        break;
                    } /* switch functail */
                break;
            case 'a': case 'A': /* APR offset selected */
                if( color_graph ) pen_color( CYAN_F );
                plota( xpoint( lo_x )+32, ypoint( 550F ), "Enter APR offset: " );
                buffer[0] = '\0';
                for( i=0; i<2; i++ ) {
                    if( isdigit( ch=getch() ) ) {
                        buffer[i] = ch;
                        plotch( ch );
                        buffer[i+1] = '\0'; /* Avoid cement wall! */
                        } /* if */
                    else if( ch == '\r' ) break;
                    else sound( 200, 5 ); /* Illegal char */
                    } /* for */
                offset = atof( buffer );
                sound( 5000, 2 );
                restore_scn( vscreen );
                break;
            case 'h': case 'H': case '?': /* Help -- display commands */
                move_to( 50, 50 );
                box( 352, 110, 1 );
                if( color_graph ) pen_color( BLUE_F );
                for( i=0; i<10; i++ ) {
                    plota( 55, i*10+55, help1[i] );
                    }
                drain(); /* drain chars from keyboard buffer */
                getch();
                restore_scn( vscreen );
                break;
            case 'i': case 'I': /* Toggle increment value */
                increment = (increment==0.001F) ? 0.005F : 0.001F;
                sound( 3000, 1 );
                sound( 1000, 1 );
                if( color_graph ) pen_color( CYAN_F );
                sprintf( buffer, "Increment changed to %01.3f", increment );
                plota( xpoint( lo_x )+32, ypoint( 450F ), buffer );
                break;
            case ENTER: case SPACEBAR: /* Terminate T4 ratio setup */
                sound( 5000, 2 );
                break;
            case ESC: /* Quit graph */
                setvmode(DEFAULTMODE);          /* Restore text mode */
                free(TT5_pts); free(SHP_pts); free(T4r_pts);
                free(coef); free(coefsig); free(yest); free(resid);
                free(y_SHP); free(y_T4r);
                return 4;
                break;
            default: /* Undefine key */
                sound( 200, 5 );
                break;
            } /* switch funchead */
        } /* do */
    while( ch != ENTER && ch != SPACEBAR && ch != ESC );

    save_scn( vscreen );    /* Save current screen in virtual screen */
    if( ch != SPACEBAR ) {  /* Plot minimum temperature lines */
        if( color_graph ) pen_color( YELLOW_F );
        plota( 0, 20, "** Set minimum shaft horsepower setting" );
        save_scn( vscreen );    /* Save current screen in virtual screen */
        min_shp = ( max_shp > minshp_ntry ) ? minshp_ntry : max_shp;
        increment = 5F;      /* Reset increment value */

        /* Set min_shp to performance line at hi_x if it would
            fall outside of lo_x and hi_x endpoints */
        if( min_shp < lo_SHP ) min_shp = lo_SHP;
        else if( min_shp > hi_SHP ) min_shp = hi_SHP;
        limit = FALSE;

        do {
            /* Plot minimum temperature lines */
            if( (limit = minimum( min_shp, y_SHP, y_T4r ))) {
                /* If border limit hit, restore min_shp and redraw */
                sound( 200, 5 );
                min_shp = b4limit;
                minimum( min_shp, y_SHP, y_T4r );
            }
            drain(); /* drain chars from keyboard buffer */
            switch( ch=getch() ) {
                case '\0': /* Function key */
                    switch( ch=getch() ) { /* Get rest of func key */
                        case UPAROW: /* Increase ratio select */
                            restore_scn( vscreen );
                            if( (! limit) &&
                                (min_shp < max_shp) ) {
                                b4limit = min_shp;
                                min_shp += increment;
                            }
                            if( min_shp >= max_shp ) sound( 200,5 );
                            limit = FALSE;
                            break;
                        case DNAROW: /* Decrease ratio select */
                            restore_scn( vscreen );
                            if( ! limit ) {
                                b4limit = min_shp;
                                min_shp -= increment;
                            }
                            limit = FALSE;
                            break;
                        default: /* Undefined function key pressed */
                            sound( 200, 5 );
                            break;
                        } /* switch functail */
                    break;
                case 'h': case 'H': case '?': /* Help -- display commands */
                    move_to( 50, 50 );
                    box( 352, 90, 1 );
                    if( color_graph ) pen_color( BLACK_F );
                    for( i=0; i<8; i++ ) {
                        plota( 55, i*10+55, help2[i] );
                        }
                    drain(); /* drain chars from keyboard buffer */
                    getch();
                    restore_scn( vscreen );
                    break;
                case 'i': case 'I': /* Toggle increment value */
                    increment = (increment==1F) ? 5F : 1F;
                    sound( 3000, 3 );
                    sound( 1000, 1 );
                    if( color_graph ) pen_color( CYAN_F );
                    sprintf( buffer, "Increment changed to %1.0f", increment );
                    plota( xpoint( lo_x )+32, ypoint( 450F ), buffer );
                    break;
                case ENTER: /* Terminate minimum SHP setup */
                    sound( 5000, 2 );
                    break;
                case ESC: /* Quit the graphic */
                    setvmode(DEFAULTMODE);          /* Restore text mode */
                    free(TT5_pts); free(SHP_pts); free(T4r_pts);
                    free(coef); free(coefsig); free(yest); free(resid);
                    free(y_SHP); free(y_T4r);
                    return 4;
                    break;
                default: /* Undefined key pressed */
                    sound( 200, 5 );
                    break;
                } /* switch funchead */
            } /* do */
        while( ch != ENTER );
        } /* if */
    else {
        min_tt5 = max_tt5; /* no minimum selected */
        min_shp = max_shp;
        }
    move_to( 0, 20 );
    plots( blankline );     /* Clear entry id title */
    save_scn( vscreen );    /* Save current screen */

    /* Get nominal temperature value */
    if( color_graph ) pen_color( CYAN_F );
    sprintf( buffer, "Enter nominal temperature value [%.0f]: ", t_nominal );
    plota( xpoint( lo_x )+8, ypoint( 450F ), buffer );
    buffer[0] = '\0';
    for( i=0; i<5; i++ ) {
        if( isdigit( ch=getch() ) ) {
            buffer[i] = ch;
            plotch( ch );
            buffer[i+1] = '\0'; /* Avoid cement wall! */
            } /* if */
        else if( ch == '\r' ) break;
        else sound( 200, 5 ); /* Illegal char */
        } /* for */
    t_nom = atof( buffer ); /* Convert temperature string to float */
    if( t_nom == 0F ) t_nom = t_nominal; /* Default value if null string */
    restore_scn( vscreen );         /* Erase query */
    result( t_nom, y_T4r );         /* Display the results */
    save_scn( vscreen );            /* Save current screen for print */
    if( color_graph ) pen_color( LTRED_F );
    plota( 0, 20, "** Press any key to continue.." );
    drain(); /* drain chars from keyboard buffer */
    getch();                        /* Wait here till key pressed */
    restore_scn( vscreen );         /* Erase query */
    sprintf( buffer, "Serial number: %lu", engine.serial );
    plota( 0, 20, buffer );         /* Put serial nr on graphic */
    save_scn( vscreen );            /* Save current screen for print */
    setvmode(DEFAULTMODE);          /* Restore text mode */
    free(TT5_pts); free(SHP_pts); free(T4r_pts);
    free(coef); free(coefsig); free(yest); free(resid);
    free(y_SHP); free(y_T4r);
    return 0;                       /* Return successful value */

} /* graph_ */


#pragma trace-
BOOL graph_grid( void )
{
extern int graph_type;
extern unsigned v_res, h_res;
extern BOOL color_graph;
extern float lo_x, hi_x, lo_y, hi_y;

    float i, j;
    char buffer[80];

    if( color_graph ) pen_color( 2 );
    plota( 0, 0, "Recompensation Data Extrapolation Plot" );
	plota( 0, 10, "Copyright (C)1990,1993 Applied Imagination, Inc." );

    for( i=400F; i<=hi_y; i+=100F ) { /* SHP value titles */
        sprintf( buffer, "%4.f", i );
        plota( 0, ypoint( i )-4, buffer );
        writedot( ypoint( i ), xpoint( lo_x )-1, color_graph ? 14 : 1 );
        }
    for( i=lo_x; i<=hi_x; i+=100F ) { /* TT5/ITT value titles */
        sprintf( buffer, "%4.f", i );
        plota( xpoint( i )-16, v_res-20, buffer );
        }
    for( i=0.6F; i<1.2F; i+=0.1F ) { /* T4 ratio vlue titles */
        sprintf( buffer, "%0.1f", i );
        plota( h_res-32, rpoint( i )-4, buffer );
        writedot( rpoint( i ), xpoint( hi_x )+1, color_graph ? 14 : 1 );
        }

    /* Enclose graph in box */
    if( color_graph ) pen_color( 4 );    /* red */
    move_to( xpoint( lo_x ), ypoint( hi_y ) ); /* Enclose graph in box */
    box( xpoint( hi_x ) - xpoint( lo_x ), ypoint( lo_y ) - ypoint( hi_y ), 0 );

    for( i=lo_y; i<=hi_y; i+=50F ) { /* Grid points */
        for( j=lo_x; j<=hi_x; j+=25F ) {
            writedot( ypoint( i ), xpoint( j ), color_graph ? 9 : 1 );
            }
        }
    move_to( 0, ypoint( 780F ) );
    vplots( "SHP" );
    move_to( h_res-24, rpoint( 0.55F ) );
    vplots( "T4 ratio" );
    plota( h_res/2-40, v_res-8, ((graph_type==GRAPH_A) ? "Tt5 raw" : "ITT") );

} /* graph_heading */

#pragma trace-
BOOL mark_pt( int x_coord, y_coord, point )
{
extern BOOL color_graph;

  int x, y, i;

    x = x_coord - 4;
    y = y_coord - 4;
    if( point == GOOD ) { /* Plus sign */
        for( i=0; i<9; i++ ) {
            writedot( y, x_coord, color_graph ? 5 : 1 );
            writedot( y_coord, x, color_graph ? 5 : 1 );
            x++;
            y++;
            }
        } /* if */
    else { /* Hollow box point */
        move_to( x, y );
        box( 8, 8, 0 );
        } /* else */
    return point;

} /* mark_pt */


#pragma trace-
void LinearFit(float *x, float *y, int num, float *coef)
/* Finds m and b in equation y=mx+b. */
{
    int i;
    float sumx=0F ,sumy=0F ,sumxy=0F ,sumx2=0F ,sumy2=0F;

    for( i=0; i<num; i++ ) {
        sumx  += x[i];
        sumx2 += x[i] * x[i];
        sumy  += y[i];
        sumy2 += y[i] * y[i];
        sumxy += x[i] * y[i];
    }

    *coef     = ((sumy * sumx2) - (sumx * sumxy)) /     /* The 'b' */
                ((num  * sumx2) - (sumx * sumx));

    *(coef+1) = ((num  * sumxy) - (sumx * sumy)) /      /* The 'm' */
                ((num  * sumx2) - (sumx * sumx));

    *(coef+2) = 0F;

} /* LinearFit */


#pragma trace-
BOOL getpts( int *obs, realtype *TT5_pts, realtype *SHP_pts, realtype *T4r_pts )
/*  Returns TRUE if only good points used,
           FALSE if invalid points used also */
{
    extern int max_point;
    extern float calc_data[];
    extern float tfp_min, tfp_max;
    extern char buffer[];

    int i, good_points = 0;
    BOOL point = GOOD, all_good = TRUE, use_all = FALSE;
    char ch;
    struct WINDOW query = { 10, 10, 13, 70, "µ PLOT Æ", ltred_f+black_b };

    /* Test all points for TFP range */
    for( i=0; i<max_point; i++ ) {
        if((calc_data[i*6+4] < tfp_min) ||
           (calc_data[i*6+4] > tfp_max))
           all_good = FALSE;
        else good_points++; /* Count the good points */
    }

    /* Inform user of any invalid points and find out if they
       want to include them in the curve */
    if( ! all_good ) {
        make_window( query, FALSE );
        sprintf( buffer, "Used %i test point(s) in calculation. Turbine flow parameter", max_point );
        printa( buffer, query.attr );
        poscurs( query.topleftrow+1, query.topleftcol );
        sprintf( buffer, "(TFP) in range on %i point(s). Accurate extrapolation plot", good_points );
        printa( buffer, query.attr );
        poscurs( query.topleftrow+2, query.topleftcol );
        sprintf( buffer, "requires TFP in range on 4 or more points." );
        printa( buffer, query.attr );
        poscurs( query.topleftrow+3, query.topleftcol );
        sprintf( buffer, "Use the %i invalid points in performance curve? (y/[n])", max_point-good_points );
        printa( buffer, brtwhite_f+black_b );
        ch = getch();
        if( ch == 'Y' || ch == 'y' ) use_all = TRUE;
    }

    /* Get the points being used in curve calc */
    *obs = -1;
    for( i=0; i<max_point; i++ ) {

        /* Test for valid point */
        point = ((calc_data[i*6+4] < tfp_min) ||
                 (calc_data[i*6+4] > tfp_max)) ?
            BAD : GOOD;

        /* Save for calculation */
        if( (point == GOOD) || (use_all) ) {
            (*obs)++; /* New number of objects */
            /* Save point for curve calculation */
            TT5_pts[*obs] = calc_data[i*6];
            T4r_pts[*obs] = calc_data[i*6+1];
            SHP_pts[*obs] = calc_data[i*6+2];
        } /* if good point.. */

    } /* for i */
    (*obs)++; /* Change from array subscr to natural number  */
    return use_all;

} /* getpts */



#pragma trace-
/* Place titles on curves */
BOOL curve_ttl( float *SHP_pos, float *T4r_pos )
{
    extern float lo_x, hi_x, lo_y, hi_y;
    extern int graph_type;

    int i, x, x_SHP, x_T4r, iskip;
    float SHP_fx, T4r_fx, fskip;
    char ttl0[4], ttl1[11], ttl2[16];

    /* Setup correct titles and char spacing */
    if( graph_type == GRAPH_A ) {
        strcpy( ttl0, "Tt5" );
        fskip = 6F;
        iskip = 6;
        }
    else { /* GRAPH_B */
        strcpy( ttl0, "ITT" );
        fskip = 12F;
        iskip = 12;
        }
    strcpy( ttl1, "SHP vs " );
    strcpy( ttl2, "T4 ratio vs " );
    strcat( ttl1, ttl0 );
    strcat( ttl2, ttl0 );


    /* Start performance title within borders */
    SHP_fx = 700F;
    for( x=0; x<1000; x++ ) {
        if( SHP_fx > lo_x+25F && SHP_pos[x] > 600F ) {
            x_SHP = x;
            break;
        }
        SHP_fx++;
    }

    for( i=0; i<10; i++ ) {
        /* Label SHP vs Tt5/ITT performance line */
        if( SHP_pos[x_SHP] > lo_y  && SHP_pos[x_SHP] < hi_y ) {
            move_to( xpoint( SHP_fx ), ypoint( SHP_pos[x_SHP] )+4 );
            plotch( ttl1[i] );
        }
        x_SHP  += iskip;
        SHP_fx += fskip;
    }

    T4r_fx = 700F;
    for( x=0; x<1000; x++ ) {
        if( T4r_fx > lo_x+25F && T4r_pos[x] > 0.45F ) {
            x_T4r = x;
            break;
        }
        T4r_fx++;
    }

    for( i=0; i<15; i++ ) {
        /* Label T4r vs Tt5/ITT performance line */
        if( T4r_pos[x_T4r] > 0.2F && T4r_pos[x_T4r] < 1.15F ) {
            move_to( xpoint( T4r_fx ), rpoint( T4r_pos[x_T4r] )+4 );
            plotch( ttl2[i] );
        }
        x_T4r  += iskip;
        T4r_fx += fskip;
    }

} /* curve_ttl */

#pragma trace-
BOOL maximum( float t4_entry, float *y_SHP, float *y_T4r )
{ /* Plot max lines, graph APR offset if !zero */
    extern float offset;
    extern float max_tt5, max_shp;
    extern float lo_x, hi_x, lo_y, hi_y;

    int i;
    char ttl[8];
    float apr_yoffs, apr_intrsc;

    /* Enter @ T4 ratio */
    move_to( xpoint( hi_x ), rpoint( t4_entry ) );

    /* Find t4r-tt5 performance line intersection */
    for( i=0; i<1000; i++ ) {
        if( y_T4r[i] >= t4_entry ) {
            max_tt5 = (float) (i + 700);
            break;
        } /* Iterator i is x-axis subscript */
    }

    /* Graph border hit? */
    if( (max_tt5 >= hi_x)   ||
        (max_tt5 <= lo_x)   ||
        (t4_entry >= 1.15F) ||
        (t4_entry <= 0.2F) )
        return TRUE;

    /* Draw line left to intersection */
    line_to( xpoint( max_tt5 ), rpoint( t4_entry ) );

    /* Plot APR offset */
    if( offset != 0F ) {

        /* Find apr intersection on Tt4 vs Tt5 line */
        apr_intrsc = max_tt5 + offset;

        apr_yoffs = y_T4r[((int) (apr_intrsc)) - 700];

        if( apr_yoffs > 1.02F ) {
            apr_yoffs = 1.02F;      /* APR offset must be <= 1.02 */

            /* Find TT5 value for apr_yoffs (apr_intrsc = Tt5 location */
            for( i=0; i<1000; i++ ) {
                if( y_T4r[i] >= apr_yoffs ) {
                    apr_intrsc = (float) (i + 800);
                    break;
                } /* Iterator i is x-axis subscript */
            }

            /* Adjust max Tt5 value */
            max_tt5 = apr_intrsc - offset;
        }

        /* line apr-tt5 intersection to right border*/
        if( apr_intrsc < hi_x ) { /* Don't plot if outside graph */
            move_to( xpoint( apr_intrsc ), rpoint( apr_yoffs ) );
            line_to( xpoint( hi_x ), rpoint( apr_yoffs ) );
        }

        /* put offset ratio into ascii buffer */
        sprintf( ttl, "%01.3f", apr_yoffs );

        /* If there is an offset.. */
        if( offset > 0F ) {
            if( apr_intrsc < hi_x ) { /* Don't plot if outside graph */
                /* line apr-tt5 intersection down to t4entry */
                move_to( xpoint( apr_intrsc ), rpoint( apr_yoffs ) );
                line_to( xpoint( apr_intrsc ), rpoint( t4_entry ) );
            }
            /* write the offset value */
            plota( xpoint( hi_x )-45, rpoint( apr_yoffs )-11, ttl );
        }
        else plota( xpoint( hi_x )-45, rpoint( apr_yoffs )+3, ttl );
    }

    /* Move cursor to max Tt5 bottom of chart */
    move_to( xpoint( max_tt5 ), ypoint( lo_y ) );

    /* Draw to Tt5 vs Tt4 line */
    line_to( xpoint( max_tt5 ), rpoint( y_T4r[((int) max_tt5)-700] ) );

    /* Find max_shp */
    max_shp = y_SHP[((int) max_tt5)-700];

    /* Hit Tt5 vs SHP line */
    line_to( xpoint( max_tt5 ), ypoint( max_shp ) );
    /* Draw to left of chart */
    line_to( xpoint( lo_x ), ypoint( max_shp ) );

    /* Display Tt4 ratio quantity */
    sprintf( ttl, "%01.3f", t4_entry );
    if( offset > 0F ) plota( xpoint( hi_x )-45, rpoint( t4_entry ) + 3, ttl );
    else plota( xpoint( hi_x )-45, rpoint( t4_entry ) - 11, ttl );

    /* Display SHP quantity */
    sprintf( ttl, "%4.0f", max_shp );
    plota( xpoint( lo_x )+5, ypoint( max_shp ) - 8, ttl );

    /* Display Tt5 quantity */
    sprintf( ttl, "%4.0f", max_tt5 );
    plota( xpoint( max_tt5 ) + 3, ypoint( lo_y )-10, ttl );

    return FALSE; /* Border limit not hit */

} /* maximum */

#pragma trace-
BOOL minimum( float shp_entry, float *y_SHP, float *y_T4r )
{
    extern float min_tt5;
    extern float lo_x, hi_x, lo_y, hi_y;

    int i;
    char ttl[8];

    move_to( xpoint( lo_x ), ypoint( shp_entry ) );

    /* Find shp-tt5 performance line intersection */
    for( i=0; i<1000; i++ ) {
        if( y_SHP[i] >= shp_entry ) {
            min_tt5 = (float) (i + 700);
            break;
        }
    }

    /* Graph border hit? */
    if( (min_tt5 <= lo_x)   ||
        (min_tt5 >= hi_x)   ||
        (shp_entry >= hi_y) ||
        (shp_entry <= lo_y) )
        return TRUE;

    line_to( xpoint( min_tt5 ), ypoint( shp_entry ) );
    line_to( xpoint( min_tt5 ), ypoint( lo_y ) );

    /* Display SHP quantity */
    sprintf( ttl, "%4.0f", shp_entry );
    plota( xpoint( lo_x )+5, ypoint( shp_entry ) + 3, ttl );

    /* Display Tt5 quantity */
    sprintf( ttl, "%4.0f", min_tt5 );
    plota( xpoint( min_tt5 ) - 35, ypoint( lo_y )-10, ttl );

    return FALSE;

} /* minimum */

#pragma trace-
BOOL result( float t_nominal, float *y_T4r )
{
    extern float max_tt5, min_tt5, max_shp, min_shp;
    extern float lo_x, hi_x;
    extern char buffer[];

    sprintf( buffer, "T nominal = %4.f F.", t_nominal );
    plota( xpoint( lo_x )+10, ypoint( 400F )-30, buffer );
    sprintf( buffer, "Max: %+4.f F at %4.f SHP. T4 ratio = %01.3f",
                        t_nominal - max_tt5,
                        max_shp,
                        y_T4r[((int) max_tt5)-700]
                        );
    plota( xpoint( lo_x )+10, ypoint( 400F )-20, buffer );
    sprintf( buffer, "Min: %+4.f F at %4.f SHP. T4 ratio = %01.3f",
                        t_nominal - min_tt5,
                        min_shp,
                        y_T4r[((int) min_tt5)-700]
                        );
    plota( xpoint( lo_x )+10, ypoint( 400F )-10, buffer );

} /* result */

#pragma trace-
int ypoint( float y )
{
  extern unsigned shp_scaled_point;
  extern unsigned v_res;

    return (v_res - (( ((int)(y)) - 350 ) * shp_scaled_point / 10 )) - 27;

} /* ypoint */

#pragma trace-
int rpoint( float r )
{
  extern unsigned t4ratio_scaled_point;
  extern unsigned v_res;

    return (v_res - (( ((int)(r*1000F)) - 200 ) * t4ratio_scaled_point / 10 )) - 27;
}

#pragma trace-
int xpoint( float x )
{
  extern unsigned t5raw_scaled_point;
  extern unsigned itt_scaled_point;
  extern int graph_type;

    if( graph_type == GRAPH_A )
        return (( ((int)(x)) - 700 ) * t5raw_scaled_point / 10 ) + 38;
    else
        return (( ((int)(x)) - 1000 ) * itt_scaled_point / 10 ) + 38;

    /* 38 is pel distance between left border & */
    /* left edge of screen */

} /* xpoint */

#pragma trace-
void vplots( char *stn ) /* Vertically plot character array */
{
    while( *stn != '\0' ) {
        plotch( *stn++ );
        move_by( -8, 8 );
        }

} /* vplots */

#pragma trace-
void plota( int x, int y, char *stn )
{
    int i;

    for( i=0; stn[i]!='\0'; i++ ) {
        move_to( x, y );
        plotch( ' ' ); /* Clear space */
        move_to( x, y );
        plotch( stn[i] );
        x += 8;
        }

} /* end plota */

#pragma trace-
BOOL save_scn( char far *scnloc ) /* Store the current graphics */
{
    extern unsigned long crtsiz; /* Size of graphic screen */
    extern char far *crtptr;       /* Pointer to physical screen */

    farmemcpy( scnloc, crtptr, crtsiz );

} /* save_scn */

#pragma trace-
BOOL restore_scn( char far *scnloc ) /* Restore graphic screen to last save */
{
    extern unsigned long crtsiz; /* Size of graphic screen */
    extern char far *crtptr;       /* Pointer to physical screen */

    farmemcpy( crtptr, scnloc, crtsiz );

} /* restore_scn */

#pragma trace-
void drain( void )
{
    while( getkey() != EOF );       /* Flush buffer */
} /* drain */

/* eof: TRPPLOT.C */
