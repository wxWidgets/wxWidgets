#ifdef __cplusplus
extern "C" {
#endif
	
#define I_DialogBeta 329
#define I_Dialog1 330
#define I_Dialog2 331
#define I_Dialog3 332
#define I_Dialog4 333
#define I_Dialog5 334
#define I_Dialog6 335
#define I_Dialog7 336
#define I_Dialog8 337
#define I_Dialog9 338
#define I_Dialog10 339
#define I_Dialog11 340
#define I_Dialog12 341

#define I_Cancel 258
#define I_Icon 261
#define I_Logo 262
#define I_Ja 265
#define I_Nein 266
#define I_Halt 267
#define I_Alle 268
#define I_Karte 269
#define I_Confirm 270
#define I_BMPWin 271
#define I_Dest 300
#define I_Text 301
#define I_OK 304
#define I_Cancel2 305
#define I_Logo16 306

#define NONE 0
#define INSTALLING 1
#define ABORTED 2
#define COMPLETED 3

#define UPDATE_ALWAYS 1
#define UPDATE_NONEXIST 2

#define DLG_PACKAGESTOINSTALL 305
#define PACKAGES 306
#define PB_SELECTALL 308
#define PB_DESELECTALL 309
#define DLG_SELECTINSTALLATIONDRIVE 311
#define PB_OK 313
#define PB_CANCEL 314
#define DRIVELIST 312
#define I_LOGO1 316
#define I_LOGO2 317
#define I_LOGO3 318
#define I_LOGO4 319
#define I_LOGO5 320
#define I_LOGO6 321
#define I_LOGO7 322
#define I_LOGO8 323
#define I_LOGOBETA 324

/* These are static */
#define I_Status 348
#define I_Welcome 349

/* Keep these in order... so we can loop */
#define I_INFOBOX 355
#define I_Info1 356
#define I_Info2 357
#define I_Info3 358
#define I_README 359
#define I_Licence 360
#define I_Agree 361
#define I_Updates1 362
#define I_Updates2 363
#define I_Updates4 364
#define I_Updates3 365
#define I_Dirs 366
#define I_InstallDir 367
#define I_Drives 368
#define I_Percent 369
#define I_Back 370
#define I_Next 371
#define I_Progress 372
#define I_Grid 373
#define I_MAX 374

#define FILEICON 3800
#define FOLDERICON 3810

#define THREAD_STACK_SIZE 32768

#ifndef MAX_PATH
#define MAX_PATH 1024
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* #define ENABLE_LOGGING */


void grabfile(char *filename);
void readme(void *param);
void install_thread(void *param);
void install_init(char *installername);
int getparseline(FILE *f, char comment, char delimiter, char quotes, char *raw, char *entry, char *entrydata, char *entrydata2);

#ifdef __cplusplus
}
#endif

