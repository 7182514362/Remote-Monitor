#include "SSHSession.h"
#include "SFTPSession.h"
#include "Log.h"
#include <libssh/sftp.h>

using namespace ssh;

SFTPSession::SFTPSession(SSHSession *session)
{
    m_session = sftp_new(session->m_session);
    if (!m_session) {
        LOG_ERROR << "Error allocating SFTP session: " << ssh_get_error(session->m_session);
        return;
    }
}

SFTPSession::~SFTPSession()
{
    sftp_free(m_session);
}

bool SFTPSession::init()
{
    int ret = sftp_init(m_session);
    if (ret != SSH_OK) {
        LOG_ERROR << "Error initializing SFTP session: " << sftp_get_error(m_session);
        sftp_free(m_session);
        return false;
    }
    return true;
}