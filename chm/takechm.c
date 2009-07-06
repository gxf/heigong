#include "chm_lib.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>

static int htmlpp(const char *buf, LONGINT64 size, const char *path);

static char bpath[2048];
struct chmFile *h;

int main(int argc, const char *argv[])
{
    struct chmUnitInfo ui;

    LIBXML_TEST_VERSION

    if (argc < 4) {
        fprintf(stderr, "Usage: %s chmfile htmlpath basepath\n", argv[0]);
        exit(1);
    }

    strcpy(bpath, argv[3]);

    h = chm_open(argv[1]);
    if (h == NULL)
    {
        fprintf(stderr, "failed to open %s\n", argv[1]);
        exit(1);
    }

    if (chm_resolve_object(h, argv[2], &ui) != 0) {
        fprintf(stderr, "path %s not found\n", argv[2]);
        exit(1);
    }

    char *buf = malloc(ui.length);

    chm_retrieve_object(h, &ui, buf, 0, ui.length);

    htmlpp(buf, ui.length, ui.path);

    free(buf);

    chm_close(h);

    return 0;
}

htmlNodePtr erase(htmlNodePtr node)
{
    xmlUnlinkNode(node);
    xmlFreeNode(node);
    return NULL;
}

htmlNodePtr strip(htmlNodePtr node)
{
    htmlNodePtr parent = node->parent, 
                prev = node->prev,
                next = node->next,
                cur;

    if (! node->children) {
        xmlUnlinkNode(node);
        xmlFreeNode(node);
        return next;
    }

    for (cur = node->children; cur; cur = cur->next)
        cur->parent = parent;
                
    if (parent && parent->children == node)
        parent->children = node->children;

    if (parent && parent->last == node)
        parent->last = node->last;

    if (prev)
        prev->next = node->children;
    if (next)
        next->prev = node->last;

    if (node->last)
        node->last->next = next;
    if (node->children) {
        node->children->prev = prev;
        next = node->children;
    }

    node->parent = NULL;
    node->prev = NULL;
    node->next = NULL;
    node->children = NULL;
    node->last = NULL;

    xmlFreeNode(node);

    return next;
}

void wash(htmlNodePtr node)
{
    xmlAttr *cur = node->properties, *next;

    while (cur) {
        const char *name = cur->name;

        next = cur->next;

        if (strcasecmp(name, "src") == 0) {
            cur = next;
            continue;
        }
        if (strcasecmp(name, "height") == 0) {
            cur = next;
            continue;
        }
        if (strcasecmp(name, "width") == 0) {
            cur = next;
            continue;
        }

        if (next) {
            next->prev = cur->prev;
        }
        if (cur->prev) {
            cur->prev->next = next;
        }
        if (node->properties == cur) {
            node->properties = next;
        }

        cur->parent = NULL;
        cur->prev = NULL;
        cur->next = NULL;

        // xmlFreeAttribute(cur);

        cur = next;
    }
}

static char ppath[2048];
static char *parent(const char *path)
{
    char *p; 
    if (path == NULL) return NULL;
    p = strrchr(path, '/');
    if (p == NULL) return NULL;
    strncpy(ppath, path, p - path);
    ppath[p - path] = '\0';
    return ppath;
}

static int exists(const char *path)
{
    struct stat statbuf;
    if (stat(path, &statbuf) != -1)
        return 1;
    else
        return 0;
}

static int rmkdir(char *path)
{
    char *i = strrchr(path, '/');

    if(path[0] == '\0'  ||  exists(path))
        return 0;

    if (i != NULL)
    {
        *i = '\0';
        rmkdir(path);
        *i = '/';
        mkdir(path, 0777);
    } else {
        mkdir(path, 0777);
    }

    if (exists(path))
        return 0;
    else
        return -1;
}

static int mkdirs(char *path)
{

    if(path == NULL || path[0] == '\0'  ||  exists(path))
        return 0;

    int r = 0;

    char *i = strrchr(path, '/');
    if (i != NULL)
    {
        *i = '\0';
        r = rmkdir(path);
        *i = '/';
    }

    return r;
}

void save(const char *path)
{
    FILE *out;
    char fpath[2048];
    char ipath[2048];
    struct chmUnitInfo ui;

    if (path == NULL) return;
    if (path[0] == '/') {
        snprintf(ipath, 2048, "%s", path);
        snprintf(fpath, 2048, "%s%s", bpath, path);
    } else {
        snprintf(ipath, 2048, "%s/%s", ppath, path);
        snprintf(fpath, 2048, "%s%s/%s", bpath, ppath, path);
    }

    // fprintf(stderr, "save %s to %s\n", ipath, fpath);

    if (chm_resolve_object(h, ipath, &ui) != 0) {
        fprintf(stderr, "path %s not found\n", ipath);
        return;
    }

    mkdirs(fpath);
    out = fopen(fpath, "w");

    char *buf = malloc(ui.length);

    chm_retrieve_object(h, &ui, buf, 0, ui.length);

    fwrite(buf, ui.length, 1, out);

    free(buf);

    fclose(out);
}

void extract(htmlNodePtr node)
{
    xmlAttr *cur = node->properties, *next;

    while (cur) {
        const char *name = cur->name;

        next = cur->next;

        if (strcasecmp(name, "height") == 0) {
            cur = next;
            continue;
        }
        if (strcasecmp(name, "width") == 0) {
            cur = next;
            continue;
        }
        if (strcasecmp(name, "src") == 0) {
            save(cur->children->content);
            cur = next;
            continue;
        }

        if (next) {
            next->prev = cur->prev;
        }
        if (cur->prev) {
            cur->prev->next = next;
        }
        if (node->properties == cur) {
            node->properties = next;
        }

        cur->parent = NULL;
        cur->prev = NULL;
        cur->next = NULL;

        // xmlFreeAttribute(cur);

        cur = next;
    }
}

enum {
    KEEP = 0,
    STRIP,
    ERASE,
    IMG
};

int decide(const char * tag)
{
    if (strcasecmp(tag, "img") == 0)
        return IMG;

    if (strcasecmp(tag, "script") == 0)
        return ERASE;
    if (strcasecmp(tag, "style") == 0)
        return ERASE;
    if (strcasecmp(tag, "meta") == 0)
        return ERASE;
    if (strcasecmp(tag, "form") == 0)
        return ERASE;
    if (strcasecmp(tag, "iframe") == 0)
        return ERASE;

    if (strcasecmp(tag, "a") == 0)
        return STRIP;
    if (strcasecmp(tag, "table") == 0)
        return STRIP;
    if (strcasecmp(tag, "tr") == 0)
        return STRIP;
    if (strcasecmp(tag, "td") == 0)
        return STRIP;

    return 0;
}

static void
traverse(htmlNodePtr root)
{
    htmlNodePtr cur_node = root, next_node;
    int policy;

    while (cur_node) {
        next_node = cur_node->next;

        switch (cur_node->type) {
        case XML_COMMENT_NODE:
            cur_node = erase(cur_node);
            break;
        case XML_ELEMENT_NODE:
            policy = decide(cur_node->name);
            if (policy == STRIP) {
                next_node = strip(cur_node);
                cur_node = NULL;
            } else if (policy == ERASE) {
                cur_node = erase(cur_node);
            } else if (policy == IMG) {
                extract(cur_node);
            } else {
                wash(cur_node);
            }
            break;
        case XML_TEXT_NODE:
            break;
        default:
            fprintf(stderr, "%d %s \n", cur_node->type, cur_node->name);
            break;
        }

        if (cur_node && cur_node->children)
            traverse(cur_node->children);

        cur_node = next_node;
    }
}


static void trim(htmlDocPtr doc) 
{
    traverse(xmlDocGetRootElement(doc));
}


static void output(htmlDocPtr doc, const char *path) 
{
    FILE *out;
    char fpath[2048];
    snprintf(fpath, 2048, "%s%s", bpath, path);
    out = fopen(fpath, "w");

    htmlSetMetaEncoding(doc, "utf-8");
    htmlDocDump(out, doc);

    fclose(out);
}

static int htmlpp(const char *buf, LONGINT64 size, const char *path) 
{
    htmlDocPtr doc;
    int parse_flags = HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING
        | HTML_PARSE_NOBLANKS |HTML_PARSE_COMPACT | HTML_PARSE_RECOVER; 

    parent(path);
    doc = htmlReadMemory(buf, size, path, NULL, parse_flags);

    if (doc == NULL) {
        fprintf(stderr, "Failed to parse %s\n", path);
	return;
    }

    trim(doc);

    output(doc, path);

    xmlFreeDoc(doc);
    xmlCleanupParser();
}
