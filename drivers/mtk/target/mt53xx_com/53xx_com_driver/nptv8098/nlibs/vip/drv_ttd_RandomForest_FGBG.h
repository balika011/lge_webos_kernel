/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2005, MediaTek, Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: drv_ttd_RandomForestModelheader.h $
 * $Revision: #1 $
 *
 *----------------------------------------------------------------------------*/

int ivpRandomForestModel[] = {3,
7,2,0,1,2,1,2,231,231,4096,4096,0,24,432,1,0,0,2,2,0,205,0,8192,1,2,3,6,2,2,231,26,7363,828,0,21,445,3,4,5,3,2,4,26,1092,7099,0,28,118,4,0,0,4,2,0,26,0,8192,1,5,0,0,4,2,4,0,8192,0,1,6,0,0,3,2,227,0,8192,0,1,
3,2,0,1,2,1,2,11,11,4096,4096,0,0,64,1,0,0,2,2,11,0,8192,0,1,2,0,0,2,2,0,11,0,8192,1,
7,2,0,1,4,1,2,219,219,4096,4096,0,26,201,1,2,3,2,2,56,218,1674,6517,0,15,864,2,0,0,3,2,56,0,8192,0,1,3,0,0,3,2,0,218,0,8192,1,4,5,6,2,2,163,1,8142,49,0,21,876,5,0,0,3,2,0,1,0,8192,1,6,0,0,3,2,163,0,8192,0,1};
int ivpRandomForestFeature[6] = {24,21,28,0,26,15};
int ivpRandomForestFeatureMap[57] = {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


/// SIVPTreeNode
typedef struct{

    unsigned int m_splitFeature;
    int m_splitThreshold;

    unsigned int m_nClass;
    int* m_classProbability;
    int*   m_classSampleCount;

    unsigned int m_nodeID;
    unsigned int m_depth;

    int m_isLeaf;
    unsigned int m_leftNode;
    unsigned int m_rightNode;

}SIVPTreeNode;

/// SIVPDecisionTree
typedef struct{

    unsigned int m_nClass;
    unsigned int m_nNode;
    
    SIVPTreeNode **mpp_treeNode;

} SIVPDecisionTree;

/// SIVPRandomForest
typedef struct{

    SIVPDecisionTree **m_forest;
    int *m_predictProb;

    unsigned int m_nTree;
    unsigned int m_nClass;

}SIVPRandomForest;

int* predict_tree( SIVPDecisionTree *dt , int *pDatum )
{
    unsigned int currentNodeIdx = 0;

    while(dt->mpp_treeNode[currentNodeIdx]->m_isLeaf == 0)
    {       
        if( pDatum[dt->mpp_treeNode[currentNodeIdx]->m_splitFeature] >= dt->mpp_treeNode[currentNodeIdx]->m_splitThreshold ) 
        {
            currentNodeIdx = dt->mpp_treeNode[currentNodeIdx]->m_leftNode;
        }
        else
        {
            currentNodeIdx = dt->mpp_treeNode[currentNodeIdx]->m_rightNode;
        }
    }

    return dt->mpp_treeNode[currentNodeIdx]->m_classProbability;
}

unsigned int predict( SIVPRandomForest *rf , int *pDatum )
{
    int i, j, k;
    int *probDist = NULL;
    int maxProb = 0;
    unsigned int maxClass = 0;

    /// initialize the output probability
    for( j=0; j<rf->m_nClass ; j++)
    {
        rf->m_predictProb[j] = 0;
    }

    /// iterate all trees
    for( j=0 ; j<rf->m_nTree ; j++ )
    {
        probDist = predict_tree(rf->m_forest[j], pDatum);

        for( k=0; k<rf->m_nClass ; k++)
        {
            rf->m_predictProb[k] += probDist[k] ;
        }
    }

    for( k=0; k<rf->m_nClass; k++)
    {
        rf->m_predictProb[k] /= rf->m_nTree;
    }

    for( i=0; i<rf->m_nClass; i++)
    {
        if(rf->m_predictProb[i]>maxProb)
        {
            maxProb = rf->m_predictProb[i];
            maxClass = i;
        }
    }

    if (IO32ReadFldAlign(TTD_FW_REG_18, TTD_FGBG_LOG))
    {
        for( k=0; k<rf->m_nClass; k++)
        {
            Printf("Class %d %d\n", k, rf->m_predictProb[k]);
        }
        Printf("maxClass %d\n", maxClass);
    }

    return maxClass;
}

void loadSIVPRandomForest(SIVPRandomForest *rf)
{
    int i, j, k;
    int posiInModel = 0;    

    rf->m_nTree = (unsigned int)ivpRandomForestModel[posiInModel];
    posiInModel++;

    rf->m_forest = (SIVPDecisionTree**)x_mem_alloc( sizeof(SIVPDecisionTree*) * rf->m_nTree );

    if (rf->m_forest == NULL)
    {            
        Printf("FGBG detection V3 allocate memory failed\n");
        return;
    }

    for (i=0;i<rf->m_nTree;i++)
    {
        rf->m_forest[i] = (SIVPDecisionTree*)x_mem_alloc( sizeof(SIVPDecisionTree) );

        if (rf->m_forest[i] == NULL)
        {            
            Printf("FGBG detection V3 allocate memory failed\n");
            return;
        }

        /// #node        
        rf->m_forest[i]->m_nNode = (unsigned int)ivpRandomForestModel[posiInModel];
        posiInModel++;

        /// #class
        rf->m_forest[i]->m_nClass = (unsigned int)ivpRandomForestModel[posiInModel];
        posiInModel++;

        if(i==0)
        {
            rf->m_nClass = rf->m_forest[i]->m_nClass;            
            rf->m_predictProb = (int*)x_mem_alloc( sizeof(int) * rf->m_nClass );

            if (rf->m_predictProb == NULL)
            {            
                Printf("FGBG detection V3 allocate memory failed\n");
                return;
            }
        }
        else
        {
            if( rf->m_forest[i]->m_nClass != rf->m_nClass )
            {
                Printf("Error: not all the tree in this folder has the same amount of class\n");
                return;
            }
        }

        rf->m_forest[i]->mpp_treeNode = (SIVPTreeNode**)x_mem_alloc( sizeof(SIVPTreeNode*) * rf->m_forest[i]->m_nNode );

        if (rf->m_forest[i]->mpp_treeNode == NULL)
        {            
            Printf("FGBG detection V3 allocate memory failed\n");
            return;
        }

        for( j=0 ; j< rf->m_forest[i]->m_nNode ; j++ )
        {
            rf->m_forest[i]->mpp_treeNode[j] = (SIVPTreeNode*)x_mem_alloc( sizeof(SIVPTreeNode) );

            if (rf->m_forest[i]->mpp_treeNode[j] == NULL)
            {            
                Printf("FGBG detection V3 allocate memory failed\n");
                return;
            }

            /// #m_nodeID
            rf->m_forest[i]->mpp_treeNode[j]->m_nodeID = (unsigned int)ivpRandomForestModel[posiInModel];
            posiInModel++;

            /// #m_leftNode
            rf->m_forest[i]->mpp_treeNode[j]->m_leftNode = (unsigned int)ivpRandomForestModel[posiInModel];
            posiInModel++;

            /// #m_rightNode
            rf->m_forest[i]->mpp_treeNode[j]->m_rightNode = (unsigned int)ivpRandomForestModel[posiInModel];
            posiInModel++;

            /// #m_depth
            rf->m_forest[i]->mpp_treeNode[j]->m_depth = (unsigned int)ivpRandomForestModel[posiInModel];
            posiInModel++;

            /// #class
            rf->m_forest[i]->mpp_treeNode[j]->m_nClass = (unsigned int)ivpRandomForestModel[posiInModel];
            posiInModel++;


            rf->m_forest[i]->mpp_treeNode[j]->m_classProbability = (int*)x_mem_alloc( sizeof(int) * rf->m_forest[i]->mpp_treeNode[j]->m_nClass );
            rf->m_forest[i]->mpp_treeNode[j]->m_classSampleCount = (int*)x_mem_alloc( sizeof(int) * rf->m_forest[i]->mpp_treeNode[j]->m_nClass );

            if ((rf->m_forest[i]->mpp_treeNode[j]->m_classProbability == NULL) || 
                (rf->m_forest[i]->mpp_treeNode[j]->m_classSampleCount == NULL))
            {            
                Printf("FGBG detection V3 allocate memory failed\n");
                return;
            }

            for( k=0 ; k<rf->m_forest[i]->mpp_treeNode[j]->m_nClass ; k++ )
            {
                rf->m_forest[i]->mpp_treeNode[j]->m_classSampleCount[k] = (int)ivpRandomForestModel[posiInModel];          
                posiInModel++;
            }

            for( k=0 ; k<rf->m_forest[i]->mpp_treeNode[j]->m_nClass ; k++ )
            {
                rf->m_forest[i]->mpp_treeNode[j]->m_classProbability[k] = ivpRandomForestModel[posiInModel];    
                posiInModel++;
            }

            /// is leaf
            rf->m_forest[i]->mpp_treeNode[j]->m_isLeaf = (int)ivpRandomForestModel[posiInModel];                
            posiInModel++;

            if(rf->m_forest[i]->mpp_treeNode[j]->m_isLeaf==0)
            {
                /// m_splitFeature
                rf->m_forest[i]->mpp_treeNode[j]->m_splitFeature = (unsigned int)ivpRandomForestModel[posiInModel];                
                posiInModel++;

                /// m_splitThreshold
                rf->m_forest[i]->mpp_treeNode[j]->m_splitThreshold = ivpRandomForestModel[posiInModel];                
                posiInModel++;
            }
            else
            {
                rf->m_forest[i]->mpp_treeNode[j]->m_splitFeature = 0;
                rf->m_forest[i]->mpp_treeNode[j]->m_splitThreshold = 0;
            }
        }
    }


}

void freeSIVPRandomForest(SIVPRandomForest *rf)
{
    int i, j;

    for (i=0;i<rf->m_nTree;i++)
    {
        for(j=0 ; j< rf->m_forest[i]->m_nNode ; j++ )
        {
            x_mem_free(rf->m_forest[i]->mpp_treeNode[j]->m_classProbability);
            x_mem_free(rf->m_forest[i]->mpp_treeNode[j]->m_classSampleCount);

            x_mem_free(rf->m_forest[i]->mpp_treeNode[j]);
        }

        x_mem_free(rf->m_forest[i]->mpp_treeNode);

        x_mem_free(rf->m_forest[i]);
    }

    x_mem_free(rf->m_forest);
    x_mem_free(rf->m_predictProb);
}

#if 0
/// DEMO
int SIVPRandomForestDemo(void)
{
    SIVPRandomForest rf;
    loadSIVPRandomForest(&rf);

    int datum[57] = {3001,3520,3696,24663,31752,45399,57493,76109,87816,52749,87047,90735,87631,112726,124184,151401,158849,155838,151644,129561,93774,71536,69878,44718,27561,15217,28763,42152,18906,4748,4097,8301,0,27344,26244,165094,415009,7127,62075,1370707,16,158849,121,888521,1025543,-137022,-737120,-874142,-729672,-866694,1039922,866694,-173228,-881073,-707845,-888521,-715293};
    /// perform prediction and print out the results
    printf("predicted as class %u\n", predict( &rf , datum ));
    printf("predicted probability: ");
    for(int i=0;i<2;i++)
    {
        printf("%d ",rf.m_predictProb[i]);
    }
    printf("\n");

    int datum2[57] = {129806,213925,183954,120997,135915,186406,158364,98084,66688,60292,47825,36821,22767,16526,14785,14109,11954,10375,10604,11373,13518,17506,22449,27577,22623,27518,27986,37113,64762,115318,104551,38447,0,40645,224336,219527,429922,1994,34968,1122208,1,213925,84,1354431,668682,685749,-1306606,-620857,-1140506,-454757,129806,1727207,1597401,84119,-1513282,-81981,-1679382};
    /// perform prediction and print out the results
    printf("predicted as class %u\n", predict( &rf , datum2 ));
    printf("predicted probability: ");
    for(int i=0;i<2;i++)
    {
        printf("%d ",rf.m_predictProb[i]);
    }
    printf("\n");

    freeSIVPRandomForest(&rf);

    return 0;

}
#endif


