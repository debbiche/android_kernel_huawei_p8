/******************************************************************************
 *
 *   Copyright (C), 2001-2011, Huawei Tech. Co., Ltd.
 *
 *******************************************************************************
 *  File Name     : tarzip.c
 *  Version       : Initial Draft
 *  Author        :
 *  Created       :
 *  Last Modified :
 *  Description   : creat .tar and .tar.gz file need zlib when complie
 *  Function List :
 *  History       :
 *  1.Date        :
 *    Author      :
 *    Modification: Created file
 *
 *******************************************************************************/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
#include<zlib.h>
#include<dirent.h>
#include<sys/stat.h>
#include<sys/time.h>
#include<time.h>
#include<pwd.h>
#include<errno.h>

#include "tarzip.h"
//#include "utils.h"

#define HEADERNUM      512
#define CHECKSUMAPPEND 256
#define NAMELEN        100
#define FILEMODE       8
#define UID            8
#define GID            8
#define FILESIZE       12
#define UNIXTIME       12
#define CHECKSUM       8

 /*tar file header struct */
typedef struct tag_header
{               /* byte offset */
  char name[NAMELEN];       /*   0 */
  char mode[FILEMODE];      /* 100 */
  char uid[UID];            /* 108 */
  char gid[GID];            /* 116 */
  char size[FILESIZE];      /* 124 */
  char mtime[UNIXTIME];     /* 136 */
  char chksum[CHECKSUM];    /* 148 */
  char typeflag;            /* 156 */
  char linkname[100];       /* 157 */
  char magic[6];            /* 257 */
  char version[2];          /* 263 */
  char uname[32];           /* 265 */
  char gname[32];           /* 297 */
  char devmajor[8];         /* 329 */
  char devminor[8];         /* 337 */
  char prefix[155];         /* 345 */
  char padding[12];         /* 500 */
                            /* 512 */
}header;

/*tar file header mode permissions data*/
static const char file_mode[]={0x31,0x30,0x30,0x36,0x36,0x36,0x20,0};

/*tar file haeder UID and GID date*/
static const char IDBYTE[] = {
0x20,0x20,0x20,0x20,
0x20,0x30,0x20,0x00,
0x20,0x20,0x20,0x20,
0x20,0x30,0x20,0x00,
};

#define log_err_record printf

/*****************************************************************************
 *  Function       : write_header
 *  Description    : write the tar file harder to header struct
 *  Input          : header*hd  the struct to write
 *                   char*file_name the input file name
 *  Output         : None
 *  Return Value   :
 *  Calls          :
 *  Called By      :
 *
 *  History        :
 *  1.Date         :
 *    Author       :
 *    Modification :
 *
 ******************************************************************************/
void write_header(header*hd, const char*file_name, long file_size)
{
    int o = 0,p = 0,q = 0,r = 0,s = 0;
    int name_len = 0;
    int i,j=0;
    char* index = (char*)hd;
    char buf[FILESIZE];
    const char* file_index;
    memset(buf, 0, FILESIZE);
    //file_index = strrchr(file_name, '/') + 1;
    file_index = file_name;
    name_len = strlen(file_index);

    if (name_len > NAMELEN)
    {
        name_len = NAMELEN;
    }
    for (i = 0; i < name_len; ++i)
    {
        index[i] = file_index[i];
        o += index[i];
    }

    j += NAMELEN;
    for (i = 0; i < FILEMODE; ++i)
    {
        index[j+i] = file_mode[i];
        p += file_mode[i];
    }
    j+=FILEMODE;

    for (i=0; i < UID+GID; ++i)
    {
        index[j+i] = IDBYTE[i];
        q += IDBYTE[i];
    }
    j += (UID+GID);
    snprintf(buf, FILESIZE, "%o", (unsigned int)file_size);
    for (i=0; i < FILESIZE; ++i)
    {
        index[i+j] = buf[i];
        r += buf[i];
    }
    j += (FILESIZE+UNIXTIME);

    o += (p+q+r+CHECKSUMAPPEND);
    snprintf(buf,CHECKSUM,"%o",o);
    for (i=0; i < CHECKSUM; ++i)
    {
        index[j+i] = buf[i];
    }

}

/*****************************************************************************
 *  Function       : write_zip_content
 *  Description    : write file content to tar zip file
 *  Input          : gzFile*hd  the output file struct
 *                   char*file_name the input file name
 *  Output         : None
 *  Return Value   :
 *  Calls          :
 *  Called By      :
 *
 *  History        :
 *  1.Date         :
 *    Author       :
 *    Modification :
 *
 ******************************************************************************/
void write_zip_content(gzFile fd,const char* file_name,long file_size)
{
    char buf[HEADERNUM];
    FILE *in = fopen(file_name, "r");
    while (file_size > 0)
    {
        memset(buf, 0, HEADERNUM);
        fread(buf, HEADERNUM, 1, in);
        gzwrite(fd, buf, HEADERNUM);
        file_size -= HEADERNUM;
    }

    fclose(in);
}

/*****************************************************************************
 *  Function       : write_content
 *  Description    : write file content to tar file
 *  Input          : FILE*hd  the output file struct
 *                   char*file_name the input file name
 *  Output         : None
 *  Return Value   :
 *  Calls          :
 *  Called By      :
 *
 *  History        :
 *  1.Date         :
 *    Author       :
 *    Modification :
 *
 ******************************************************************************/
void write_content(FILE* fd,const char* file_name,long file_size)
{
    char buf[HEADERNUM];
    FILE *in = fopen(file_name, "r");
    while (file_size > 0)
    {
        memset(buf, 0, HEADERNUM);
        fread(buf, HEADERNUM, 1, in);
        fwrite(buf, HEADERNUM, 1, fd);
        file_size -= HEADERNUM;
    }

    fclose(in);
}

/*****************************************************************************
 *  Function       : tar_zip_dir_recursive
 *  Description    : tar dir recusively
 *  Input          : gzFile* out   --  opened fd
 *                   char* dirname --  dir path ,absolute path
 *  Output         : None
 ******************************************************************************/
static void tar_zip_dir_recursive(gzFile out, const char * dirname, const char * parent)
{
    DIR *dp;
    struct dirent * dirp;
    char output_file_name_buf[MAX_FILE_NAME_LEN] = {0};
    char* ptr = NULL;
    if ((dp = opendir(dirname)) == NULL)
    {
        return;
    }
    memset(output_file_name_buf, 0, MAX_FILE_NAME_LEN);
    strcpy(output_file_name_buf, dirname);
    ptr = output_file_name_buf + strlen(output_file_name_buf);
    if (*(ptr - 1) != '/')
    {
        *ptr++ = '/';
        *ptr = 0;
    }
    while ((dirp = readdir(dp)) != NULL)
    {
        if (strcmp(dirp->d_name,".")==0 || strcmp(dirp->d_name,"..") == 0)
        {
            continue;
        }
        strcpy(ptr, dirp->d_name);

        //printf("tar file : %s\n",output_file_name_buf);
        struct stat tmpbuf;
        if (lstat(output_file_name_buf,&tmpbuf) < 0)
        {
            continue;
        }
        if (S_ISREG(tmpbuf.st_mode))
        {
            header t;
            memset(&t, 0x0, sizeof(t));
            if (parent)
            {
                char szfilename[NAMELEN] = {0};
                snprintf(szfilename, NAMELEN, "%s/%s", parent, dirp->d_name);
                //printf("tar filename:%s\n", szfilename);
                write_header(&t, szfilename, tmpbuf.st_size);
            }
            else
            {
                write_header(&t, dirp->d_name, tmpbuf.st_size);
            }
            gzwrite(out, &t, sizeof(header));
            write_zip_content(out, output_file_name_buf,tmpbuf.st_size);
        }
        else if (S_ISDIR(tmpbuf.st_mode))
        {
            char szpath[NAMELEN] = {0};
            strncpy(szpath, dirp->d_name, NAMELEN - 1);
            if (parent)
            {
                snprintf(szpath, NAMELEN - 1, "%s/%s", parent, dirp->d_name);
                tar_zip_dir_recursive(out, output_file_name_buf, szpath);
            }
            else
            {
                tar_zip_dir_recursive(out, output_file_name_buf, NULL);
            }
        }
    }
    closedir(dp);
}

/*****************************************************************************
 *  Function       : tar_zip_files
 *  Description    : tar and zip input files to output file
 *  Input          : char**input_names  iput files name array ,absolute path
 *                   char*output_name output file name ,absolute path
 *  Output         : None
 ******************************************************************************/
void tar_zip_files(int name_count, const char**input_names, char*output_name, int keep_parent)
{
    //int name_count = 5;
    gzFile out = NULL;
    if (output_name)
    {
        out = gzopen(output_name, "w");
    }
    if(!out)
    {
        return;
    }
    struct passwd * p = getpwnam("system");
    if (p)
    {
        chown(output_name, p->pw_uid, p->pw_gid);
    }
    const char**index = input_names;
    header t;
    int i;
    for (i=0; i < name_count; ++i)
    {
        if (*index == NULL)
        {
            ++index;
            continue;
        }
        struct stat buf;
        if (lstat(*index,&buf) < 0)
        {
            ++index;
            continue;
        }
        if (S_ISREG(buf.st_mode))
        {
            header t;
            memset(&t, 0x0, sizeof(t));
            write_header(&t, strrchr(*index, '/') + 1, buf.st_size);
            gzwrite(out, &t, sizeof(header));
            write_zip_content(out, *index, buf.st_size);
        }
        else if (S_ISDIR(buf.st_mode))
        {
            if (keep_parent)
            {
                char * p = NULL;
                char szTmp[NAMELEN] = {0};
                strncpy(szTmp, *index, NAMELEN - 1);
                p = szTmp + strlen(szTmp);
                if (*(--p) == '/') // delete end '/'
                {
                    *p = '\0';
                }
                p = strrchr(szTmp, '/');
                //printf("%s, keep parent=%s\n", __FUNCTION__, p + 1);
                tar_zip_dir_recursive(out, *index, p + 1);
            }
            else
            {
                tar_zip_dir_recursive(out, *index, NULL);
            }
        }
        ++index;
    }
    memset(&t, 0x0, sizeof(t));
    gzwrite(out, &t, sizeof(header));
    gzclose(out);
}

#if 0
static void copy_dir_recursively(const char * srcpath, const char * dstpath, int keep_parent)
{
    char szCmd[MAX_FILE_NAME_LEN] = {0};
    char szSubName[MAX_FILE_NAME_LEN] = {0};
    char szDstPath[MAX_FILE_NAME_LEN] = {0};
    char szParentName[BUF_LEN_64] = {0};
    char * pszSubName = NULL;
    DIR * dp = NULL;
    struct dirent * entry = NULL;
    struct stat statbuf;

    if (NULL == srcpath || NULL == dstpath)
    {
        log_err_record("%s, srcpath or dstpath doesn't exist!", __FUNCTION__);
        return;
    }
    if (access(srcpath, F_OK))
    {
        log_err_record("%s, srcpath doesn't exists!", __FUNCTION__);
        return;
    }
    if (access(dstpath, F_OK) && mkdir_p(dstpath, FILE_PERM_755))
    {
        log_err_record("%s, create dst dir %s failed!", __FUNCTION__, dstpath);
    }
    // set path name
    memset((void*)szSubName, 0, MAX_FILE_NAME_LEN);
    strncpy(szSubName, srcpath, MAX_FILE_NAME_LEN - 1);
    pszSubName =  szSubName + strlen(szSubName);
    //INFO("%s, srcpath=%s\n", __FUNCTION__, szSubName);
    if (*(pszSubName - 1) != '/')
    {
        *pszSubName++ = '/';
        log_err_record("%s, srcpath=%s", __FUNCTION__, szSubName);
    }
    if (keep_parent)
    {
        char * p = NULL;
        char szTmp[BUF_LEN_64] = {0};
        strncpy(szTmp, szSubName, strlen(szSubName) - 1); // not include '/'
        p = strrchr(szTmp, '/');
        strncpy(szParentName, p + 1, BUF_LEN_64);
        snprintf(szDstPath, MAX_FILE_NAME_LEN, "%s/%s", dstpath, szParentName);
        log_err_record("%s, szDstPath=%s", __FUNCTION__, szDstPath);
        mkdir_p(szDstPath, FILE_PERM_755);
    }
    else
    {
        strncpy(szDstPath, dstpath, MAX_FILE_NAME_LEN - 1);
    }
    // opendir for loop
    if (NULL == (dp = opendir(srcpath)))
    {
        log_err_record("%s, open srcpath failed!", __FUNCTION__);
        return;
    }
    while (NULL != (entry = readdir(dp)))
    {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
        {
            continue;
        }
        strcpy(pszSubName, entry->d_name);
        //INFO("....stat %s\n", szSubName);
        if (0 > stat (szSubName, &statbuf))
        {
            log_err_record("%s, stat path[%s] error!", __FUNCTION__, szSubName);
            continue;
        }
        if (S_ISREG(statbuf.st_mode))
        {
            copy_file_to_dir(szSubName, szDstPath);
        }
        else if (S_ISDIR(statbuf.st_mode))
        {
            copy_dir_recursively(szSubName, szDstPath, keep_parent);
        }
    }
    closedir(dp);
}

/*****************************************************************************
 *  Function       : move_input_files
 *  Description    : move all files into dstpath(output_name)
 *  Input          : char**input_names  iput files name array ,absolute path
 *                   char*output_name output file name, absolute path
 *  Output         : None
 ******************************************************************************/
static void move_input_files(int name_count, const char** input_names, char* output_name, int keep_parent)
{
    int i = 0;
    int ret = 0;
    const char** index = input_names;

    if (NULL == input_names || NULL == output_name)
    {
        return;
    }
    if (access(output_name, F_OK))
    {
        /* create path */
        log_err_record("%s, create dir [%s]", __FUNCTION__, output_name);
        ret = mkdir_p(output_name, FILE_PERM_755);
        if (ret == -1 && errno == EEXIST)
        {
            ret = chmod(output_name, FILE_PERM_755);
        }
        if (ret == -1)
        {
            log_err_record("Unable to mkdir or chmod[%s]: %s\n", strerror(errno), output_name);
            return ;
        }
    }

    for (i = 0; i < name_count; ++i)
    {
        if (*index == NULL)
        {
            ++index;
            continue;
        }
        struct stat buf;
        if (lstat(*index,&buf) < 0)
        {
            ++index;
            continue;
        }
        if (S_ISREG(buf.st_mode))
        {
            copy_file_to_dir(*index, output_name);
        }
        else if (S_ISDIR(buf.st_mode))
        {
            copy_dir_recursively(*index, output_name, keep_parent);
        }
        ++index;
    }
}

/*****************************************************************************
 *  Function       : tar_files
 *  Description    : tar input files to output file
 *  Input          : char**input_names  iput files name array ,absolute path
 *                   char*output_name output file name, absolute path
 *  Output         : None
 ******************************************************************************/
void tar_files(int name_count, const char**input_names, char*output_name)
{
    //int name_count = 5;
    FILE* out = NULL;
    if (output_name)
    {
        out = fopen(output_name, "w");
    }
    if (!out)
    {
        return;
    }
    struct passwd * p = getpwnam("system");
    if (p)
    {
        chown(output_name, p->pw_uid, p->pw_gid);
    }
    const char**index = input_names;
    header t;
    int i;
    for (i = 0; i < name_count; ++i)
    {
        if(*index == NULL)
        {
            ++index;
            continue;
        }
        struct stat buf;
        if (lstat(*index,&buf) < 0)
        {
            ++index;
            continue;
        }
        if (S_ISREG(buf.st_mode))
        {
            header t;
            memset(&t, 0x0, sizeof(t));
            write_header(&t, *index, buf.st_size);
            fwrite(&t, sizeof(header), 1, out);
            write_content(out, *index, buf.st_size);
        }
        else if (S_ISDIR(buf.st_mode))
        {
            DIR *dp;
            struct dirent * dirp;
            if ((dp = opendir(*index)) == NULL)
            {
                continue;
            }
            while ((dirp = readdir(dp)) != NULL)
            {
                char output_file_name_buf[MAX_FILE_NAME_LEN];
                memset(output_file_name_buf, 0, MAX_FILE_NAME_LEN);
                strcpy(output_file_name_buf, *index);
                char*ptr =  output_file_name_buf + strlen(output_file_name_buf);
                if (*(ptr - 1) != '/')
                {
                    *ptr++ = '/';
                    *ptr = 0;
                }
                if (strcmp(dirp->d_name,".") == 0 || strcmp(dirp->d_name,"..") == 0)
                {
                    continue;
                }
                strcpy(ptr, dirp->d_name);

                struct stat tmpbuf;
                if (lstat(output_file_name_buf,&tmpbuf) < 0)
                {
                    continue;
                }
                if (!S_ISREG(tmpbuf.st_mode))
                {
                    continue;
                }

                header t;
                memset(&t,0x0,sizeof(t));
                write_header(&t,output_file_name_buf, tmpbuf.st_size);
                fwrite(&t, sizeof(header), 1, out);
                write_content(out,output_file_name_buf, tmpbuf.st_size);
            }
            closedir(dp);
        }
        ++index;
    }
    memset(&t, 0x0, sizeof(t));
    fwrite(&t, sizeof(header), 1, out);
    fclose(out);
}

/*****************************************************************************
 *  Function       : pack_files
 *  Description    : pack input files to output file
 *  Input          : char**input_names  iput files name array ,absolute path
 *                   char*output_name output file name ,absolute path
 *                   int compress  is compress flag , zero no compress
 *  Output         : None
 *  Return Value   :
 *  Calls          :
 *  Called By      :
 *
 *  History        :
 *  1.Date         :
 *    Author       :
 *    Modification :
 *
 ******************************************************************************/
void pack_files(int argc, const char**input_names,  const char* output_path, char *file_prefix, char*output_name, int compress, int keep_parent)
{
    int i=0;
    /*if path not exist create*/
    if (access(output_path, F_OK))
    {
        int ret = mkdir(output_path, 0755);
        if (0 != ret)
        {
            return;
        }
        struct passwd * p = getpwnam("system");
        if (p)
        {
            chown(output_path, p->pw_uid, p->pw_gid);
        }
    }
    char output_file_name_buf[MAX_FILE_NAME_LEN] = {0};

    strcpy(output_file_name_buf, output_path);
    strcat(output_file_name_buf, "/");
    strcat(output_file_name_buf, file_prefix);
    if (!output_name)
    {
        struct tm *tstruct;
        time_t tsec;
        char  logbuf[LOG_BUF_MAX_LEN] = {0};
        tsec = time(NULL);
        tstruct = localtime(&tsec);
        sprintf(logbuf, "%04d%02d%02d%02d%02d%02d_all", //year months days hour min second
                        tstruct->tm_year + 1900,
                        tstruct->tm_mon + 1,
                        tstruct->tm_mday,
                        tstruct->tm_hour,
                        tstruct->tm_min,
                        tstruct->tm_sec);
        strcat(output_file_name_buf, logbuf);
    }
    else
    {
        strcat(output_file_name_buf, output_name);
    }
    if (E_PACK_ZIP == compress)
    {
        strcat(output_file_name_buf, ".tar.gz");
        tar_zip_files(argc, input_names, output_file_name_buf, keep_parent);
    }
    else if (E_PACK_TAR == compress)
    {
        strcat(output_file_name_buf,".tar");
        tar_files(argc, input_names, output_file_name_buf);
    }
    else if (E_PACK_MOVE == compress)
    {
        move_input_files(argc, input_names, output_file_name_buf, keep_parent);
    }
}
#endif

