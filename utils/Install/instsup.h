#ifdef __cplusplus
extern "C" {
#endif	
unsigned long color_depth(void);
void sysreboot(void);
int mesg(char *format, ...);
int checktext(char *text, char *buffer, int buflen);
long findtext(char *text);
char *decode(char *input);
void viewfile(char *filename);
void stripcrlf(char *buffer);
unsigned long drivefree(int drive);
void error(char *format, ...);
void setdrivedir(char *drivedir);
void settempdir(void);
void getbootdrive(void);
int cmdrun(char *progname);
void PM_backslash(char *s);
int sendmessage(int destination, int messid);
void DoGUI(void);
#ifdef __cplusplus
}
#endif
int parse_ini(char *filename);
void updatepercent(void);