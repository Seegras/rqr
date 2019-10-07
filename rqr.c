/* 
    rqr.c -- RuneQuest Random Character Generator
             By Peter Keel <seegras@discordia.ch>

    RQR is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3.0

*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <popt.h>
#include <math.h>
#include "rqr.h"

#define GETOPT_NUMBER                        1010

int rqgen_of();
int rqgen_hr();
int genname();
int diceinit(void);
int dice(int num, int size);

struct character ch;
char charname[40];

static void printHelp(void) {
    fprintf(stderr,"\n                     -=[RuneQuest Character Generation]=-\n");
    fprintf(stderr,"                                 [Version 3.01]\n");
    fprintf(stderr,"                          <kludged up by Peter Keel>\n");
    fprintf(stderr,"                            <seegras@discordia.ch>\n");
    fprintf(stderr,"\n\n");
    fprintf(stderr,"Usage: rqr [OPTION]... [FILE]...\n");
    fprintf(stderr,"-h, --help                 print this message\n");
    fprintf(stderr,"-v, --version              print the version\n");
    fprintf(stderr,"-n, --number               specify a number of characters\n");
    fprintf(stderr,"-r, --houserules           use houserules\n");
    fprintf(stderr,"\n");
}

static void printVersion(void) {
    fprintf(stderr,"\n                     -=[RuneQuest Character Generation]=-\n");
    fprintf(stderr,"                                 [Version 3.01]\n");
    fprintf(stderr,"                          <kludged up by Peter Keel>\n");
    fprintf(stderr,"                            <seegras@discordia.ch>\n");
    fprintf(stderr,"\n\n");
    fprintf(stderr,"  RuneQuest(TM) Character Generator. Primarily thought as aid for gamemasters\n");
    fprintf(stderr,"  for helping them to create non-player characters.\n");
    fprintf(stderr,"\n");
    fprintf(stderr,"  Please note that this version complies to my house-rules,  and not the  \n");
    fprintf(stderr,"  official RuneQuest(TM) rules.\n");
    fprintf(stderr,"\n");
    fprintf(stderr,"  Free for copying, using, spreading, collecting and reverse-engineering. \n");
    fprintf(stderr,"  Not free for sale or for being copyrighted.                                \n");
    fprintf(stderr,"\n");
    fprintf(stderr,"  Internet:  Email: killer@discordia.ch   Homepage: http://killer.discordia.ch \n\n\n");
}

int main(int argc, const char ** argv) {

FILE *fp;
int times, k, rc;
int help = 0, version = 0, getnumber = 0, house = 0;
char * optarg;
char * number;
const char * leftarg;
const char * fname;
struct poptOption optionsTable[] = {
    { "help", 'h', 0, &help, 0, "gives help" },
    { "version", 'v', 0, &version, 0 },
    { "houserules", 'r', 0, &house, 0 },
    { "number", 'n', POPT_ARG_STRING, 0, GETOPT_NUMBER },
    { 0, 0, 0, 0, 0 } 
};
poptContext poptcon;

/*--- Initialisation of the random number generator ---*/
diceinit();

/*--- parameter-processing ----------------------------*/
/*--- argv[dat] points to file, argv[par] to number ---*/

    poptcon = poptGetContext("popti", argc, argv, optionsTable, 0);
    while ((rc = poptGetNextOpt(poptcon)) > 0) { 
        optarg = poptGetOptArg(poptcon);
        switch (rc) {
            case 'h': 
                help = 1;
                    break;
            case 'v': 
                version = 1;
                    break;
            case 'r': 
                house = 1;
                    break;
            case GETOPT_NUMBER:
                number = optarg;
                if ((number == NULL) || number[0] == '-') {
                    fprintf(stderr, "no parameter for -n given\n");
                    getnumber = 0;
                }
                else
                    getnumber = 1;
                break;
            default:
                fprintf(stderr, "Internal Error with parameter-processing\n");
                exit(1);
        }
    }
    leftarg = poptGetArg(poptcon);
    fname = leftarg;

    poptFreeContext(poptcon);
    if (version) printVersion();
    if (help) printHelp();


/*--- open file, if impossible, use stdout -------------*/
    if (fname)  {
        if ((fp = fopen( fname, "w")) == NULL) {
            perror("Could not open file");
            fp = stdout; }
        }
    else fp = stdout;

/*--- how many characters do we need? ------------------*/
    if (getnumber) {
        times = atoi(number);
        if ((times == 0) && (getnumber != 0))
        times = 1;
    }
    else times = 1;

/*--- print header -------------------------------------*/
if (!(help) && !(version)) {
    fprintf(fp,"\n                     -=[RuneQuest Character Generation]=-\n");
    fprintf(fp,"                                 [Version 3.01]\n");
    fprintf(fp,"                          <kludged up by Peter Keel>\n");
    fprintf(fp,"                            <seegras@discordia.ch>\n");
    fprintf(fp,"\n");
    fprintf(fp,"  Es wurden %d Charaktere generiert.\n\n", times);

/*--- begin character generation loop ------------------*/
    for (k=0; k < times; k++) { 

/*--- call rqgen-function ------------------------------*/
    if (house == 1) {
        rqgen_hr();
        genname();
/*--- output of generated character --------------------*/
        fprintf(fp,"\nName          : %s", charname);
        fprintf(fp,"\nEigenschaften : ST %2d  KO %2d  GR %2d  IN %2d  MA %2d  GE %2d  AU %2d  GW %2d",
            ch.eigen[0], ch.eigen[1], ch.eigen[2], ch.eigen[3], ch.eigen[4], ch.eigen[5], ch.eigen[6], ch.eigen[7]);
        fprintf(fp,"\nAttribute     : LP %2d  AP %2d  MP %2d  ", ch.eigen[8], ch.eigen[9], ch.eigen[10]);
        fprintf(fp,"BW %.1f ", sqrt((ch.eigen[2]+ch.eigen[5])/2.5));
        fprintf(fp,"  GR-Klasse %2d  GE-Klasse %2d  ",ch.attr[1], ch.attr[2]);
        fprintf(fp,"\nBoni          : ");
        fprintf(fp,"Bewegung %2.0f  Gewandtheit %2.0f  Handhabung %2.0f  ", ch.boni[1], ch.boni[0], ch.boni[2]);
        fprintf(fp,"Heimlichkeit %2.0f  ", ch.boni[3]);
        fprintf(fp,"\n                ");
        fprintf(fp,"Kommunikation %2.0f  Wahrnehmung %2.0f  Wissen %2.0f  ", ch.boni[4], ch.boni[6], ch.boni[5]);
        fprintf(fp,"Magie %2.0f  ", ch.boni[7]);
        fprintf(fp,"\n                ");
        fprintf(fp,"Angriff %2.0f  Abwehr %2.0f  ", ch.boni[8], ch.boni[9]);
        fprintf(fp,"\nKoerperzonen  : ");
        fprintf(fp,"Kopf %2d LP ", ch.lp[0]);
        fprintf(fp,"Arme %2d LP ", ch.lp[1]);
        fprintf(fp,"Brust %2d LP ", ch.lp[2]);
        fprintf(fp,"Bauch %2d LP ", ch.lp[3]);
        fprintf(fp,"Beine %2d LP ", ch.lp[4]);
        fprintf(fp,"\n\n");
    }
    else {
        rqgen_of();
        genname();
        fprintf(fp,"\nName          : %s", charname);
        fprintf(fp,"\nEigenschaften : ST %2d  KO %2d  GR %2d  IN %2d  MA %2d  GE %2d  AU %2d  ",
            ch.eigen[0], ch.eigen[1], ch.eigen[2], ch.eigen[3], ch.eigen[4], ch.eigen[5], ch.eigen[6]);
        fprintf(fp,"\nAttribute     : LP %2d  AP %2d  MP %2d  ", ch.eigen[8], ch.eigen[9], ch.eigen[10]);
        fprintf(fp,"BW %2d  GR-Klasse %2d  GE-Klasse %2d  ",ch.attr[0], ch.attr[1], ch.attr[2]);
        fprintf(fp,"\nBoni          : ");
        fprintf(fp,"Gewandtheit %2.0f  Handhabung %2.0f  ", ch.boni[0], ch.boni[2]);
        fprintf(fp,"Heimlichkeit %2.0f  ", ch.boni[3]);
        fprintf(fp,"\n                ");
        fprintf(fp,"Kommunikation %2.0f  Wahrnehmung %2.0f  Wissen %2.0f  ", ch.boni[4], ch.boni[6], ch.boni[5]);
        fprintf(fp,"Magie %2.0f  ", ch.boni[7]);
        fprintf(fp,"\n                ");
        fprintf(fp,"Angriff %2.0f  Abwehr %2.0f  ", ch.boni[8], ch.boni[9]);
        fprintf(fp,"\nKoerperzonen  : ");
        fprintf(fp,"Kopf %2d LP ", ch.lp[0]);
        fprintf(fp,"Arme %2d LP ", ch.lp[1]);
        fprintf(fp,"Brust %2d LP ", ch.lp[2]);
        fprintf(fp,"Bauch %2d LP ", ch.lp[3]);
        fprintf(fp,"Beine %2d LP ", ch.lp[4]);
        fprintf(fp,"\n\n");
    }
}
}
/*--- clean up ---------------------------------*/
fclose(fp);
return 0;
}



/*---------------------------------------------------------*/
/*--- Roll Dice Functions  -  Needs time.h ----------------*/
/*--- dice(number of dices, number of sides)               */
/*--- diceinit()                                           */
/*---------------------------------------------------------*/
int dice(int num, int size) {
    int val=0;

    while (num) {
        val += (rand() % size+1);
        --num;
    }
    return(val);
}
/*--- initialize dice w/ random seed ----------------------*/
int diceinit() {
    long tick;

    srand(time(&tick) %20000 );
    return(0);
}


/*--------------------------------------------------------------------*/
/*--- RuneQuest Character Generations Sequence -----------------------*/
/*--- needs dice and diceinit ----------------------------------------*/
/*--------------------------------------------------------------------*/

int rqgen_of(){

    /*--- calculate characteristics ---*/
    ch.eigen[0] = dice(3,6);
    ch.eigen[1] = dice(3,6);
    ch.eigen[2] = dice(2,6)+6;
    ch.eigen[3] = dice(2,6)+6;
    ch.eigen[4] = dice(3,6);
    ch.eigen[5] = dice(3,6);
    ch.eigen[6] = dice(3,6);
    ch.eigen[8] = (ch.eigen[1]+ch.eigen[2]+1)/2;
    ch.eigen[9] = ch.eigen[1]+ch.eigen[2];
    ch.eigen[10] = ch.eigen[4];

    /*--- calculate boni ---*/
    ch.boni[0] = (ch.eigen[5]-10)+((ch.eigen[0]-10)/2)-(ch.eigen[2]-10);
    ch.boni[2] = (ch.eigen[5]-10)+(ch.eigen[3]-10)+((ch.eigen[0]-10)/2);
    ch.boni[3] = (ch.eigen[5]-10)-(ch.eigen[2]-10)-((ch.eigen[4]-10));
    ch.boni[4] = (ch.eigen[3]-10)+(ch.eigen[6]-10)/2+((ch.eigen[4]-10)/2);
    ch.boni[5] = (ch.eigen[3]-10);
    ch.boni[6] = (ch.eigen[3]-10)+(ch.eigen[4]-10)/2+((ch.eigen[1]-10)/2);
    ch.boni[7] = (ch.eigen[3]-10)+(ch.eigen[4]-10)+((ch.eigen[5]-10)/2);
    ch.boni[8] = ch.boni[2];
    ch.boni[9] = ch.boni[0];

    /*--- calculate hitpoints ---*/
    ch.lp[0]=(int)ch.eigen[8]/3;                /* Kopf */
    ch.lp[1]=(int)ch.eigen[8]/4;                /* Arme */
    ch.lp[2]=(int)(ch.eigen[8]*2)/5;            /* Brust */
    ch.lp[3]=(int)ch.eigen[8]/3;                /* Bauch */
    ch.lp[4]=(int)ch.eigen[8]/3;                /* Beine */

    /*--- Set movement speed ---*/
    ch.attr[0] = 3;

    /*--- calculate GR-Class ---*/
    if (ch.eigen[2] < 11)
        ch.attr[1]=3;
    else if (ch.eigen[2] > 10 && ch.eigen[2] < 15)
        ch.attr[1]=2;
    else if (ch.eigen[2] > 19)
        ch.attr[1]=0;
    else
        ch.attr[1]=1;

    /*--- calculate GE-Class ---*/
    if (ch.eigen[5] < 11)
        ch.attr[2]=4;
    else if (ch.eigen[5] > 10 && ch.eigen[2] < 15)
        ch.attr[2]=3;
    else if (ch.eigen[5] > 19)
        ch.attr[2]=1;
    else
        ch.attr[2]=2;

    return 0;
}

int rqgen_hr(){
    /*--- calculate characteristics ---*/
    ch.eigen[0] = dice(3,6);
    ch.eigen[1] = dice(3,6);
    ch.eigen[2] = dice(2,6)+6;
    ch.eigen[3] = dice(2,6)+6;
    ch.eigen[4] = dice(3,6);
    ch.eigen[5] = dice(3,6);
    ch.eigen[6] = dice(3,6);
    ch.eigen[7]= (dice(2,6)+7+ch.eigen[2])/2;
    ch.eigen[8] = (ch.eigen[1]+ch.eigen[7]+1)/2;
    ch.eigen[9] = ch.eigen[1]+ch.eigen[7];
    ch.eigen[10] = ch.eigen[4];

    /*--- calculate boni ---*/
    ch.boni[0] = (ch.eigen[5]-10)+((ch.eigen[0]-10)/2)-(ch.eigen[2]-10);
    ch.boni[1] = (ch.eigen[5]-10)+((ch.eigen[0]-10)/2)+((ch.eigen[2]-10)/2)-(ch.eigen[7]-10);
    ch.boni[2] = (ch.eigen[5]-10)+(ch.eigen[3]-10)+((ch.eigen[0]-10)/2);
    ch.boni[3] = (ch.eigen[5]-10)-(ch.eigen[2]-10)-((ch.eigen[4])/2);
    ch.boni[4] = (ch.eigen[3]-10)+(ch.eigen[6]-10)+((ch.eigen[4]-10)/2);
    ch.boni[5] = (ch.eigen[3]-10);
    ch.boni[6] = (ch.eigen[3]-10)+(ch.eigen[4]-10)+((ch.eigen[1]-10)/2);
    ch.boni[7] = (ch.eigen[3]-10)+(ch.eigen[4]-10)+((ch.eigen[5]-10)/2);
    ch.boni[8] = ch.boni[1];
    ch.boni[9] = ch.boni[0];

    /*--- calculate hitpoints ---*/
    ch.lp[0]=(int)ch.eigen[8]/3;                /* Kopf */
    ch.lp[1]=(int)ch.eigen[8]/4;                /* Arme */
    ch.lp[2]=(int)(ch.eigen[8]*2)/5;            /* Brust */
    ch.lp[3]=(int)ch.eigen[8]/3;                /* Bauch */
    ch.lp[4]=(int)ch.eigen[8]/3;                /* Beine */

    /*--- Set movement speed ---*/
    ch.attr[0] = 3;

    /*--- calculate GR-Class ---*/
    if (ch.eigen[2] < 11)   /* GR-Klasse */
        ch.attr[1]=3;
    else if (ch.eigen[2] > 10 && ch.eigen[2] < 15)
        ch.attr[1]=2;
    else if (ch.eigen[2] > 19)
        ch.attr[1]=0;
    else
        ch.attr[1]=1;

    /*--- calculate GE-Class ---*/
    if (ch.eigen[5] < 11)  /* GE-Klasse */
        ch.attr[2]=4;
    else if (ch.eigen[5] > 10 && ch.eigen[2] < 15)
        ch.attr[2]=3;
    else if (ch.eigen[5] > 19)
        ch.attr[2]=1;
    else
        ch.attr[2]=2;

    return 0;
}

int lengthof(char **ar) {
    int n = 0;
    while (*ar) {
        n++;
        ar++;
    }
    return (n);
}

#define rnd(n) (unsigned)(random() % (n))

int genname () {
int letters, vowel, numvowels, numcons;
char name[40];
char *vowels[] = { "a", "a","e","e","e","ei","i","i","o","u","y",0 };
char *cons[] = {"b","c","ch","ck","cr","d","d","dr""f","fr","fl","g",
                "gl","h","h","j","k","kr","l","m","n","n","n","n","nt",
                "p","ph","qu","r","r","r","s","s","sh","sh","st","t",
                "t","t","t","t","th","tr""v","w","x","y","z",0 };

    numvowels = lengthof(vowels);
    numcons = lengthof(cons);

    name[0] = '\0';
    vowel = (rnd(2) ? 1 : 0);
    if (vowel)
        strcat(name, vowels[rnd(numvowels - 1)]);
    else
        strcat(name, cons[rnd(numcons - 1)]);
    name[0] = toupper(name[0]);
    for (letters = rnd(4) + 3; letters > 0; letters--)         {
        vowel = (vowel ? 0 : 1);
        if (vowel)
            strcat(name, vowels[rnd(numvowels)]);
        else
            strcat(name, cons[rnd(numcons)]);
    }
    strncpy(charname,name,sizeof(charname));
    return(0);
}
