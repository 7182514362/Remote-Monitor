#include "sshutil.h"
#include "SFTP.h"
#include "SSH.h"

SSHUtil::SSHUtil(QString host, int port, QString user, QString password)
{
    m_option = new ssh::SSHOption(
        {host.toStdString(), port, user.toStdString(), password.toStdString()});
    m_sshSession = new ssh::SSHSession(m_option);
    m_ssh = new ssh::SSH(m_sshSession);
    m_sftpSession = new ssh::SFTPSession(m_sshSession);
    m_sftp = new ssh::SFTP(m_sshSession, m_sftpSession);
}

SSHUtil::~SSHUtil(){
    delete m_sftp;
    delete m_sftpSession;
    delete m_ssh;
    delete m_sshSession;
}

bool SSHUtil::connect()
{
    if (!m_sshSession->connect()) {
        return false;
    }

    if (!m_ssh->authenticate()) {
        return false;
    }
    return true;
}

bool SSHUtil::init()
{
    if (!m_sftpSession->init()) {
        return false;
    }
    return true;
}

std::shared_ptr<std::vector<ssh::SFTPAttr>> SSHUtil::listDir(QString dir)
{
    return m_sftp->listDir(dir.toStdString());
}
