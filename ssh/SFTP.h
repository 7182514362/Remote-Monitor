#ifndef SFTP_H_
#define SFTP_H_


#include <string>
#include <memory>
#include <vector>

namespace ssh
{
    class SSHSession;
    class SFTPSession;

    struct SFTPAttr
    {
        std::string name;
        std::string owner;
        std::string group;
        uint32_t uid;
        uint32_t gid;

        uint32_t type;
        uint32_t permission;
        uint64_t size;
        uint64_t atime;
        uint64_t createTime;
        uint64_t mtime;

        SFTPAttr(char *n, char *o, char *g, uint32_t uid,
                 uint32_t gid, uint32_t type, uint32_t per,
                 uint64_t size, uint64_t atime, uint64_t ctime,
                 uint64_t mtime)
            : name(n), owner(o), group(g), uid(uid), gid(gid),
              type(type), permission(per), size(size), atime(atime), createTime(ctime), mtime(mtime) {}
    };

    class SFTP
    {

    public:
        SFTP(SSHSession *ssh, SFTPSession *sftp) : m_sshSession(ssh), m_sftpSession(sftp){}
        ~SFTP(){}

    public:
        void createFile(std::string file);
        void readFile(std::string file);
        void writeFile(std::string file);
        void sendFile(std::string file);
        void recvFile(std::string file);
        void renameFile(std::string file);

        void getFileState(std::string file); // permission/owner/group/
        void setFileState(std::string file);

        void makeDir(std::string dir);
        bool listDir(const std::string& dir,std::vector<SFTPAttr>& attrs);
        void remvDir(std::string dir);
        void dirInfo(std::string dir);

        void fileSysInfo();

    private:
        SSHSession *m_sshSession;
        SFTPSession *m_sftpSession;
    };
}

#endif
