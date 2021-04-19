#include "SSHSession.h"
#include "Log.h"
#include <libssh/libssh.h>
#include <QMessageBox>

using namespace ssh;

SSHSession::SSHSession(SSHOption *option)
    : m_option(*option)
{
    m_session = ssh_new();
    if (m_session == nullptr)
    {
        throw "ssh_new failed";
    }
    init(m_session);
}

SSHSession::~SSHSession()
{
    ssh_disconnect(m_session);
    ssh_free(m_session);
}

bool SSHSession::connect()
{
    int rc = ssh_connect(m_session);
    if (rc != SSH_OK)
    {
        LOG_ERROR << ssh_get_error(m_session);
        return false;
    }
    return true;
}

void SSHSession::init(ssh_session session)
{
    int log_verbosity = SSH_LOG_NOLOG;
    ssh_options_set(session, SSH_OPTIONS_HOST, m_option.host.c_str());
    ssh_options_set(session, SSH_OPTIONS_PORT, &m_option.port);
    ssh_options_set(session, SSH_OPTIONS_LOG_VERBOSITY, &log_verbosity);
    ssh_options_set(session, SSH_OPTIONS_USER, m_option.user.c_str());
    ssh_options_set(session, SSH_OPTIONS_SSH_DIR, NULL);
}

bool authenticate_server(ssh_session session, bool accept_new_host)
{
    enum ssh_known_hosts_e state;
    unsigned char *hash = nullptr;
    ssh_key srv_pubkey = nullptr;
    size_t hlen;
    char *hexa;
    int rc = 0;

    rc = ssh_get_server_publickey(session, &srv_pubkey);
    if (rc < 0)
    {
        LOG_ERROR << "ssh_get_server_publickey failed";
        return false;
    }
    rc = ssh_get_publickey_hash(srv_pubkey, SSH_PUBLICKEY_HASH_SHA1, &hash, &hlen);
    ssh_key_free(srv_pubkey);
    if (rc < 0)
    {
        LOG_ERROR << "ssh_get_publickey_hash failed";
        return false;
    }
    state = ssh_session_is_known_server(session);

    bool success = false;
    switch (state)
    {
    case SSH_KNOWN_HOSTS_OK:
        /* OK */
        success = true;
        break;
    case SSH_KNOWN_HOSTS_CHANGED:
        hexa = ssh_get_hexa(hash, hlen);
        LOG_WARN << "Host key for server changed: " << hexa;
        LOG_WARN << "For security reasons, connection will be stopped";
        ssh_string_free_char(hexa);
        break;
    case SSH_KNOWN_HOSTS_OTHER:
        LOG_WARN << "The host key for this server was not found but an other type of key exists.";
        LOG_WARN << "An attacker might change the default server key to"
                 << "confuse your client into thinking the key does not exist.";

        break;
    case SSH_KNOWN_HOSTS_NOT_FOUND:
        LOG_WARN << "Could not find known host file, create?\nyes";
        /* FALL THROUGH to SSH_SERVER_NOT_KNOWN behavior */
    case SSH_KNOWN_HOSTS_UNKNOWN:
        hexa = ssh_get_hexa(hash, hlen);
        LOG_WARN << "The server is unknown. Do you trust the host key?";
        LOG_WARN << "Public key hash: " << hexa;
        ssh_string_free_char(hexa);
        if (!accept_new_host)
        {
            LOG_ERROR << "SSH_KNOWN_HOSTS_UNKNOWN, reject!";
            break;
        }
        // update local known_hosts file
        rc = ssh_session_update_known_hosts(session);
        if (rc < 0)
        {
            LOG_ERROR << "update known hosts failed: " << strerror(errno);
            break;
        }
        success = true;
        break;
    case SSH_KNOWN_HOSTS_ERROR:
        LOG_ERROR << "SSH_KNOWN_HOSTS_ERROR" << ssh_get_error(session);
        success = false;
        break;
    default:
        LOG_ERROR <<"Unkown error";
        break;
    }

    ssh_clean_pubkey_hash(&hash);
    return success;
}

bool authenticate_user(ssh_session session, std::string password)
{
    int rc = ssh_userauth_password(session, NULL, password.c_str());
    if (rc != SSH_AUTH_SUCCESS)
    {
        LOG_ERROR << "Error authenticating with password:";
        LOG_ERROR << ssh_get_error(session);
        ssh_disconnect(session);
        return false;
    }
    return true;
}

bool SSHSession::authentication(bool accept_new_host)
{
    return authenticate_server(m_session, accept_new_host) && authenticate_user(m_session, m_option.password);
}

bool SSHSession::execute_cmd()
{
    ssh_channel channel;
    int rc;
    char buffer[256];
    int nbytes;

    try
    {
        channel = ssh_channel_new(m_session);
        if (channel == NULL)
            throw "ssh_channel_new";

        rc = ssh_channel_open_session(channel);
        if (rc != SSH_OK)
        {
            ssh_channel_free(channel);
            throw "ssh_channel_open_session";
        }

        rc = ssh_channel_request_exec(channel, "ps au");
        if (rc != SSH_OK)
        {
            ssh_channel_close(channel);
            ssh_channel_free(channel);
            throw "ssh_channel_request_exec";
        }

        nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
        while (nbytes > 0)
        {
            LOG_INFO << buffer;
            nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
        }

        if (nbytes < 0)
        {
            ssh_channel_close(channel);
            ssh_channel_free(channel);
            throw "nbytes < 0";
        }
        ssh_channel_send_eof(channel);
        ssh_channel_close(channel);
        ssh_channel_free(channel);
    }
    catch (const char *msg)
    {
        LOG_ERROR << msg;
        return false;
    }

    return true;
}
