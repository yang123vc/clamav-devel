/*
 *  Phishing module: domain list implementation.
 *
 *  Copyright (C) 2006 T�r�k Edvin <edwintorok@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA 02110-1301, USA.
 *
 *  $Log: phish_domaincheck_db.c,v $
 *  Revision 1.2  2006/09/26 18:55:36  njh
 *  Fixed portability issues
 *
 *  Revision 1.1  2006/09/13 19:40:27  njh
 *  First draft
 *
 *  Revision 1.1  2006/09/12 19:38:39  acab
 *  Phishing module merge - libclamav
 *
 *  Revision 1.3  2006/08/20 21:18:11  edwin
 *  Added the script used to generate iana_tld.sh
 *  Added checks for phish_domaincheck_db
 *  Added phishing module design document from wiki (as discussed with aCaB).
 *  Updated .wdb/.pdb format documentation (in regex_list.c)
 *  Fixed some memory leaks in regex_list.c
 *  IOW: cleanups before the deadline.
 *  I consider my module to be ready for evaluation now.
 *
 *  Revision 1.2  2006/08/09 16:26:44  edwin
 *  Forgot to add these files
 *
 */


#if HAVE_CONFIG_H
#include "clamav-config.h"
#endif

#ifdef CL_EXPERIMENTAL

#ifndef CL_DEBUG
#define NDEBUG
#endif

#ifdef CL_THREAD_SAFE
#ifndef _REENTRANT
#define _REENTRANT
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#ifdef	HAVE_STRINGS_H
#include <strings.h>
#endif
#include <ctype.h>

#include <limits.h>
#include "clamav.h"
#include <sys/types.h>

#ifdef	HAVE_REGEX_H
/*#define USE_PCRE*/
#include <regex.h>
#endif

#if defined(HAVE_READDIR_R_3) || defined(HAVE_READDIR_R_2)
#include <stddef.h>
#endif

#include "others.h"
#include "defaults.h"
#include "str.h"
#include "filetypes.h"
#include "mbox.h"
#include "phish_domaincheck_db.h"
#include "regex_list.h"
#include "matcher-ac.h"


static struct regex_matcher domainlist_matcher;

int domainlist_match(const char* real_url,const char* display_url,int hostOnly,unsigned short* flags)
{
	const char* info;
	int rc = regex_list_match(&domainlist_matcher,real_url,display_url,hostOnly,&info);
	if(rc && info && info[0]) {/*match successfull, and has custom flags*/
		if(strlen(info)==3 && isxdigit(info[0]) && isxdigit(info[1]) && isxdigit(info[2])) {
			unsigned short notwantedflags=0;
			sscanf(info,"%hx",&notwantedflags);
		        *flags &= ~notwantedflags;/* filter unwanted phishcheck flags */	
		}
		else {
			cli_warnmsg("Phishcheck:Unknown flag format in domainlist, 3 hex digits expected");
		}
	}
	return rc;
}

int init_domainlist(void)
{
	return	init_regex_list(&domainlist_matcher);
}

int is_domainlist_ok(void)
{
	return is_regex_ok(&domainlist_matcher);
}

int cli_loadpdb(FILE* fd,unsigned int options)
{
	return load_regex_matcher(&domainlist_matcher,fd,options);
}

void domainlist_cleanup(void)
{
	regex_list_cleanup(&domainlist_matcher);
}

void domainlist_done(void)
{
	regex_list_done(&domainlist_matcher);
}

#endif
