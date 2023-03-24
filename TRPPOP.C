/* File: TRPPOP.C
*/

#pragma trace-
#include <stdio.h>
#include <bios.h>
#include <dos.h>
#include <malloc.h>
#include "trpdefs.h"

#pragma trace-
extern int black_b, blue_b, green_b, cyan_b, red_b, magenta_b, brown_b, white_b;
extern int black_f, blue_f, green_f, cyan_f, red_f, magenta_f, brown_f, white_f;
extern int gray_f, ltblue_f, ltgreen_f, ltcyan_f, ltred_f;
extern int ltmagenta_f, yellow_f, brtwhite_f;
extern int vidbase;             /* video text base segment address */


/* assembly language functions called from popup */
void vpoke(int segment, int offset, int value);
int vpeek(int segment, int offset);

#define CRTWIDTH   80     /* width of the CRT  (in characters) */
#define CRTHEIGHT  25     /* height of the CRT (in characters) */
#define ABORT      0xFF   /* number returned if escape pressed */

  struct video_bytes {
    unsigned char  ch;    /* character value of video word */
    unsigned char  attr;  /* attribute value of video word */
    };

  union video_word {
    struct video_bytes byte;
    int word;
    };

#pragma trace-
int popup(int row, int col, char *menu[], BOOL hots[] )
{
/* display a popup menu (menu[0] .. menu[n])
   within a window located at the screen position
   specified by row and col.  Use the up-arrow and
   down-arrow keys to position the cursor.  Press
   the enter key to select the option under the
   cursor.

   Returns: 0 if menu[0] selected
            1 if menu[1] selected
                ...
           -1 if Esc key pressed                */

  int c, i, j, length, address, offset, offset2;
  int currow, maxrow, minrow, direction;
  int choice, bwidth, bheight, width = 0, height = 0;
  int *buffer, *bufptr;
  char *strptr, *strptr2, *(*hotkey)[];
  union video_word video;
  BOOL trace_variable, invalid_choice = FALSE;

/* turn off cursor */
  cursoff();

/* calculate address of row and col */
  address = (row * CRTWIDTH * 2) + (col * 2);

/* calculate width and height of menu window */
  for (i=0; menu[i] != NULL; i++) {
    length = strlen(menu[i]);
    if (length > width) width = length;
    height++;
    }

/* width and height must be greater than 0 */
  if (width == 0 || height == 0) {
    printf("\n*** popup function: invalid menu argument ***\n");
    exit(0);
    }

  /* allow for a border around the text */
  bwidth = width + 2;
  bheight = height + 2;


/* allocate buffer for saving area of screen beneath popup
   and array of hotkey characters
*/
  buffer = calloc(bwidth * bheight, 2);
  hotkey = calloc(height, sizeof(char *));
  if ((buffer == NULL) || hotkey == NULL) {
    printf("\n*** popup function: out of memory ***\n");
    exit(0);
    }



/* save area of screen that will be modified */
  currow = address;
  bufptr = buffer;
  for (i = 0; i < bheight; i++) {
    offset = currow;
    for (j = 0; j < bwidth; j++) {
      *bufptr++ = vpeek(vidbase, offset);
      offset += 2;
      }
    currow += (CRTWIDTH * 2);
    }

/* draw menu outline */
  video.byte.attr = cyan_b + blue_f;
  offset = address;
  video.byte.ch = '\xD5';                 /* upper left corner */
  vpoke(vidbase, offset, video.word);
  offset += (bwidth - 1) * 2;
  video.byte.ch = '\xB8';                 /* upper right corner */
  vpoke(vidbase, offset, video.word);
  offset = address + (bheight - 1) * CRTWIDTH * 2;
  video.byte.ch = '\xD4';                 /* lower left corner */
  vpoke(vidbase, offset, video.word);
  offset += (bwidth - 1) * 2;
  video.byte.ch = '\xBE';                 /* lower right corner */
  vpoke(vidbase, offset, video.word);
  offset = address + 2;
  offset2 = offset + (bheight - 1) * CRTWIDTH * 2;
  video.byte.ch = '\xcd';                 /* horizontal lines */
  for (i = 0; i < width; i++) {
    vpoke(vidbase, offset, video.word);
    offset += 2;
    vpoke(vidbase, offset2, video.word);
    offset2 += 2;
    }
  offset = address + CRTWIDTH * 2;
  offset2 = offset + (bwidth - 1) * 2;
  video.byte.ch = '\xB3';                 /* vertical lines */
  for (i = 0; i < height; i++) {
    vpoke(vidbase, offset, video.word);
    offset += CRTWIDTH * 2;
    vpoke(vidbase, offset2, video.word);
    offset2 += CRTWIDTH * 2;
    }

/* make the first uppercase character in each line of
   the menu a hotkey
*/
  for( i=0; i<height; i++ )
        (*hotkey)[i] = strpbrk( menu[i], "ABCDEFGHIJKLMNOPQRSTUVWXYZ" );

/* fill in menu text */
  minrow = address + (CRTWIDTH * 2) + 2;
  currow = minrow;
  video.byte.attr = WHITE_B|BLACK_F;
  for (i = 0; i < height; i++) {
    strptr = menu[i];
    offset = currow;
    for (j = 0; j < width; j++) {
      if (*strptr == '\0') video.byte.ch = ' ';
      else {
        if( (i!=0) && (hots[i] == TRUE) && (strptr == (*hotkey)[i]) )
            video.byte.attr = cyan_b + brtwhite_f;
        video.byte.ch = *strptr++;
        }
      vpoke(vidbase, offset, video.word);
      offset += 2;
      if( video.byte.attr == cyan_b + brtwhite_f )
        video.byte.attr = cyan_b + blue_f;
      }
    video.byte.attr = cyan_b + blue_f;
    maxrow = currow;
    currow += (CRTWIDTH * 2);
    }

/* get menu choice */
  currow = minrow;
  choice = 0;

  if( ! hots[0] ) {
    c = FUNCTION;
    direction = DNAROW;
  }
  else c = getch();
  while( ( c != ENTER ) && ( c != ESC ) ) {
    if( c == FUNCTION ) {
      if( hots[choice] ) c = getch(); /* get second character of key */
      else c = direction;
      direction = c;    /* Save function key pressed */
      j = choice;
      offset = currow;
      strptr = menu[choice];
      switch (c) {

        case DNAROW:
          if( currow == maxrow ) {
            currow = minrow;
            choice = 0;
            }
          else {
            currow += CRTWIDTH * 2;
            choice++;
            }
          break;

        case UPAROW:
          if( currow == minrow ) {
            currow = maxrow;
            choice = height - 1;
            }
          else {
            currow -= CRTWIDTH * 2;
            choice--;
            }
          break;

        default:
          sound( 200, 5 );
        }
      offset2 = currow;
      strptr2 = menu[choice];
      for (i = 0; i < width; i++) {
        video.byte.attr = cyan_b + blue_f;
        if (*strptr == '\0') video.byte.ch = ' ';
        else {
          if( (strptr == (*hotkey)[j]) && (hots[j] == TRUE) )
            video.byte.attr = cyan_b + brtwhite_f;
          video.byte.ch = *strptr++;
          }
        vpoke(vidbase, offset, video.word);
        offset += 2;
        if (*strptr2 == '\0') video.byte.ch = ' ';
        else video.byte.ch = *strptr2++;
        video.byte.attr = WHITE_B|BLACK_F;
        vpoke(vidbase, offset2, video.word);
        offset2 += 2;
        }
      if( hots[choice] ) c = getch(); /* If landed on valid choice wait for user */
      else c = FUNCTION; /* else try next one */
      }
    else {
      for (i = 0; i < height; i++) {
        if( (toupper(c) == *((*hotkey)[i])) && hots[i] ) {
          choice = i;
          break;
          }
        }
      if (i < height) break;
      else { /* Not a hotkey so just beep */
        sound( 200, 5 );
        c = getch();
      }
    }
  }

/* restore saved screen area beneath popup menu */
  currow = address;
  bufptr = buffer;
  for (i = 0; i < bheight; i++) {
    offset = currow;
    for (j = 0; j < bwidth; j++) {
      vpoke(vidbase, offset, *bufptr++);
      offset += 2;
      }
    currow += (CRTWIDTH * 2);
    }

/* free the allocated memory */
  free(buffer);
  free(hotkey);

/* return the menu choice */
  curson();
  if( c == ESC ) return -1;
  else return choice;
}


/* eof: TRPPOP.C */
