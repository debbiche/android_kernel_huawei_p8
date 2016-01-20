

#ifndef __BSP_RFILE_H__
#define __BSP_RFILE_H__

#ifdef __cplusplus /* __cplusplus */
extern "C"
{
#endif /* __cplusplus */


#define     RFILE_RDONLY            (0x00000000)
#define     RFILE_WRONLY            (0x00000001)
#define     RFILE_RDWR              (0x00000002)
#define     RFILE_CREAT             (0x00000040)
#define     RFILE_EXCL              (0x00000080)
#define     RFILE_TRUNC             (0x00000200)
#define     RFILE_APPEND            (0x00000400)
#define     RFILE_DIRECTORY         (0x00004000)


#define     RFILE_NAME_MAX          255             /* max length of the file name*/

typedef struct {
    u64             d_ino;
    u64             d_off;
    u16             d_reclen;
    u8              d_type;
    s8              d_name[0];
}RFILE_DIRENT_STRU;

/* struct timespec */
typedef struct {
    u64         tv_sec;         /* seconds */
    u64         tv_nsec;        /* nanoseconds */
}RFILE_TIMESPEC_STRU;

struct rfile_stat_stru
{
    u64                     ino;
    u32                     dev;
    u16                     mode;
    u32                     nlink;
    u32                     uid;
    u32                     gid;
    u32                     rdev;
    u64                     size;
    RFILE_TIMESPEC_STRU     atime;
    RFILE_TIMESPEC_STRU     mtime;
    RFILE_TIMESPEC_STRU     ctime;
    u32                     blksize;
    u64                     blocks;
};


void adp_rfile_init(void);
s32 bsp_rfile_init(void);

s32 bsp_open(const s8 *path, s32 flags, s32 mode);
s32 bsp_close(u32 fp);
s32 bsp_write(u32 fd, const s8 *ptr, u32 size);
s32 bsp_read(u32 fd ,s8 *ptr, u32 size);
s32 bsp_lseek(u32  fd, long offset, s32 whence);
long bsp_tell(u32 fd);
s32 bsp_remove(const s8 *pathname);
s32 bsp_mkdir(s8 *dirName, s32 mode);
s32 bsp_rmdir(s8 *path);
s32 bsp_opendir(s8 *dirName);
s32 bsp_readdir(u32 fd, void  *dirent, u32 count);
s32 bsp_closedir(s32 pDir);
s32 bsp_stat(s8 *name, void *pStat);
s32 bsp_access(s8 *path, s32 mode);
s32 bsp_rename( const char * oldname, const char * newname );



#ifdef __cplusplus /* __cplusplus */
}
#endif /* __cplusplus */

#endif    /*  __BSP_RFILE_H__ */
