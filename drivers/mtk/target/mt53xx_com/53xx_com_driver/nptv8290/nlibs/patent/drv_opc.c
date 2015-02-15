/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/
#ifndef CC_MTK_LOADER

#include "drv_meter.h"
#include "drv_ycproc.h"
#include "api_backlt.h"
#include "nptv_debug.h"
#include "hw_ospe.h"
#include "hw_nr.h"
#include "hw_sw.h"

#define DYNAMIC_BACKLIGHT_ON        SV_TRUE
#define DBL_MAX_DIFF_THRESHOLD	    0x0A  
#define DBL_TOTAL_DIFF_THRESHOLD  	0x30 
#define DBL_MAX_NOISE_THRESHOLD     0x01
#define DBL_NORMALIZED_HIST_PREC    12
#define ALTERNATE_DBL_AND_AL_ON     SV_TRUE

#define LUMA_TABLE_SIZE         33
#define OPC_HIST_NORMBASE     1000
#define OPC_HIST_MAX_THD       990
#define PWM_TABLE_MAX            3

#define AbsDiff(a, b)  (((a)>(b))?((a)-(b)):((b)-(a)))

UINT32 opc_cnt = 0;

typedef struct _opc_info
{
    UINT8 OPC_CURVE_P1;
    UINT8 OPC_CURVE_P2;    
    UINT8 OPC_CURVE_G1;    
    UINT8 OPC_CURVE_G2;    
    UINT8 OPC_CURVE_GSLP;    
    UINT8 OPC_CURVE_PWM;    
} OPC_INFO;

OPC_INFO PreOPCInfo[16];

UINT16 OPC_PWM_INDEX[PWM_TABLE_MAX][LUMA_TABLE_SIZE] = 
    {
     {  128, 131, 135, 139, 143, 147, 151, 155, 159, 163, 167, 171, 175, 179, 183, 187, 191, 195, 199, 203, 207, 211, 215, 219, 223, 227, 231, 235, 239, 243, 247, 251, 255, },   // strong
     {  128, 131, 135, 139, 143, 147, 151, 155, 159, 163, 167, 171, 175, 179, 183, 187, 191, 195, 199, 203, 207, 211, 215, 219, 223, 227, 231, 235, 239, 243, 247, 251, 255, },   // middle
     {  128, 131, 135, 139, 143, 147, 151, 155, 159, 163, 167, 171, 175, 179, 183, 187, 191, 195, 199, 203, 207, 211, 215, 219, 223, 227, 231, 235, 239, 243, 247, 251, 255, }    // low
    };

UINT16 OPCLuminanceTable[PWM_TABLE_MAX][LUMA_TABLE_SIZE][LUMA_TABLE_SIZE] = // content x pwm
{
    {
        { 9, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 19, },
        { 20, 20, 21, 22, 22, 23, 24, 24, 25, 25, 26, 27, 27, 28, 28, 28, 30, 30, 31, 31, 32, 32, 33, 34, 34, 35, 35, 36, 37, 37, 38, 38, 39, },
        { 46, 47, 48, 50, 51, 53, 54, 55, 56, 57, 60, 61, 62, 64, 65, 66, 68, 69, 70, 71, 73, 74, 76, 77, 78, 79, 81, 82, 83, 85, 86, 87, 88, },
        { 94, 96, 99, 102, 105, 108, 111, 113, 117, 119, 122, 125, 128, 131, 133, 136, 138, 141, 144, 147, 149, 152, 155, 157, 160, 163, 165, 168, 171, 173, 176, 178, 181, },
        { 170, 175, 181, 186, 191, 197, 202, 206, 213, 217, 222, 227, 231, 237, 242, 247, 252, 257, 262, 266, 271, 276, 281, 286, 291, 295, 300, 305, 310, 315, 319, 324, 329, },
        { 276, 283, 292, 300, 309, 318, 326, 335, 344, 351, 360, 368, 376, 385, 392, 400, 408, 417, 425, 432, 440, 448, 455, 463, 473, 479, 488, 495, 503, 511, 518, 526, 534, },
        { 413, 425, 438, 450, 463, 476, 490, 503, 516, 527, 540, 552, 565, 577, 588, 600, 612, 625, 637, 648, 660, 673, 685, 697, 709, 720, 732, 744, 755, 767, 778, 789, 802, },
        { 584, 599, 619, 636, 654, 673, 691, 710, 728, 744, 762, 780, 798, 815, 831, 848, 866, 883, 900, 916, 933, 950, 968, 985, 1002, 1017, 1034, 1051, 1067, 1084, 1099, 1116, 1133, },
        { 789, 809, 835, 857, 883, 908, 933, 958, 982, 1003, 1028, 1052, 1076, 1100, 1121, 1144, 1168, 1192, 1215, 1236, 1259, 1283, 1306, 1330, 1353, 1373, 1397, 1419, 1442, 1465, 1484, 1507, 1530, },
        { 1024, 1051, 1086, 1115, 1148, 1181, 1213, 1245, 1278, 1306, 1338, 1369, 1400, 1431, 1458, 1489, 1520, 1551, 1581, 1607, 1638, 1669, 1698, 1730, 1761, 1787, 1817, 1847, 1876, 1905, 1932, 1962, 1991, },
        { 1297, 1330, 1373, 1410, 1452, 1493, 1534, 1575, 1616, 1651, 1692, 1732, 1771, 1810, 1844, 1882, 1923, 1961, 2000, 2035, 2073, 2112, 2150, 2189, 2228, 2261, 2299, 2336, 2374, 2412, 2445, 2483, 2520, },
        { 1598, 1641, 1694, 1739, 1791, 1843, 1894, 1944, 1995, 2039, 2089, 2139, 2188, 2236, 2278, 2327, 2375, 2424, 2472, 2514, 2562, 2610, 2658, 2706, 2753, 2794, 2842, 2889, 2936, 2981, 3022, 3069, 3116, },
        { 1936, 1989, 2054, 2110, 2171, 2233, 2295, 2357, 2419, 2472, 2532, 2591, 2651, 2711, 2762, 2821, 2879, 2937, 2996, 3048, 3105, 3164, 3222, 3281, 3339, 3388, 3445, 3503, 3559, 3615, 3665, 3722, 3778, },
        { 2308, 2369, 2446, 2514, 2588, 2662, 2736, 2808, 2882, 2946, 3017, 3089, 3161, 3231, 3293, 3361, 3433, 3503, 3572, 3633, 3702, 3772, 3842, 3911, 3981, 4040, 4109, 4178, 4244, 4313, 4372, 4440, 4507, },
        { 2717, 2788, 2877, 2955, 3042, 3130, 3215, 3303, 3388, 3464, 3547, 3633, 3715, 3800, 3870, 3953, 4035, 4119, 4202, 4273, 4354, 4437, 4518, 4600, 4681, 4751, 4832, 4912, 4994, 5073, 5142, 5221, 5301, },
        { 3147, 3233, 3340, 3429, 3532, 3633, 3733, 3833, 3933, 4020, 4118, 4216, 4313, 4409, 4494, 4591, 4687, 4782, 4878, 4961, 5058, 5153, 5247, 5342, 5437, 5520, 5613, 5704, 5799, 5893, 5973, 6066, 6156, },
        { 3615, 3713, 3834, 3938, 4057, 4173, 4289, 4403, 4518, 4618, 4730, 4844, 4955, 5066, 5163, 5273, 5383, 5494, 5605, 5699, 5809, 5918, 6028, 6138, 6244, 6339, 6447, 6555, 6663, 6767, 6861, 6968, 7073, },
        { 4137, 4254, 4390, 4508, 4642, 4776, 4908, 5040, 5172, 5286, 5416, 5545, 5672, 5800, 5910, 6037, 6162, 6289, 6416, 6526, 6651, 6775, 6900, 7026, 7151, 7258, 7381, 7506, 7629, 7752, 7856, 7978, 8098, },
        { 4691, 4818, 4975, 5112, 5264, 5414, 5564, 5713, 5864, 5994, 6143, 6288, 6433, 6577, 6703, 6845, 6989, 7134, 7273, 7400, 7544, 7686, 7829, 7969, 8111, 8234, 8376, 8517, 8650, 8794, 8913, 9051, 9189, },
        { 5289, 5427, 5602, 5753, 5924, 6094, 6264, 6431, 6598, 6747, 6912, 7078, 7242, 7404, 7545, 7706, 7867, 8029, 8192, 8332, 8491, 8651, 8811, 8971, 9130, 9266, 9425, 9583, 9742, 9898, 10030, 10190, 10350, },
        { 5904, 6064, 6264, 6433, 6626, 6815, 7003, 7191, 7379, 7544, 7730, 7915, 8097, 8278, 8437, 8619, 8800, 8981, 9162, 9318, 9500, 9679, 9857, 10040, 10210, 10370, 10550, 10720, 10900, 11070, 11230, 11400, 11570, },
        { 6573, 6752, 6970, 7165, 7377, 7588, 7798, 8008, 8217, 8400, 8608, 8813, 9017, 9218, 9396, 9598, 9799, 10000, 10200, 10380, 10580, 10780, 10980, 11180, 11370, 11550, 11750, 11940, 12140, 12330, 12510, 12700, 12890, },
        { 7314, 7504, 7748, 7958, 8193, 8429, 8664, 8897, 9129, 9332, 9561, 9787, 10020, 10240, 10440, 10670, 10890, 11110, 11340, 11530, 11750, 11980, 12200, 12420, 12640, 12830, 13050, 13269, 13490, 13710, 13900, 14110, 14330, },
        { 8102, 8322, 8597, 8829, 9092, 9353, 9614, 9872, 10130, 10360, 10610, 10870, 11120, 11370, 11590, 11840, 12090, 12340, 12580, 12800, 13050, 13300, 13540, 13790, 14040, 14250, 14490, 14740, 14980, 15219, 15430, 15680, 15910, },
        { 9018, 9249, 9552, 9809, 10100, 10390, 10680, 10960, 11250, 11500, 11780, 12070, 12340, 12620, 12869, 13150, 13419, 13700, 13969, 14219, 14490, 14769, 15040, 15310, 15590, 15830, 16100, 16369, 16650, 16910, 17150, 17420, 17680, },
        { 9996, 10250, 10580, 10870, 11190, 11520, 11830, 12150, 12470, 12750, 13060, 13380, 13690, 13990, 14269, 14569, 14880, 15190, 15500, 15769, 16080, 16390, 16690, 16990, 17300, 17560, 17870, 18160, 18460, 18760, 19020, 19320, 19610, },
        { 11100, 11380, 11750, 12070, 12430, 12780, 13140, 13490, 13850, 14160, 14510, 14860, 15200, 15550, 15850, 16200, 16540, 16880, 17230, 17530, 17870, 18210, 18530, 18880, 19210, 19510, 19850, 20180, 20510, 20850, 21140, 21470, 21790, },
        { 12330, 12650, 13060, 13410, 13810, 14210, 14600, 15000, 15400, 15740, 16130, 16520, 16910, 17290, 17620, 18000, 18380, 18770, 19140, 19480, 19860, 20230, 20620, 20980, 21360, 21690, 22060, 22430, 22810, 23180, 23510, 23870, 24240, },
        { 13660, 14019, 14480, 14869, 15319, 15760, 16200, 16640, 17070, 17450, 17890, 18320, 18740, 19170, 19540, 19960, 20390, 20810, 21240, 21610, 22030, 22450, 22870, 23290, 23710, 24070, 24480, 24900, 25310, 25720, 26070, 26489, 26900, },
        { 15030, 15450, 15960, 16390, 16880, 17360, 17840, 18330, 18810, 19230, 19700, 20180, 20650, 21130, 21540, 22010, 22480, 22940, 23410, 23820, 24290, 24750, 25210, 25680, 26139, 26539, 27000, 27450, 27910, 28370, 28770, 29220, 29660, },
        { 16380, 16820, 17370, 17860, 18390, 18920, 19450, 19970, 20500, 20960, 21480, 22000, 22520, 23030, 23480, 23990, 24500, 25020, 25530, 25970, 26480, 26989, 27500, 28010, 28500, 28939, 29450, 29950, 30450, 30930, 31370, 31870, 32360, },
        { 17710, 18180, 18770, 19280, 19850, 20430, 21000, 21570, 22140, 22630, 23200, 23760, 24320, 24880, 25360, 25920, 26470, 27030, 27580, 28070, 28620, 29170, 29720, 30270, 30810, 31280, 31820, 32370, 32900, 33450, 33900, 34440, 34980, },
        { 18910, 19430, 20080, 20630, 21250, 21870, 22480, 23090, 23700, 24230, 24840, 25440, 26039, 26639, 27160, 27760, 28360, 28950, 29550, 30070, 30660, 31250, 31830, 32430, 33010, 33520, 34120, 34700, 35270, 35850, 36360, 36940, 37510, },
    }
    ,
    {
        { 9, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 19, },
        { 20, 20, 21, 22, 22, 23, 24, 24, 25, 25, 26, 27, 27, 28, 28, 28, 30, 30, 31, 31, 32, 32, 33, 34, 34, 35, 35, 36, 37, 37, 38, 38, 39, },
        { 46, 47, 48, 50, 51, 53, 54, 55, 56, 57, 60, 61, 62, 64, 65, 66, 68, 69, 70, 71, 73, 74, 76, 77, 78, 79, 81, 82, 83, 85, 86, 87, 88, },
        { 94, 96, 99, 102, 105, 108, 111, 113, 117, 119, 122, 125, 128, 131, 133, 136, 138, 141, 144, 147, 149, 152, 155, 157, 160, 163, 165, 168, 171, 173, 176, 178, 181, },
        { 170, 175, 181, 186, 191, 197, 202, 206, 213, 217, 222, 227, 231, 237, 242, 247, 252, 257, 262, 266, 271, 276, 281, 286, 291, 295, 300, 305, 310, 315, 319, 324, 329, },
        { 276, 283, 292, 300, 309, 318, 326, 335, 344, 351, 360, 368, 376, 385, 392, 400, 408, 417, 425, 432, 440, 448, 455, 463, 473, 479, 488, 495, 503, 511, 518, 526, 534, },
        { 413, 425, 438, 450, 463, 476, 490, 503, 516, 527, 540, 552, 565, 577, 588, 600, 612, 625, 637, 648, 660, 673, 685, 697, 709, 720, 732, 744, 755, 767, 778, 789, 802, },
        { 584, 599, 619, 636, 654, 673, 691, 710, 728, 744, 762, 780, 798, 815, 831, 848, 866, 883, 900, 916, 933, 950, 968, 985, 1002, 1017, 1034, 1051, 1067, 1084, 1099, 1116, 1133, },
        { 789, 809, 835, 857, 883, 908, 933, 958, 982, 1003, 1028, 1052, 1076, 1100, 1121, 1144, 1168, 1192, 1215, 1236, 1259, 1283, 1306, 1330, 1353, 1373, 1397, 1419, 1442, 1465, 1484, 1507, 1530, },
        { 1024, 1051, 1086, 1115, 1148, 1181, 1213, 1245, 1278, 1306, 1338, 1369, 1400, 1431, 1458, 1489, 1520, 1551, 1581, 1607, 1638, 1669, 1698, 1730, 1761, 1787, 1817, 1847, 1876, 1905, 1932, 1962, 1991, },
        { 1297, 1330, 1373, 1410, 1452, 1493, 1534, 1575, 1616, 1651, 1692, 1732, 1771, 1810, 1844, 1882, 1923, 1961, 2000, 2035, 2073, 2112, 2150, 2189, 2228, 2261, 2299, 2336, 2374, 2412, 2445, 2483, 2520, },
        { 1598, 1641, 1694, 1739, 1791, 1843, 1894, 1944, 1995, 2039, 2089, 2139, 2188, 2236, 2278, 2327, 2375, 2424, 2472, 2514, 2562, 2610, 2658, 2706, 2753, 2794, 2842, 2889, 2936, 2981, 3022, 3069, 3116, },
        { 1936, 1989, 2054, 2110, 2171, 2233, 2295, 2357, 2419, 2472, 2532, 2591, 2651, 2711, 2762, 2821, 2879, 2937, 2996, 3048, 3105, 3164, 3222, 3281, 3339, 3388, 3445, 3503, 3559, 3615, 3665, 3722, 3778, },
        { 2308, 2369, 2446, 2514, 2588, 2662, 2736, 2808, 2882, 2946, 3017, 3089, 3161, 3231, 3293, 3361, 3433, 3503, 3572, 3633, 3702, 3772, 3842, 3911, 3981, 4040, 4109, 4178, 4244, 4313, 4372, 4440, 4507, },
        { 2717, 2788, 2877, 2955, 3042, 3130, 3215, 3303, 3388, 3464, 3547, 3633, 3715, 3800, 3870, 3953, 4035, 4119, 4202, 4273, 4354, 4437, 4518, 4600, 4681, 4751, 4832, 4912, 4994, 5073, 5142, 5221, 5301, },
        { 3147, 3233, 3340, 3429, 3532, 3633, 3733, 3833, 3933, 4020, 4118, 4216, 4313, 4409, 4494, 4591, 4687, 4782, 4878, 4961, 5058, 5153, 5247, 5342, 5437, 5520, 5613, 5704, 5799, 5893, 5973, 6066, 6156, },
        { 3615, 3713, 3834, 3938, 4057, 4173, 4289, 4403, 4518, 4618, 4730, 4844, 4955, 5066, 5163, 5273, 5383, 5494, 5605, 5699, 5809, 5918, 6028, 6138, 6244, 6339, 6447, 6555, 6663, 6767, 6861, 6968, 7073, },
        { 4137, 4254, 4390, 4508, 4642, 4776, 4908, 5040, 5172, 5286, 5416, 5545, 5672, 5800, 5910, 6037, 6162, 6289, 6416, 6526, 6651, 6775, 6900, 7026, 7151, 7258, 7381, 7506, 7629, 7752, 7856, 7978, 8098, },
        { 4691, 4818, 4975, 5112, 5264, 5414, 5564, 5713, 5864, 5994, 6143, 6288, 6433, 6577, 6703, 6845, 6989, 7134, 7273, 7400, 7544, 7686, 7829, 7969, 8111, 8234, 8376, 8517, 8650, 8794, 8913, 9051, 9189, },
        { 5289, 5427, 5602, 5753, 5924, 6094, 6264, 6431, 6598, 6747, 6912, 7078, 7242, 7404, 7545, 7706, 7867, 8029, 8192, 8332, 8491, 8651, 8811, 8971, 9130, 9266, 9425, 9583, 9742, 9898, 10030, 10190, 10350, },
        { 5904, 6064, 6264, 6433, 6626, 6815, 7003, 7191, 7379, 7544, 7730, 7915, 8097, 8278, 8437, 8619, 8800, 8981, 9162, 9318, 9500, 9679, 9857, 10040, 10210, 10370, 10550, 10720, 10900, 11070, 11230, 11400, 11570, },
        { 6573, 6752, 6970, 7165, 7377, 7588, 7798, 8008, 8217, 8400, 8608, 8813, 9017, 9218, 9396, 9598, 9799, 10000, 10200, 10380, 10580, 10780, 10980, 11180, 11370, 11550, 11750, 11940, 12140, 12330, 12510, 12700, 12890, },
        { 7314, 7504, 7748, 7958, 8193, 8429, 8664, 8897, 9129, 9332, 9561, 9787, 10020, 10240, 10440, 10670, 10890, 11110, 11340, 11530, 11750, 11980, 12200, 12420, 12640, 12830, 13050, 13269, 13490, 13710, 13900, 14110, 14330, },
        { 8102, 8322, 8597, 8829, 9092, 9353, 9614, 9872, 10130, 10360, 10610, 10870, 11120, 11370, 11590, 11840, 12090, 12340, 12580, 12800, 13050, 13300, 13540, 13790, 14040, 14250, 14490, 14740, 14980, 15219, 15430, 15680, 15910, },
        { 9018, 9249, 9552, 9809, 10100, 10390, 10680, 10960, 11250, 11500, 11780, 12070, 12340, 12620, 12869, 13150, 13419, 13700, 13969, 14219, 14490, 14769, 15040, 15310, 15590, 15830, 16100, 16369, 16650, 16910, 17150, 17420, 17680, },
        { 9996, 10250, 10580, 10870, 11190, 11520, 11830, 12150, 12470, 12750, 13060, 13380, 13690, 13990, 14269, 14569, 14880, 15190, 15500, 15769, 16080, 16390, 16690, 16990, 17300, 17560, 17870, 18160, 18460, 18760, 19020, 19320, 19610, },
        { 11100, 11380, 11750, 12070, 12430, 12780, 13140, 13490, 13850, 14160, 14510, 14860, 15200, 15550, 15850, 16200, 16540, 16880, 17230, 17530, 17870, 18210, 18530, 18880, 19210, 19510, 19850, 20180, 20510, 20850, 21140, 21470, 21790, },
        { 12330, 12650, 13060, 13410, 13810, 14210, 14600, 15000, 15400, 15740, 16130, 16520, 16910, 17290, 17620, 18000, 18380, 18770, 19140, 19480, 19860, 20230, 20620, 20980, 21360, 21690, 22060, 22430, 22810, 23180, 23510, 23870, 24240, },
        { 13660, 14019, 14480, 14869, 15319, 15760, 16200, 16640, 17070, 17450, 17890, 18320, 18740, 19170, 19540, 19960, 20390, 20810, 21240, 21610, 22030, 22450, 22870, 23290, 23710, 24070, 24480, 24900, 25310, 25720, 26070, 26489, 26900, },
        { 15030, 15450, 15960, 16390, 16880, 17360, 17840, 18330, 18810, 19230, 19700, 20180, 20650, 21130, 21540, 22010, 22480, 22940, 23410, 23820, 24290, 24750, 25210, 25680, 26139, 26539, 27000, 27450, 27910, 28370, 28770, 29220, 29660, },
        { 16380, 16820, 17370, 17860, 18390, 18920, 19450, 19970, 20500, 20960, 21480, 22000, 22520, 23030, 23480, 23990, 24500, 25020, 25530, 25970, 26480, 26989, 27500, 28010, 28500, 28939, 29450, 29950, 30450, 30930, 31370, 31870, 32360, },
        { 17710, 18180, 18770, 19280, 19850, 20430, 21000, 21570, 22140, 22630, 23200, 23760, 24320, 24880, 25360, 25920, 26470, 27030, 27580, 28070, 28620, 29170, 29720, 30270, 30810, 31280, 31820, 32370, 32900, 33450, 33900, 34440, 34980, },
        { 18910, 19430, 20080, 20630, 21250, 21870, 22480, 23090, 23700, 24230, 24840, 25440, 26039, 26639, 27160, 27760, 28360, 28950, 29550, 30070, 30660, 31250, 31830, 32430, 33010, 33520, 34120, 34700, 35270, 35850, 36360, 36940, 37510, },
    }
    ,
    {
        { 9, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 19, },
        { 20, 20, 21, 22, 22, 23, 24, 24, 25, 25, 26, 27, 27, 28, 28, 28, 30, 30, 31, 31, 32, 32, 33, 34, 34, 35, 35, 36, 37, 37, 38, 38, 39, },
        { 46, 47, 48, 50, 51, 53, 54, 55, 56, 57, 60, 61, 62, 64, 65, 66, 68, 69, 70, 71, 73, 74, 76, 77, 78, 79, 81, 82, 83, 85, 86, 87, 88, },
        { 94, 96, 99, 102, 105, 108, 111, 113, 117, 119, 122, 125, 128, 131, 133, 136, 138, 141, 144, 147, 149, 152, 155, 157, 160, 163, 165, 168, 171, 173, 176, 178, 181, },
        { 170, 175, 181, 186, 191, 197, 202, 206, 213, 217, 222, 227, 231, 237, 242, 247, 252, 257, 262, 266, 271, 276, 281, 286, 291, 295, 300, 305, 310, 315, 319, 324, 329, },
        { 276, 283, 292, 300, 309, 318, 326, 335, 344, 351, 360, 368, 376, 385, 392, 400, 408, 417, 425, 432, 440, 448, 455, 463, 473, 479, 488, 495, 503, 511, 518, 526, 534, },
        { 413, 425, 438, 450, 463, 476, 490, 503, 516, 527, 540, 552, 565, 577, 588, 600, 612, 625, 637, 648, 660, 673, 685, 697, 709, 720, 732, 744, 755, 767, 778, 789, 802, },
        { 584, 599, 619, 636, 654, 673, 691, 710, 728, 744, 762, 780, 798, 815, 831, 848, 866, 883, 900, 916, 933, 950, 968, 985, 1002, 1017, 1034, 1051, 1067, 1084, 1099, 1116, 1133, },
        { 789, 809, 835, 857, 883, 908, 933, 958, 982, 1003, 1028, 1052, 1076, 1100, 1121, 1144, 1168, 1192, 1215, 1236, 1259, 1283, 1306, 1330, 1353, 1373, 1397, 1419, 1442, 1465, 1484, 1507, 1530, },
        { 1024, 1051, 1086, 1115, 1148, 1181, 1213, 1245, 1278, 1306, 1338, 1369, 1400, 1431, 1458, 1489, 1520, 1551, 1581, 1607, 1638, 1669, 1698, 1730, 1761, 1787, 1817, 1847, 1876, 1905, 1932, 1962, 1991, },
        { 1297, 1330, 1373, 1410, 1452, 1493, 1534, 1575, 1616, 1651, 1692, 1732, 1771, 1810, 1844, 1882, 1923, 1961, 2000, 2035, 2073, 2112, 2150, 2189, 2228, 2261, 2299, 2336, 2374, 2412, 2445, 2483, 2520, },
        { 1598, 1641, 1694, 1739, 1791, 1843, 1894, 1944, 1995, 2039, 2089, 2139, 2188, 2236, 2278, 2327, 2375, 2424, 2472, 2514, 2562, 2610, 2658, 2706, 2753, 2794, 2842, 2889, 2936, 2981, 3022, 3069, 3116, },
        { 1936, 1989, 2054, 2110, 2171, 2233, 2295, 2357, 2419, 2472, 2532, 2591, 2651, 2711, 2762, 2821, 2879, 2937, 2996, 3048, 3105, 3164, 3222, 3281, 3339, 3388, 3445, 3503, 3559, 3615, 3665, 3722, 3778, },
        { 2308, 2369, 2446, 2514, 2588, 2662, 2736, 2808, 2882, 2946, 3017, 3089, 3161, 3231, 3293, 3361, 3433, 3503, 3572, 3633, 3702, 3772, 3842, 3911, 3981, 4040, 4109, 4178, 4244, 4313, 4372, 4440, 4507, },
        { 2717, 2788, 2877, 2955, 3042, 3130, 3215, 3303, 3388, 3464, 3547, 3633, 3715, 3800, 3870, 3953, 4035, 4119, 4202, 4273, 4354, 4437, 4518, 4600, 4681, 4751, 4832, 4912, 4994, 5073, 5142, 5221, 5301, },
        { 3147, 3233, 3340, 3429, 3532, 3633, 3733, 3833, 3933, 4020, 4118, 4216, 4313, 4409, 4494, 4591, 4687, 4782, 4878, 4961, 5058, 5153, 5247, 5342, 5437, 5520, 5613, 5704, 5799, 5893, 5973, 6066, 6156, },
        { 3615, 3713, 3834, 3938, 4057, 4173, 4289, 4403, 4518, 4618, 4730, 4844, 4955, 5066, 5163, 5273, 5383, 5494, 5605, 5699, 5809, 5918, 6028, 6138, 6244, 6339, 6447, 6555, 6663, 6767, 6861, 6968, 7073, },
        { 4137, 4254, 4390, 4508, 4642, 4776, 4908, 5040, 5172, 5286, 5416, 5545, 5672, 5800, 5910, 6037, 6162, 6289, 6416, 6526, 6651, 6775, 6900, 7026, 7151, 7258, 7381, 7506, 7629, 7752, 7856, 7978, 8098, },
        { 4691, 4818, 4975, 5112, 5264, 5414, 5564, 5713, 5864, 5994, 6143, 6288, 6433, 6577, 6703, 6845, 6989, 7134, 7273, 7400, 7544, 7686, 7829, 7969, 8111, 8234, 8376, 8517, 8650, 8794, 8913, 9051, 9189, },
        { 5289, 5427, 5602, 5753, 5924, 6094, 6264, 6431, 6598, 6747, 6912, 7078, 7242, 7404, 7545, 7706, 7867, 8029, 8192, 8332, 8491, 8651, 8811, 8971, 9130, 9266, 9425, 9583, 9742, 9898, 10030, 10190, 10350, },
        { 5904, 6064, 6264, 6433, 6626, 6815, 7003, 7191, 7379, 7544, 7730, 7915, 8097, 8278, 8437, 8619, 8800, 8981, 9162, 9318, 9500, 9679, 9857, 10040, 10210, 10370, 10550, 10720, 10900, 11070, 11230, 11400, 11570, },
        { 6573, 6752, 6970, 7165, 7377, 7588, 7798, 8008, 8217, 8400, 8608, 8813, 9017, 9218, 9396, 9598, 9799, 10000, 10200, 10380, 10580, 10780, 10980, 11180, 11370, 11550, 11750, 11940, 12140, 12330, 12510, 12700, 12890, },
        { 7314, 7504, 7748, 7958, 8193, 8429, 8664, 8897, 9129, 9332, 9561, 9787, 10020, 10240, 10440, 10670, 10890, 11110, 11340, 11530, 11750, 11980, 12200, 12420, 12640, 12830, 13050, 13269, 13490, 13710, 13900, 14110, 14330, },
        { 8102, 8322, 8597, 8829, 9092, 9353, 9614, 9872, 10130, 10360, 10610, 10870, 11120, 11370, 11590, 11840, 12090, 12340, 12580, 12800, 13050, 13300, 13540, 13790, 14040, 14250, 14490, 14740, 14980, 15219, 15430, 15680, 15910, },
        { 9018, 9249, 9552, 9809, 10100, 10390, 10680, 10960, 11250, 11500, 11780, 12070, 12340, 12620, 12869, 13150, 13419, 13700, 13969, 14219, 14490, 14769, 15040, 15310, 15590, 15830, 16100, 16369, 16650, 16910, 17150, 17420, 17680, },
        { 9996, 10250, 10580, 10870, 11190, 11520, 11830, 12150, 12470, 12750, 13060, 13380, 13690, 13990, 14269, 14569, 14880, 15190, 15500, 15769, 16080, 16390, 16690, 16990, 17300, 17560, 17870, 18160, 18460, 18760, 19020, 19320, 19610, },
        { 11100, 11380, 11750, 12070, 12430, 12780, 13140, 13490, 13850, 14160, 14510, 14860, 15200, 15550, 15850, 16200, 16540, 16880, 17230, 17530, 17870, 18210, 18530, 18880, 19210, 19510, 19850, 20180, 20510, 20850, 21140, 21470, 21790, },
        { 12330, 12650, 13060, 13410, 13810, 14210, 14600, 15000, 15400, 15740, 16130, 16520, 16910, 17290, 17620, 18000, 18380, 18770, 19140, 19480, 19860, 20230, 20620, 20980, 21360, 21690, 22060, 22430, 22810, 23180, 23510, 23870, 24240, },
        { 13660, 14019, 14480, 14869, 15319, 15760, 16200, 16640, 17070, 17450, 17890, 18320, 18740, 19170, 19540, 19960, 20390, 20810, 21240, 21610, 22030, 22450, 22870, 23290, 23710, 24070, 24480, 24900, 25310, 25720, 26070, 26489, 26900, },
        { 15030, 15450, 15960, 16390, 16880, 17360, 17840, 18330, 18810, 19230, 19700, 20180, 20650, 21130, 21540, 22010, 22480, 22940, 23410, 23820, 24290, 24750, 25210, 25680, 26139, 26539, 27000, 27450, 27910, 28370, 28770, 29220, 29660, },
        { 16380, 16820, 17370, 17860, 18390, 18920, 19450, 19970, 20500, 20960, 21480, 22000, 22520, 23030, 23480, 23990, 24500, 25020, 25530, 25970, 26480, 26989, 27500, 28010, 28500, 28939, 29450, 29950, 30450, 30930, 31370, 31870, 32360, },
        { 17710, 18180, 18770, 19280, 19850, 20430, 21000, 21570, 22140, 22630, 23200, 23760, 24320, 24880, 25360, 25920, 26470, 27030, 27580, 28070, 28620, 29170, 29720, 30270, 30810, 31280, 31820, 32370, 32900, 33450, 33900, 34440, 34980, },
        { 18910, 19430, 20080, 20630, 21250, 21870, 22480, 23090, 23700, 24230, 24840, 25440, 26039, 26639, 27160, 27760, 28360, 28950, 29550, 30070, 30660, 31250, 31830, 32430, 33010, 33520, 34120, 34700, 35270, 35850, 36360, 36940, 37510, }
    }
};

// ====================================================================================================
// OPC
// ====================================================================================================
UINT8 bDBLMaxNoiseThreshold;
UINT8 bDBLMaxDiffThreshold; // next 0xd0a5
UINT8 bDBLTotalDiffThreshold; // next 0xd0a6
UINT8 bDBLNormlizedHistPrec; // next 0xd0a7
UINT8 bAlternateTurn;
UINT8 fgDynamicBacklightOn;
UINT8 bDBLUpdate;
UINT8 bDBLSceneChange;
UINT8 fgDBLOnToOFF;

void vDrvDBLHWInit(void)
{
    fgDynamicBacklightOn = DYNAMIC_BACKLIGHT_ON;

    vRegWriteFldAlign(DBL_00, DYNAMIC_BACKLIGHT_ON, DBL_ENABLE);

}

void vDrvDBLSWInit(void)
{
    bDBLMaxDiffThreshold = DBL_MAX_DIFF_THRESHOLD;
    bDBLTotalDiffThreshold = DBL_TOTAL_DIFF_THRESHOLD;
    bDBLMaxNoiseThreshold = DBL_MAX_NOISE_THRESHOLD;
    bDBLNormlizedHistPrec = DBL_NORMALIZED_HIST_PREC;

    vIO32WriteFldAlign(OPC_0A, 995, OPC_MAX_CONTENT_TH);
    vIO32WriteFldAlign(OPC_01, 0, OPC_ON_OFF);
    vIO32WriteFldAlign(OPC_01, 0, OPC_CONTROL_ON_OFF);    
    vIO32WriteFldAlign(OPC_0E, 128, RUN_QUE_SIZE);        
    vIO32WriteFldAlign(OPC_10, 75, OPC_MAX_DIFF_THD);    
    vIO32WriteFldAlign(OPC_10, 300, OPC_TOTAL_DIFF_THD);        
}

UINT32 u4aPreNormHist[RGB_MAX_HIST_NUM];

UINT8 bDrvOPCSceneChange(const UINT16 *u2aNormHist)
{
    UINT32 u4MaxDiff = 0;
    UINT32 u4Diff = 0;	
    UINT32 u4TotalDiff = 0;
    UINT8  bi;
    UINT16 u2MaxDiffThd = IO32ReadFldAlign(OPC_10, OPC_MAX_DIFF_THD);
    UINT16 u2TotalDiffThd = IO32ReadFldAlign(OPC_10, OPC_TOTAL_DIFF_THD);

	if (u2aNormHist == NULL)
	{
		return SV_FALSE;
	}

	for (bi = 0; bi < RGB_MAX_HIST_NUM; bi++)
	{
		u4Diff = AbsDiff(u2aNormHist[bi], u4aPreNormHist[bi]);
		u4TotalDiff += u4Diff;
		
		u4MaxDiff = (u4MaxDiff < u4Diff) ? u4Diff : u4MaxDiff;

		u4aPreNormHist[bi] = u2aNormHist[bi];
	}

    vIO32WriteFldAlign(OPC_0F, u4MaxDiff, OPC_MAX_DIFF);
    vIO32WriteFldAlign(OPC_0F, u4TotalDiff, OPC_TOTAL_DIFF);

	if ((u4MaxDiff > u2MaxDiffThd ) && 
	    (u4TotalDiff > u2TotalDiffThd))
	{
		return SV_TRUE;
	}
	else
	{
		return SV_FALSE;
	}
}

UINT8 bDrvOPCFindMaxContent(const UINT16 *u2aNormHist)
{
    UINT16 bi;
    UINT32 u4HistCount;
    UINT32 u4MaxThd, u4MaxContent;
    
    UINT16 u2HStart, u2HEnd, u2VStart, u2VEnd;
    UINT16 u2MaxThdRatio = IO32ReadFldAlign(OPC_0A, OPC_MAX_CONTENT_TH);

    u2HStart = IO32ReadFldAlign(MAXMETER_00, C_MAX_METER_X_START);
    u2HEnd = IO32ReadFldAlign(MAXMETER_00, C_MAX_METER_X_END);
    u2VStart = IO32ReadFldAlign(MAXMETER_01, C_MAX_METER_Y_START);
    u2VEnd = IO32ReadFldAlign(MAXMETER_01, C_MAX_METER_Y_END);    

    u4HistCount = (((u2HEnd - u2HStart + 1) * (u2VEnd - u2VStart + 1))>>5)<<5;
    u4MaxThd = (u4HistCount * u2MaxThdRatio + 500) / 1000;

    for(bi = RGB_MAX_HIST_NUM - 1; bi > 0; bi--)
    {
        u4HistCount -= (UINT32)u2aNormHist[bi] << 5;
        if(u4HistCount < u4MaxThd)
            break;
    }
    
    u4MaxContent = MIN((bi+1)<<3, 255);
    vIO32WriteFldAlign(OPC_01, u4MaxContent, OPC_MAX_CONTENT);   
    
    return u4MaxContent;
}

void vDrvOPCTableSearch(UINT16 u2LuminanceTable[3][LUMA_TABLE_SIZE][LUMA_TABLE_SIZE], UINT8 bMaxContent, OPC_INFO *OPCInfo)
{
    UINT8 u1PWMIndex = MIN(IO32ReadFldAlign(OPC_0E, PWM_TABLE_IDX), PWM_TABLE_MAX-1);
    UINT16 u2CurrentLuminance = u2LuminanceTable[u1PWMIndex][(((UINT16)bMaxContent+1)>>3)][LUMA_TABLE_SIZE-1]; // content x pwm
    UINT8 bSearchedPWM, bSearchedContent;

    // find where PWM can reduce to
    for(bSearchedPWM = 0; bSearchedPWM < LUMA_TABLE_SIZE; bSearchedPWM++)
    {
        if(u2LuminanceTable[u1PWMIndex][LUMA_TABLE_SIZE - 1][bSearchedPWM] >= u2CurrentLuminance)
        {
            break;
        }
    }

    if(bSearchedPWM > (LUMA_TABLE_SIZE-1)) // for klockwork warning
    { 
        bSearchedPWM = LUMA_TABLE_SIZE-1;
    }

    // find where content should extend to
    for(bSearchedContent = LUMA_TABLE_SIZE - 1; bSearchedContent > 0; bSearchedContent--)
    {
        if(u2LuminanceTable[u1PWMIndex][bSearchedContent][bSearchedPWM] <= u2CurrentLuminance)
        {
            break;
        }
    }

    // calculate curve info
    bSearchedContent = (UINT8)MIN((UINT16)bSearchedContent<<3 , 255);

    if(u1PWMIndex > (PWM_TABLE_MAX-1))  // for klockwork warning
    {        
        u1PWMIndex = PWM_TABLE_MAX-1;    
    }    

    if(bSearchedPWM > (LUMA_TABLE_SIZE-1)) // for klockwork warning
    { 
        bSearchedPWM = LUMA_TABLE_SIZE-1;  
    }
    
    OPCInfo->OPC_CURVE_PWM = OPC_PWM_INDEX[u1PWMIndex][bSearchedPWM];

    vIO32WriteFldAlign(OPC_0A, OPCInfo->OPC_CURVE_PWM, OPC_SEARCHED_PWM);
        
    if(bSearchedContent > (bMaxContent<<1)) // constrain max content gain to 2.0
    {
        OPCInfo->OPC_CURVE_P2 = MIN(bMaxContent<<1, 128);
        OPCInfo->OPC_CURVE_P1 = MIN(bMaxContent, 128);
        OPCInfo->OPC_CURVE_G2 = 0xFF;
        OPCInfo->OPC_CURVE_G1 = 0xFF;                     
    }
    else
    {
        bMaxContent = MAX(bMaxContent, 1);
        OPCInfo->OPC_CURVE_P2 = (UINT8)MIN((UINT16)bSearchedContent, 255);
        OPCInfo->OPC_CURVE_P1 = bSearchedContent >> 1;
        OPCInfo->OPC_CURVE_G2 = (UINT8)MIN(MAX((((UINT16)bSearchedContent)<<7)/bMaxContent, 128), 255);
        OPCInfo->OPC_CURVE_G1 = OPCInfo->OPC_CURVE_G2;       
    }

    if (OPCInfo->OPC_CURVE_P1 == OPCInfo->OPC_CURVE_P2)
    {
        OPCInfo->OPC_CURVE_GSLP = 0;
    }
    else
    {
        OPCInfo->OPC_CURVE_GSLP = (((INT16)OPCInfo->OPC_CURVE_G1-(INT16)OPCInfo->OPC_CURVE_G2)<<7)/((INT16)OPCInfo->OPC_CURVE_P2-(INT16)OPCInfo->OPC_CURVE_P1);        
    }
}

typedef enum
{
	OPC_ITEM_P1 = 0,
	OPC_ITEM_P2 = 1,
	OPC_ITEM_G1  = 2,
	OPC_ITEM_G2  = 3,
	OPC_ITEM_PWM  = 4,
	OPC_ITEM_MAX,
} OPC_QUE_ITEM;

UINT32 OPCDataQue[5][256];
UINT32 OPCQueCounter[5] = {0, 0, 0, 0, 0};

UINT32 u4ReturnQueAverage(UINT32 dwValue, UINT8 bItem)
{
	UINT32 bi;
	UINT32 PWMAverage=0;
	UINT32 OPCQueSize = IO32ReadFldAlign(OPC_0E, RUN_QUE_SIZE);

	if (OPCQueCounter[bItem] == 0)
	{
		OPCQueCounter[bItem]++;
		OPCDataQue[bItem][0] = dwValue;
		return dwValue;
	}

	if (OPCQueCounter[bItem] < OPCQueSize)
	{
		OPCQueCounter[bItem]++;

	}
	else
	{
        OPCQueCounter[bItem] = OPCQueSize;
	}

	for (bi = OPCQueCounter[bItem] - 1; bi > 0 ; bi--)
	{
		OPCDataQue[bItem][bi] = OPCDataQue[bItem][bi - 1];
	}

	// Insert New Item
	OPCDataQue[bItem][0] = dwValue;

	// Average Running Que
	for (bi = 0; bi < OPCQueCounter[bItem]; bi++)
	{
		PWMAverage += OPCDataQue[bItem][bi];
	}

	PWMAverage = PWMAverage / OPCQueCounter[bItem];

	return PWMAverage;
}

void vDrvClearTPF(void)
{
	OPCQueCounter[0] = 0;
	OPCQueCounter[1] = 0;
	OPCQueCounter[2] = 0;
	OPCQueCounter[3] = 0;
	OPCQueCounter[4] = 0;
}

UINT32 u4MotionThd[8] = {15000, 8000, 4000, 2000, 1000, 250,  50,  10};    // from fast (short queue) to slow (long queue)
UINT16 u2MotionQue[8] = {    8,   16,   48,   64,   80,  96, 128, 255};    // from fast (short queue) to slow (long queue)
#define OPC_MOTION_THD 10000
#define OPC_MAX_QUE_SIZE 128
#define OPC_MIN_QUE_SIZE  64

UINT32 u4DrvGet3DNrMotion(void)
{
    UINT32 u4Cur3DNrMotion = 0; //IO32ReadFldAlign(NR_3DNR4C, PW_MOCNT_FINAL);

    return u4Cur3DNrMotion;
}

UINT32 u4DrvGetMotionQueue(UINT32 u4CurrentMotion, UINT32 u4CurrentQueueSize)
{
    if(u4CurrentMotion > OPC_MOTION_THD)
    {
        u4CurrentQueueSize--;
    }
    else
    {
        u4CurrentQueueSize++;
    }

       
    return MAX(MIN(u4CurrentQueueSize, OPC_MAX_QUE_SIZE), OPC_MIN_QUE_SIZE);
}

void vDrvOPCInfoTPF(OPC_INFO *OPCInfo, UINT8 bOPCSceneChange)
{
    OPC_INFO TargetOPCInfo;
    
    UINT8 bRunQueSize = IO32ReadFldAlign(OPC_0E, RUN_QUE_SIZE);
    
    if(!bOPCSceneChange)
    {
        // todo : fast rising, slow falling      
    	TargetOPCInfo.OPC_CURVE_G1 = OPCInfo->OPC_CURVE_G1;
	    TargetOPCInfo.OPC_CURVE_G2 = OPCInfo->OPC_CURVE_G2;
    	TargetOPCInfo.OPC_CURVE_P1 = OPCInfo->OPC_CURVE_P1;
	    TargetOPCInfo.OPC_CURVE_P2 = OPCInfo->OPC_CURVE_P2;
        TargetOPCInfo.OPC_CURVE_PWM = OPCInfo->OPC_CURVE_PWM;        
      
        // adaptively change queue size
        bRunQueSize = u4DrvGetMotionQueue(u4DrvGet3DNrMotion(), bRunQueSize);
        
        vIO32WriteFldAlign(OPC_0E, bRunQueSize, RUN_QUE_SIZE);

        if(TargetOPCInfo.OPC_CURVE_PWM > OPCInfo->OPC_CURVE_PWM)    // need to rise-up in PWM, content from dark to bright
        {
           	OPCInfo->OPC_CURVE_G1 = u4ReturnQueAverage(TargetOPCInfo.OPC_CURVE_G1, OPC_ITEM_P1);
    	    OPCInfo->OPC_CURVE_G2 = u4ReturnQueAverage(TargetOPCInfo.OPC_CURVE_G2, OPC_ITEM_P2);
    	    OPCInfo->OPC_CURVE_P1 = u4ReturnQueAverage(TargetOPCInfo.OPC_CURVE_P1, OPC_ITEM_G1);
	        OPCInfo->OPC_CURVE_P2 = u4ReturnQueAverage(TargetOPCInfo.OPC_CURVE_P2, OPC_ITEM_G2);
            OPCInfo->OPC_CURVE_PWM = u4ReturnQueAverage(TargetOPCInfo.OPC_CURVE_PWM, OPC_ITEM_PWM);
        
        	OPCInfo->OPC_CURVE_G1 = u4ReturnQueAverage(TargetOPCInfo.OPC_CURVE_G1, OPC_ITEM_P1);
    	    OPCInfo->OPC_CURVE_G2 = u4ReturnQueAverage(TargetOPCInfo.OPC_CURVE_G2, OPC_ITEM_P2);
    	    OPCInfo->OPC_CURVE_P1 = u4ReturnQueAverage(TargetOPCInfo.OPC_CURVE_P1, OPC_ITEM_G1);
	        OPCInfo->OPC_CURVE_P2 = u4ReturnQueAverage(TargetOPCInfo.OPC_CURVE_P2, OPC_ITEM_G2);
            OPCInfo->OPC_CURVE_PWM = u4ReturnQueAverage(TargetOPCInfo.OPC_CURVE_PWM, OPC_ITEM_PWM);        
        }
        else
        {
        	OPCInfo->OPC_CURVE_G1 = u4ReturnQueAverage(TargetOPCInfo.OPC_CURVE_G1, OPC_ITEM_P1);
    	    OPCInfo->OPC_CURVE_G2 = u4ReturnQueAverage(TargetOPCInfo.OPC_CURVE_G2, OPC_ITEM_P2);
        	OPCInfo->OPC_CURVE_P1 = u4ReturnQueAverage(TargetOPCInfo.OPC_CURVE_P1, OPC_ITEM_G1);
    	    OPCInfo->OPC_CURVE_P2 = u4ReturnQueAverage(TargetOPCInfo.OPC_CURVE_P2, OPC_ITEM_G2);
            OPCInfo->OPC_CURVE_PWM = u4ReturnQueAverage(TargetOPCInfo.OPC_CURVE_PWM, OPC_ITEM_PWM);
        }
    }
    else
    {  

		OPCQueCounter[0] = 0;
		OPCQueCounter[1] = 0;
		OPCQueCounter[2] = 0;
		OPCQueCounter[3] = 0;
		OPCQueCounter[4] = 0;            
		
    	OPCInfo->OPC_CURVE_G1 = u4ReturnQueAverage(OPCInfo->OPC_CURVE_G1, OPC_ITEM_P1);
        OPCInfo->OPC_CURVE_G2 = u4ReturnQueAverage(OPCInfo->OPC_CURVE_G2, OPC_ITEM_P2);
    	OPCInfo->OPC_CURVE_P1 = u4ReturnQueAverage(OPCInfo->OPC_CURVE_P1, OPC_ITEM_G1);
        OPCInfo->OPC_CURVE_P2 = u4ReturnQueAverage(OPCInfo->OPC_CURVE_P2, OPC_ITEM_G2);
        OPCInfo->OPC_CURVE_PWM = u4ReturnQueAverage(OPCInfo->OPC_CURVE_PWM, OPC_ITEM_PWM);        

    }
}

void vDrvOPCResultUpdate(void)
{
    // write opc curve
	vIO32WriteFldMulti(DBL_01, 
	                    P_Fld(IO32ReadFldAlign(OPC_0B, OPC_SW_P1), DBL_P1) |
	                    P_Fld(IO32ReadFldAlign(OPC_0B, OPC_SW_P2), DBL_P2) |
	                    P_Fld(IO32ReadFldAlign(OPC_0B, OPC_SW_G1), DBL_G1) |
	                    P_Fld(IO32ReadFldAlign(OPC_0B, OPC_SW_G2), DBL_G2));    
	vIO32WriteFldAlign(DBL_00, 
	                          IO32ReadFldAlign(OPC_0C, OPC_SW_G_SLOPE), DBL_G_SLOPE);


    // write backlight
    DRVCUST_SetBacklightDuty(VDP_ARG_BACKLT_OPC, IO32ReadFldAlign(OPC_0C, OPC_SW_PWM));  	    
}

void vDrvOPCInfoToSwReg(OPC_INFO *OPCInfo)
{
    vIO32WriteFldAlign(OPC_0B, OPCInfo->OPC_CURVE_P1, OPC_SW_P1);
    vIO32WriteFldAlign(OPC_0B, OPCInfo->OPC_CURVE_P2, OPC_SW_P2);        
    vIO32WriteFldAlign(OPC_0B, OPCInfo->OPC_CURVE_G1, OPC_SW_G1);
    vIO32WriteFldAlign(OPC_0B, OPCInfo->OPC_CURVE_G2, OPC_SW_G2);
    vIO32WriteFldAlign(OPC_0C, OPCInfo->OPC_CURVE_GSLP, OPC_SW_G_SLOPE);    
    vIO32WriteFldAlign(OPC_0C, OPCInfo->OPC_CURVE_PWM, OPC_SW_PWM);            
    vDrvOPCResultUpdate();
}

void vDrvOPCSetDefault(void)
{
    vIO32WriteFldAlign(OPC_0B, 0x80, OPC_SW_P1);
    vIO32WriteFldAlign(OPC_0B, 0x80, OPC_SW_P2);        
    vIO32WriteFldAlign(OPC_0B, 0x80, OPC_SW_G1);
    vIO32WriteFldAlign(OPC_0B, 0x80, OPC_SW_G2);
    vIO32WriteFldAlign(OPC_0C, 0x0, OPC_SW_G_SLOPE);    
    vIO32WriteFldAlign(OPC_0C, 0xFF, OPC_SW_PWM);        
    vDrvOPCResultUpdate();
}

UINT16 u2MaxContentThd = 994;

void vDrvAdaptiveSetMaxThd(const UINT16 *u2aNormHist)    
{
    UINT8 bi;
    UINT32 u4HistCount = 0;
    UINT32 u4HistSum = 0;    
    UINT32 u4APL = 0;    
    UINT16 u2MaxContentThdApplied = u2MaxContentThd;
    
    for(bi = 0; bi < RGB_MAX_HIST_NUM; bi++)
    {
        u4HistSum += u2aNormHist[bi] * MIN(((bi+1) << 4),255);
        u4HistCount += u2aNormHist[bi];
    }

    if(u4HistCount == 0)
    {
        u4APL = 0x0;
    }
    else
    {
        u4APL = u4HistSum / u4HistCount;
    }  

    if(u4APL > 0x80)
    {
        u2MaxContentThdApplied = u2MaxContentThdApplied + ((u4APL-0x80)>>5);
    }
    else
    {
        u2MaxContentThdApplied = u2MaxContentThdApplied - ((0x80-u4APL)>>5);
    }

    vIO32WriteFldAlign(OPC_0A, u2MaxContentThdApplied, OPC_MAX_CONTENT_TH);
}

void vDrvOPCProc(void)
{
    UINT16 OPCRGBMaxNormHist[RGB_MAX_HIST_NUM];
    UINT16 OPCRGBMaxHist[RGB_MAX_HIST_NUM];    
    UINT8  bOPCSceneChange = SV_FALSE;
    UINT8  bMaxContent = 0;
    OPC_INFO sOPCInfo;
    // a. parameters:
        // a. 17x17 luminance array
        // b. rgb histogram
    // 0. if on
    if(IO32ReadFldAlign(OPC_01, OPC_ON_OFF))
    {
        // 0. read curve (with normalization)
            // in : rgb histogram
            // out : rgb normalized histogram
        
        if(IO32ReadFldAlign(OPC_01, OPC_CONTROL_HIST_SEL))  //use luma histogram
        {
        u1DrvGetRGBHist(OPCRGBMaxNormHist, OPC_HIST_NORMBASE);        
        u1DrvGetRGBHist(OPCRGBMaxHist, 0);        
        }
        else
        {
            u1DrvGetLumaHistWeight(OPCRGBMaxNormHist, OPC_HIST_NORMBASE);    
            u1DrvGetLumaHistWeight(OPCRGBMaxHist, 0);            
        }

        // 1. detect scene change
            // in : histogram
            // out : scene change flag
        bOPCSceneChange = bDrvOPCSceneChange(OPCRGBMaxNormHist);
        // 2. find maximum
            // in : rgb histogram
            // out : maximum value

        vDrvAdaptiveSetMaxThd(OPCRGBMaxNormHist);
        
        bMaxContent = bDrvOPCFindMaxContent(OPCRGBMaxHist);        
        // 3. table searching
            // in : 17x17 luminance table, current pwm, max content
            // in : maximum value
            // out : (pwm, content) of maximum content
        vDrvOPCTableSearch(OPCLuminanceTable, bMaxContent, &sOPCInfo);
        // 4. temporal filtering
            // in : opc info, scene change flag
            // out : filtered opc info
        vDrvOPCInfoTPF(&sOPCInfo, bOPCSceneChange);
        // 5. update to sw register          
        vDrvOPCInfoToSwReg(&sOPCInfo);
    }
    else
    {
        vDrvClearTPF();
        // 7. set default
        vDrvOPCSetDefault();      
    }
    opc_cnt++;    
}

#endif
