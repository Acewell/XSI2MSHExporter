#include "ZeroParser.h"
#include "ZeroData.h"

//***************************************************************************************
//
//	Function : ZeroReadHeader
//	Author : CGF
//	Date : 3/6/01
//	Arguments :	fp=input file
//	Comments :	reads and verifies Zero header from file

//***************************************************************************************
void ZeroReadHeader(FILE *) 
{

}


//***************************************************************************************
//
//	Function : ZeroRead
//	Author : CGF
//	Date : 3/6/01
//	Arguments :		fp=input file
//					lList=output linear list
//	Return Value :
//	Comments :

//***************************************************************************************
void ZeroRead(FILE* fp, CZeroData *pData) 
{
	CZeroParser ParserObj;
	ParserObj.ParseFile(fp,pData);
}

//***************************************************************************************
//
//	Function : CreateZeroFileStructure
//	Author : CGF
//	Date : 3/6/01
//	Arguments :		fp = input file
//					list = output list
//	Comments :		Reads in a zero file and fills in a CGCLinearList

//***************************************************************************************
void CreateZeroFileStructure(FILE* fp, CZeroData *pData) 
{ 
    // Read Header and Verify
    ZeroReadHeader(fp);
    ZeroRead(fp, pData);
}


//***************************************************************************************
//
//	Class CZeroParser
//
//***************************************************************************************
CZeroParser::CZeroParser()
{
}

CZeroParser::~CZeroParser()
{
}

void CZeroParser::ParseFile(FILE *fp, CZeroData *pData)
{
    unsigned size;

	// read the block header
   	CMSHBlock::BlockHeader header;
	CMSHBlock::ReadHeader(fp, &header);

    size = header.size;
	if(header.type=='RDEH')
	{
		while (size)
		{
			// read a block header
			CMSHBlock::BlockHeader header;
			CMSHBlock::ReadHeader(fp,&header);

			// deduct size
			_ASSERTE(size >= sizeof(CMSHBlock::BlockHeader) + header.size);
			size -= sizeof(CMSHBlock::BlockHeader);
			size -= header.size;

			// parse the block
			switch (header.type) {
    			// right handed file
				case '1HSM':
		    		pData ->SetMSHVersion(1);
					ParseMSH1(fp, header.size, pData);
    				break;
    		
        		// left handed file
    			case '2HSM':
		    		pData ->SetMSHVersion(2);
					ParseMSH1(fp, header.size, pData);
    				break;
    		
        		// read skeleton
    			case 'LEKS':
    				pData ->ParseSkeleton(fp, &header, 0);
    				break;
    		
        		// read skeleton
    			case '2LKS':
    				pData ->ParseSkeleton(fp, &header, 1);
    				break;
    		
        		// read blend factors
    			case 'DNLB':
    				pData ->ParseSkeletonBlendFactors(fp, &header, 0);
    				break;
    		
        		// read blend factors
    			case '2NLB':
    				pData ->ParseSkeletonBlendFactors(fp, &header, 1);
    				break;
    		
    			// read animation
    		   case 'MINA':
    			   pData ->ParseAnimation(fp, &header, 0);
    			   break;
				
    			// read animation
    		   case '2MNA':
    			   pData ->ParseAnimation(fp, &header, 1);
    			   break;
				
			   case 'L1LC':
				   pData ->ParseAnimTriggerCollisions(fp, &header);
				   break;

				default:
    				CMSHBlock::SkipBlock(fp, header.size);
    				break;
			}
		}
	}
	else if(header.type=='1HSM')
	{
		pData ->SetMSHVersion(1);
		ParseMSH1(fp, header.size, pData);
	}
	else if(header.type=='2HSM')
	{
		pData ->SetMSHVersion(2);
		ParseMSH1(fp, header.size, pData);
	}
}   	

void CZeroParser::ParseMSH1(FILE *fp, unsigned int size, CZeroData *pData)
{
	// while there is size left...
	while (size)
	{
		// read a block header
		CMSHBlock::BlockHeader header;
		CMSHBlock::ReadHeader(fp,&header);
		
		// deduct size
		_ASSERTE(size >= sizeof(CMSHBlock::BlockHeader) + header.size);
		size -= sizeof(CMSHBlock::BlockHeader);
		size -= header.size;

		// parse the block
		switch (header.type)
		{
		    
         case 'FNIF':
    			pData ->ParseFileInfo(fp,&header);
    			break;
    
    		case 'FNIS':
    			pData ->ParseSceneInfo(fp,&header);
    			break;
    
    		case 'IBMA':
    			pData ->ParseAmbience(fp,&header);
    			break;
    
    		case 'DGOF':
    			pData ->ParseFog(fp,&header);
    			break;
    
    		case 'RMAC':
    			pData ->ParseCamera(fp,&header);
    			break;
    			
    		case 'PTGL':
    			pData ->ParsePointLight(fp,&header);
    			break;
    			
    		case 'ITGL':
    			pData ->ParseInfiniteLight(fp,&header);
    			break;
    			
    		case 'STGL':
    			pData ->ParseSpotLight(fp,&header);
    			break;
    			
    		case 'TESM':
    			pData ->ParseMaterialSet(fp,&header);
    			break;
    			
    		case 'LTAM':
    			pData ->ParseMaterialList(fp,&header);
    			break;
    			
    		case 'LDOM':
    			pData ->ParseModel(fp,&header);
    			break;

			//---------------------------------------------------------------
			// Model ID's are gone, so we can't read old-style cloths anymore.
    		case 'HTLC':
    			CMSHBlock::SkipBlock(fp, header.size);
    			break;
    

    		default:
    			CMSHBlock::SkipBlock(fp, header.size);
	    		break;
		}
	}
}

