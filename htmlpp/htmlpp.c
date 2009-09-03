#include <stdio.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <assert.h>
#include <ctype.h>


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
        /*
        if (strcasecmp(name, "id") == 0) {
            fprintf(stderr, "%s.%s = %s\n", node->name, cur->name, cur->children->content);
            cur = next;
            continue;
        }
        */

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
    ERASE
};

int decide(const char * tag)
{
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
                // strip code is still buggy, disable for now
                // fprintf(stderr, "strip %s\n", cur_node->name);
                next_node = strip(cur_node);
                cur_node = NULL;
            } else if (policy == ERASE) {
                //fprintf(stderr, "erase %s\n", cur_node->name);
                cur_node = erase(cur_node);
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


static void output(htmlDocPtr doc) 
{
    htmlSetMetaEncoding(doc, "utf-8");
    htmlDocDump(stdout, doc);
}

// Skip any label before <HTML>
static int pre_trim(const char* filename)
{
    FILE* fpr, * fpw;
    if (NULL == (fpr = fopen(filename, "r"))){
        fprintf(stderr, "Fail to open file %s\n.", filename);
        return 0;
    }
    if (NULL == (fpw = fopen(filename, "r+"))){
        fprintf(stderr, "Fail to open file %s\n.", filename);
        return 0;
    }
    int ch;
    long start  = 0;
    long end    = 0;
    int term    = 0;
    do{
        ch = fgetc(fpr);
        long pos = ftell(fpr);
        if (ch == '<'){
            ch = fgetc(fpr);
            switch(ch){
                case '!':
                    while((ch = fgetc(fpr)) != '>');
                    break;
                case 'h':
                case 'H':
                    if ((toupper(ch = fgetc(fpr)) == 'T') &&
                        (toupper(ch = fgetc(fpr)) == 'M') &&
                        (toupper(ch = fgetc(fpr)) == 'L')
                       ){
                        end = pos;
                        term = 1;
                    }
                    break;
                default:
                    if (start == 0){
                        start = pos;
                    }
                    break;
            }
        }
    }
    while(!term);

    if(0 == start)
        return 1;

    fseek(fpr, end, SEEK_SET);
    fseek(fpw, start, SEEK_SET);

    size_t sz = 1024;
    size_t num_read;
    size_t num_write;

    char buf[sz];
    term = 0;

    do{
        num_read = fread(buf, 1, sz, fpr);
        num_write = fwrite(buf, 1, num_read, fpw); 

        assert(num_write == num_read);

        if (num_read != sz){
            fputc(EOF, fpw);
            term = 1;
        }
    }while(!term);

    return 1;
}

int main(int argc, char **argv) 
{
    char *filename = argv[1];
    htmlDocPtr doc;
    int parse_flags = HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING
        | HTML_PARSE_NOBLANKS |HTML_PARSE_COMPACT | HTML_PARSE_RECOVER; 

    LIBXML_TEST_VERSION

    if (argc != 2) {
        fprintf(stderr, "Usage: %s in.html\n", argv[0]);
        return(1);
    }

    pre_trim(filename);

    doc = htmlReadFile(filename, NULL, parse_flags);

    if (doc == NULL) {
        fprintf(stderr, "Failed to parse %s\n", filename);
	return;
    }

    trim(doc);

    output(doc);

    xmlFreeDoc(doc);
    xmlCleanupParser();
}
