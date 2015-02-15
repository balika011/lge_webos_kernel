/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2007,2008,2009,2010 Sony Corporation.
 */
#ifndef XMLP_INDEF_H
#define XMLP_INDEF_H

#define XML_L_CONTENT_TYPE_TEXT      0
#define XML_L_CONTENT_TYPE_ELEMENT   1

#define XML_L_NODE_TYPE_ROOT         0
#define XML_L_NODE_TYPE_NAMESPACE    1
#define XML_L_NODE_TYPE_ATTRIBUTE    2
#define XML_L_NODE_TYPE_ELEMENT      3
#define XML_L_NODE_TYPE_TEXT         4

typedef struct xml_l_prologue_tag {
    int32_t ref;
    binstr_t *version_info;
    binstr_t *encoding_decl;
} xml_l_prologue_t;

typedef struct xml_l_attribute_tag {
    int32_t ref;
    binstr_t *name;
    binstr_t *value;
} xml_l_attribute_t;

typedef struct xml_l_attribute_vec_tag {
    /*int32_t ref;*/
    int32_t size;
    xml_l_attribute_t *attr[1];
} xml_l_attribute_vec_t;

struct xml_l_element_tag;
typedef struct xml_l_content_tag {
    int32_t ref;
    int32_t type;
    union {
        struct xml_l_element_tag *element;
        binstr_t               *text;
    } c;
} xml_l_content_t;

typedef struct xml_l_content_vec_tag {
    int32_t ref;
    int32_t size;
    xml_l_content_t *content[1];
} xml_l_content_vec_t;

typedef struct xml_l_element_tag {
    int32_t ref;
    binstr_t            *name;
    xml_l_attribute_vec_t *namespaces;
    xml_l_attribute_vec_t *attrs;
    xml_l_content_vec_t   *contents;
} xml_l_element_t;

typedef struct xml_l_document_tag {
    int32_t ref;
    xml_l_prologue_t *prologue;
    xml_l_element_t  *element;
} xml_l_document_t;

typedef struct xml_l_node_tag {
    int32_t ref;
    int32_t type;
    union {
        xml_l_document_t  *root;
        xml_l_element_t   *element;
        xml_l_attribute_t *ns;
        xml_l_attribute_t *attr;
        binstr_t          *text;
    } c;
} xml_l_node_t;

typedef struct xml_l_list_tag {
    void *item;
    struct xml_l_list_tag *next;
} xml_l_list_t;

typedef struct xml_l_internal_state_tag {
    xml_l_list_t *namespaces;
    int32_t nsdepth;
    int32_t flags;
    int32_t allocedmem, calledmem, toofree;
} xml_l_internal_state_t;

#define npi_xml_l_prologue_version_info(x) ((x)->version_info)
#define npi_xml_l_prologue_encoding_decl(x) ((x)->encoding_decl)
#define npi_xml_l_prologue_set_version_info(x,v) ((x)->version_info=(v))
#define npi_xml_l_prologue_set_encoding_decl(x,v) ((x)->encoding_decl=(v))
extern retcode_t
npi_xml_l_prologue_create(npi_cmncontext_t in_context, xml_l_prologue_t **out_prologue);
extern retcode_t
npi_xml_l_prologue_free(npi_cmncontext_t in_context, xml_l_prologue_t **io_prologue);

#define npi_xml_l_attribute_copy(x) ((x)->ref++, (x))
#define npi_xml_l_attribute_name(x) ((x)->name)
#define npi_xml_l_attribute_value(x) ((x)->value)
#define npi_xml_l_attribute_set_name(x,v) ((x)->name=(v))
#define npi_xml_l_attribute_set_value(x,v) ((x)->value=(v))
extern retcode_t
npi_xml_l_attribute_create(npi_cmncontext_t in_context, xml_l_attribute_t **out_attr);
extern retcode_t
npi_xml_l_attribute_free(npi_cmncontext_t in_context, xml_l_attribute_t **io_attr);

#define npi_xml_l_attribute_vec_attr(x,i) ((x)->attr[i])
#define npi_xml_l_attribute_vec_size(x) ((x)->size)
#define npi_xml_l_attribute_vec_set_attr(x,i,v) ((x)->attr[i]=(v))
#define npi_xml_l_attribute_vec_clear_attr(x,i) ((x)->attr[i]=0)
extern retcode_t
npi_xml_l_attribute_vec_create(npi_cmncontext_t in_context, xml_l_attribute_vec_t **out_attrvec, int32_t in_size);
extern retcode_t
npi_xml_l_attribute_vec_free(npi_cmncontext_t in_context, xml_l_attribute_vec_t **io_attrvec);

#define npi_xml_l_content_copy(x) ((x)->ref++, (x))
#define npi_xml_l_content_type(x) ((x)->type)
#define npi_xml_l_content_element(x) ((x)->c.element)
#define npi_xml_l_content_text(x) ((x)->c.text)
#define npi_xml_l_content_set_type(x,v) ((x)->type=(v))
#define npi_xml_l_content_set_element(x,v) ((x)->c.element=(v))
#define npi_xml_l_content_set_text(x,v) ((x)->c.text=(v))
extern retcode_t
npi_xml_l_content_create(npi_cmncontext_t in_context, xml_l_content_t **out_cont);
extern retcode_t
npi_xml_l_content_free(npi_cmncontext_t in_context, xml_l_content_t **io_cont);

#define npi_xml_l_content_vec_content(x,i) ((x)->content[i])
#define npi_xml_l_content_vec_size(x) ((x)->size)
#define npi_xml_l_content_vec_set_content(x,i,v) ((x)->content[i]=(v))
extern retcode_t
npi_xml_l_content_vec_create(npi_cmncontext_t in_context, xml_l_content_vec_t **out_contvec, int32_t in_size);
extern retcode_t
npi_xml_l_content_vec_free(npi_cmncontext_t in_context, xml_l_content_vec_t **io_contvec);

#define npi_xml_l_element_copy(x) ((x)->ref++, (x))
#define npi_xml_l_element_name(x) ((x)->name)
#define npi_xml_l_element_namespaces(x) ((x)->namespaces)
#define npi_xml_l_element_attributes(x) ((x)->attrs)
#define npi_xml_l_element_contents(x) ((x)->contents)
#define npi_xml_l_element_set_name(x,v) ((x)->name=(v))
#define npi_xml_l_element_set_namespaces(x,v) ((x)->namespaces=(v))
#define npi_xml_l_element_set_attributes(x,v) ((x)->attrs=(v))
#define npi_xml_l_element_set_contents(x,v) ((x)->contents=(v))
extern retcode_t
npi_xml_l_element_create(npi_cmncontext_t in_context, xml_l_element_t **out_element);
extern retcode_t
npi_xml_l_element_free(npi_cmncontext_t in_context, xml_l_element_t **io_element);

#define npi_xml_l_document_prologue(x) ((x)->prologue)
#define npi_xml_l_document_element(x) ((x)->element)
#define npi_xml_l_document_set_prologue(x,v) ((x)->prologue=(v))
#define npi_xml_l_document_set_element(x,v) ((x)->element=(v))
extern retcode_t
npi_xml_l_document_create(npi_cmncontext_t in_context, xml_l_document_t **out_xmldoc);
extern retcode_t
npi_xml_l_document_free(npi_cmncontext_t in_context, xml_l_document_t **io_xmldoc);

#define npi_xml_l_node_copy(x) ((x)->ref++, (x))
#define npi_xml_l_node_type(x) ((x)->type)
#define npi_xml_l_node_root(x) ((x)->c.root)
#define npi_xml_l_node_element(x) ((x)->c.element)
#define npi_xml_l_node_namespace(x) ((x)->c.ns)
#define npi_xml_l_node_attribute(x) ((x)->c.attr)
#define npi_xml_l_node_text(x) ((x)->c.text)
#define npi_xml_l_node_set_type(x,v) ((x)->type=(v))
#define npi_xml_l_node_set_root(x,v) ((x)->c.root=(v))
#define npi_xml_l_node_set_element(x,v) ((x)->c.element=(v))
#define npi_xml_l_node_set_namespace(x,v) ((x)->c.ns=(v))
#define npi_xml_l_node_set_attribute(x,v) ((x)->c.attr=(v))
#define npi_xml_l_node_set_text(x,v) ((x)->c.text=(v))
extern retcode_t
npi_xml_l_node_create(npi_cmncontext_t in_context, xml_l_node_t **out_xmlnode);
extern retcode_t
npi_xml_l_node_create_root(npi_cmncontext_t in_context, xml_l_node_t **out_xmlnode, xml_l_document_t *in_document);
extern retcode_t
npi_xml_l_node_create_element(npi_cmncontext_t in_context, xml_l_node_t **out_xmlnode, xml_l_element_t *in_element);
extern retcode_t
npi_xml_l_node_create_namespace(npi_cmncontext_t in_context, xml_l_node_t **out_xmlnode, xml_l_attribute_t *in_ns);
extern retcode_t
npi_xml_l_node_create_attribute(npi_cmncontext_t in_context, xml_l_node_t **out_xmlnode, xml_l_attribute_t *in_attr);
extern retcode_t
npi_xml_l_node_create_text(npi_cmncontext_t in_context, xml_l_node_t **out_xmlnode, binstr_t *in_text);
extern retcode_t
npi_xml_l_node_free(npi_cmncontext_t in_context, xml_l_node_t **io_xmlnode);
/*extern retcode_t
npi_xml_l_node_copy(npi_cmncontext_t in_context, xml_l_node_t **out_node, xml_l_node_t *in_node);*/


#define npi_xml_l_node_vec_node(x,i) ((x)->node[i])
#define npi_xml_l_node_vec_size(x) ((x)->size)
#define npi_xml_l_node_vec_set_node(x,i,v) ((x)->node[i]=(v))
extern retcode_t
npi_xml_l_node_vec_create(npi_cmncontext_t in_context, xml_node_vec_t **out_nodevec, int32_t in_size);
extern retcode_t
npi_xml_l_node_vec_free(npi_cmncontext_t in_context, xml_node_vec_t **io_nodevec);

extern retcode_t
npi_xml_l_is_init(npi_cmncontext_t in_context, xml_l_internal_state_t *out_state);

extern retcode_t
npi_xml_l_conv_itos(npi_cmncontext_t in_context, int32_t in_i, binstr_t **out_str);

extern retcode_t
npi_xml_l_conv_ttos(npi_cmncontext_t in_context, time_date_t *in_t, binstr_t **out_str);

/* Convert the unsigned multibyte integer (big endian, in_str) into its decimal string representation */
extern retcode_t
npi_xml_l_conv_umbitos(npi_cmncontext_t in_context, binstr_t *in_str, binstr_t **out_str);

#endif /* XMLP_INDEF_H */
