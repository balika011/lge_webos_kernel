/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2012 Sony Corporation.
 */

#ifndef PKT_CONTROL_ATTRIBUTES_H
#define PKT_CONTROL_ATTRIBUTES_H

#define PKT_L_IMPORT_TRANSFERABLE_AGENT_ENTRYPT   "TransferMove"
#define PKT_L_CONTROL_ATTR_NO_MAX_LEN             (-1)

/* @note attr_max_len is only applicable for PKT_DATA_TYPE_STRING & PKT_DATA_TYPE_BYTE_ARRAY and the
 * value PKT_L_CONTROL_ATTR_NO_MAX_LEN indicates no max len specified. */
typedef struct pkt_l_control_attributes_desc_tag {
    pkt_data_type_t   attr_type;    /**< Control Attribute data type */
    u_int8_t         *attr_name;    /**< Attribute Name */
    int32_t           attr_max_len; /**< -1 - No Max len specified */
} pkt_l_control_attributes_desc;

/* TEMPLATE ID -> PKT_ID_IMPORT_TRANSFERABLE -> Control Attributes */
static const pkt_l_control_attributes_desc pkt_l_transferable_attributes[] = {
    { PKT_DATA_TYPE_BYTE_ARRAY, (u_int8_t *) "playGrantedParameterBlock" , 1000                           }, /* Limitation in the  Plankton code template */
    { PKT_DATA_TYPE_INTEGER,    (u_int8_t *) "playGrantedLocalFlags"     , PKT_L_CONTROL_ATTR_NO_MAX_LEN  },
    { PKT_DATA_TYPE_STRING,     (u_int8_t *) "licenseContainer"          , PKT_L_CONTROL_ATTR_NO_MAX_LEN  },
    { PKT_DATA_TYPE_STRING,     (u_int8_t *) "agentControlUid"           , PKT_L_CONTROL_ATTR_NO_MAX_LEN  },
    { PKT_DATA_TYPE_INTEGER,    (u_int8_t *) "instanceId"                , PKT_L_CONTROL_ATTR_NO_MAX_LEN  },
    { PKT_DATA_TYPE_INTEGER,    (u_int8_t *) "proximityRequiredFreshness", PKT_L_CONTROL_ATTR_NO_MAX_LEN  }
};

/* TEMPLATE ID -> PKT_ID_IMPORT_TRANSFERABLE_AGENT -> Control Attributes */
static const pkt_l_control_attributes_desc pkt_l_transferable_agent_attributes[] = {
    { PKT_DATA_TYPE_STRING, (u_int8_t *) "licenseContainer"   , PKT_L_CONTROL_ATTR_NO_MAX_LEN },
    { PKT_DATA_TYPE_STRING, (u_int8_t *) "delegate1Uid"       , PKT_L_CONTROL_ATTR_NO_MAX_LEN },
    { PKT_DATA_TYPE_STRING, (u_int8_t *) "delegate1EntryPoint", PKT_L_CONTROL_ATTR_NO_MAX_LEN },
    { PKT_DATA_TYPE_STRING, (u_int8_t *) "delegate2Uid"       , PKT_L_CONTROL_ATTR_NO_MAX_LEN },
    { PKT_DATA_TYPE_STRING, (u_int8_t *) "delegate2EntryPoint", PKT_L_CONTROL_ATTR_NO_MAX_LEN }
};

/* TEMPLATE ID -> PKT_ID_IMPORT_TRANSFERABLE_NOFRESHNESS -> Control Attributes */
static const pkt_l_control_attributes_desc pkt_l_transferable_nofreshness_attributes[] = {
    { PKT_DATA_TYPE_BYTE_ARRAY, (u_int8_t *) "playGrantedParameterBlock" , 1000                           }, /* Limitation in the  Plankton code template */
    { PKT_DATA_TYPE_INTEGER,    (u_int8_t *) "playGrantedLocalFlags"     , PKT_L_CONTROL_ATTR_NO_MAX_LEN  },
    { PKT_DATA_TYPE_STRING,     (u_int8_t *) "licenseContainer"          , PKT_L_CONTROL_ATTR_NO_MAX_LEN  },
    { PKT_DATA_TYPE_STRING,     (u_int8_t *) "agentControlUid"           , PKT_L_CONTROL_ATTR_NO_MAX_LEN  },
    { PKT_DATA_TYPE_INTEGER,    (u_int8_t *) "instanceId"                , PKT_L_CONTROL_ATTR_NO_MAX_LEN  }
};

#endif /* PKT_CONTROL_ATTRIBUTES_H */
