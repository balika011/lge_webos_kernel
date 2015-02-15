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
// PIDSelect.cpp : implementation file
//

#include "stdafx.h"
#include "3desTS.h"
#include "PIDSelect.h"

#include "glob.h"
#include "dmx_drm_drvif.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern char keyout[256];
extern FILE* KeyOut;

//
FILE* KeyFileHandle;

char KeyFileName[256];
/////////////////////////////////////////////////////////////////////////////
// PIDSelect dialog


PIDSelect::PIDSelect(CWnd* pParent /*=NULL*/)
	: CDialog(PIDSelect::IDD, pParent)
{
	//{{AFX_DATA_INIT(PIDSelect)
	mSameKey = 1;
	mSameEO = 1;
	mPIDSelectMsg = _T("");
	m_KeyOut = _T("KeyOut.txt");
	m_KeyFile = _T("");
	//}}AFX_DATA_INIT
}


void PIDSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PIDSelect)
	DDX_Radio(pDX, IDC_RADIO1, mSameKey);
	DDX_Radio(pDX, IDC_RADIO3, mSameEO);
	DDX_Text(pDX, IDC_PIDSelectTxt, mPIDSelectMsg);
	DDX_Text(pDX, IDC_EDIT_KeyOut, m_KeyOut);
	DDX_Text(pDX, IDC_TXT_KEY_FILE, m_KeyFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PIDSelect, CDialog)
	//{{AFX_MSG_MAP(PIDSelect)
	ON_BN_CLICKED(IDC_BUTTON_KeyGen, OnBUTTONKeyGen)
	ON_BN_CLICKED(IDC_Button_KEY_File, OnButtonKEYFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PIDSelect message handlers

void PIDSelect::OnBUTTONKeyGen() 
{
	// TODO: Add your control notification handler code here
	static char bb[256];

	char	str[256];
    unsigned short	pid;
	char	*stopstr;
	unsigned char	keyE[24],keyO[24];
	int		jj=0,k=0;

    extern  ENPID   enpid[_MAX_PID_NO];
	extern  void keygen(unsigned char* kout, int sz);
	extern  FILE* KeyOut;
	extern	void fputn(FILE* fs, unsigned char *cp, size_t n);
	extern	char keyout[256];
    extern	unsigned char KeyGenNo;
	extern	volatile unsigned char KeyLen;
	UpdateData(TRUE);

	mPIDSelectMsg.Format("Generating ...\n");
	//OutputDebugString("test\n");
	
	if (KeyLen != 24 && KeyLen != 8)
	{
	    sprintf(bb,"should choose DES or 3DES, can't key gen...\n");				
		AfxMessageBox(bb);	
		return;
	}
	
	if (0 == KeyOut)
	{
	    strcpy(keyout,m_KeyOut.GetBuffer(256));	
	    if (0 == strlen(keyout))
	    {
	        sprintf(bb,"File name is empty...\n");				
			AfxMessageBox(bb);	
		    return;
		}
			
		if( (KeyOut = fopen( keyout, "wb" )) == NULL )
		{
		    sprintf(bb,"error open KeyOut file...\n");				
	        AfxMessageBox(bb);	
		    return;
		}
	}
	else
	    {rewind(KeyOut);}
    
    fprintf(KeyOut,(KeyLen== 24)? "3DES\n" : "DES\n");
	fprintf(KeyOut,"pid evenkey oddkey:\n");


	for(int i=0; i<_MAX_PID_NO;i++)
	{		

		GetDlgItemText(IDC_PID1+i,str,256);
		pid = (unsigned short) (strtoul(str,&stopstr,16) & 0x1fff);

		enpid[i].pid = pid;
		enpid[i].valid = IsDlgButtonChecked(IDC_CHECK1+i);

		//gen key process
		if (enpid[i].valid) //gen key
		{
			
			if (0 == jj) // 1st, must gen, don't check mSameKey
			{
				keygen(keyE,KeyLen);
				//enpid[i].key
				if (mSameEO) 
				{
					memcpy(keyO, keyE, KeyLen);
				}
				else
				{
					keygen(keyO,KeyLen);
				}
				memcpy(enpid[i].keyE, keyE, KeyLen);
				memcpy(enpid[i].keyO, keyO, KeyLen);
				goto _TONext;
			}
			// i = 1 ..
			if (mSameKey) //all use same old key
			{
				memcpy(enpid[i].keyE, keyE, KeyLen);
				memcpy(enpid[i].keyO, keyO, KeyLen);
			}
			else // all re-gen
			{
				keygen(keyE,KeyLen);
				if (mSameEO) 
				{
					memcpy(keyO, keyE, KeyLen);
				}
				else
				{
					keygen(keyO,KeyLen);
				}
				memcpy(enpid[i].keyE, keyE, KeyLen);
				memcpy(enpid[i].keyO, keyO, KeyLen);
			}
_TONext:
			
			fprintf(KeyOut,"%04x ",enpid[i].pid);
			fputn(KeyOut, enpid[i].keyE, KeyLen);
			fprintf(KeyOut," ");
			fputn(KeyOut, enpid[i].keyO, KeyLen);
			fprintf(KeyOut,"\n");
			jj++;
		}//if
	}//for

	//sprintf(str[0],"%s","asdf");
	//sprintf(bb,"%d %s %s pid %d %d chk %d %d",IDC_PID1, str[0], str[1], pid[0],pid[1], chk[0],chk[1]);
	//AfxMessageBox(bb);
	KeyGenNo = jj;
	fclose(KeyOut); KeyOut = NULL;
	mPIDSelectMsg.Format("Generating %d pair keys\n",jj);
	UpdateData(FALSE);
}

int PIDSelect::PIDReadKey1(FILE *keyf)
{
    int i=0, j=0;
	char temp[5];// temp2[3];
	char *stopstr;
	extern  ENPID   enpid[_MAX_PID_NO];
	extern volatile unsigned char KeyLen;
    extern	unsigned char KeyGenNo;
     
    for(i=0; i< _MAX_PID_NO; i++)
	{
		enpid[i].valid = 0;
		enpid[i].pid = 0;
	}
 
	i = 0;
	if (fread(temp, sizeof(char), 4, keyf) < 4)
		return(1);
	temp[4]=0;//must have, release version doesn't do init	
	enpid[i].pid = (unsigned short) (strtoul(temp,&stopstr,16) & 0x1fff);
	fread(temp, sizeof(char), 1, keyf); // read 1 space

	for (j = 0; j< KeyLen; j++)
	{
		if ( fread(temp, sizeof(char), 2, keyf) < 2)
			return(2);
		temp[2] = 0;
		enpid[i].keyE[j] = (unsigned char)(strtoul(temp,&stopstr,16) & 0xff);
	}

	fread(temp, sizeof(char), 1, keyf); // read 1 space

	for (j = 0; j< KeyLen; j++)
	{
		if ( fread(temp, sizeof(char), 2, keyf) < 2)
			return(3);
		temp[2] = 0;
		enpid[i].keyO[j] = (unsigned char)(strtoul(temp,&stopstr,16) & 0xff);
	}	

	enpid[i].valid = 1;
    KeyGenNo++;
    
    mPIDSelectMsg.Format("p[0]=[%04x]: even:%02x%02x ...\n",enpid[0].pid, enpid[0].keyE[0], enpid[0].keyE[1] );
	UpdateData(FALSE);
	return(0);
}

void PIDSelect::OnButtonKEYFile() 
{
	// TODO: Add your control notification handler code here
	char tempDir[_MAX_PATH];
	//FILE *lpf;
	char bb[256];
	
	
	GetCurrentDirectory(_MAX_PATH,tempDir);
    static char BASED_CODE szFilter[] = "txt file(*.txt)|*.txt|All Files(*.*)|*.*|";
	CFileDialog filerequest(TRUE, NULL, NULL, 0, szFilter,NULL );
	
	if(filerequest.DoModal()==IDOK)
	{
		//if((lpf=fopen(filerequest.GetPathName().GetBuffer(256), "rb"))== NULL)
		//{
		//	AfxMessageBox("open file got problem, re-select one...");

		//	return;
		//}
		m_KeyFile = filerequest.GetFileName();
		UpdateData(FALSE);
		//sprintf(tsin,"%s",filerequest.GetPathName().GetBuffer(256));
		strcpy(KeyFileName,filerequest.GetPathName().GetBuffer(256));
		
		//sprintf(s,"read this is a book %s",filerequest.GetFileName());
		//AfxMessageBox(s);
		//UpdateData(FALSE);
				
		// open file and read key
		if (KeyFileHandle != NULL)
		    fclose(KeyFileHandle);
		    
        if( (KeyFileHandle = fopen( KeyFileName, "rt" )) == NULL )
	    {
	        sprintf(bb,"error open Key file...\n");				
	        AfxMessageBox(bb);		
	        return;
	    }	
	    //read key		    
		PIDReadKey1(KeyFileHandle); //+++ read return
		fclose(KeyFileHandle);

		return;
		    

	}	
}


