#include "stdafx.h"
#include "File.h"
#include "InputOutputStream.h"
#include "InputStream.h"

InputStream *InputStream::getResourceAsStream(const wstring &fileName)
{
	return new FileInputStream( File( fileName ) );
}