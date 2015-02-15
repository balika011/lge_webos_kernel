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
// Jovi Chang
//
// 3desTSDlg.cpp : implementation file
// Note:
//      Now, Decrypt only use the resident key(enpid[]), doesn't read the key file...
// [Known Bugs]
//      1. CBC mode still got some bug ??? not sure
// [limitation]
// 1. Decrypt doesn't read key table, run it only when (1) after Encrypt & without change the keys  and (2) binary mode
// 2. Now Input TS should be binary mode
// [History]
// 20050114: ver 1.3
//           change buff type to unsigned char, and all related
// 20050113: ver 1.2.3
//			 1. solve problem, when press stop, the encrypt count and decrypt count will be reset then the count is not correct
//			 2. found a  problem in GETPID() and st_272.ts
//					buff char 0x90c0 but pid1 = *(buf+1) always get 0xffc0, not 0xc0 ???
//              because buff type is char(sign), cast to pid(unsign) will have sign bit 0xffxx
//              => should use unsigned char
// 20041102: ver 1.2.2
//			 add read key input file, now support strict format, read 1 key only
//           Release version will not do zero memory initialization
// 20040910: ver 1.2.1
//			 add some dialog box when get sync byte error
//			 CBC mode test iv nonzero.... seems no problem after add fflush +++ ???
//			 iv should be zero, and inter-block
// 20040623: ver 1.2.0
//			adaptation should skip and not encrypted.
// 20040602: ver 1.1.3
//			add a button, for embedded descramble bits in Sync byte(bit5 bit6) or scrambling_control_field
// 20040531: add a button, for fix even or odd key, or random even/odd
// 20040528: ver 1.1.2 for POD, not modify 0x47, only modify scramble control bit
// 20040414: ver 1.1
//      1.solve Release version crash problem ???
//		  if thread enable all desC->SendMessage(WM_USER_REFRESH); got crash problem in Release version,
//        change to desC->UpdateData(FALSE); => not crash in Release ver, but crash in Debug version
//        solution ???: (now will not crash in both mode)
//		   OnUSERRefresh()=> OnUSERRefresh(WPARAM wParam, LPARAM lParam).
//         (BTW, use PostMessage faster ?.  .h => WM_APP not WM_USER)
//      2. modify all printf to PRINT
//      3. move out the des3key() out of loop => about 3 times faster
// 20040413: ver 1.0
//      TS Out choose move to 1)area, chose mode then open file, o/w default text mode => read 0a, write 0d0a.
//      fix the decrypt typo, and bugs
//      (buf[0] & 0x30) == 0x20
// 20040412:
//		add decrypt
//		find encrpt=> decrypt, not original reault ???
//		find fwrite: 0a => 0d0a in binary mode ???, re-run is ok ??? why ???
// 20040411: (ver 0.9)
//      modify layout, keyout file select move to PID dialog, force do 1)OK button to avoid ambiguous
//      add DES
// 20040410:
//      create a thread to support "stop" button
//      TS Out can be text or binary
//      add CBC, but only 1 IV now
//      TSInit... still need to check...
#include "stdafx.h"
#include "3desTS.h"
#include "3desTSDlg.h"

#include "PIDSelect.h"

#include "dmx_drm_drvif.h"
#include "glob.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
//for debugging
#define _3DEBUG

#ifdef _3DEBUG
#define PRINT   OutputDebugString
#else
#define PRINT
#endif

//+++ Intel is little endian...
#define SYNC_BYTE		0x47
#define MODE_BYPASS  0
#define MODE_ENCRYPT 1
#define MODE_DECRYPT 2

unsigned short GETPID(unsigned char* buf);

void TSComplete();
void keygen(unsigned char* kout, int sz);
int SeekSync(FILE* stream);

int PidMatch(unsigned short pid );



/*-----------------------------------------------------------------------------
                    global variables
-----------------------------------------------------------------------------*/
FILE *TSIn, *TSOut, *PIDOut, *KeyOut;
volatile size_t TSLen;
volatile unsigned char KeyLen;
unsigned char buff[208];
ENPID   enpid[_MAX_PID_NO];
char tsin[256],tsout[256],pidout[256],keyout[256];
static char bb[256];
unsigned char KeyGenNo = 0; // totally how many key need to check
HANDLE  hEncryptThread = NULL;
unsigned long encrypt_count = 0, decrypt_count = 0;

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMy3desTSDlg dialog

CMy3desTSDlg::CMy3desTSDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMy3desTSDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMy3desTSDlg)
	m_TSIn = _T("");
	m_CBC = 0;
	m_DES = 0;
	mTSOut = _T("TSOut.ts");
	mPIDOut = _T("PIDOut.txt");
	mMsg = _T("");
	mRadioText = 1;
	mToEncrypt = 1;
	mKeySelection = 2;
	mUseCABit = 1;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMy3desTSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMy3desTSDlg)
	DDX_Text(pDX, IDC_TXT_TSIn, m_TSIn);
	DDX_Radio(pDX, IDC_RADIO_CBC, m_CBC);
	DDX_Radio(pDX, IDC_RADIO3_DES, m_DES);
	DDX_Text(pDX, IDC_EDIT1_TSOut, mTSOut);
	DDX_Text(pDX, IDC_EDIT3_PIDOut, mPIDOut);
	DDX_Text(pDX, IDC_Msg, mMsg);
	DDX_Radio(pDX, IDC_Radio_Binary, mRadioText);
	DDX_Radio(pDX, IDC_RADIO_Encrypt, mToEncrypt);
	DDX_Radio(pDX, IDC_Button_KEY_SELECTTION, mKeySelection);
	DDX_Radio(pDX, IDC_CA_Bit, mUseCABit);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMy3desTSDlg, CDialog)
	//{{AFX_MSG_MAP(CMy3desTSDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_Button1_TSIn, OnButton1TSIn)
	ON_BN_CLICKED(IDC_BUT_Sele_OK, OnBUTSeleOK)
	ON_BN_CLICKED(IDC_BUTTON1_Sel_PID, OnBUTTON1SelPID)
	ON_BN_CLICKED(IDC_BUTTON1_Start, OnBUTTON1Start)
	ON_BN_CLICKED(IDC_RADIO3_DES, OnRadio3Des)
	ON_MESSAGE(WM_USER_REFRESH,OnUSERRefresh)
	ON_BN_CLICKED(IDC_RADIO4, OnRadio4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMy3desTSDlg message handlers

BOOL CMy3desTSDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	 TSLen = 188;
	 KeyLen = 24;
	 TSIn = TSOut = PIDOut = KeyOut = NULL;
	 UserBreak = FALSE;
	 STARTEncrypt = FALSE;
     srand( (unsigned)time( NULL ) );
     mMsg.Format(_T("Please follow the steps..."));
     UpdateData(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMy3desTSDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMy3desTSDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMy3desTSDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMy3desTSDlg::OnOK()
{
	// TODO: Add extra validation here

	CDialog::OnOK();
}

void CMy3desTSDlg::OnCancel()
{
	// TODO: Add extra cleanup here

    UpdateData(TRUE);
	CDialog::OnCancel();
}

void CMy3desTSDlg::OnButton1TSIn()
{

	char tempDir[_MAX_PATH];

	GetCurrentDirectory(_MAX_PATH,tempDir);
    static char BASED_CODE szFilter[] = "ts file(*.mpg;*.trp;*.ts;*.bin)|*.mpg;*.trp;*.ts;*.bin|All Files(*.*)|*.*|";
	CFileDialog filerequest(TRUE, NULL, NULL, 0, szFilter,NULL );

	if(filerequest.DoModal()==IDOK)
	{
		m_TSIn = filerequest.GetFileName();
		UpdateData(FALSE);

		strcpy(tsin,filerequest.GetPathName().GetBuffer(256));
	}
}

void CMy3desTSDlg::OnBUTSeleOK()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	mMsg.Format(_T(""));

	if (TSInit() == -1)
	{
	    mMsg.Format(_T("Open Files Error !!!"));
	}
	else
	{
	    mMsg.Format(_T("Open Files successful !!!"),1);

	}


    //mMsg.Format(_T("test %d\n"),1);
	UpdateData(FALSE);
}

//==========================================================================================

void fputn(FILE* fs, unsigned char *cp, size_t n)
{
	size_t i;

	for(i=0; i<n; i++)
	{
		fprintf(fs,"%02x",*cp++ & 0xff);
	}
}


/******************************************************************************
 * function:
 * description:
 * In:
 * Out:
 * return:
 * Note:
 * TODO:
 ******************************************************************************/
void keygen(unsigned char* kout, int sz)
{
	unsigned char ktmp;

	for (int j= 0; j< sz; j++)
	{
		ktmp = (unsigned char) (rand() & 0xff);
		memcpy(&kout[j],&ktmp,1);
	}

	#ifdef _3DEBUG
	for(int i = 0; i< sz; i++)
	{
		sprintf(bb, "%02x", kout[i]);
		PRINT(bb);
    }
	PRINT("\n");
	#endif

}

//
// SeekSync()
//
// Note: "0D0A" len = 1, only read 0A in text mode ...
// TODO:
//      1. read ahead 188/204, to know if really sync, and to know it's 188 or 204
int SeekSync(FILE* stream)
{
	int count = 0, total = 0, result = 0, ret = 0;
	char buffer;

	while( !feof( stream ) )
	{
	  /* Attempt to read in 1 bytes: */

      count = fread( &buffer, sizeof( char ), 1, stream );

      if( ferror( stream ) )
      {
         PRINT( "SeekSync Read error\n" );
         ret = 1;
         break;
      }

      /* Total up actual bytes read */

	  if (SYNC_BYTE == buffer)
	  {
      	 result = fseek( stream, -1L, SEEK_CUR);
      	 if( result ){
            PRINT( "Fseek failed\n" );
	  	 }
		 break;
	  }
	  total += count;
	}

	if ( feof(stream) && buffer != SYNC_BYTE) {ret = 1; total-=1;}

	sprintf(bb, "Number of bytes skip = %d\n", total);
    PRINT( bb );

    return (ret);
}

/******************************************************************************
 * function:
 * description:
 * In:
 * Out:
 * return:
 * Note:
 * TODO:
 ******************************************************************************/
int CMy3desTSDlg::TSInit()
{

	strcpy(tsout,mTSOut.GetBuffer(256));
	strcpy(pidout,mPIDOut.GetBuffer(256));


	if (0 == strlen(tsin) ||0 == strlen(tsout)||0 == strlen(pidout))
	{
		sprintf(bb,"Some File name is empty !!!");
		AfxMessageBox(bb);
		return (-1);
    }

    if ((TSIn != NULL) || (TSOut != NULL) || (PIDOut != NULL) )
    {
        TSComplete();
    }

    if( (TSIn = fopen( tsin, "rb" )) == NULL )
	{
	    sprintf(bb,"error open TSIn file...\n");
	    AfxMessageBox(bb);
	    return(-1);
	}

	if (mRadioText == 1)
	    TSOut = fopen( tsout, "wt" );
	else
	    TSOut = fopen( tsout, "wb" );

	if( TSOut == NULL )
	{
	    sprintf(bb,"error open TSOut file...\n");
	    AfxMessageBox(bb);
	    return(-1);
	}

	if( (PIDOut = fopen( pidout, "wt" )) == NULL )
	{
	    sprintf(bb,"error open PIDOut file...\n");
	    AfxMessageBox(bb);
		return(-1);
	}

	return(0);
}

/******************************************************************************
 * function:
 * description:
 * In:
 * Out:
 * return:
 * Note:
 * TODO:
 ******************************************************************************/
void TSComplete()
{
    _flushall();
    if (TSIn) fclose(TSIn);
    if (TSOut){fflush(TSOut); fclose(TSOut);}
    if (PIDOut) {fflush(PIDOut); fclose(PIDOut);}
    if (KeyOut) {fflush(KeyOut); fclose(KeyOut);}
	TSIn = TSOut = PIDOut = KeyOut = NULL;

    return;
}

/******************************************************************************
 * function:
 * description:
 * In:
 * Out:
 * return:
 * Note:
 * TODO:
 ******************************************************************************/
size_t CMy3desTSDlg::GetPacket(FILE *stream, unsigned char *buf)
{
	size_t count = 0;

    if (stream == NULL)
	{
		PRINT("GetPacket, File not open\n");
		return (0);
	}

	if ( !feof( stream ) )
	{
	  /* Attempt to read in TSLen bytes: */
      count = fread( buf, sizeof( char ), TSLen, stream );

      if( ferror( stream ) )
      {
         PRINT( "GetPacket Read error\n" );
      }
    }

    if (mToEncrypt)
    {
        if ((*buf & SYNC_BYTE) != SYNC_BYTE)
        {
        	sprintf(bb,"\nNot sync, data = 0x%02x !!!\n", *(unsigned char*)buf);
            PRINT(bb);
            AfxMessageBox(bb); //2004/09/10 add
            count = 0;
        }
    }

    return (count);
}

//
void
memxor(unsigned char *dest,unsigned char *src,int cnt)
{
	int i;

	for(i=0;i<cnt;i++)
		dest[i] ^= src[i];
}

void
HEX2ASC(unsigned char val, unsigned char* str, unsigned char base)
{
    unsigned char tmp0,tmp1;
    tmp1 = val % base;
    tmp0 = val / base;

    str[0] = (tmp0 > 9)? ('a'+(tmp0 - 10)) : ('0'+ tmp0);
    str[1] = (tmp1 > 9)? ('a'+(tmp1 - 10)) : ('0'+ tmp1);

    return;
}


/******************************************************************************
 * function:
 * description:
 * In:
 * Out:
 * return:
 * Note:
 * TODO:
 ******************************************************************************/
int CMy3desTSDlg::EnPacket(unsigned char *buf, unsigned char flag)
{
	unsigned char key[24],work[8];//,plain[8],cipher[8];
    static DES3_KS kne,knd;
    static DES_KS  des_kne, des_knd;
	size_t test;
	unsigned short pid = 0;
	unsigned char *skip4 = buf+4;
	int whichpid = -1;
	unsigned char evenodd = mKeySelection; // init read button select.

	unsigned char    h2asc[3];
	unsigned char iv[8]={0,0,0,0,0,0,0,0};//{0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef}; //inter-block
	unsigned char   adaptfield = 0, payload_offset = 0; // should < 184

    pid = GETPID(buf);
    fprintf(PIDOut,"%04x",pid);

    if (MODE_BYPASS != flag && 0 != KeyGenNo )
    {
	    // check pid to know if need encrypt

        if ( (whichpid =PidMatch(pid)) >= 0 )
        {
            adaptfield = (buf[3]>>4 ) & 0x3;
            if (0 == (adaptfield & 0x1) ) // 00 and 10 => skip
            {
                goto _DO_Output;
            }
            else if (adaptfield == 0x3) // adaptation + payload
            {
                payload_offset = 1 + buff[4];//calc adap len
            }
            else // 0x1 payload only => all encrypt
            {
                payload_offset = 0;
            }

            // now we need to encrypt
	        // init key, CBC or ECB

	        // get a random to know Even or Odd, modify sync byte
	        // 0100 0111, 0100 =>last 2 bits 10: even(0x20), 11: odd(0x30)
	        //
	        encrypt_count++;
	        if (evenodd > 1) // if by random
	            evenodd = (unsigned char) (rand() & 0x1);
	        if(evenodd) //even
	        {
	            memcpy(key, enpid[whichpid].keyE,KeyLen);
				if (mUseCABit)
				{
					buf[3] = (buf[3] & 0x3f) | 0x80;
				}
				else
				{
					buf[0]|=0x20;
				}
	            fprintf(PIDOut," even",pid);
	        }
	        else //odd
	        {
	            memcpy(key, enpid[whichpid].keyO,KeyLen);
				if (mUseCABit)
				{
					buf[3] = (buf[3] & 0x3f) | 0xc0;
				}
				else
				{
					buf[0]|=0x30;
				}
	            fprintf(PIDOut," odd",pid);
	        }

	        // encryption, use a Even/Odd key
	        //skip4+=4;
	        if (m_DES)
	        {
	            deskey(des_kne,key,0);
	        }
	        else
	        {
	            des3key(kne,key,0);
	        }

	        for (test= payload_offset; test <= (TSLen-4 -8); test+=8) // if left < 8B, don't do it
	        {
	            memcpy(work,skip4+test,8); //get plain text
	            if (m_CBC)
	            {
	                memxor(work,iv,8);
	            }
	            if (m_DES)
	            {
	                des(des_kne,work);
	            }
	            else
	            {
	                des3(kne,work);
	            }

                memcpy(skip4+test,work,8); // copy back

                if (m_CBC)
                {
                    memcpy(iv,work,8);
                }

            }//encrypt
        }//PidMatch
    }//if not bypass and has gen key

_DO_Output:
    fprintf(PIDOut,"\n");
    if(mRadioText == 1) //text output
    {
        for (int i = 0; i< (int)TSLen; i++)
        {
            HEX2ASC(buf[i], (unsigned char*) h2asc,16);
            fprintf(TSOut,"%c%c ",h2asc[0],h2asc[1]);
            if(0==(i+1)%10)fprintf(TSOut,"\n");
        }
        fprintf(TSOut,"\n");
    }
    else
    {
        test = fwrite( buf, sizeof( char ), TSLen, TSOut );

    }

	fflush(TSOut); //2004/09/08 debug test
    return(0);

}

int PidMatch(unsigned short pid )
{
    int i;
    for(i=0; i< _MAX_PID_NO; i++)
    {
        if (enpid[i].valid && (pid == enpid[i].pid) )
            return(i);
    }
    if( i >= _MAX_PID_NO)
        return(-1);

	return(-1);
}

unsigned short GETPID(unsigned char* buf)
{
    unsigned short pid = 0;

    buf++;
    pid = ( (*buf <<8) |  *(buf+1) ) & 0x1fff;

    return(pid);
}

/******************************************************************************
 * function:
 * description:
 * In:
 * Out:
 * return:
 * Note:
 * TODO:
 ******************************************************************************/
void CMy3desTSDlg::OnBUTTON1SelPID()
{
	// TODO: Add your control notification handler code here
	PIDSelect dlg;
	dlg.DoModal();
}

/******************************************************************************
 * function:
 * description:
 * In:
 * Out:
 * return:
 * Note:
 * TODO:
 ******************************************************************************/
DWORD WINAPI EncryptThread(LPVOID para)
{
    CMy3desTSDlg *desC;
    desC = (CMy3desTSDlg*)para;
    size_t res =0;
    unsigned long pks=0;
    unsigned short pid = 0x1fff;
    desC->mMsg.Format(_T("Encrypting ..."));
    desC->PostMessage(WM_USER_REFRESH);

    if (desC->mToEncrypt)
    {
        if (SeekSync(TSIn))
        {
            desC->mMsg.Format(_T("Error, no Sync Byte !!!"));

        	desC->PostMessage(WM_USER_REFRESH);
        	AfxMessageBox("Error, no Sync Byte !!!");
            goto _THREAD_END;
        }
    }

    while (TRUE)
    {
        if(desC->UserBreak)
            break;

        if ((res = desC->GetPacket(TSIn, buff)) < TSLen )
        {
	        desC->mMsg.Format(_T("Encrypting End File ..."));

			desC->PostMessage(WM_USER_REFRESH);
			break;
        }
        pks++;
		if ((pks & 0xff) == 0)
		{
            desC->mMsg.Format(_T("read %ld ..."),pks);
    		desC->PostMessage(WM_USER_REFRESH);
        }

        if (desC->mToEncrypt)
            desC->EnPacket(buff,MODE_ENCRYPT);
        else
            desC->DePacket(buff,MODE_DECRYPT);
    }

_THREAD_END:

    TSComplete();
    desC->SetDlgItemText(IDC_BUTTON1_Start,"Start");
	desC->STARTEncrypt = FALSE;
    desC->mMsg.Format(_T("Encrypt End!!! total packets %ld, encrypt %ld, decrypt %ld\n"), pks, encrypt_count, decrypt_count);

    desC->PostMessage(WM_USER_REFRESH);
    AfxEndThread(0);
    return(0);
}

/******************************************************************************
 * function:
 * description:
 * In:
 * Out:
 * return:
 * Note:
 * TODO:
 ******************************************************************************/
void CMy3desTSDlg::OnBUTTON1Start()
{
	// TODO: Add your control notification handler code here
	unsigned long tid;

	UpdateData(TRUE);

	if (FALSE == STARTEncrypt)
	{
        mMsg.Format(_T("Open Files successful !!!"),1);
        UpdateData(FALSE);

	    if ((NULL == TSIn) || (NULL == TSOut) || (NULL == PIDOut) )
	    {
	        sprintf(bb,"Please do 1. File (Re)Select and Press OK");
	        AfxMessageBox(bb);
	        return;
	    }
	    encrypt_count = 0; decrypt_count = 0;
	    STARTEncrypt = TRUE;
	    SetDlgItemText(IDC_BUTTON1_Start,"Stop");
   		UserBreak = FALSE;

    	hEncryptThread = CreateThread(0,0x1000, EncryptThread,this, 0, &tid);
        if (NULL == hEncryptThread)
        {
            mMsg.Format(_T("Create Thread Fail !!!"));
        }
        else
        {
            mMsg.Format(_T("Create Thread Success !!!"));
        }

	}
	else //want to stop the thread
	{
    	UserBreak = TRUE;
		hEncryptThread = NULL;
	}

	UpdateData(FALSE);

	return;
}



/******************************************************************************
 * function:
 * description:
 * In:
 * Out:
 * return:
 * Note:
 * TODO:
 ******************************************************************************/
LRESULT CMy3desTSDlg::OnUSERRefresh(WPARAM wParam, LPARAM lParam)// 20040414, add para, o/w stack got problem when release version
{
	UpdateData(FALSE);
	return 0;
}


/******************************************************************************
 * function:
 * description:
 * In:
 * Out:
 * return:
 * Note:
 * TODO:
 ******************************************************************************/
void CMy3desTSDlg::OnRadio3Des()
{
	// TODO: Add your control notification handler code here
	KeyLen = 24;
	return;
}


/******************************************************************************
 * function:
 * description:
 * In:
 * Out:
 * return:
 * Note:
 * TODO:
 ******************************************************************************/
void CMy3desTSDlg::OnRadio4()
{
	// TODO: Add your control notification handler code here
	KeyLen = 8;
	return;
}


/******************************************************************************
 * function:
 * description:
 * In:
 * Out:
 * return:
 * Note:
 * TODO:
 ******************************************************************************/
int CMy3desTSDlg::DePacket(unsigned char *buf, unsigned char flag)
{
	unsigned char key[24],work[8];//,plain[8],cipher[8];
    static DES3_KS kne,knd;
    static DES_KS  des_kne, des_knd;
	size_t test;
	unsigned short pid = 0;
	unsigned char *skip4 = buf+4;
	int whichpid = -1;
	unsigned char evenodd = 0;
	//char header[4];
	char    h2asc[3];
	/*static*/ unsigned char iv[8]={0,0,0,0,0,0,0,0};//{0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef}; //inter-block
	static unsigned char ivtmp[8];
	unsigned char	cabit = 0;
	unsigned char   adaptfield = 0, payload_offset = 0; // should < 184

    pid = GETPID(buf);
    fprintf(PIDOut,"%04x",pid);

    if (mUseCABit)
	{
	    cabit = buf[3] >> 6;
	}
	else
	{
	    cabit = (buf[0] >> 4) & 0x3;
	}

    if (MODE_BYPASS != flag && 0 != KeyGenNo && ( cabit & 0x2 )) //  encrypt check, buf [3] msb = 1 means scrambled
    {
	    // check pid to know if need decrypt

        if ( (whichpid =PidMatch(pid)) >= 0 )
        {
            adaptfield = (buf[3]>>4 ) & 0x3;
            if (0 == (adaptfield & 0x1) ) // 00 and 10 => skip
            {
                goto _ToOutput;
            }
            else if (adaptfield == 0x3) // adaptation + payload
            {
                payload_offset = 1 + buff[4];//calc adap len
            }
            else // 0x1 payload only => all encrypt
            {
                payload_offset = 0;
            }

            // now we need to encrypt
	        // init key, CBC or ECB

	        // get a random to know Even or Odd, modify sync byte
	        // 0100 0111, 0100 =>last 2 bits 10: even(0x20), 11: odd(0x30)
	        //
	        //memcpy(header, buf,4);

	        // check high nibble lower 2 bit

	        if ((cabit & 0x3) == 0x2) //even
	        {
                memcpy(key, enpid[whichpid].keyE,KeyLen);
				if (mUseCABit) { buf[3] &= 0x3f;} // clear ca bit
				else {buf[0] &= ~0x20;} // clear ca bit
	            fprintf(PIDOut," even",pid);
	        }
	        else if ((cabit & 0x3) == 0x3) // odd key
	        {
	            memcpy(key, enpid[whichpid].keyO,KeyLen);
				if (mUseCABit) {buf[3]&= 0x3f;} // clear ca bit
				else {buf[0]&= ~0x30;} // clear ca bit
	            fprintf(PIDOut," odd",pid);
	        }
	        else
	        {
	            fprintf(PIDOut," don't know how to handle...",pid);
	            goto _ToOutput;
	        }

            decrypt_count++;

	        if (m_DES)
	        {
	            deskey(des_knd,key,1);
	        }
	        else
	        {
	            des3key(knd,key,1);
	        }

	        for (test = payload_offset; test <= (TSLen-4 -8); test+=8)
	        {
	            memcpy(work,skip4+test,8); //get plain text

	            if(m_CBC)
	            {	/* Save incoming ciphertext for chain */
			        memcpy(ivtmp,work,8);
		        }

	            if (m_DES)
	            {
	                des(des_knd,work);
	            }
	            else
	            {
	                des3(knd,work);
	            }

		        if(m_CBC)
		        {	/* Unchain block, save ciphertext for next */
			        memxor(work,iv,8);
			        memcpy(iv,ivtmp,8);
		        }

                memcpy(skip4+test,work,8); // copy back

            }//for
        }//PidMatch
    }//if

_ToOutput:
    fprintf(PIDOut,"\n");
    if(mRadioText == 1)
    {
        for (int i = 0; i< (int)TSLen; i++)
        {
            HEX2ASC(buf[i], (unsigned char*) h2asc,16);
            fprintf(TSOut,"%c%c ",h2asc[0],h2asc[1]);
            if(0==(i+1)%10)fprintf(TSOut,"\n");
        }
        fprintf(TSOut,"\n");
    }
    else
    {
        test = fwrite( buf, sizeof( char ), TSLen, TSOut );
    }

    fflush(TSOut); //2004/09/08 debug test
    return(0);
}


