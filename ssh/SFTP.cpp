#include "SFTP.h"
#include "Log.h"
#include "SSHSession.h"
#include "SFTPSession.h"

#include <libssh/libssh.h>
#include <libssh/sftp.h>
#include <sys/stat.h>

using namespace ssh;

void SFTP::makeDir(std::string dirname)
{
    sftp_session sftp_session = m_sftpSession->m_session;
    int rc;

    rc = sftp_mkdir(sftp_session, dirname.c_str(), S_IRWXU);
    if (rc != SSH_OK)
    {
        if (sftp_get_error(sftp_session) != SSH_FX_FILE_ALREADY_EXISTS)
        {
            LOG_ERROR << "Can't create directory: " << ssh_get_error(m_sshSession->m_session);
            return;
        }
    }
}

bool SFTP::listDir(const std::string& dirname, std::vector<SFTPAttr> &attrs)
{
    sftp_dir dir;
    dir = sftp_opendir(m_sftpSession->m_session, dirname.c_str());
    if (!dir)
    {
        LOG_ERROR << "Directory not opened: " << ssh_get_error(m_sshSession->m_session);
        return false;
    }

    sftp_attributes attr;

    while ((attr = sftp_readdir(m_sftpSession->m_session, dir)) != NULL)
    {
        attrs.emplace_back(attr->name,
                           attr->owner, attr->group,
                           attr->uid, attr->gid, (uint32_t)attr->type,
                           attr->permissions, attr->size, attr->atime,
                           attr->createtime, attr->mtime);
        sftp_attributes_free(attr);
    }

    if (!sftp_dir_eof(dir))
    {
        LOG_ERROR << "Can't list directory:" << ssh_get_error(m_sshSession->m_session);
        sftp_closedir(dir);
        return false;
    }

    if (sftp_closedir(dir) != SSH_OK)
    {
        LOG_ERROR << "Can't close directory: " << ssh_get_error(m_sshSession->m_session);
        return false;
    }
    return true;
}
