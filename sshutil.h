#ifndef SSHUTIL_H
#define SSHUTIL_H

#include <memory>
#include <vector>
#include <QString>

namespace ssh {

class SSHOption;
class SSHSession;
class SSH;
class SFTPSession;
class SFTP;
struct SFTPAttr;
} // namespace ssh

class SSHUtil
{
public:
    SSHUtil(QString host, int port, QString user, QString password);
    ~SSHUtil();

    bool connect();
    bool init();

    std::shared_ptr<std::vector<ssh::SFTPAttr>> listDir(QString dir);

    ssh::SSHOption *getOption() { return m_option; }

private:
    ssh::SSHOption *m_option;
    ssh::SSHSession *m_sshSession;
    ssh::SSH *m_ssh;
    ssh::SFTPSession *m_sftpSession;
    ssh::SFTP *m_sftp;
};

#endif // SSHUTIL_H
