#ifndef _ZEROEXPORTER_H
#define _ZEROEXPORTER_H

#include <stdio.h>

class CZeroData;

class CZeroExporter
{
public:
	CZeroExporter();
	virtual ~CZeroExporter();

	void ExportFile(FILE *fp, CZeroData *pData);
};

#endif