#ifndef SSH_SESSION_H
#define SSH_SESSION_H

#include <string>

extern "C" {
struct ssh_session_struct;
typedef struct ssh_session_struct *ssh_session;
}

namespace ssh {
struct SSHOption
{
    std::string host;
    int port;
    std::string user;
    std::string password;
    //std::string ssh_dir;
    SSHOption(const std::string &h, int p, const std::string &u, const std::string &pass)
        : host(h)
        , port(p)
        , user(u)
        , password(pass)
    {}
};

class SSHSession
{
public:
    SSHSession(SSHOption *option);

    ~SSHSession();

    bool connect();
    bool authentication(bool accept_new_host);
    bool execute_cmd();

    SSHOption &getOption() { return m_option; }

private:
    void init(ssh_session session);

    friend class SSH;
    friend class SFTP;
    friend class SFTPSession;

private:
    ssh_session m_session;
    SSHOption m_option;
    std::string m_host;
};
} // namespace ssh

#endif
