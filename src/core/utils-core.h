/**
 *  @author dmilith
 *
 *   © 2014 - VerKnowSys
 *
 */

#ifndef __UTILS_CORE_H__
#define __UTILS_CORE_H__


#include "../globals/globals.h"

#include <QtCore>
#include <string.h>


QString readFileContents(const QString& fileName);
void setDefaultEncoding();
bool isWebApp(const QString& basePath);


#endif

