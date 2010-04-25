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

static struct zip *zz;

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

int zsize(struct zip *z, const char filename[])
{
    struct zip_stat st;

    if (zip_stat(z, filename, ZIP_FL_UNCHANGED, &st) != 0) {
        fprintf(stderr, "path %s not found\n", filename);
	return -1;
    }

    return st.size;
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
		if (strcmp("toc", id) == 0) {
		    ncx = xmlGetProp(cur_node, "href");
		    if (ncx != NULL && strstr(ncx, "ncx") != NULL) return ncx;
		}
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

char *unescape(char xmltext[])
{
    static char buffer[4096];
    char *p, *q;

    if (strlen(xmltext) >= 4096) return xmltext;
    
    for (p = xmltext, q = buffer; *p != '\0'; ++p, ++q) {
	if (*p == '&') {
	    if (strncmp("&#x", p, 3) == 0) {
		int c = 0;
		for (p += 3; *p != ';'; ++p) {
		    int h = *p;

		    if (h >= '0' && h <= '9') h -= '0';
		    else if (h >= 'a' && h <= 'f') h = h + 10 - 'a';
		    else if (h >= 'A' && h <= 'F') h = h + 10 - 'A';
		    else h = 0;

		    c = c * 16 + h;
		}

		if (c <= 0x7F) {
		    *q = c;
		} else if (c < 0x7FF) {
		    *q++ = 0xc0 | ((c >> 6) & 0x1f);
		    *q = 0x80 | (c & 0x3f);
		} else {
		    *q++ = 0xe0 | ((c >> 12) & 0x0f);
		    *q++ = 0x80 | ((c >> 6) & 0x3f);
		    *q = 0x80 | (c & 0x3f);
		}
	    } else if (strncmp("&#", p, 2) == 0) {
		int c = 0;
		for (p += 2; *p != ';'; ++p) {
		    int h = (*p) - '0';
		    if (h < 0 || h > 9) h = 0;
		    c = c * 10 + h;
		}

		if (c <= 0x7F) {
		    *q = c;
		} else if (c < 0x7FF) {
		    *q++ = 0xc0 | ((c >> 6) & 0x1f);
		    *q = 0x80 | (c & 0x3f);
		} else {
		    *q++ = 0xe0 | ((c >> 12) & 0x0f);
		    *q++ = 0x80 | ((c >> 6) & 0x3f);
		    *q = 0x80 | (c & 0x3f);
		}
	    } else if (strncmp("&lt;", p, 4) == 0) {
		p += 3;
		*q = '<';
	    } else if (strncmp("&gt;", p, 4) == 0) {
		p += 3;
		*q = '>';
	    } else if (strncmp("&apos;", p, 6) == 0) {
		p += 5;
		*q = '\'';
	    } else if (strncmp("&quot;", p, 6) == 0) {
		p += 5;
		*q = '\"';
	    } else if (strncmp("&amp;", p, 5) == 0) {
		p += 4;
		*q = '&';
	    } else {
		*q = *p;
	    }
	} else {
	    *q = *p;
	}
    }
    *q = '\0';
    return buffer;
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
		    char filename[4096], *p;
		    int size;

		    sprintf(filename, "%s%s", parent_dir, href);
		    p = strrchr(filename, '#');
		    if (p != NULL) *p = '\0';

		    size = zsize(zz, filename);
		    if (size > 0) {
			printf("%s\n%s%s\n%d\n", unescape(title), parent_dir, href, size);
		    }
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
		char filename[4096], *p;
		int size;

		sprintf(filename, "%s%s", parent_dir, src);
		p = strrchr(filename, '#');
		if (p != NULL) *p = '\0';
		
		size = zsize(zz, filename);
		if (size > 0) {
		    char *tabs = level_to_tabs(level);
		    printf("%s%s\n", tabs, unescape(label));
		    printf("%s%s%s\n", tabs, parent_dir, src);
		    printf("%s%d\n", tabs, size);
		    label = NULL;
		    src = NULL;
		}
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
    const char metafile[] = "META-INF/container.xml";
    char *rootfile, *ncxfile, *p;
    char buf[1024];
    struct zip *z;
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

    zz = z = zipopen(argv[1]);

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

