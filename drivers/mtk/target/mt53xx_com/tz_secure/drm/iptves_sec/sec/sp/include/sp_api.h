/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008,2009 Sony Corporation
 */
#ifndef SP_API_H
#define SP_API_H

#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================================*/
/**
 * @file
 * SP (service protocol) processing library
 *
 * require: keymng, time, base
 */
/*==========================================================================*/

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
/**
 * status values
 */
#define SP_STATUS_SUCCESS           0x0000
#define SP_STATUS_OTHERS            0x8001
#define SP_STATUS_VERSION_ERR       0x8002
#define SP_STATUS_DEVINFO_ERR       0x8003
#define SP_STATUS_USAGERULEREF_ERR  0x8004
#define SP_STATUS_ACTION_ID_ERR     0x8005
#define SP_STATUS_ACTION_PARAM_ERR  0x8006
#define SP_STATUS_ACTION_DENIED     0x8007
#define SP_STATUS_ACTION_FAILED     0x8008
#define SP_STATUS_MSG_FORMAT_ERR    0x8009

/**
 * version
 */
#define SP_PROTOCOL_VER             0x0100

/**
 * GetPermission Request length
 */
#define SP_MSG_LEN_GETPERM_REQ          64
#define SP_MSG_LEN_GETPERM_REP_VOD      34
#define SP_MSG_LEN_GETPERM_REP_IPMC     50
#define SP_MSG_LEN_GETPERM_REP_DL       34
#define SP_MSG_LEN_GETPERM_REP_CP_DL    26
#define SP_GETPERM_REQ_PRIVDATA_LEN     27
#define SP_GETPERM_REQ_EXTINFO_LEN      0x000A

/**
 * GetTrustedTime Request length
 */
#define SP_MSG_LEN_GETTT_REQ            4
#define SP_MSG_LEN_GETTT_REP            10

/**
 * Packed Message Request length
 */
#define SP_MSG_LEN_PROTOCOL_VER         2
#define SP_MSG_LEN_MESSAGE_ID           2
#define SP_MSG_LEN_MESSAGEBOX_NUM       2
#define SP_MSG_LEN_REQMSG_SZ            2
#define SP_MSG_LEN_REQMSG               SP_MSG_LEN_GETPERM_REQ

/**
 * message id
 */
#define SP_MSG_ID_GETPERM_REQ           0x0001
#define SP_MSG_ID_GETPERM_REP           0x0002
#define SP_MSG_ID_GETTT_REQ             0x0003
#define SP_MSG_ID_GETTT_REP             0x0004
#define SP_MSG_ID_PACKED_REQ            0x0101
#define SP_MSG_ID_PACKED_REP            0x0102

/**
 * Max Reply Message length
 *   Packed Reply Message size for GetPerm(IPMC) x 2 x 16 + GetTT
 *     1684: 8+((2+50)*2)*16+(2+10)
 */
#define SP_MSG_MAX_LEN_REP              1684

/**
 * Reply Message length
 */
#define SP_MSG_LEN_STATUS               2

/**
 * URR length for IPMC
 */
#define SP_URR_LEN_IPMC                 6

/**
 * URR offset
 */
#define SP_URR_SP_OFFSET                0
#define SP_URR_RESERVED_OFFSET          2
#define SP_URR_WKEY_ID_OFFSET           3
#define SP_URR_FLAG_OFFSET              5

/**
 * ContentID offset
 */
#define SP_CID_SP_OFFSET                0
#define SP_CID_RESERVED_OFFSET          2
#define SP_CID_WKEY_ID_OFFSET           3
#define SP_CID_WKEY_VER_OFFSET          5
#define SP_CID_PRIVATEDATA_OFFSET       6
#define SP_CID_TIERBITS_OFFSET          8

/**
 * ContentID size
 */
#define SP_CID_TIERBITS_LEN             8

/**
 * URR odd/even flag
 */
#define SP_URR_FLAG_EVEN                0x00
#define SP_URR_FLAG_ODD                 0x01

/**
 * device info size
 */
#define SP_DEVINFO_LEN                  12

/*============================================================================
 * Type Definitions
 *==========================================================================*/
/**
 * handle
 */
typedef void * sp_handle_t;

/*============================================================================
 * Error Codes
 *==========================================================================*/
#define RET_SP_ILLEGAL_REQUEST            (RET_SP_BASE|1)
#define RET_SP_NO_MORE_REQUEST            (RET_SP_BASE|2)
#define RET_SP_TOO_LONG_PACKED_REQ        (RET_SP_BASE|3)
#define RET_SP_VERIFICATION_FAILURE       (RET_SP_BASE|4)
#define RET_SP_INVALID_MSG_RECEIVED       (RET_SP_BASE|5)
#define RET_SP_TOO_MANY_LICENSES          (RET_SP_BASE|6)
#define RET_SP_SAVE_ERROR                 (RET_SP_BASE|7)
#define RET_SP_STATUS_OTHERS              (RET_SP_BASE|0x8001)
#define RET_SP_STATUS_VERSION_ERR         (RET_SP_BASE|0x8002)
#define RET_SP_STATUS_DEVINFO_ERR         (RET_SP_BASE|0x8003)
#define RET_SP_STATUS_USAGERULEREF_ERR    (RET_SP_BASE|0x8004)
#define RET_SP_STATUS_ACTION_ID_ERR       (RET_SP_BASE|0x8005)
#define RET_SP_STATUS_ACTION_PARAM_ERR    (RET_SP_BASE|0x8006)
#define RET_SP_STATUS_ACTION_DENIED       (RET_SP_BASE|0x8007)
#define RET_SP_STATUS_ACTION_FAILED       (RET_SP_BASE|0x8008)
#define RET_SP_STATUS_MSG_FORMAT_ERR      (RET_SP_BASE|0x8009)

/**
 * convert from status values to error code
 */
#define RET_SP_STATUS(s)                  ((s) - SP_STATUS_OTHERS + RET_SP_STATUS_OTHERS)

/*============================================================================
 * APIs
 *==========================================================================*/
/**
 * Initialize sp handle
 *
 * @param [in]     in_thid              Trust Hierarchy ID
 * @param [in]     in_req_num           Request number
 * @param [in]     in_req_lst           Request list
 * @param [out]    out_handle           SP handle
 * @retval RET_OK                       Success
 * @retval RET_NOMEM                    Fail to allocate memory
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_SP_ILLEGAL_REQUEST       @p in_req_lst has illegal requests or packed pattern
 */
extern int
sp_Construct(po_thid_t                  in_thid,
             u_int32_t                  in_req_num,
             const po_req_t            *in_req_lst,
             sp_handle_t               *out_handle);

/**
 * Finalize sp handle
 *
 * @param [in,out] io_handle            SP handle
 * @param [out]    out_success_num      Success number for processed request
 * @retval RET_OK                       Success
 */
extern int
sp_Destruct(sp_handle_t                 io_handle,
            u_int32_t                  *out_success_num);

/**
 * Make sp message
 *
 * @param [in,out] io_handle            SP handle
 * @param [out]    out_msg              Output message
 * @param [in,out] io_msg_len           Output message length
 * @param [out]    out_need_to_rec      Flag for necessity to record transaction flag
 * @retval RET_OK                       Success
 * @retval RET_NOMEM                    Fail to allocate memory
 * @retval RET_ILLEGALHANDLE            Illegal handle is passed to this API
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 * @retval RET_SP_NO_MORE_REQUEST       There are no requests to send (not error)
 * @retval RET_SP_TOO_LONG_PACKED_REQ   @p out_msg length is not enough
 */
extern int
sp_MakeMessage(sp_handle_t              io_handle,
               u_int8_t                *out_msg,
               u_int32_t               *io_msg_len,
               bool_t                  *out_need_to_rec);

/**
 * Process sp message
 *
 * @param [in,out] io_handle            SP handle
 * @param [in]     in_msg               Input message
 * @param [in]     in_msg_len           Input message length
 * @param [in]     in_srv_sbj           Subject of server certificate
 * @param [in]     in_trans_flag        Transaction Flag
 * @retval RET_OK                               Success
 * @retval RET_NOMEM                            Fail to allocate memory
 * @retval RET_ILLEGALHANDLE                    Illegal handle is passed to this API
 * @retval RET_ILLEGALARGS                      Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR                    An unexpected error has occurred
 * @retval RET_SP_NO_MORE_REQUEST               There are no requests to process (not error)
 * @retval RET_SP_VERIFICATION_FAILURE          Invalid message recieved
 * @retval RET_SP_INVALID_MSG_RECEIVED          Invalid message recieved
 * @retval RET_SP_SAVE_ERROR                    Fail to save read-write NVS
 * @retval RET_SP_STATUS_OTHERS                 SP status code (8001h)
 * @retval RET_SP_STATUS_VERSION_ERR            SP status code (8002h)
 * @retval RET_SP_STATUS_DEVINFO_ERR            SP status code (8003h)
 * @retval RET_SP_STATUS_USAGERULEREF_ERR       SP status code (8004h)
 * @retval RET_SP_STATUS_ACTION_ID_ERR          SP status code (8005h)
 * @retval RET_SP_STATUS_ACTION_PARAM_ERR       SP status code (8006h)
 * @retval RET_SP_STATUS_ACTION_DENIED          SP status code (8007h)
 * @retval RET_SP_STATUS_ACTION_FAILED          SP status code (8008h)
 * @retval RET_SP_STATUS_MSG_FORMAT_ERR         SP status code (8009h)
 */
extern int
sp_ProcessMessage(sp_handle_t           io_handle,
                  u_int8_t             *in_msg,
                  u_int32_t             in_msg_len,
                  u_int8_t             *in_srv_sbj,
                  u_int8_t              in_trans_flag);

#if !defined(IPTVES_PUBLISH_BUILD)
/*----------------------------------------------------------------------------
 * Functions for Test/Tools
 *--------------------------------------------------------------------------*/
/**
 * Set manufacturer for cts
 *
 * @retval RET_OK                       Success
 */
extern int
sp_SetManufacturerForCTS(void);

#endif /* !IPTVES_PUBLISH_BUILD */

#ifdef __cplusplus
}
#endif

#endif /* SP_API_H */
