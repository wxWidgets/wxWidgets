%{
void guile_main(void *closure, int argc, char **argv) {
    char buffer[1024];
    void SWIG_init();
    SWIG_init();
    printf("starting Guile...\n");
    printf("guile >");
    while (fgets(buffer,1024,stdin)) {
	gh_eval_str(buffer);
	printf("guile >");
    }
}

void main(int argc, char **argv) {
    gh_enter(argc,argv, guile_main);
}
%}




