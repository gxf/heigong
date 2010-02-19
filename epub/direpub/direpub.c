#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <errno.h>
#include "zip.h"

struct zip *zipopen(const char filename[])
{
    struct zip *z;
    int error = 0;

    z = zip_open(filename, 0, &error);
    if (z == NULL) {
	char errstr[1024];
	zip_error_to_str(errstr, 1024, error, errno);
	fprintf(stderr, "Error opening %s: %s", filename, errstr);
	exit(1);
    }

    return z;
}

int zread(struct zip *z, const char filename[], char *buf, int size)
{
    int flags = ZIP_FL_UNCHANGED;
    struct zip_file *f = zip_fopen(z, filename, flags);
    int n, nread = 0;

    if (f == NULL) {
	fprintf(stderr, "Error opening %s in zip archive: %s", 
		filename, zip_strerror(z));

	return -1;
    }

    while (nread < size) {
	n = zip_fread(f, buf + nread, size - nread);
	if (n == 0) {
	    zip_fclose(f);
	    break;
	} else if (n > 0) {
	    nread += n;
	    continue;
	}

	fprintf(stderr, "Error reading %s in zip archive: %s", 
		filename, zip_file_strerror(f));
	zip_fclose(f);
	return -1;
    }

    return nread;
}

xmlDocPtr zread_xml(struct zip *z, const char filename[])
{
    struct zip_stat st;
    char *buf;
    xmlDocPtr doc;

    if (zip_stat(z, filename, ZIP_FL_UNCHANGED, &st) != 0) {
        fprintf(stderr, "path %s not found\n", filename);
        exit(1);
    }


    buf = malloc(st.size);
    if (buf == NULL) {
	fprintf(stderr, "out of memory\n");
	exit(1);
    }

    st.size = zread(z, st.name, buf, st.size);

    if (st.size < 0) {
	fprintf(stderr, "read %s error\n", filename);
	exit(1);
    }

    //write(1, buf, st.size);

    doc = xmlReadMemory(buf, st.size, filename, NULL, 0);
    free(buf);
    if (doc == NULL) {
	fprintf(stderr, "Failed to parse %s\n", filename);
	exit(1);
    }

    return doc;
}

char *find_rootfile(xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    char *rootfile;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
	if (cur_node->type == XML_ELEMENT_NODE) {
	    if (strcasecmp(cur_node->name, "rootfile") == 0) {
		rootfile = xmlGetProp(cur_node, "full-path");
		if (rootfile != NULL) return rootfile;
	    }
	}

	rootfile = find_rootfile(cur_node->children);
	if (rootfile != NULL) return rootfile;
    }

    return NULL;
}

char *find_ncxfile(xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    char *id, *ncx;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
	if (cur_node->type == XML_ELEMENT_NODE) {
	    if (strcasecmp(cur_node->name, "item") == 0) {
		id = xmlGetProp(cur_node, "id");
		if (strcmp("ncx", id) != 0) continue;
		ncx = xmlGetProp(cur_node, "href");
		if (ncx != NULL) return ncx;
	    }
	}

	ncx = find_ncxfile(cur_node->children);
	if (ncx != NULL) return ncx;
    }

    return NULL;
}

int print_guide_references(xmlNode * a_node, const char parent_dir[])
{
    xmlNode *cur_node = NULL;
    int r = 0;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
	if (cur_node->type == XML_ELEMENT_NODE) {
	    if (strcasecmp(cur_node->name, "reference") == 0) {
		char *title = xmlGetProp(cur_node, "title");
		char *href = xmlGetProp(cur_node, "href");
		if (title != NULL && href != NULL) {
		    printf("%s\n%s%s\n", title, parent_dir, href);
		    ++r;
		}
	    }
	}

	r += print_guide_references(cur_node->children, parent_dir);
    }

    return r;
}

char *find_navlabel(xmlNode * a_node)
{
    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
	if (cur_node->type == XML_ELEMENT_NODE) {
	    if (strcasecmp(cur_node->name, "text") == 0) {
		if (cur_node->children != NULL)
		return cur_node->children->content;
	    }
	}
    }

    return NULL;
}

char *level_to_tabs(int level)
{
    static char buf[64];
    int i;

    if (level >= 64) {
	return "TOO MANY LEVELS";
    }

    for (i = 0; i < level; ++i) {
	buf[i] = '\t';
    }

    buf[level] = '\0';

    return buf;
}

int print_navpoints(xmlNode * a_node, const char parent_dir[], int level)
{
    xmlNode *cur_node = NULL;
    char *label = NULL, *src = NULL;
    int r = 0;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
	if (cur_node->type == XML_ELEMENT_NODE) {
	    if (strcasecmp(cur_node->name, "navLabel") == 0) {
		label = find_navlabel(cur_node->children);
	    } else if (strcasecmp(cur_node->name, "content") == 0){
		src = xmlGetProp(cur_node, "src");
	    } else if (strcasecmp(cur_node->name, "navPoint") == 0){
		r += print_navpoints(cur_node->children, parent_dir, level + 1);
	    }

	    if (label != NULL && src != NULL) {
		printf("%s%s\n", level_to_tabs(level), label);
		printf("%s%s%s\n", level_to_tabs(level), parent_dir, src);
		label = NULL;
		src = NULL;
		++r;
	    }
	}
    }

    return r;
}

int print_navmap(xmlNode * a_node, const char parent_dir[])
{
    xmlNode *cur_node = NULL;
    int r = 0;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
	if (cur_node->type == XML_ELEMENT_NODE) {
	    if (strcasecmp(cur_node->name, "navPoint") == 0) {
		r += print_navpoints(cur_node->children, parent_dir, 0);
		continue;
	    }
	}

	r += print_navmap(cur_node->children, parent_dir);
    }

    return r;
}

int main(int argc, const char *argv[])
{
    int force_opf = 0, n = 0;
    struct zip *z;
    const char metafile[] = "META-INF/container.xml";
    char *rootfile, *ncxfile, *p;
    char buf[1024];
    xmlDocPtr meta_doc, root_doc, ncx_doc;

    LIBXML_TEST_VERSION

    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s epubfile\n", argv[0]);
        exit(1);
    }

    if (argc == 3) {
	if (strcmp("opf", argv[2]) == 0)
	    force_opf = 0;
    }

    z = zipopen(argv[1]);

    meta_doc = zread_xml(z, metafile);
    rootfile = find_rootfile(xmlDocGetRootElement(meta_doc));
    rootfile = strcpy(buf, rootfile);
    xmlFreeDoc(meta_doc);

    root_doc = zread_xml(z, rootfile);

    p = strrchr(buf, '/');
    if (p == NULL) {
	buf[0] = '\0';
	p = buf;
    } else {
	*(++p) = '\0';
    }

    ncxfile = find_ncxfile(xmlDocGetRootElement(root_doc));

    if (force_opf || ncxfile == NULL) {
	n = print_guide_references(xmlDocGetRootElement(root_doc), buf);
    }

    if (n == 0 && ncxfile != NULL) {
	strcat(buf, ncxfile);
	ncx_doc = zread_xml(z, buf);
	*p = '\0';
	n = print_navmap(xmlDocGetRootElement(ncx_doc), buf);
	xmlFreeDoc(ncx_doc);
    }

    if (n == 0 && !force_opf) {
	n = print_guide_references(xmlDocGetRootElement(root_doc), buf);
    }

    xmlFreeDoc(root_doc);

    zip_close(z);

    return 0;
}

