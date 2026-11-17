#ifndef __REVISION_H__
#define __REVISION_H__
 #define _REVISION      "@rev_id_str@"
 #define _HASH          "@rev_hash_str@"
 #define FILEVER        0,0,@rev_id@,0
 #define PRODUCTVER     0,0,@rev_id@,0
 #define REV_DATE       "@rev_date@"
 #define STRFILEVER     "0, 0, @rev_id@, @rev_hash@"
 #define STRPRODUCTVER  "0, 0, @rev_id@, @rev_hash@"

 #define _DATE                      "@rev_date@"
 #define _BRANCH                    "@rev_branch@"
 #define _CMAKE_COMMAND             R"(@CMAKE_COMMAND@)"
 #define _CMAKE_VERSION             R"(@CMAKE_VERSION@)"
 #define _CMAKE_HOST_SYSTEM         R"(@CMAKE_HOST_SYSTEM_NAME@ @CMAKE_HOST_SYSTEM_VERSION@)"
 #define _SOURCE_DIRECTORY          R"(@CMAKE_SOURCE_DIR@)"
 #define _BUILD_DIRECTORY           R"(@BUILDDIR@)"
 #define _MYSQL_EXECUTABLE          R"(@MYSQL_EXECUTABLE@)"
 #define _FULL_DATABASE             "world_548_20240722.sql"
 #define VER_COMPANYNAME_STR        "Pandria 5.4.8 Developers"
 #define VER_LEGALCOPYRIGHT_STR     "(c)2008-@rev_year@ Pandria 5.4.8"
 #define VER_FILEVERSION            0,0,0
 #define VER_FILEVERSION_STR        "@rev_hash@ @rev_date@ (@rev_branch@ branch)"
 #define VER_PRODUCTVERSION         VER_FILEVERSION
 #define VER_PRODUCTVERSION_STR     VER_FILEVERSION_STR
#endif // __REVISION_H__
