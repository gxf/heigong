#include "chm_lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

int print_html(struct chmFile *h,
              struct chmUnitInfo *ui,
              void *context)
{
    if(ui->flags & CHM_ENUMERATE_FILES) {
        char *p = strrchr(ui->path, '.');
        if (p && (strcasecmp(p, ".htm") == 0 || strcasecmp(p, ".html") == 0)) {
            printf("%s\n", ui->path);
        }
    }
	
    return CHM_ENUMERATOR_CONTINUE;
}

int main(int argc, const char *argv[])
{
    struct chmFile *h;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s chmfile\n", argv[0]);
        exit(1);
    }

    h = chm_open(argv[1]);
    if (h == NULL)
    {
        fprintf(stderr, "failed to open %s\n", argv[1]);
        exit(1);
    }

    if (! chm_enumerate(h, CHM_ENUMERATE_ALL, print_html, NULL)) {
        fprintf(stderr, "file format error\n"); exit(1); }

    chm_close(h);

    return 0;
}
