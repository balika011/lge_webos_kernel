/** @addtogroup pkt pkt
 * @ingroup marlin
 *  @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2007,2008,2009,2010,2011 Sony Corporation.
 */
#ifndef PKT_TYPES_H
#define PKT_TYPES_H

#define PKT_L_OP_NOP        0
#define PKT_L_OP_PUSH       1
#define PKT_L_OP_DROP       2
#define PKT_L_OP_DUP        3
#define PKT_L_OP_SWAP       4
#define PKT_L_OP_ADD        5
#define PKT_L_OP_MUL        6
#define PKT_L_OP_SUB        7
#define PKT_L_OP_DIV        8
#define PKT_L_OP_MOD        9
#define PKT_L_OP_NEG        10
#define PKT_L_OP_CMP        11
#define PKT_L_OP_AND        12
#define PKT_L_OP_OR         13
#define PKT_L_OP_XOR        14
#define PKT_L_OP_NOT        15
#define PKT_L_OP_SHL        16
#define PKT_L_OP_SHR        17
#define PKT_L_OP_JMP        18
#define PKT_L_OP_JSR        19
#define PKT_L_OP_JSRR       20
#define PKT_L_OP_RET        21
#define PKT_L_OP_BRA        22
#define PKT_L_OP_BRP        23
#define PKT_L_OP_BRN        24
#define PKT_L_OP_BRZ        25
#define PKT_L_OP_PEEK       26
#define PKT_L_OP_POKE       27
#define PKT_L_OP_PEEKB      28
#define PKT_L_OP_POKEB      29
#define PKT_L_OP_PUSHSP     30
#define PKT_L_OP_POPSP      31
#define PKT_L_OP_CALL       32
#define PKT_L_OP_STOP       255

enum {
    /*
     * fixed
     */
    PKT_L_SYS_SYSTEM_NO_OPERATION           = 0,
    PKT_L_SYS_SYSTEM_DEBUG_PRINT            = 1,
    PKT_L_SYS_FIND_SYSTEM_CALL_BY_NAME      = 2,
    PKT_L_SYS_SYSTEM_HOST_GET_OBJECT        = 3,
    PKT_L_SYS_SYSTEM_HOST_SET_OBJECT        = 4,

    PKT_L_SYS_SYSTEM_DEBUG_PRINT_INT        =16383,
    /*
     * standard
     */
    PKT_L_SYS_STANDARD                        = 16384,

    /* time */
    PKT_L_SYS_SYSTEM_HOST_GET_TRUSTED_TIME,

    /* localtime (deprecated) */
    PKT_L_SYS_SYSTEM_HOST_GET_LOCAL_TIME,
    PKT_L_SYS_SYSTEM_HOST_GET_LOCAL_TIME_OFFSET,
    PKT_L_SYS_SYSTEM_HOST_GET_LOCAL_TRUSTED_TIME,

    /* vm (deprecated) */
    PKT_L_SYS_SYSTEM_HOST_SPAWN_VM,
    PKT_L_SYS_SYSTEM_HOST_CALL_VM,
    PKT_L_SYS_SYSTEM_HOST_RELEASE_VM,

    /* extended */
    PKT_L_SYS_OCTOPUS_LINKS_IS_NODE_REACHABLE
};

typedef struct {
    u_int8_t           type[4];
    int32_t            size;
    u_int8_t          *buf;
} pkt_l_atom_t;

typedef struct {
    char              *name;
    int32_t            namelen;
    int32_t            offset;
} pkt_l_entry_t;

typedef struct {
    u_int32_t   size;
    u_int32_t   used;
    u_int8_t    buf[4];
} pkt_l_ds_t;

typedef struct pkt_l_cs_tag {
    np_size_t          size;
    int32_t            ds_start;
    int32_t            n_entries;
    pkt_l_entry_t     *entries;
    u_int8_t          *ownerid;
    np_size_t          owneridsize;
    void              *attrs;
    struct pkt_l_cs_tag    *next;
    u_int8_t                buf[4];
} pkt_l_cs_t;

#define PKT_L_MAX_CODEMODULE    4
#define PKT_L_MAX_VMCOUNT       8

struct pkt_l_context_tag;

typedef struct pkt_l_vm_tag {
    pkt_l_ds_t            *data;
    pkt_l_cs_t            *code;
    u_int8_t              *sp;
    pkt_hostcontext_t     *context;

    /* childs */
    u_int32_t            childs;

    /* top */
    struct pkt_l_context_tag    *top;

    /* vm handle */
    u_int32_t            vmhandle;

    struct {
        int32_t    maxuse_sp;
        int32_t    maxuse_callsp;
    } stat;
} pkt_l_vm_t;

typedef struct pkt_l_context_tag {
    /* codemodules */
    int32_t      codemodulecount;
    binstr_t    *codemodules[PKT_L_MAX_CODEMODULE];
    binstr_t    *codemoduleids[PKT_L_MAX_CODEMODULE];
    binstr_t    *ownerids[PKT_L_MAX_CODEMODULE];
    void        *attrs[PKT_L_MAX_CODEMODULE];
    /* vm */
    u_int32_t    vmcount;
    pkt_l_vm_t  *vms[PKT_L_MAX_VMCOUNT];
} pkt_l_context_t;

#define PKT_L_CALLSTACK_MAX    128

typedef struct {
    /* info */
    pkt_l_vm_t            *vm;
    pkt_l_cs_t            *cs;
    pkt_l_ds_t            *ds;
    pkt_l_entry_t         *entry;

    /* status */
    u_int8_t           *ip;
    u_int32_t          *sp;
    int32_t             call_sp;
    u_int8_t           *call_stack[PKT_L_CALLSTACK_MAX];

    /* bounds */
    u_int8_t            *ipL;
    u_int8_t            *ipU;
    u_int8_t            *dpL;
    u_int8_t            *dpU;
    u_int32_t           *spL;
    u_int32_t           *spU;
} pkt_l_vmexec_t;

#endif /* PKT_TYPES_H */
/** @} */
