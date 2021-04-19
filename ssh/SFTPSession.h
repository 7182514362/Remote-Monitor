#ifndef SFTP_SESSION_H
#define SFTP_SESSION_H

#include <vector>

extern "C"
{
    struct sftp_session_struct;
    typedef struct sftp_session_struct *sftp_session;
}

namespace ssh
{
    class SSHSession;

    class SFTPSession
    {
    public:
        SFTPSession(SSHSession *session);
        ~SFTPSession();

        bool init();

        friend class SFTP;

    private:
        sftp_session m_session;
    };
} // namespace ssh

#endif
